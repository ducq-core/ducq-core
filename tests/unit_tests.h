#ifndef _UNIT_TEST_HEADER_
#define _UNIT_TEST_HEADER_


#define build_cmocka_unit_test(elem) \
	cmocka_unit_test(elem),

#define build_forward_declarations(elem) \
	void elem(void** state);


	#endif // _UNIT_TEST_HEADER_