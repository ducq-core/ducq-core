#include <time.h> 

#include "ducq_log.h"

char *ducq_level_tostr(enum ducq_log_level level) {
	switch(level) {
		case DUCQ_LOG_DEBUG:   return "debug";
		case DUCQ_LOG_INFO:    return "info";
		case DUCQ_LOG_WARNING: return "warning";
		case DUCQ_LOG_ERROR:   return "error";
		default:               return "unknown";
	}
}

char *ducq_getnow(char *buffer, size_t size) {
	time_t timer = time(NULL);
	strftime(buffer, size, "%FT%T", localtime(&timer));
	return buffer;
}

