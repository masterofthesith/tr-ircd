#TR-IRCD
------

## Short installation information, for advanced users.

Use the following commands:

`./configure --prefix=/destination/directory`

`make install`

See the INSTALL document for info on configuring and compiling
TR-IRCD.

## Short uninstallation information, for advanced users.

Use the following command:

`make uninstall`

## Following packages are definitely needed when compiling 
TR-IRCD:

- GNU Make Utility: gmake
- GNU Bison 
- GNU flex or another lexer.
- Optional OpenSSL
- An operating environment supporting posix threads

## Please have a look at the example.conf file, especially when you want to activate features like the proxy scanner or the web server.

The TR-IRCD IRC Software version 5.9.5 introduces a new ability. It is able to do proxy scanning on every incoming connection. (Except server connections)

This feature has been realised as a module, which can be loaded and unloaded when the server is running. The appropriate module name is proxymon.so

This module reads out its specific configuration from the  config file defined in the ircd.conf, proxy_monitor_options section.

If not enabled through the ircd.conf, you can then use

`/quote set proxymon on|off to enable or disable it.`

Proxymon is based on libopm, which you have to install extra. Read the information in the INSTALL file for detailed configuration information.

After proxymon is running, it will do scans on every connecting user, and do an action the way you have provided with the action directive of its configuration file.

Moreover, detailed information of any scan connection is displayed in the &PROXYMON channel, if you have negfail_notices enabled.

The module additionally provides the

`/proxychk <check|stats> <ip> <type> <port>`

command, using which you can get statistics via /proxychk stats or do scans on ip addresses you wish. You can also provide the command with an additional proxy type and port, if you want.

`/proxychk check 127.0.0.1 HTTP 8080`

For Example.

The available proxy types are:
HTTP
HTTPPOST
WINGATE
SOCKS4
SOCKS5
ROUTER

TR-IRCD now includes a small web server which can be configured to help administrators to administer the ircd via a web interface.

This small feature can be started via a module, named httpd.so within an irc session. Its as well as possible to run the ircd in webconfig only mode via

`./ircd --webconfig-only --webconfig-bind 127.0.0.1 --webconfig-port 8081`

The interface allows you to change existing settings, add new configuration blocks, save the existing setting, and also rehash or restart the ircd.

This small webserver does not serve existing html documents. All of them are generated internally. 

You are encouraged to use ssl, by creating a signed certificate. The httpd can also be used to support ssl connections.

The web server can be used to save your complete configuration down, including any loaded maskfiles, or modules-that-are-loaded-via-conf.

Installation information for trircd-5.9.5-enhanced IrcServer

Many things have changed since trircd-4 was released. Now it is no more necessary to recompile a whole server when changing OperServ Name for example.

Such information is no longer stored in config.h or options.h It is available in the ircd.conf server config file.

The file options.h has become obsolete, so is the ./config script that created it.

The precompilation configuration is now done via the configure script in the root of the source directory.

Following options can be of interest when compiling ircd.

`--prefix=/path/to/install/location`

This line will cause the server be installed to the given location.

`--disable-shared-modules `

This line will cause the shared library support in the server be disabled.

`--enable[disable]-openssl`

This will enable/disable usage of openssl to create keys for encrypted links.

`--with-maxclients=2048`

This line will set the default value for maximum number of clients to 2048. It can be reset by the ircd.conf

`--enable-network-loop={POLL,SELECT,SIGIO,KQUEUE,DEVPOLL}`

Will offer alternatives to poll() in the network code. Especially, FreeBSD users should use --enable-kqueue. Solaris also does fine with --enable-devpoll.

`--with-fakehost-postfix=COM`

This will allow you to set another top level domain postfix when creating fake hostnames, other than TR in network.BB54035602.TR -> network.BB54035602.COM

`--with-opm-libdir`
`--with-opm-includedir`

When libopm is detected, the ircd will compile the proxymon module based on LIBOPM, written and copyright by Erik Fears. This will do proxy scans of type http/httppost/socks as well as cisco router and wingate. For this to work you have to install the libopm, available from

`cvs -d:pserver:anon@cvs.blitzed.org:/data/cvs co libopm`

The normal installation of libopm will place the libraries to /usr/local/libopm/lib But you may choose any other location. The ircd configure script defaultly assumes the opm libraries be  in the above path, and the opm includes be under the /usr/local/libopm/include directory. If you have installed them onto another location, use the other two options to tell configure them.

After that you type

`make install`

To install the ircd. In the given location, following directories are created

etc
lib
bin
man
share
var

In share/ there are configuration files like the example.conf which should be renamed to ircd.conf after checking.

The ircd.conf should then be manually copied into etc/  directory. If you use the open proxy monitor, please tell its location to the ircd within the ircd.conf An example has been provided under share/example-socks.conf The new maskfile for klines, quarantines has also an example configuration under share/example-maskfile.conf.

In lib/ are the modules, if the ircd is compiled with module support.

In bin/ is the ircd, md5sum and ircpasswd executables. Additionally converter.sh, a script to convert older ircd configuration files to the new format has been added.

In var/ are the pidfile and possible logfiles, if debug is active.

In share/ are the documents and content of the HELP command.

In man/ are the manpages regarding the ircd binary and  ircd.conf file.

The compilation requires the GNU Make utility. It will fail otherwise.


*************************************************************

To Uninstall TR-IRCD, you can use

`make uninstall`
or
`make uninstall_all`

These will not remove your logfiles or configuration files.

*************************************************************

TR-IRCD Coding Team (c) 2023
