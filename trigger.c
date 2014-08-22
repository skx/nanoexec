#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>


int main (const int argc, const char **argv)
{

    if ( argc > 1 )
    {
        //
        //  Broadcast on the localhost - This is sufficient, oddly enough.
        //
        const char *url = "tcp://*:4444";

        int sock = nn_socket (AF_SP, NN_PUB);
        assert (sock >= 0);
        assert(nn_bind (sock, url) );

        //
        //  This is icky.
        //
        nn_sleep (100);

        int i;
        for ( i = 1; i < argc; i++ )
        {
            printf ("Sending message: '%s'\n", argv[i]);
            int bytes = nn_send (sock, argv[i], strlen(argv[i])+1, 0);
            assert (bytes == strlen(argv[i])+1);
        }

        return nn_shutdown (sock, 0);
    }
    else
    {
        printf("Usage: %s hostname:command\n", argv[0]);
        exit(1);
    }

}
