#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include "server.h"
#include "list.h"

#define maxFd(x, y) ((x) > (y) ? (x) : (y))

#define DEBUG(n, a...) //printf(n, ## a)

typedef struct _clientObject {
    unsigned int id;
    int socket;
    char *address;
    size_t addressLen;
    unsigned int port;
    struct sockaddr_in sockaddr;
    char *request;
    size_t reqLen;
    size_t reqPos;
    char *response;
    size_t rspLen;
    size_t rspPos;
    unsigned int keepAlive;
} clientObject;

struct _serverObject {
    serverType type;
    char *address;
    size_t addressLen;
    unsigned int port;
    int socket;
    struct sockaddr_in sockaddr;
    listObject *list;
};

/**
 * Class constructor. This method creates a new instance of our 
 * Server Object and initializes it with the constructor parameters.
 * 
 * @link https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
 * @link https://www.tutorialspoint.com/unix_sockets/socket_structures.htm
 * @link https://linux.die.net/man/3/inet_aton
 */
serverObject *serverConstruct(serverType type, const char *address, size_t addressLen,  unsigned int port) {
    DEBUG("serverConstruct()\n");
    serverObject *instance = (serverObject *)malloc(sizeof(serverObject));
    memset(instance, 0, sizeof(serverObject));
    instance->type = type;
    instance->address = (char *)malloc(addressLen);
    memcpy(instance->address, address, addressLen);
    instance->addressLen = addressLen;
    instance->port = port;
    instance->list = listConstruct();

    /** 
     * The code below initializes the socket address struct
     * according to the underlying socket type that will be
     * used by our server
     */
    switch (instance->type) {
        case UNIX:
            DEBUG("YO SOY UNIQUIS!\n");
            instance->sockaddr.sin_family = AF_UNIX;
            break;
        case IPv4:
            DEBUG("YO SOY IPv4!\n");
            instance->sockaddr.sin_family = AF_INET;
            instance->sockaddr.sin_port = htons(instance->port);
            break;
        case IPv6:
            DEBUG("YO SOY IPv6!\n");
            instance->sockaddr.sin_family = AF_INET6;
            instance->sockaddr.sin_port = htons(instance->port);
            break;
    }

    return instance;
}

/**
 * Class destructor.
 */
void serverDestruct(serverObject *instance) {
    DEBUG("serverDestruct()\n");
    if (instance != NULL) {
        if (instance->address != NULL) {
            free(instance->address);
            instance->address = NULL;
        }

        listDestruct(instance->list);
        instance->list = NULL;

        free(instance);
        instance = NULL;
    }
}

/**
 * @link https://www.man7.org/linux/man-pages/man2/socket.2.html
 * @link https://www.man7.org/linux/man-pages/man2/setsockopt.2.html
 * @link https://www.man7.org/linux/man-pages/man2/bind.2.html
 * @link https://www.man7.org/linux/man-pages/man2/listen.2.html
 */
