#ifndef _AUTOOPT_H
#define _AUTOOPT_H

#define AUTOOPTS \
_CLIOPT_(h, help, noarg, "", "print this help message and exit") \
_CLIOPT_(v, verbose, noarg, "", "enable verbose mode") \
_CLIOPT_(V, version, noarg, "", "show version number") \
_CLIOPT_(f, rc, arg, "PATHNAME", "load configuration file") \
_CLIOPT_(p, printconf, noarg, "", "print configuration file") \
_OPT_( , restricted, noarg, "", "Prevent the guest from accessing the Internet") \
_OPT_(4, ipv4, noarg, "", "Enable IPv4 only") \
_OPT_(6, ipv6, noarg, "", "Enable IPv6 only") \
_OPT_( , host4, arg, "IPV4ADDR/prefix", "Virtual address for the host exposed to the guest") \
_OPT_( , host6, arg, "IPV6ADDR/prefix", "Virtual address for the host exposed to the guest") \
_OPT_( , hostname, arg, "HOSTNAME", "Hostname exposed to the guest in DHCP hostname option") \
_OPT_( , tftp_server_name, arg, "HOSTNAME", "Hostname exposed to the guest in the DHCP TFTP server name option") \
_OPT_( , tftp_path, arg, "PATHNAME", "Path of the files served by TFTP") \
_OPT_( , bootfile, arg, "PATHNAME", "Boot file name exposed to the guest via DHCP") \
_OPT_( , dhcp, arg, "IPV4ADDR", "Start of the DHCP range") \
_OPT_( , vnameserver, arg, "IPV4ADDR", "Virtual address for the DNS server exposed to the guest") \
_OPT_( , vnameserver6, arg, "IPV6ADDR", "Virtual address for the DNS server exposed to the guest") \
_OPT_( , vdnssearch, arg, "IPV4ADDR[,IPV4ADDR]...", "DNS search names exposed to the guest via DHCP") \
_OPT_( , vdomainname, arg, "DOMAINNAME", "Domain name exposed to the guest via DHCP") \
_OPT_( , mtu, arg, "INT", "MTU when sending packets to the guest (Default: IF_MTU_DEFAULT)") \
_OPT_( , mru, arg, "INT", "MTU when receiving packets from the guest (Default: IF_MRU_DEFAULT)") \
_OPT_( , disable_host_loopback, noarg, "", "Prohibit connecting to 127.0.0.1") \
_OPT_( , disable_dns, noarg, "", "slirp will not redirect/serve any DNS packet") \
_OPT_( , disable_dhcp, noarg, "", "slirp will not reply to any DHCP requests") \
_OPT_( , tcpfwd, multiarg, "[hostIP:]hostport:guestIP:guestport", "forward a TCP port (multi tcpfwd allowed)") \
_OPT_( , udpfwd, multiarg, "[hostIP:]hostport:guestIP:guestport", "forward a UDP port (multi udpfwd allowed)") \
_OPT_( , unixfwd, multiarg, "[slirpIP:]slirpport:path", "forward a TCP port to a UNIX socket (multi unixfwd allowed)") \
_OPT_( , cmdfwd, multiarg, "[slirpIP:]slirpport:cmd", "forward a TCP port to an external command (multi cmdfwd allowed)") 

#define AUTOPOSOPTS

#define _OPT_(ch, opt, arg, __, ___) OPT_ ## opt,
#define _CLIOPT_ _OPT_
#define _RCOPT_ _OPT_
#define _POSOPT_ _OPT_
enum options_enum {
	AUTOOPTS
		OPT_NUMOPTS };
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_

#define AUTOOPT(x) OPT_ ## x
#define autoopt(name) _autoopt(AUTOOPT(name))
#define ismulti(name) _ismulti(AUTOOPT(name))

char *_autoopt(int index);
int _ismulti(int index);

int getautoopt(int argc, char *argv[]);
int getautoposopt(int argc, char *argv[]);
int readrc(char *path);
void printconf(void);
void usage(void);

#endif
