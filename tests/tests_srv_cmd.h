#ifndef _CMD_TESTS_HEADER_
#define _CMD_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_cmd_tests(apply) \
	apply(cmd_load_can_load_a_command) \
	apply(cmd_load_correct_number_of_commands) \
	apply(cmd_load_commands_have_expected_name) \
	apply(cmd_dispatch_ducq_recv_receive_dispatch_params) \
	apply(cmd_dispatch_recv_fails_return_rc) \
	apply(cmd_dispatch_recv_msg_buffer_size_minus_one_ok) \
	apply(cmd_dispatch_recv_msg_exact_buffer_size_too_big) \
	apply(cmd_dispatch_command_a_receive_params) \
	apply(cmd_dispatch_command_b_receive_params) \
	apply(cmd_dispatch_command_receive_buffer_always_null_terminated) \
	apply(cmd_dispatch_sender_receive_ack) \
	apply(cmd_dispatch_sender_receive_nack_with_command_rc) \
	apply(cmd_dispatch_sender_receive_nack_if_command_unknown) \
	apply(cmd_dispatch_returns_send_ack_state_over_command_state)
	
foreach_cmd_tests(build_forward_declarations)




#define build_cmd_tests() \
	foreach_cmd_tests(build_cmocka_unit_test)

#endif // _CMD_TESTS_HEADER_