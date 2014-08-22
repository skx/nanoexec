nanoexec - nanomsg based command execution
------------------------------------------

This repository contains a simple client/server "thing" which allows
for the execution of remote commands via a shared nanonmsg queue.

In brief:

* There is a central host which issues commands.
* There are an arbitrary number of listeners, which subscribe to messages from that master host.
   * When a message is seen for the current host it will be executed.

How is this useful?  Well it could allow client-side operations to be remotely
initiated by a central host.

For example I have a sysadmin tool called Slaughter, which is something like cfengine, but without the client-server part.  Each host must have a crontab entry installed to run the tool once an hour or so.

If _this_ system were installed on all managed nodes the central hub could instruct them to trigger immediately



Setup
-----

One host will be called "master.example.com", that is the central host.

All other hosts will connect to this host, and await the arrival of messages.
When a message is received it will be executed via `system()`.

On each host compile and execute `nanoexec`, specifying the details of
the master.  For example:

    # ./nanoexec tcp://master.example.com:4444
    Filtering on hostname: host1.example.com

On the master host you can now inject commands to _any_ of the listening
hosts:

    # ./trigger host1.example.com:uptime



Security
--------

You'll want to open the port which is used for communication, `4444` in the
example(s) above.

You'll want to open that port only to managed nodes, since there is an
obvious security risk.



Building
--------

There is a debian package of the nanomsg library, it is not available for
Wheezy but backports cleanly:

* https://packages.debian.org/libnanomsg0


Steve
--
