#include <Arduino.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
#include <time.h>
#include "network_manager.h"

namespace {
const char *AP_SSID = "SmartPillbox_AP";
const char *AP_PASSWORD = nullptr;
const byte DNS_PORT = 53;
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_GATEWAY(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

DNSServer dnsServer;
WebServer webServer(80);
Preferences preferences;

String savedSsid;
String savedPassword;
bool portalRunning = false;
bool shouldStopPortal = false;
bool timeSyncStarted = false;
unsigned long stopPortalAt = 0;

void startTimeSync() {
    if (timeSyncStarted) {
        return;
    }

    configTime(8 * 3600, 0, "ntp.aliyun.com", "cn.pool.ntp.org", "pool.ntp.org");
    timeSyncStarted = true;
    Serial.println("NTP time sync started.");
}

String htmlHeader(const String &title) {
    return "<!doctype html><html><head><meta charset='utf-8'>"
           "<meta name='viewport' content='width=device-width,initial-scale=1'>"
           "<title>" + title + "</title>"
           "<style>body{font-family:Arial,sans-serif;margin:24px;background:#f5f7fb;color:#17202a}"
           "button,input,select{width:100%;box-sizing:border-box;padding:12px;margin:8px 0;font-size:16px}"
           "button{background:#1f7a5a;color:white;border:0;border-radius:6px}"
           "section{max-width:420px;margin:auto;background:white;padding:18px;border-radius:8px;box-shadow:0 2px 14px #0001}"
           "small{color:#667}</style></head><body><section>";
}

String htmlFooter() {
    return "</section></body></html>";
}

String jsonEscape(const String &value) {
    String escaped;
    escaped.reserve(value.length() + 8);
    for (size_t index = 0; index < value.length(); index++) {
        char current = value[index];
        if (current == '\\' || current == '"') {
            escaped += '\\';
        }
        escaped += current;
    }
    return escaped;
}

bool savedNetworkVisible() {
    if (savedSsid.isEmpty()) {
        return false;
    }

    Serial.println("Scanning for saved WiFi...");
    int count = WiFi.scanNetworks(false, true);
    for (int index = 0; index < count; index++) {
        if (WiFi.SSID(index) == savedSsid) {
            WiFi.scanDelete();
            return true;
        }
    }
    WiFi.scanDelete();
    return false;
}

bool connectToSavedWifi(unsigned long timeoutMs) {
    if (savedSsid.isEmpty()) {
        return false;
    }

    Serial.print("Connecting to saved WiFi: ");
    Serial.println(savedSsid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSsid.c_str(), savedPassword.c_str());

    unsigned long startAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAt < timeoutMs) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected, IP: ");
        Serial.println(WiFi.localIP());
        startTimeSync();
        return true;
    }

    Serial.println("Saved WiFi connect failed.");
    WiFi.disconnect(false);
    return false;
}

void handleRoot() {
    String page = htmlHeader("Smart Pillbox WiFi");
    page += "<h2>Smart Pillbox WiFi</h2>"
            "<p><small>选择 WiFi 后输入密码，药盒会保存信息并在下次启动时自动连接。</small></p>"
            "<form method='POST' action='/save'>"
            "<select id='ssid' name='ssid'><option>正在扫描...</option></select>"
            "<input name='password' type='password' placeholder='WiFi 密码'>"
            "<button type='submit'>连接并保存</button>"
            "</form><button onclick='scan()'>重新扫描</button>"
            "<script>async function scan(){let s=document.getElementById('ssid');s.innerHTML='<option>正在扫描...</option>';"
            "let r=await fetch('/scan');let a=await r.json();s.innerHTML='';"
            "a.forEach(n=>{let o=document.createElement('option');o.value=n.ssid;o.textContent=n.ssid+' ('+n.rssi+' dBm)';s.appendChild(o);});"
            "if(!a.length)s.innerHTML='<option>未找到 WiFi</option>';};scan();</script>";
    page += htmlFooter();
    webServer.send(200, "text/html", page);
}

void handleScan() {
    int count = WiFi.scanNetworks(false, true);
    String json = "[";
    for (int index = 0; index < count; index++) {
        if (index > 0) {
            json += ",";
        }
        json += "{\"ssid\":\"" + jsonEscape(WiFi.SSID(index)) + "\",\"rssi\":" + String(WiFi.RSSI(index)) + "}";
    }
    json += "]";
    WiFi.scanDelete();
    webServer.send(200, "application/json", json);
}

void handleSave() {
    String ssid = webServer.arg("ssid");
    String password = webServer.arg("password");

    if (ssid.isEmpty()) {
        webServer.send(400, "text/html", htmlHeader("WiFi Error") + "<h2>请选择 WiFi</h2><a href='/'>返回</a>" + htmlFooter());
        return;
    }

    savedSsid = ssid;
    savedPassword = password;
    if (!connectToSavedWifi(15000)) {
        webServer.send(200, "text/html", htmlHeader("WiFi Failed") + "<h2>连接失败</h2><p>请检查密码后重试。</p><a href='/'>返回</a>" + htmlFooter());
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
        WiFi.softAP(AP_SSID, AP_PASSWORD);
        return;
    }

    preferences.putString("ssid", savedSsid);
    preferences.putString("password", savedPassword);
    webServer.send(200, "text/html", htmlHeader("WiFi Saved") + "<h2>连接成功</h2><p>WiFi 已保存，之后启动会自动连接。</p>" + htmlFooter());
    shouldStopPortal = true;
    stopPortalAt = millis() + 1200;
}

void startPortal() {
    Serial.println("Starting WiFi config portal...");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    dnsServer.start(DNS_PORT, "*", AP_IP);
    webServer.on("/", handleRoot);
    webServer.on("/scan", handleScan);
    webServer.on("/save", HTTP_POST, handleSave);
    webServer.on("/generate_204", handleRoot);
    webServer.on("/hotspot-detect.html", handleRoot);
    webServer.on("/fwlink", handleRoot);
    webServer.onNotFound(handleRoot);
    webServer.begin();

    portalRunning = true;
    shouldStopPortal = false;
    Serial.println("Connect phone to SmartPillbox_AP and open http://192.168.4.1");
}

void stopPortal() {
    webServer.stop();
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    portalRunning = false;
    shouldStopPortal = false;
    Serial.println("WiFi config portal stopped.");
}
}

void network_init() {
    Serial.println("Initializing network...");

    preferences.begin("wifi", false);
    savedSsid = preferences.getString("ssid", "");
    savedPassword = preferences.getString("password", "");

    if (savedNetworkVisible() && connectToSavedWifi(10000)) {
        return;
    }

    startPortal();
}

void network_update() {
    if (WiFi.status() == WL_CONNECTED) {
        startTimeSync();
    }

    if (!portalRunning) {
        return;
    }

    dnsServer.processNextRequest();
    webServer.handleClient();

    if (shouldStopPortal && millis() >= stopPortalAt) {
        stopPortal();
    }
}

bool network_is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

bool network_get_local_time(char *buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0) {
        return false;
    }

    struct tm timeInfo;
    if (!getLocalTime(&timeInfo, 10)) {
        return false;
    }

    strftime(buffer, bufferSize, "%H:%M:%S", &timeInfo);
    return true;
}
