#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

//
//  Get the (full) hostname of this host.
//
char *get_hostname()
{
    /**
     * Get the short version.
     */
    char res[1024];
    res[sizeof(res)-1] = '\0';
    gethostname(res, sizeof(res)-1);

    /**
     * Attempt to get the full vrsion.
     */
    struct hostent *hstnm;
    hstnm = gethostbyname (res);
    if (hstnm)
    {
        return(strdup(hstnm->h_name));
    }

    if ( getenv( "HOSTNAME" ) != NULL )
    {
        return(strdup(getenv("HOSTNAME" ) ));
    }

    printf("Failed to find hostname\n");
    exit(1);
}


int main (const int argc, const char **argv)
{
    //
    //  Get the name of the queue
    //
    if ( argc != 2 )
    {
        printf( "Usage: %s queue-specification\n", argv[0] );
        printf("   e.g. %s tcp://master.example.com:444\n", argv[0] );
        exit(1);
    }

    //
    //  Create a socket.
    //
    int sock = nn_socket (AF_SP, NN_SUB);
    assert (sock >= 0);

    //
    //  Get our hostname
    //
    const char *h = get_hostname();

    //
    //  Filter so that we ignore messages not intended for us.
    //
    printf("Filtering on hostname: %s\n", h);
    assert (nn_setsockopt (sock, NN_SUB, NN_SUB_SUBSCRIBE, h, strlen(h) ) >= 0);
    assert( nn_connect(sock,argv[1]));

    while (1)
    {

        char *buf = NULL;
        int bytes = nn_recv (sock, &buf, NN_MSG, 0);
        assert (bytes >= 0);
        printf ("RECEIVED %s\n", buf);

        //
        //  The message will be - "hostname:cmd"
        //
        //  Get the command and execute it.
        //
        char *cmd = buf + strlen(h) + 1;
        printf("CMD: '%s'\n", cmd );
        system( cmd );

        nn_freemsg (buf);
    }
    return nn_shutdown (sock, 0);
}
