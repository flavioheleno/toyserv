#ifndef SERVER_H
#define SERVER_H

#include <string.h>

typedef enum {
    UNIX,
    IPv4,
    IPv6
} serverType;
typedef struct _serverObject serverObject;

serverObject *serverConstruct(
    serverType type,
    const char *address,
    size_t addressLen,
    int port
);

void serverDestruct(serverObject *instance);

int serverListen(serverObject *instance);
int serverLoop(serverObject *instance);
void serverStop(serverObject *instance);

#endif /* SERVER_H */