/*
 * autoopt - handle program options, help, configuration file 
 * Copyright(C) 2024 Renzo Davoli, VirtualSquare.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <getopt.h>

static char *progname;

#include <autoopt.h>

enum {noarg, arg, multi, multiarg, cli = 0x4, rc = 0x8};
enum {optional, required, remaining, POSTYPES};

static char *autoopts[OPT_NUMOPTS];
static char multicount[OPT_NUMOPTS];

#define _OPT_(ch, opt, arg, __, ___) arg + cli + rc,
#define _CLIOPT_(ch, opt, arg, __, ___) arg + cli,
#define _RCOPT_(ch, opt, arg, __, ___) arg + rc,
#define _POSOPT_(ch, opt, arg, __, ___) arg,
static char autoopt_flags[] = {
	AUTOOPTS
		0
};
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_

#define autoopt_has_arg(i) !!(autoopt_flags[i] & arg)
#define autoopt_multi(i) !!(autoopt_flags[i] & multi)
#define autoopt_cli(i) !!(autoopt_flags[i] & cli)
#define autoopt_rc(i) !!(autoopt_flags[i] & rc)

char *_autoopt(int index) {
	return autoopts[index];
}

int _ismulti(int index) {
	return autoopt_multi(index);
}

#define _OPT_(ch, opt, _, __, ___) #opt,
#define _CLIOPT_ _OPT_
#define _RCOPT_ _OPT_
#define _POSOPT_ _OPT_
static char *option_names[] = {
	AUTOOPTS
		NULL
};
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_

#define __OPTnoarg(...) 
#define __OPTarg(...) __VA_OPT__(":")
#define __OPTmulti(...) 
#define __OPTmultiarg(...) __VA_OPT__(":")
#define _OPT_(ch, opt, arg, __, ___) #ch __OPT ## arg(ch)
#define _CLIOPT_ _OPT_
#define _RCOPT_(...)
#define _POSOPT_(...)
static char short_options[] = AUTOOPTS;
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_
#undef __OPTnoarg
#undef __OPTarg
#undef __OPTmulti
#undef __OPTmultiarg

#define _OPT_(ch, opt, arg, __, ___) #ch " " [0],
#define _CLIOPT_ _OPT_
#define _RCOPT_ _OPT_
#define _POSOPT_ _OPT_
static char short_options_index[] = {
	AUTOOPTS
		0
};
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_

#define _OPT_(ch, opt, arg, __, ___) { #opt, arg & 1, NULL, 0 },
#define _CLIOPT_ _OPT_
#define _RCOPT_(...)
#define _POSOPT_(...)
static struct option long_options[] = {
  AUTOOPTS
	{NULL, 0, NULL, 0}
};
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_

#define _POS_(opt, type) {AUTOOPT(opt), type},
struct {
	int index;
	int type;
} autoposopts[] = {
	AUTOPOSOPTS
	{-1, 0}
};
#undef _POS_

int option_index(const char *tag) {
	for (int i = 0; option_names[i]; i++) {
		if (strcmp(tag, option_names[i]) == 0)
			return i;
	}
	return -1;
}

static void printconfline(int i, char *optname, char *helparg, char *help, int maxnamelen) {
	if (!autoopt_rc(i))
		return;
	fprintf(stderr, "# %s %s\n#      %s\n", optname, helparg, help);
	if (autoopts[i]) {
		if (autoopt_multi(i)) {
			if (autoopt_has_arg(i)) {
				for (char *s = autoopts[i]; s && *s; s += strlen(s) + 1)
					fprintf(stderr, "%-*s %s\n", maxnamelen + 1, optname, s);
			} else {
				fprintf(stderr, "%-*s %d\n", maxnamelen + 1, optname, multicount[i]);
			}
		} else {
			fprintf(stderr, "%-*s", maxnamelen + 1, optname);
			if (autoopt_has_arg(i))
				fprintf(stderr, " %s", autoopts[i]);
			fprintf(stderr, "\n");
		}
	} else
		fprintf(stderr, "# %s not set\n", optname);
	fprintf(stderr, "\n");
}


void printconf(void) {
	int maxnamelen = 0;
	for (int i = 0; option_names[i]; i++) {
		int l = strlen(option_names[i]);
		if (l > maxnamelen) maxnamelen = l;
	}
#define _OPT_(_, opt, __, helparg, help) printconfline(AUTOOPT(opt), #opt, helparg, help, maxnamelen);
#define _CLIOPT_(...)
#define _RCOPT_ _OPT_
#define _POSOPT_(...)
	AUTOOPTS
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_
}

static void printusageline(char *option, char ch, char *helparg, char *help) {
	if (ch)
		printf("  -%c %s\n", ch, helparg);
	int fillerlen = 20 - strlen(option) - strlen(helparg) - 1;
	if (fillerlen >= 0)
		printf("  --%s %s%*s %s\n", option, helparg, fillerlen, "", help);
	else
		printf("  --%s %s\n  %-22s %s\n", option, helparg, "", help);
}

static void printpos(char *opt, int type) {
	switch (type) {
		case required: printf(" %s", opt); break;
		case optional: printf(" [%s]", opt); break;
		case remaining: printf(" [%s ...]", opt); break;
	}
}

void usage(void) {
	printf("Usage:\n  %s [OPTIONS]", progname);
#define _POS_(opt, type) printpos(#opt, type);
	AUTOPOSOPTS
#undef _POS_
	printpos(NULL, -1); // avoid warning if no positional opts
	printf("\n\nOptions:\n");
#define _OPT_(ch, opt, __, helparg, help) printusageline(#opt, #ch "" [0], helparg, help);
#define _CLIOPT_ _OPT_
#define _RCOPT_(...)
#define _POSOPT_ _OPT_
			AUTOOPTS
#undef _OPT_
#undef _CLIOPT_
#undef _RCOPT_
#undef _POSOPT_
				exit(1);
}

static char *addmulti(char *autoarg, char *arg) {
	size_t arglen = strlen(arg) + 1;
	size_t len;
	for (len = 0; autoarg && autoarg[len];
			len += strlen(autoarg + len)  + 1)
		;
	char *new = realloc(autoarg, len + arglen + 1);
	if (new) {
		strcpy(new + len, arg);
		new[len + arglen] = 0;
		return new;
	} else
		return autoarg;
}

int readrc(char *path) {
	int rv = 0;
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		fprintf(stderr, "Error opening conf file %s: %s\n", path, strerror(errno));
		return 1;
	}
	char *line = NULL;
	size_t len = 0;
	ssize_t n = 0;
	while ((n = getline(&line, &len, f)) != -1) {
		char _tag[n-1], _arg[n-1];
    _tag[0] = _arg[0] = 0;
    sscanf(line, "%s %[^\n]", _tag, _arg);
		if (*_tag) {
			if (*_tag != '#') {
				int index = option_index(_tag);
				if (autoopt_rc(index) && index >= 0) {
					if (autoopt_multi(index)) {
						if (autoopt_has_arg(index)) {
							if (*_arg) {
								autoopts[index] = addmulti(autoopts[index], _arg);
								multicount[index]++;
							} else {
								fprintf(stderr, "%s: missing arg for option %s\n",  path, option_names[index]);
								rv = 1;
							}
						} else {
							if (*_arg) {
								char *tail;
								int l = strtol(_arg, &tail, 10);
								if (l > 0 && *tail == 0) {
									autoopts[index] = "";
									multicount[index] += l;
								} else {
									fprintf(stderr, "%s: invalid option %s\n", path, option_names[index]);
									rv = 1;
								}
							} else {
								autoopts[index] = "";
								multicount[index]++;
							}
						}
					} else {
						if (autoopts[index]) {
							fprintf(stderr, "%s: duplicate option %s\n", path, option_names[index]);
							rv = 1;
						} else {
							if (autoopt_has_arg(index)) {
								if (*_arg) {
									autoopts[index] = strdup(_arg);
								} else {
									fprintf(stderr, "%s: missing arg for option %s\n",  path, option_names[index]);
									rv = 1;
								}
							} else {
								if (*_arg) {
									fprintf(stderr, "%s: extra arg for option %s\n",  path, option_names[index]);
									rv = 1;
								} else
									autoopts[index] = "";
							}
						}
					}
				} else {
					fprintf(stderr, "%s: unknown option %s\n", path, _tag);
					rv = 1;
				}
			}
		}
	}
	if (line) free(line);
	return rv;
}

#ifndef _GNU_SOURCE
const char *strchrnul(const char *s, int c) {
  while (*s != 0 && *s != c)
    s++;
  return s;
}
#endif

static int setarg(int c, int index, char *optarg) {
	if (c)
		index = strchrnul(short_options_index, c) - short_options_index;
	if (autoopt_multi(index)) {
		multicount[index]++;
		if (optarg && *optarg)
			autoopts[index] = addmulti(autoopts[index], optarg);
		else
			autoopts[index] = "";
	} else {
		if (autoopts[index]) {
			fprintf(stderr, "Duplicate option %s\n", option_names[index]);
			return 1;
		} else
			autoopts[index] = optarg ? optarg : "";
	}
	return 0;
}

static int getposargs(int argc, char *argv[]) {
	int count_postypes[POSTYPES] = {0, 0, 0};
	int totentry = 0;
	int error = 0;
	for (int i = 0; autoposopts[i].index >= 0; i++) {
		count_postypes[autoposopts[i].type]++;
		totentry++;
	}
	if (argc < count_postypes[required]) {
		fprintf(stderr, "too few arguments in command line\n");
		return 1;
	}
	if (count_postypes[remaining] == 0 &&
			argc > count_postypes[required] + count_postypes[optional]) {
		fprintf(stderr, "too many arguments in command line\n");
		return 1;
	}
	int arglo, arghi, poslo, poshi;
	for (arglo = 0, poslo = 0;
			arglo < argc && autoposopts[poslo].type == required; arglo++, poslo++)
		error |= setarg(0, autoposopts[poslo].index, argv[arglo]);
	for (arghi = argc - 1, poshi = totentry - 1;
			arglo <= arghi && autoposopts[poshi].type == required; arghi--, poshi--) 
		error |= setarg(0, autoposopts[poshi].index, argv[arghi]);
	for ( ; arglo <= arghi && autoposopts[poslo].type == optional; arglo++, poslo++)
		error |= setarg(0, autoposopts[poslo].index, argv[arglo]);
	if (autoposopts[poslo].type == remaining)
		for ( ; arglo <= arghi; arglo++)
			error |= setarg(0, autoposopts[poslo].index, argv[arglo]);

	return error;
}

int getautoopt(int argc, char *argv[]) {
	int c = 0;
  progname = basename(argv[0]);
	int error = 0;
  while (c != -1) {
		int index;
    c = getopt_long(argc, argv, short_options, long_options, &index);
    switch (c) {
      case -1:
        break;
      case '?':
        usage();
				break;
      default:
				error |= setarg(c, index, optarg);
        break;
    }
  }
	//error |= getposargs(argc - optind, argv + optind);

	//if (autoopts[AUTOOPT(help)] || error)
		//usage();
	return error;
}

int getautoposopt(int argc, char *argv[]) {
	return getposargs(argc - optind, argv + optind);
}
