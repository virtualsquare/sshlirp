/* dummy stub functions to avoid warnings on unused glibc functions */

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

struct passwd *getpwnam(const char *name) {
	(void) name;
	return NULL;
}

struct passwd *getpwuid(uid_t uid) {
	(void) uid;
	return NULL;
}

int getpwnam_r(const char *name, struct passwd *pwd,
            char *buf, size_t buflen, struct passwd **result) {
	(void) name;
	(void) pwd;
	(void) buf;
	(void) buflen;
	(void) result;
	return -1;
}

int getpwuid_r(uid_t uid, struct passwd *pwd,
            char *buf, size_t buflen, struct passwd **result) {
	(void) uid;
	(void) pwd;
	(void) buf;
	(void) buflen;
	(void) result;
	return -1;
}

