#include <stdio.h>

#include "server.h"

int main(int argc, const char **argv) {
    const char *addr = "127.0.0.1";
    unsigned int port = 8081;

    /* ServerObject sObj = new ServerObject(); */
    serverObject *server = serverConstruct(IPv4, addr, strlen(addr), port);
    /* sObj->listen(); */
    unsigned int r = serverListen(server);
    if (r != 0) {
        printf("Error on serverListen: %d: %s\n", r, strerror(r));

        return 1;
    }

    r = serverLoop(server);
    if (r != 0) {
        printf("Error on serverLoop: %d: %s\n", r, strerror(r));

        return 1;
    }

    /* sObj->stop(); */
    serverStop(server);
    /* unset(sObj) */
    serverDestruct(server);

    return 0;
}

// https://www.man7.org/linux/man-pages/man5/protocols.5.html