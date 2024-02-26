<!--
.\" Copyright (C) 2024 VirtualSquare. Project Leader: Renzo Davoli
.\"
.\" This is free documentation; you can redistribute it and/or
.\" modify it under the terms of the GNU General Public License,
.\" as published by the Free Software Foundation, either version 2
.\" of the License, or (at your option) any later version.
.\"
.\" The GNU General Public License's references to "object code"
.\" and "executables" are to be interpreted as the output of any
.\" document formatting or typesetting system, including
.\" intermediate and printed output.
.\"
.\" This manual is distributed in the hope that it will be useful,
.\" but WITHOUT ANY WARRANTY; without even the implied warranty of
.\" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
.\" GNU General Public License for more details.
.\"
.\" You should have received a copy of the GNU General Public
.\" License along with this manual; if not, write to the Free
.\" Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
.\" MA 02110-1301 USA.
.\"
-->
# NAME

sshlirp -- instant vpn

# SYNOPSIS

`sshlirp` [*options*]

`sshlirp-`*architecure* [*options*]

# DESCRIPTION

`sshlirp` creates an "instant vpn". It converts a text based shell
connection (e.g. ssh[1]) into a VDE virtual private network.

`sshlirp` is a dynamically linked executable while `sshlirp-`*architecure*
(e.g. `sshlirp-x86_64`, `sshlirp-mips`, `sshlirp-riscv64`) are statically linked
executable, ready to be copied on remote systems to achieve a high compatibiity
on software architectures (kernel and lib versions, distributions etc).

See also the EXAMPLE section below.

# OPTIONS

  `-v`, `--verbose`
: enable verbose mode

  `-V`, `--version`
: show version number

  `-f` *PATHNAME*, `--rc` *PATHNAME*
: load configuration file

  `-p`, `--printconf`
: print configuration file

  `--restricted`
: Prevent the guest from accessing the Internet

  `-4`, `--ipv4`
: Enable IPv4 only

  `-6`, `--ipv6`
: Enable IPv6 only

  `--host4` *IPV4ADDR/prefix*
: Virtual address for the host exposed to the guest

  `--host6` *IPV6ADDR/prefix*
: Virtual address for the host exposed to the guest

  `--hostname` *HOSTNAME*
: Hostname exposed to the guest in DHCP hostname option

  `--tftp_server_name` *HOSTNAME*
: Hostname exposed to the guest in the DHCP TFTP server name option

  `--tftp_path` *PATHNAME*
: Path of the files served by TFTP

  `--bootfile` *PATHNAME*
: Boot file name exposed to the guest via DHCP

  `--dhcp` *IPV4ADDR*
: Start of the DHCP range

  `--vnameserver` *IPV4ADDR*
: Virtual address for the DNS server exposed to the guest

  `--vnameserver6` *IPV6ADDR*
: Virtual address for the DNS server exposed to the guest

  `--vdnssearch` *IPV4ADDR[,IPV4ADDR]...*
: DNS search names exposed to the guest via DHCP

  `--vdomainname` *DOMAINNAME*
: Domain name exposed to the guest via DHCP

  `--mtu` *MTU*
: MTU when sending packets to the guest (Default: `IF_MTU_DEFAULT`)

  `--mru` *MRU*
: MTU when receiving packets from the guest (Default: `IF_MRU_DEFAULT`)

  ` --disable_host_loopback`
: Prohibit connecting to 127.0.0.1

  `--disable_dns`
: slirp will not redirect/serve any DNS packet

  `--disable_dhcp`
: slirp will not reply to any DHCP requests

  `--tcpfwd` *[hostIP:]hostport:guestIP:guestport*
: forward a TCP port (multi tcpfwd allowed)

  `--udpfwd` *[hostIP:]hostport:guestIP:guestport*
: forward a UDP port (multi udpfwd allowed)

  `--unixfwd` *[slirpIP:]slirpport:path*
: forward a TCP port to a UNIX socket (multi unixfwd allowed)

  `--cmdfwd` *[slirpIP:]slirpport:cmd*
: forward a TCP port to an external command (multi cmdfwd allowed)

  `-h`, `--help`
: Display a short help message and exit.

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

# SEE ALSO
vdens(1), vde\_plug(1), ssh(1), ssh-keygen(1)

# AUTHOR
VirtualSquare. Project leader: Renzo Davoli.

