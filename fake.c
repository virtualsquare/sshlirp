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

#include <stdint.h>
#include <stdarg.h>

typedef int SysprofClock;
typedef int64_t SysprofTimeStamp;
typedef int32_t SysprofTimeSysprofan;

SysprofClock sysprof_clock;

void sysprof_collector_mark_vprintf (int64_t time, int64_t duration,
		const char *group, const char *mark, const char *message_format, va_list args) {
	(void) time;
	(void) duration;
	(void) group;
	(void) mark;
	(void) message_format;
	(void) args;
}

unsigned int sysprof_collector_request_counters (unsigned int n_counters) {
	(void) n_counters;
	return 0;
}

void sysprof_collector_define_counters (void *counters,
 unsigned int n_counters) {
	(void) counters;
	(void) n_counters;
}

void sysprof_collector_set_counters (const unsigned int *counters_ids,
 const void *values, unsigned int n_counters) {
	(void) counters_ids;
	(void) values;
	(void) n_counters;
}
