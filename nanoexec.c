#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>




//
// Change to given user
//
void chuser(const char *username )
{
    struct passwd *pw;
    int rv;

    pw = getpwnam(username);
    if (!pw)
    {
        fprintf(stderr, "Failed to find user %s\n", username );
        exit(1);
    }

    if (initgroups(pw->pw_name, pw->pw_gid) != 0 ||
        setgid(pw->pw_gid) != 0 || setuid(pw->pw_uid) != 0)
    {
        fprintf(stderr, "Couldn't change to '%.32s' uid=%lu gid=%lu\n",
                username,
                (unsigned long)pw->pw_uid,
                (unsigned long)pw->pw_gid
            );
        exit(1);
    }
}



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



//
//  Entry point to our code
//
int main (int argc, char *argv[])
{
    //
    //  Options parsing
    //
    int c;

    while (1)
    {
        static struct option long_options[] =
            {
                {"user", required_argument, 0, 'u'},
                {0, 0, 0, 0}
            };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long(argc, argv, "u:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 'u':
            chuser( optarg );
            break;

        default:
            exit (1);
        }
    }


    //
    //  Ensure we have the name of the queue on the command-line,
    // after any (optional) arguments.
    //
    if ( optind >= argc )
    {
        printf( "Usage: %s [options] queue-specification\n", argv[0] );
        printf("   e.g. %s --user nobody tcp://master.example.com:444\n", argv[0] );
        exit(1);
    }

    //
    //  Create the socket.
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
    //  We accept messages prefixed with our hostname, as returned
    // by the `get_hostname()` function, or the magic string "ALL".
    //
    //
    printf("Filtering on hostname: %s\n", h);
    assert (nn_setsockopt (sock, NN_SUB, NN_SUB_SUBSCRIBE, h, strlen(h) ) >= 0);
    assert (nn_setsockopt (sock, NN_SUB, NN_SUB_SUBSCRIBE, "ALL", 3 ) >= 0);
    assert( nn_connect(sock,argv[optind]));


    //
    //  Wait for messages, forever.
    //
    while (1)
    {
        //
        //  Receive a single message.
        //
        char *buf = NULL;
        int bytes = nn_recv (sock, &buf, NN_MSG, 0);
        assert (bytes >= 0);
        printf ("RECEIVED %s\n", buf);

        //
        //  The message will be ":"-deliminated.
        //
        //
        //  Find the first part after that ":" and execute it.
        //
        char *cmd = strchr(buf, ':' );
        if ( cmd != NULL )
        {
            printf("CMD: '%s'\n", cmd+1 );
            system( cmd+1 );
        }

        //
        //  Free the received string
        //
        nn_freemsg (buf);
    }

    //
    //  Not reached.
    //
    return nn_shutdown (sock, 0);
}
