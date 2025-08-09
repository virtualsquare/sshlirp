/*
 * sshlirp: instant VPN based on VDE and slirp
 * Copyright(C) 2024  Renzo Davoli, Virtualsquare
 *
 * sshlirp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <libvdeplug.h>
#include <autoopt.h>

#include <libvdeslirp.h>

#define NTOP_BUFSIZE 128
unsigned char buf[10240];

static const char **vdnssearch_copy(char *list) {
	const char **retval = NULL;
	int i, count;
	if (list == NULL)
		return NULL;
	for (i = 0, count = 2; list[i] != 0; i++)
		count += (list[i] == ',');
	retval = malloc(count * sizeof(char *));
	if (retval != NULL) {
		const char **scan = retval;;
		char *toktmp, *item;
		for (; (item = strtok_r(list, ",", &toktmp)) != NULL; list = NULL)
			*scan++ = strdup(item);
		*scan = NULL;
	}
	return retval;
}

static void vdnssearch_free(const char **argv) {
	const char **scan;
	for (scan = argv; *scan; scan++)
		free((void *) *scan);
	free(argv);
}

static void verbose_configuration(SlirpConfig *cfg) {
	char buf[NTOP_BUFSIZE];
	fprintf(stderr, "SLIRP configuration\n");
	fprintf(stderr, "version       %d\n", cfg->version);
	fprintf(stderr, "ipv4-enable   %d\n", cfg->in_enabled);
	fprintf(stderr, "ipv4-network  %s\n", inet_ntop(AF_INET, &cfg->vnetwork, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv4-netmask  %s\n", inet_ntop(AF_INET, &cfg->vnetmask, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv4-host     %s\n", inet_ntop(AF_INET, &cfg->vhost, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv6-enabled  %d\n", cfg->in6_enabled);
	fprintf(stderr, "ipv6-prefix   %s\n", inet_ntop(AF_INET6, &cfg->vprefix_addr6, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv6-preflen  %d\n", cfg->vprefix_len);
	fprintf(stderr, "ipv6-host     %s\n", inet_ntop(AF_INET6, &cfg->vhost6, buf, NTOP_BUFSIZE));
	fprintf(stderr, "hostname      %s\n", cfg->vhostname);
	fprintf(stderr, "tftp-servname %s\n", cfg->tftp_server_name);
	fprintf(stderr, "tftp-path     %s\n", cfg->tftp_path);
	fprintf(stderr, "bootfile      %s\n", cfg->bootfile);
	fprintf(stderr, "dhcp-start    %s\n", inet_ntop(AF_INET, &cfg->vdhcp_start, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv4-vDNS     %s\n", inet_ntop(AF_INET, &cfg->vnameserver, buf, NTOP_BUFSIZE));
	fprintf(stderr, "ipv6-vDNS     %s\n", inet_ntop(AF_INET6, &cfg->vnameserver6, buf, NTOP_BUFSIZE));
	fprintf(stderr, "vDNS-search   ");
	if (cfg->vdnssearch) {
		const char **scan;
		for (scan = cfg->vdnssearch; *scan; scan++)
			fprintf(stderr, "%s ", *scan);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "vdomainname   %s\n", cfg->vdomainname);
	fprintf(stderr, "MTU(0=def)    %zd\n", cfg->if_mtu);
	fprintf(stderr, "MRU(0=def)    %zd\n", cfg->if_mru);
	fprintf(stderr, "disable-lback %d\n", cfg->disable_host_loopback);
	fprintf(stderr, "enable-emu    %d\n", cfg->enable_emu);
	if (cfg->version >= 3)
		fprintf(stderr, "disable_dns   %d\n", cfg->disable_dns);
	if (cfg->version >= 4)
		fprintf(stderr, "disable_dhcp  %d\n", cfg->disable_dhcp);
}

int setslirpcfg(SlirpConfig *cfg) {
	if (autoopt(restricted)) cfg->restricted = 1;
	if (autoopt(ipv4)) cfg->in6_enabled = 0;
	if (autoopt(ipv6)) cfg->in_enabled = 0;
	if (autoopt(host4)) {
		int prefix = 24;
		char *slash = strchr(autoopt(host4), '/');
		if (slash) {
			prefix = atoi(slash+1);
			*slash = 0;
		}
		inet_pton(AF_INET, autoopt(host4), &(cfg->vhost));
		vdeslirp_setvprefix(cfg, prefix);
	}
	if (autoopt(host6)) {
		int prefix = 64;
		char *slash = strchr(autoopt(host6), '/');
		if (slash) {
			prefix = atoi(slash+1);
			*slash = 0;
		}
		inet_pton(AF_INET6, autoopt(host6), &(cfg->vhost6));
		vdeslirp_setvprefix6(cfg, prefix);
	}
	if (autoopt(hostname)) cfg->vhostname = autoopt(hostname);
	if (autoopt(tftp_server_name)) cfg->tftp_server_name = autoopt(tftp_server_name);
	if (autoopt(tftp_path)) cfg->tftp_path = autoopt(tftp_path);
	if (autoopt(bootfile)) cfg->bootfile = autoopt(bootfile);
	if (autoopt(dhcp)) inet_pton(AF_INET, autoopt(dhcp), &(cfg->vdhcp_start));
	if (autoopt(vnameserver)) inet_pton(AF_INET, autoopt(vnameserver), &(cfg->vnameserver));
	if (autoopt(vnameserver6)) inet_pton(AF_INET6, autoopt(vnameserver6), &(cfg->vnameserver6));
	if (autoopt(vdnssearch)) cfg->vdnssearch = vdnssearch_copy(autoopt(vdnssearch));
	if (autoopt(vdomainname)) cfg->vdomainname = autoopt(vdomainname);
	if (autoopt(mtu)) cfg->if_mtu = atoi(autoopt(mtu));
	if (autoopt(mru)) cfg->if_mru = atoi(autoopt(mru));
	if (autoopt(disable_host_loopback)) cfg->disable_host_loopback = 1;
	if (cfg->version >= 3)
		if (autoopt(disable_dns)) cfg->disable_dns = 1;
	if (cfg->version >= 4)
		if (autoopt(disable_dhcp)) cfg->disable_dhcp = 1;
	if (autoopt(verbose)) verbose_configuration(cfg);
	return 0;
}

/* like strtok_r but this allows empty fields */
static char *strtok_rep_r (char *s, const char *delim, char **save_ptr)
{
	char *end;
	if (s == NULL) s = *save_ptr;
	if (*s == '\0')
		return *save_ptr = s, NULL;
	end = s + strcspn (s, delim);
	if (*end == '\0')
		return *save_ptr = end, s;
	return *end = '\0', *save_ptr = end + 1, s;
}

