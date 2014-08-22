nanoexec - nanomsg based command execution
------------------------------------------

This repository contains a simple client/server "thing" which allows
for the execution of remote commands via a shared [nanomsg](http://nanomsg.org/) queue.

In brief:

* There is a central host which creates a queue through which commands can be broadcast.
* There are an arbitrary number of listeners, which subscribe to the master-queue.
   * When a message is seen for the current host it will be executed.

How is this useful?  Well it could allow client-side operations to be remotely
initiated by a central host.

For example I have a sysadmin tool called [Slaughter](http://www.steve.org.uk/Software/slaughter/), which is something like cfengine, but without the client-server part.  Each host must have a crontab entry installed to run the tool once an hour or so.

If _this_ system were installed on all managed nodes then the central hub could instruct them to trigger execution immediately.



Setup
-----

One host will be called "master.example.com", that is the central host.

All other hosts will connect to this host, and await the arrival of messages.
When a message is received it will be executed via `system()`.

On each host you wish to be able to receive/execute commands you should
compile and execute `nanoexec`, specifying the details of the master.

For example:

    # ./nanoexec tcp://master.example.com:4444
    Filtering on hostname: host1.example.com

On the master host itself you can now inject commands to _any_ of the listening
hosts:

    # ./trigger --host=host1.example.com --command=uptime

If you want to mass-broadcast you could also use the magic string "`ALL`":

    # ./trigger --host=ALL --command=uptime



Security
--------

The agents that listen for messages will connect to the master host,
and will not themselves listen upon the network.

The master node will have to have a port open for communiction, `4444`
in the examples above.   To restrict the attack surface you'll almost
certainly want to open that port only to hosts that you expect to connect
to it.

**NOTE** If you choose a high-port for the connection then it is not
necessary to run either the listener or the injector as root.

If you launch `nanoexec` as root you can configure it to drop permissions to
the given user via the `--user` argument, for example:

    # ./nanoexec --user=nobody tcp://master.example.com:4444


Building
--------

There is a debian package of the [nanomsg library](https://packages.debian.org/libnanomsg0), it is not available for Wheezy but backports cleanly.

I've produced a backported library, and a binary release of this repository
which you can find here:

* http://packages.steve.org.uk/nanoexec/




Steve
--
