# sshlirp: create an instant VPN using VDE and slirp

`sshlirp` creates an "instant VPN". It converts a text based shell
connection (e.g. ssh[1]) into a VDE virtual private network.
Neither root access nor configuration is required on the remote system.

`sshlirp` is a dynamically linked executable while `sshlirp-`*architecure*
(e.g. `sshlirp-x86_64`, `sshlirp-mips`, `sshlirp-riscv64`) are statically linked
executable, ready to be copied on remote systems to achieve a high compatibiity
on software architectures (kernel and lib versions, distributions etc).

Slirp was created long time ago (1995) by Danny Gasparovski
to emulate a PPP, SLIP, or CSLIP connection to the Internet
using a text-based shell account allowing a free upgrade to full TCP-IP
access to the Internet for subscribers of remote terminal services only.

`sshlirp` uses the same approach to promote a ssh text based remote connection to a
Virtual Private Network endpoint.

## Install

Prerequisites: glibc, libglib2.0 and libslirp static libraries.
(glibc, libglib2.0 static libraries are already included in Debian packages
 libc6-dev and libglib2.0-dev respectively).

Current Debian package for libslirp does not include the static library.
The static library can be generated from its sources as follows:

```
$ git clone https://gitlab.freedesktop.org/slirp/libslirp.git
$ cd libslirp
$ meson build . --default-library=both
$ ninja -C build
```

To compile sshlirp, get the source code and from the root of the source tree run:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

# EXAMPLE

Scenario: a user has an account on a remote linux host providing ssh access.
Let us define the name of that host as `remote.mydomain.org`.
Let us also assume that the user has configured a key pair authentication on
`remote.mydomain.org` as described in ssh-keygen[1].

The VPN can be acrivated in three simple steps:

* copy the static executable on the remote host:
```
$ cp sshlirp-x86_64 remote.mydomain.org:/tmp
```

* start a vdens namespace connected to the remote vpn server
```
$ vdens -R 9.9.9.9 cmd://"ssh remote.mydomain.org /tmp/sshlirp-x86_64"
```

* configure the ip address and route of the vdens, e.g. using udhcpc:
```
$ /sbin/udhcpc -i vde0
```

Now all the programs running in the vdens use the vpn and appear on the Internet
as if they were running on `remote.mydomain.org`.
