#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "fifo_seqnum.h"

int main(){

    int server_fd, dummy_fd, client_fd;
    char clientFIFO[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;

    umask(0); // No permissions should be masked
    /* Creates the server's well-known FIFO and opens it for reading */
    /*  S_IRUSR: read permission for the owner (user)
        S_IWUSR: write permission for the owner (user)
        S_IWGRP: write permission for the group.
        ! EERNO is an error that indicates that the named piped already exists.
    */
        if(mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1){
            errExit("mkfifo %s", SERVER_FIFO);
            /* printf("mkfifo %s", SERVER_FIFO); */
        }

        server_fd = open(SERVER_FIFO, O_RDONLY);
        if (server_fd < 0)
            errExit("open %s", SERVER_FIFO);
           /*  printf("open %s", SERVER_FIFO); */

        /* The dummy_fd is an extra write descriptor that prevents EOF from showing up */
        dummy_fd = open(SERVER_FIFO, O_WRONLY);
        if (dummy_fd < 0)
            errExit("open %s", SERVER_FIFO);
            /* printf("open %s", SERVER_FIFO); */

        /* We ignore the SIPIPIE signal so that if the server attempts to write to a client
        FIFO that doesn't have a reader, then, rather than being sent a SIGPIPE signal it
        receives an EPIPE error from the write() system call. Then the process won't be killed
        by the SIGPIPE signal. */
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            errExit("signal");
            /* printf("signal"); */

        while(1){ /* Read requests and send responses */
            if(read(server_fd, &req, sizeof(struct request)) != sizeof(struct request)){
                fprintf(stderr, "Error reading request, discarding...\n");
                continue;
            }
            /* Open client FIFO (previously created by the client) */
            snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) req.pid);
            client_fd = open(clientFIFO, O_WRONLY);
            if (client_fd == -1) {
                /* Open failed, give up on client */
                errMsg("open %s", clientFIFO);
                /* printf("open %s", clientFIFO); */
                continue;
            }
            /* Send response and close FIFO */
            resp.seqNum = seqNum;
            if (write(client_fd, &resp, sizeof(struct response))
                != sizeof(struct response)){
                    fprintf(stderr, "Error writing to FIFO %s\n", clientFIFO);
                }
            if (close(client_fd) == -1)
                errMsg("close");
                /* printf("close"); */
            seqNum += req.seqLen; /* Update the sequence number */
        }


    return (EXIT_SUCCESS);
}