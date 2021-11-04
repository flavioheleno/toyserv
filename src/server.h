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
    unsigned int port
);

void serverDestruct(serverObject *instance);

unsigned int serverListen(serverObject *instance);
unsigned int serverLoop(serverObject *instance);
void serverStop(serverObject *instance);

#endif /* SERVER_H */