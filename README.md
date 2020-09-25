# *** This project is no longer maintained, work on a newer GUI is active at https://github.com/Sinodun/stubby_manager ***

# Stubby Manager GUI

A Prototype GUI to aid using Stubby on macOS.

Developed by Sinodun IT.

## Installation

* This code assumes that getdns and stubby have already been built and installed.
* Also that the file stubby-ui-helper from the stubby build output is manually 
installed in /usr/local/sbin and has setUID privileges.
* Additionally, a plist called org.getdns.stubby.plist for the stubby service 
must be installed in /Library/LaunchDaemons (for example plist content, see
https://github.com/getdnsapi/stubby/macos/org.getdns.stubbydev.plist)

As part of installation, authorization rights for Stubby actions must also be
installed. Example plist files are also available in
https://github.com/getdnsapi/stubby/macos/

Run the following from the command line:

```
$ security authorizationdb write net.getdnsapi.stubby.daemon.run < rights.daemon.run.plist
$ security authorizationdb write net.getdnsapi.stubby.dns.local < rights.dns.local.plist
```

## Icons

To generate a new set of icons (Stubby.icns) from a single image

* cd into the Icons directory
* update the Icon1024.png file with the new image
* run `source create_icns.src`


## Contributors

* Georgina Hawes
* Sara Dickinson
* John Dickinson
* Jim Hague
* Molly Carton
