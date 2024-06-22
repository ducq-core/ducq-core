#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <dlfcn.h>

#include "unit_tests_cmd.h"


struct fixture * fix_new(const char *cmd_name) {
	struct fixture *fix = malloc(sizeof(struct fixture));
	if(!fix) return NULL;


	char path[100] = "";
	snprintf(path, 100, "./commands/%s.so", cmd_name);
	fix->handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
	if(!fix->handle) {
		fprintf(stderr, "dlopen failed: %s\n", dlerror());
		free(fix);
		return NULL;
	}



	fix->command = dlsym(fix->handle, "command");
	char *err = dlerror();
	if(!fix->command  || err) {
		fprintf(stderr, "dlsym failed: %s\n", err);
		dlclose(fix->handle);
		free(fix);
		return NULL;
	}

	return fix;
}


int fix_free(struct fixture *fix) {
	if(!fix) return -1;
	if(fix->handle) dlclose(fix->handle);
	free(fix);
	return 0;
}



int mock_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args) {
	check_expected(function_name);
	check_expected(level);
	return 0;
}