static void vde_slirp_dofwd(struct vdeslirp *slirp, int is_udp, char *arg, int verbose) {
	for (char *next = arg + strlen(arg) + 1; *arg; arg = next) {
		next = arg + strlen(arg) + 1;
		char *fwditem = arg;
		char *fldtmp;
		char *haddrstr, *hport, *gaddrstr, *gport;
		struct in_addr host_addr, guest_addr;

		haddrstr = strtok_rep_r(fwditem, ":", &fldtmp);
		hport = strtok_rep_r(NULL, ":", &fldtmp);
		gaddrstr = strtok_rep_r(NULL, ":", &fldtmp);
		gport = strtok_rep_r(NULL, "", &fldtmp);
		if (gport == NULL) {
			gport = gaddrstr;
			gaddrstr = hport;
			hport = haddrstr;
			haddrstr = "0.0.0.0";
		}
		if (*haddrstr == 0)
			haddrstr = "0.0.0.0";
		if (inet_pton(AF_INET, haddrstr, &host_addr) == 1 &&
				inet_pton(AF_INET, gaddrstr, &guest_addr) == 1) {
			int retvalue = vdeslirp_add_fwd(slirp, is_udp,
					host_addr, atoi(hport),
					guest_addr, atoi(gport));
			if (verbose) {
				fprintf(stderr, "%sfwd host   %s %d -> guest %s %d: %s\n",
						is_udp ? "udp" : "tcp",
						haddrstr, atoi(hport), gaddrstr, atoi(gport), strerror(retvalue == 0 ? 0 : errno));
			}
		}
	}
}

static void vde_slirp_dounixfwd(struct vdeslirp *slirp, char *arg, int verbose) {
	for (char *next = arg + strlen(arg) + 1; *arg; arg = next) {
		next = arg + strlen(arg) + 1;
		char *fwditem = arg;
		char *fldtmp;
		char *haddrstr, *hport, *path;
		struct in_addr host_addr;
		arg = NULL;

		haddrstr = strtok_rep_r(fwditem, ":", &fldtmp);
		hport = strtok_rep_r(NULL, ":", &fldtmp);
		path = strtok_rep_r(NULL, "", &fldtmp);
		if (path == NULL) {
			path = hport;
			hport = haddrstr;
			haddrstr = "0.0.0.0";
		}
		if (*haddrstr == 0)
			haddrstr = "0.0.0.0";
		if (inet_pton(AF_INET, haddrstr, &host_addr) == 1 &&
				path != 0) {
			int retvalue = vdeslirp_add_unixfwd(slirp, path, &host_addr, atoi(hport));
			if (verbose) {
				char buf[NTOP_BUFSIZE];
				fprintf(stderr, "unixfwd       gw %s %d -> host %s: %s\n",
						inet_ntop(AF_INET, &host_addr, buf, NTOP_BUFSIZE),
						atoi(hport), path, strerror(retvalue == 0 ? 0 : errno));
			}
		}
	}
}

