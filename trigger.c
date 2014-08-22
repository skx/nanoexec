#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>



void send_command(const char *host, const char *command)
{

    //
    //  Broadcast on the localhost - This is sufficient, oddly enough.
    //
    const char *url = "tcp://*:4444";

    int sock = nn_socket(AF_SP, NN_PUB);
    assert(sock >= 0);
    assert(nn_bind(sock, url));

    //
    //  This is icky.
    //
    nn_sleep(100);

    //
    //  Build up the buffer to send
    //
    char *buf = (char *) malloc(strlen(host) + strlen(command) + 2);
    if (buf == NULL)
    {
        printf("Failed to allocate memory for command\n");
        exit(1);
    }


    sprintf(buf, "%s:%s", host, command);
    int bytes = nn_send(sock, buf, strlen(buf) + 1, 0);
    if (bytes != strlen(buf) + 1)
    {
        fprintf(stderr, "WARNING: Short send\n");
    }

    free(buf);

    nn_shutdown(sock, 0);
}


int main(int argc, char **argv)
{
    /**
     * Host + Command to issue.
     */
    char *command = NULL;
    char *host = NULL;

    int c;

    while (1)
    {
        static struct option long_options[] = {
            {"host", required_argument, 0, 'h'},
            {"command", required_argument, 0, 'c'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "h:c:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 'c':
            if (command)
                free(command);
            command = strdup(optarg);
            break;
        case 'h':
            if (host)
                free(host);
            host = strdup(optarg);
            break;

        default:
            exit(1);
        }
    }


    if (host && command)
    {
        send_command(host, command);
    } else
    {
        printf("Usage: %s --host [hostname|ALL] --command \"some command\"\n", argv[0]);
    }

    return 0;
}
