#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../mock_ducq_client.h"
#include "tests_srv_log.h"


#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"


int mock_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args) {
	check_expected(ctx);
	check_expected(function_name);
	check_expected(level);
}

void srv_log_calls_log_callback(void **state) {
	// arrange
	char *ctx = "CONTEXT";
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, ctx, mock_log);

	expect_string(mock_log, ctx, ctx);
	expect_string(mock_log, function_name, __func__);
	expect_value(mock_log, level, DUCQ_LOG_INFO);

	// act
	ducq_srv_log(srv, DUCQ_LOG_INFO, __func__, "sender_id", "");

	// teardown
	ducq_srv_free(srv);
}



void srv_log_sent_to_monitor_route(void **state) {
	// arrange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_monitor_route(srv, true);

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock(NULL);
	sub2->ducq     = ducq_new_mock(NULL);
	sub1->route    = strdup(DUCQ_MONITOR_ROUTE);
	sub2->route    = strdup("*");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;

	srv->subs = sub2;

	char expected_buffer[DUCQ_MSGSZ] = "";
	size_t expected_size = snprintf(expected_buffer, DUCQ_MSGSZ, "INFO,%s,sender_id,message", __func__);
	expect_value(_send, ducq, sub1->ducq);
	expect_string(_send, buf, expected_buffer);
	expect_value(_send, *count, expected_size);
	will_return(_send, DUCQ_OK);

	// act
	ducq_srv_log(srv, DUCQ_LOG_INFO, __func__, "sender_id", "message");

	// teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}


void srv_log_dont_sent_to_monitor_route_if_not_set(void **state) {
	// arrange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_monitor_route(srv, false);

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock(NULL);
	sub2->ducq     = ducq_new_mock(NULL);
	sub1->route    = strdup(DUCQ_MONITOR_ROUTE);
	sub2->route    = strdup("*");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;

	srv->subs = sub2;

	// act
	ducq_srv_log(srv, DUCQ_LOG_INFO, __func__, "sender_id", "message");

	// teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}




void srv_log_warn_macro(void **state) {
// arrange
	char *ctx = "CONTEXT";
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, ctx, mock_log);

	expect_string(mock_log, ctx, ctx);
	expect_string(mock_log, function_name, __func__);
	expect_value(mock_log, level, DUCQ_LOG_WARN);

	ducq_i *ducq = ducq_new_mock(NULL);

	// act
	ducq_log(WARN, "message");

	// teardown
	ducq_srv_free(srv);
	ducq_free(ducq);
}