static void vde_slirp_docmdfwd(struct vdeslirp *slirp, char *arg, int verbose) {
	for (char *next = arg + strlen(arg) + 1; *arg; arg = next) {
		next = arg + strlen(arg) + 1;
		char *fwditem = arg;
		char *fldtmp;
		char *haddrstr, *hport, *cmd;
		struct in_addr host_addr;
		arg = NULL;

		haddrstr = strtok_rep_r(fwditem, ":", &fldtmp);
		hport = strtok_rep_r(NULL, ":", &fldtmp);
		cmd = strtok_rep_r(NULL, "", &fldtmp);
		if (cmd == NULL) {
			cmd = hport;
			hport = haddrstr;
			haddrstr = "0.0.0.0";
		}
		if (*haddrstr == 0)
			haddrstr = "0.0.0.0";
		if (inet_pton(AF_INET, haddrstr, &host_addr) == 1 &&
				cmd != 0) {
			int retvalue = vdeslirp_add_cmdexec(slirp, cmd, &host_addr, atoi(hport));
			if (verbose) {
				char buf[NTOP_BUFSIZE];
				fprintf(stderr, "cmdfwd        gw %s %d -> '%s': %s\n",
						inet_ntop(AF_INET, &host_addr, buf, NTOP_BUFSIZE) ,
						atoi(hport), cmd, strerror(retvalue == 0 ? 0 : errno));
			}
		}
	}
}

void setslirp_runtimecfg(struct vdeslirp *slirp, SlirpConfig *cfg) {
	if (autoopt(tcpfwd))
		vde_slirp_dofwd(slirp, 0, autoopt(tcpfwd), !!autoopt(verbose));
	if (autoopt(udpfwd))
		vde_slirp_dofwd(slirp, 1, autoopt(udpfwd), !!autoopt(verbose));
	if (autoopt(unixfwd))
		vde_slirp_dounixfwd(slirp, autoopt(unixfwd), !!autoopt(verbose));
	if (autoopt(cmdfwd))
		vde_slirp_docmdfwd(slirp, autoopt(cmdfwd), !!autoopt(verbose));

	if (cfg->vdnssearch != NULL)
		vdnssearch_free(cfg->vdnssearch);
}

ssize_t sreceive(void *opaque, void *buf, size_t count) {
	return vdeslirp_send(opaque, buf, count);
}

void mainloop(void) {
	SlirpConfig slirpcfg;
	vdeslirp_init(&slirpcfg, VDE_INIT_DEFAULT);
	setslirpcfg(&slirpcfg);
	struct vdeslirp *slirp = vdeslirp_open(&slirpcfg);
	if (slirp == NULL)
		return;
	setslirp_runtimecfg(slirp, &slirpcfg);
	VDESTREAM *vs = vdestream_open(slirp, STDOUT_FILENO,
			sreceive, NULL);
	struct pollfd pfd[] = {{STDIN_FILENO, POLLIN | POLLHUP, 0},
		{vdeslirp_fd(slirp), POLLIN | POLLHUP, 0}};
	while (poll(pfd, 2, -1) > 0) {
		ssize_t n;
		if ((pfd[0].revents | pfd[1].revents) & POLLHUP)
			break;
		if (pfd[0].revents & POLLIN) {
			n = read(STDIN_FILENO, buf, 10240);
			//fprintf(stderr, "-> %zd\n", n);
			if (n <= 0) break;
			vdestream_recv(vs, buf, n);
		}
		if (pfd[1].revents & POLLIN) {
			n = vdeslirp_recv(slirp, buf, 10240);
			//fprintf(stderr, "<- %zd\n", n);
			if (n <= 0) break;
			vdestream_send(vs, buf, n);
		}
	}
	vdeslirp_close(slirp);
}

int main(int argc, char *argv[]) {
	int error = getautoopt(argc,argv);
	if (autoopt(help) || error)
		usage();
	if (autoopt(version)) {
		printf("VERSION 0.0\n"); exit(1);
		exit(1);
	}
	error = getautoposopt(argc,argv);
	if (error)
		usage();
	if (autoopt(rc))
		readrc(autoopt(rc));
	if (autoopt(printconf))
		printconf();
	mainloop();
}
