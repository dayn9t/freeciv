#ifndef FC_TEST_FRAMEWORK_H
#define FC_TEST_FRAMEWORK_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#define fc_assert_int_eq(a, b) assert_int_equal((a), (b))
#define fc_assert_ptr_null(p) assert_null(p)
#define fc_assert_ptr_not_null(p) assert_non_null(p)
#define fc_assert_true(x) assert_true(x)
#define fc_assert_false(x) assert_false(x)
#define fc_assert_str_eq(a, b) assert_string_equal((a), (b))

#endif
