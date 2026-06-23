#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

void network_init();
void network_update();
bool network_is_connected();
bool network_get_local_time(char *buffer, size_t bufferSize);

#endif