unsigned int serverListen(serverObject *instance) {
    DEBUG("serverListen()\n");
    if (inet_aton(instance->address, &instance->sockaddr.sin_addr) == 0) {
        DEBUG("Error at inet_aton\n");

        return errno;
    }

    instance->socket = socket(instance->sockaddr.sin_family, SOCK_STREAM, 0);
    if (instance->socket <= 0) {
        DEBUG("Error at socket\n");
        
        return errno;
    }

    int flags = fcntl(instance->socket, F_GETFL);
    if (flags < 0) {
        DEBUG("Error reading flags\n");
        close(instance->socket);

        return errno;
    }

    if (fcntl(instance->socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        DEBUG("Error setting nonblock\n");
        close(instance->socket);

        return errno;
    }

    int opt = 1;
    if (setsockopt(instance->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        DEBUG("Error calling setsockopt SO_REUSEADDR\n");
        close(instance->socket);

        return errno;
    }

    if (setsockopt(instance->socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        DEBUG("Error calling setsockopt SO_REUSEPORT\n");
        close(instance->socket);

        return errno;
    }

    if (bind(instance->socket, (struct sockaddr *)&instance->sockaddr, sizeof(instance->sockaddr)) < 0) {
        DEBUG("Error at bind\n");
        close(instance->socket);

        return errno;
    }

    if (listen(instance->socket, 0) < 0) {
        DEBUG("Error at listen\n");
        close(instance->socket);

        return errno;
    }

    printf("Listening on %s:%d\n", instance->address, instance->port);

    return 0;
}

/**
 * @link https://www.man7.org/linux/man-pages/man2/select.2.html
 * @link https://www.man7.org/linux/man-pages/man2/accept.2.html
 * @link https://www.man7.org/linux/man-pages/man2/read.2.html
 * @link https://www.man7.org/linux/man-pages/man2/write.2.html
 */
unsigned int serverLoop(serverObject *instance) {
    DEBUG("serverLoop()\n");
    unsigned int nfds;
    fd_set rfds, wfds;
    clientObject *client;
    struct timeval timeout = {0};

    while (instance->socket > 0) {
        nfds = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        nfds = maxFd(nfds, instance->socket);
        FD_SET(instance->socket, &rfds);

        listRewind(instance->list);
        while (listValid(instance->list)) {
            DEBUG("Client loop\n");
            client = (clientObject *)listCurrent(instance->list);
            if (client->socket > 0) {
                nfds = maxFd(nfds, client->socket);
                FD_SET(client->socket, &rfds);
                FD_SET(client->socket, &wfds);
            }

            if (listValid(instance->list)) {
                listNext(instance->list);
            }
        }

        int r = select(nfds + 1, &rfds, &wfds, NULL, &timeout);
        if (r < 0) {
            // erro!
            DEBUG("Select returned an error: %s\n", strerror(errno));
        }

        if (FD_ISSET(instance->socket, &rfds)) {
            // server has a pending connection
            DEBUG("Client connection pending!\n");
            socklen_t addrlen;
            struct sockaddr_in cliAddr;
            addrlen = sizeof(cliAddr);
            memset(&cliAddr, 0, sizeof(cliAddr));

            int sock = accept(instance->socket, (struct sockaddr *)&cliAddr, &addrlen);
            DEBUG("Client socket fd: %d\n", sock);
            if (sock < 0) {
                //
            }

            int flags = fcntl(sock, F_GETFL);
            if (flags < 0) {
                DEBUG("Error reading flags\n");
                close(sock);

                return errno;
            }

            if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
                DEBUG("Error setting nonblock\n");
                close(sock);

                return errno;
            }

            clientObject *client = (clientObject *)malloc(sizeof(clientObject));
            memset(client, 0, sizeof(clientObject));
            client->socket = sock;
            client->reqLen = 1024;
            client->reqPos = 0;
            client->rspLen = 1024;
            client->rspPos = 0;
            memcpy(&client->sockaddr, &cliAddr, addrlen);
            client->id = listAdd(instance->list, (void *)client);
        }

        // DEBUG("Clients to check: %d\n", listCount(instance->list));

        listRewind(instance->list);
        while (listValid(instance->list)) {
            client = (clientObject *)listCurrent(instance->list);
            DEBUG("Client ID: %d\n", client->id);
            if (client->socket == 0) {
                if (listValid(instance->list)) {
                    listNext(instance->list);
                }

                continue;
            }

            if (FD_ISSET(client->socket, &rfds)) {
                // read client request
                DEBUG("Client socket can be read from\n");
                client->request = (char *)malloc(sizeof(char) * client->reqLen);
                memset(client->request, 0, client->reqLen);
                ssize_t nbytes;
                do {
                    nbytes = read(
                        client->socket, 
                        client->request + client->reqPos, 
                        client->reqLen - client->reqPos
                    );
                    client->reqPos += nbytes;
                    DEBUG("Client read: %zd bytes (pos: %zu)\n", nbytes, client->reqPos);
                } while (client->reqLen > client->reqPos && nbytes > 0);

                if (nbytes == -1) {
                    // deu ruim
                }

                // At this point is safe to process our request and free its buffer
                DEBUG("%s\n", client->request);
            }

            if (FD_ISSET(client->socket, &wfds)) {
                // write client response
                DEBUG("Client socket can be written to\n");
                client->response = (char *)malloc(sizeof(char) * client->rspLen);
                memset(client->response, 0, client->rspLen);
                sprintf(
                    client->response, 
                    "HTTP/1.1 200 OK\r\nHost: 127.0.0.1:8081\r\nContent-Type: text/plain\r\nContent-Length: 20\r\n\r\nOlar, turupem? %d\n",
                    client->id
                );
                ssize_t nbytes;
                do {
                    nbytes = write(
                        client->socket,
                        client->response + client->rspPos,
                        client->rspLen - client->rspPos
                    );
                    client->rspPos += nbytes;
                    DEBUG("Client write: %zd bytes (pos: %zu)\n", nbytes, client->rspPos);
                } while (client->rspLen > client->rspPos && nbytes > 0);

                if (client->keepAlive == 0) {
                    listDel(instance->list, client->id);
                    free(client->request);
                    free(client->response);
                    close(client->socket);
                    free(client);

                    DEBUG("Bye bye client\n");
                }
            }

            if (listValid(instance->list)) {
                listNext(instance->list);
            }
        }
    }

    return 0;
}

/**
 * @link https://www.man7.org/linux/man-pages/man2/close.2.html
 */
void serverStop(serverObject *instance) {
    DEBUG("serverStop()\n");

    listRewind(instance->list);
    while (listValid(instance->list)) {
        clientObject *client = (clientObject *)listCurrent(instance->list);
        listDel(instance->list, client->id);
        free(client->request);
        free(client->response);
        close(client->socket);
        free(client);
        if (listValid(instance->list)) {
            listNext(instance->list);
        }
    }

    if (instance->socket > 0) {
        close(instance->socket);
        instance->socket = 0;
    }
}
