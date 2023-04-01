#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include "fifo_seqnum.h"

/*
    This program file the client A. Initially, they will use
    FIFOs to communicate with the server.
*/

static char clientFifo[CLIENT_FIFO_NAME_LEN]; //We create a string with the size of the client name

static void removeFifo(void){
    unlink(clientFifo);
}

int main(int argc, char **argv){
    int server_fd, client_fd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);
        /* printf("%s [seq-len...]\n", argv[0]); */

    /* Create client's FIFO for server answers (before sending request, to avoid a race) */
    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());
    if(mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1){
            errExit("mkfifo %s", clientFifo);
            /* printf("mkfifo %s", SERVER_FIFO); */
        }
    if (atexit(removeFifo) != 0){
        errExit("atexit");
        /* printf("atexit"); */
    }

    /* Construct the request message, open server FIFO, and send request. */
    req.pid = getpid();
    req.seqLen = (argc > 1)? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1)
        errExit("open %s", SERVER_FIFO);
        /* printf("open %s", SERVER_FIFO); */

    if (write(server_fd, &req, sizeof(struct request)) != sizeof(struct request))
        fatal("Can't write to server");
        /* printf("Can't write to server"); */

    /* Open client's FIFO, read and display response */

    client_fd = open(clientFifo, O_RDONLY);
    if (client_fd == -1)
        errExit("open %s", clientFifo);
        /* printf("open %s", clientFifo); */

    /* The server response will be stored in &resp */
    if (read(client_fd, &resp, sizeof(struct response))
            != sizeof(struct response))
        fatal("Can't read response from server");
        /* printf("Can't read response from server"); */

    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}