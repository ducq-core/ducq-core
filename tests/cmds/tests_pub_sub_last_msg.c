#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdlib.h>
#include <string.h>

#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "ducq_reactor.h"


static char ack_res[] = "ACK";
static size_t ack_len = sizeof(ack_res) - 1;



typedef struct {
	struct fixture *pub;
	struct fixture *sub;
	ducq_reactor *reactor;
	ducq_i *sub_client;
	ducq_i *pub_client;
} fix_t;

#define get_pub(fix) get_command(&fix->pub)
#define get_sub(fix) get_command(&fix->sub)

int pub_sub_last_msg_group_setup(void **state) {
	fix_t *fix = malloc(sizeof(fix_t));
	if (!fix) return -1;

	fix->pub = fix_new("pub");
	if (!fix->pub) {
		free(fix);
		return -1;
	}	

	fix->sub = fix_new("sub");
	if (!fix->sub) {
		fix_free(fix->pub);
		free(fix);
		return -1;
	}

	*state = fix;
	return 0;
}

int pub_sub_last_msg_tests_setup(void **state) {
	fix_t *fix      = *state;
	fix->reactor    = ducq_reactor_new();
	fix->sub_client = ducq_new_mock("subscriber");
	fix->pub_client = ducq_new_mock("publisher");
	
	ducq_reactor_add_client(fix->reactor, 1, fix->sub_client);
	ducq_reactor_add_client(fix->reactor, 2, fix->pub_client);

	return 0;
}

int pub_sub_last_msg_tests_teardown(void **state) {
	fix_t *fix = *state;

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_reactor_free(fix->reactor);
	return 0;
}

int pub_sub_last_msg_group_teardown(void **state) {
	fix_t *fix = *state;
	fix_free(fix->pub);
	fix_free(fix->sub);
	free(fix);
	return 0;
}





void pub_sub_last_msg_send_ack_if_not_requested(void **state) {
	fix_t *fix = *state;

	expect_value(_send, ducq, fix->pub_client);
	expect_string(_send, buf, ack_res);
	expect_value(_send, *count, ack_len);
	will_return(_send, DUCQ_OK);

	char pub_cmd[] = "publish route\nlast_message";
	size_t pub_len = strlen(pub_cmd);

	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd, pub_len);


	expect_value(_send, ducq, fix->sub_client);
	expect_string(_send, buf, ack_res);
	expect_value(_send, *count, ack_len);
	will_return(_send, DUCQ_OK);

	char sub_cmd[] = "subscribe route\n";
	size_t sub_len = strlen(sub_cmd);
	get_sub(fix)(fix->reactor, fix->sub_client, sub_cmd, sub_len);
}

void pub_sub_last_msg_send_last_if_requested(void **state) {
	fix_t *fix = *state;

	expect_value(_send, ducq, fix->pub_client);
	expect_string(_send, buf, ack_res);
	expect_value(_send, *count, ack_len);
	will_return(_send, DUCQ_OK);

	char pub_cmd[] = "publish route\nlast_message";
	size_t pub_len = strlen(pub_cmd);

	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd, pub_len);



	char sub_cmd[] = "subscribe route\nlast";
	size_t sub_len = strlen(sub_cmd);
	
	expect_value(_send, ducq, fix->sub_client);
	expect_string(_send, buf, pub_cmd);
	expect_value(_send, *count, pub_len);
	will_return(_send, DUCQ_OK);

	get_sub(fix)(fix->reactor, fix->sub_client, sub_cmd, sub_len);
}

void pub_sub_last_msg_send_last_of_route(void **state) {
	fix_t *fix = *state;
	

	expect_value_count(_send, ducq, fix->pub_client, 4);
	expect_string_count(_send, buf, ack_res, 4);
	expect_value_count(_send, *count, ack_len, 4);
	will_return_count(_send, DUCQ_OK, 4);

	char pub_cmd1[] = "publish route/1\na first message";
	size_t pub_len1 = strlen(pub_cmd1);
	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd1, pub_len1);

	char pub_cmd2[] = "publish requested/route\nfirst message";
	size_t pub_len2 = strlen(pub_cmd2);
	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd2, pub_len2);

	char pub_cmd3[] = "publish requested/route\nexpected_message";
	size_t pub_len3 = strlen(pub_cmd3);
	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd3, pub_len3);

	char pub_cmd4[] = "publish route/2\nanother message";
	size_t pub_len4 = strlen(pub_cmd4);
	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd4, pub_len4);

	

	char sub_cmd[] = "subscribe requested/route\nlast";
	size_t sub_len = strlen(sub_cmd);
	
	expect_value(_send, ducq, fix->sub_client);
	expect_string(_send, buf, pub_cmd3);
	expect_value(_send, *count, pub_len3);
	will_return(_send, DUCQ_OK);

	get_sub(fix)(fix->reactor, fix->sub_client, sub_cmd, sub_len);
}

void pub_sub_last_msg_ignore_wildcards(void **state) {
	fix_t *fix = *state;


	expect_value(_send, ducq, fix->pub_client);
	expect_string(_send, buf, ack_res);
	expect_value(_send, *count, ack_len);
	will_return(_send, DUCQ_OK);

	char pub_cmd[] = "publish route\nlast_message";
	size_t pub_len = strlen(pub_cmd);

	get_pub(fix)(fix->reactor, fix->pub_client, pub_cmd, pub_len);

	
	char sub_cmd[] = "subscribe route/*\nlast";
	size_t sub_len = strlen(sub_cmd);
	
	expect_value(_send, ducq, fix->sub_client);
	expect_string(_send, buf, ack_res);
	expect_value(_send, *count, ack_len);
	will_return(_send, DUCQ_OK);

	get_sub(fix)(fix->reactor, fix->sub_client, sub_cmd, sub_len);
}
