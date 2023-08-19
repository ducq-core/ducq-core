#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../mock_ducq_client.h"
#include "tests_log.h"


#include "../src/ducq_reactor.h"


int mock_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args) {
	check_expected(ctx);
	check_expected(function_name);
	check_expected(level);
}

void log_calls_log_callback(void **state) {
	// arrange
	char *ctx = "CONTEXT";
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, ctx, mock_log);

	expect_string(mock_log, ctx, ctx);
	expect_string(mock_log, function_name, __func__);
	expect_value(mock_log, level, DUCQ_LOG_INFO);

	// act
	ducq_reactor_log(reactor, DUCQ_LOG_INFO, __func__, "sender_id", "");

	// teardown
	ducq_reactor_free(reactor);
}



void log_sent_to_monitor_route(void **state) {
	// arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_monitor_route(reactor, true);

	ducq_i *ducq1 =  ducq_new_mock("sub_id_1");
	ducq_i *ducq2 =  ducq_new_mock("sub_id_2");
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_subscribe(reactor, ducq1, DUCQ_MONITOR_ROUTE);
	ducq_reactor_subscribe(reactor, ducq2, "*");

	char expected_buffer[DUCQ_MSGSZ] = "";
	size_t expected_size = snprintf(expected_buffer, DUCQ_MSGSZ, "INFO,%s,sender_id,message", __func__);
	expect_value(_send, ducq, ducq1);
	expect_string(_send, buf, expected_buffer);
	expect_value(_send, *count, expected_size);
	will_return(_send, DUCQ_OK);

	// act
	ducq_reactor_log(reactor, DUCQ_LOG_INFO, __func__, "sender_id", "message");

	// teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(reactor);
}


void log_dont_sent_to_monitor_route_if_not_set(void **state) {
	// arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_monitor_route(reactor, false);

	ducq_i *ducq1 =  ducq_new_mock("sub_id_1");
	ducq_i *ducq2 =  ducq_new_mock("sub_id_2");
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_subscribe(reactor, ducq1, DUCQ_MONITOR_ROUTE);
	ducq_reactor_subscribe(reactor, ducq2, "*");
	
	// act
	ducq_reactor_log(reactor, DUCQ_LOG_INFO, __func__, "sender_id", "message");

	// teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(reactor);
}




void log_warn_macro(void **state) {
// arrange
	char *ctx = "CONTEXT";
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, ctx, mock_log);

	expect_string(mock_log, ctx, ctx);
	expect_string(mock_log, function_name, __func__);
	expect_value(mock_log, level, DUCQ_LOG_WARN);

	ducq_i *ducq = ducq_new_mock(NULL);

	// act
	ducq_log(WARN, "message");

	// teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}

