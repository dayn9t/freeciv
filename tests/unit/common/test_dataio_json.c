/***********************************************************************
 Freeciv - Copyright (C) 2025 - The Freeciv Team
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

/* Define FREECIV_JSON_CONNECTION to enable JSON functions */
#ifndef FREECIV_JSON_CONNECTION
#define FREECIV_JSON_CONNECTION
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>
#include <jansson.h>
#include <string.h>

/* utility */
#include "mem.h"
#include "support.h"

/* common/networking */
#include "dataio.h"

/* Test buffer size */
#define TEST_BUFFER_SIZE 256

/***********************************************************************
  Helper function to initialize json_data_out for JSON mode
***********************************************************************/
static void json_output_init(struct json_data_out *dout)
{
  unsigned char *buffer = fc_malloc(TEST_BUFFER_SIZE);
  dio_output_init(&dout->raw, buffer, TEST_BUFFER_SIZE);
  dout->json = json_object();
}

/***********************************************************************
  Helper function to initialize json_data_out for raw (binary) mode
***********************************************************************/
static void raw_output_init(struct json_data_out *dout)
{
  unsigned char *buffer = fc_malloc(TEST_BUFFER_SIZE);
  dio_output_init(&dout->raw, buffer, TEST_BUFFER_SIZE);
  dout->json = NULL;
}

/***********************************************************************
  Helper function to cleanup json_data_out
***********************************************************************/
static void output_cleanup(struct json_data_out *dout)
{
  if (dout->json) {
    json_decref(dout->json);
    dout->json = NULL;
  }
  if (dout->raw.dest) {
    FC_FREE(dout->raw.dest);
    dout->raw.dest = NULL;
  }
}

/***********************************************************************
  Test dio_put_uint8_json in JSON mode
***********************************************************************/
static void test_dio_put_uint8_json_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_uint8_json(&dout, loc, 42);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 42);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint8_json in raw (binary) mode
***********************************************************************/
static void test_dio_put_uint8_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  unsigned char *buffer;

  (void) state;

  raw_output_init(&dout);
  buffer = dout.raw.dest;

  loc = plocation_field_new("test_field");
  dio_put_uint8_json(&dout, loc, 42);

  /* In raw mode, the value is written directly to the buffer */
  assert_int_equal(buffer[0], 42);
  assert_int_equal(dout.raw.current, 1);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint8_json with max value
***********************************************************************/
static void test_dio_put_uint8_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_uint8_json(&dout, loc, 255);

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 255);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint8_json with zero value
***********************************************************************/
static void test_dio_put_uint8_json_zero(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("zero_value");
  dio_put_uint8_json(&dout, loc, 0);

  value = json_object_get(dout.json, "zero_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 0);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint16_json in JSON mode
***********************************************************************/
static void test_dio_put_uint16_json_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_uint16_json(&dout, loc, 1000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 1000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint16_json with max value
***********************************************************************/
static void test_dio_put_uint16_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_uint16_json(&dout, loc, 65535);

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 65535);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint16_json in raw mode
***********************************************************************/
static void test_dio_put_uint16_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;

  (void) state;

  raw_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_uint16_json(&dout, loc, 1000);

  /* In raw mode, 16-bit values are stored in network byte order */
  assert_int_equal(dout.raw.current, 2);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint32_json in JSON mode
***********************************************************************/
static void test_dio_put_uint32_json_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_uint32_json(&dout, loc, 100000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 100000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint32_json with max value
***********************************************************************/
static void test_dio_put_uint32_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_uint32_json(&dout, loc, 2147483647); /* INT_MAX for safety */

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 2147483647);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_uint32_json in raw mode
***********************************************************************/
static void test_dio_put_uint32_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;

  (void) state;

  raw_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_uint32_json(&dout, loc, 100000);

  /* In raw mode, 32-bit values use 4 bytes */
  assert_int_equal(dout.raw.current, 4);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint8_json in JSON mode with positive value
***********************************************************************/
static void test_dio_put_sint8_json_positive(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint8_json(&dout, loc, 42);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 42);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint8_json in JSON mode with negative value
***********************************************************************/
static void test_dio_put_sint8_json_negative(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint8_json(&dout, loc, -42);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), -42);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint8_json with zero value
***********************************************************************/
static void test_dio_put_sint8_json_zero(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("zero_value");
  dio_put_sint8_json(&dout, loc, 0);

  value = json_object_get(dout.json, "zero_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 0);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint8_json with min value
***********************************************************************/
static void test_dio_put_sint8_json_min(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("min_value");
  dio_put_sint8_json(&dout, loc, -128);

  value = json_object_get(dout.json, "min_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), -128);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint8_json with max value
***********************************************************************/
static void test_dio_put_sint8_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_sint8_json(&dout, loc, 127);

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 127);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint16_json in JSON mode with positive value
***********************************************************************/
static void test_dio_put_sint16_json_positive(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint16_json(&dout, loc, 1000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 1000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint16_json in JSON mode with negative value
***********************************************************************/
static void test_dio_put_sint16_json_negative(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint16_json(&dout, loc, -1000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), -1000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint16_json with min value
***********************************************************************/
static void test_dio_put_sint16_json_min(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("min_value");
  dio_put_sint16_json(&dout, loc, -32768);

  value = json_object_get(dout.json, "min_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), -32768);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint16_json with max value
***********************************************************************/
static void test_dio_put_sint16_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_sint16_json(&dout, loc, 32767);

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 32767);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint32_json in JSON mode with positive value
***********************************************************************/
static void test_dio_put_sint32_json_positive(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint32_json(&dout, loc, 100000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), 100000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint32_json in JSON mode with negative value
***********************************************************************/
static void test_dio_put_sint32_json_negative(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_field");
  dio_put_sint32_json(&dout, loc, -100000);

  value = json_object_get(dout.json, "test_field");
  assert_non_null(value);
  assert_true(json_is_integer(value));
  assert_int_equal(json_integer_value(value), -100000);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint32_json with min value
***********************************************************************/
static void test_dio_put_sint32_json_min(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("min_value");
  dio_put_sint32_json(&dout, loc, -2147483647);

  value = json_object_get(dout.json, "min_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), -2147483647);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_sint32_json with max value
***********************************************************************/
static void test_dio_put_sint32_json_max(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("max_value");
  dio_put_sint32_json(&dout, loc, 2147483647);

  value = json_object_get(dout.json, "max_value");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 2147483647);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_string_json in JSON mode
***********************************************************************/
static void test_dio_put_string_json_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("test_string");
  dio_put_string_json(&dout, loc, "Hello, World!");

  value = json_object_get(dout.json, "test_string");
  assert_non_null(value);
  assert_true(json_is_string(value));
  assert_string_equal(json_string_value(value), "Hello, World!");

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_string_json with empty string
***********************************************************************/
static void test_dio_put_string_json_empty(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("empty_string");
  dio_put_string_json(&dout, loc, "");

  value = json_object_get(dout.json, "empty_string");
  assert_non_null(value);
  assert_true(json_is_string(value));
  assert_string_equal(json_string_value(value), "");

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_string_json with special characters
***********************************************************************/
static void test_dio_put_string_json_special(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("special_string");
  dio_put_string_json(&dout, loc, "Test\nWith\tSpecial\"Chars");

  value = json_object_get(dout.json, "special_string");
  assert_non_null(value);
  assert_true(json_is_string(value));
  assert_string_equal(json_string_value(value), "Test\nWith\tSpecial\"Chars");

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_string_json with unicode string
***********************************************************************/
static void test_dio_put_string_json_unicode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("unicode_string");
  dio_put_string_json(&dout, loc, "Unicode: \xC3\xA9\xC3\xA0\xC3\xBC");

  value = json_object_get(dout.json, "unicode_string");
  assert_non_null(value);
  assert_true(json_is_string(value));

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_string_json in raw mode
***********************************************************************/
static void test_dio_put_string_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;

  (void) state;

  raw_output_init(&dout);

  loc = plocation_field_new("test_string");
  dio_put_string_json(&dout, loc, "Test");

  /* In raw mode, string is stored with null terminator */
  assert_int_equal(dout.raw.current, 5); /* "Test" + null terminator */

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool8_json in JSON mode with true value
***********************************************************************/
static void test_dio_put_bool8_json_true(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("bool_true");
  dio_put_bool8_json(&dout, loc, TRUE);

  value = json_object_get(dout.json, "bool_true");
  assert_non_null(value);
  assert_true(json_is_true(value));

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool8_json in JSON mode with false value
***********************************************************************/
static void test_dio_put_bool8_json_false(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("bool_false");
  dio_put_bool8_json(&dout, loc, FALSE);

  value = json_object_get(dout.json, "bool_false");
  assert_non_null(value);
  assert_true(json_is_false(value));

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool8_json in raw mode
***********************************************************************/
static void test_dio_put_bool8_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  unsigned char *buffer;

  (void) state;

  raw_output_init(&dout);
  buffer = dout.raw.dest;

  loc = plocation_field_new("bool_value");
  dio_put_bool8_json(&dout, loc, TRUE);

  /* In raw mode, bool8 is stored as a single byte (1 for true) */
  assert_int_equal(buffer[0], 1);
  assert_int_equal(dout.raw.current, 1);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool32_json in JSON mode with true value
***********************************************************************/
static void test_dio_put_bool32_json_true(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("bool_true");
  dio_put_bool32_json(&dout, loc, TRUE);

  value = json_object_get(dout.json, "bool_true");
  assert_non_null(value);
  assert_true(json_is_true(value));

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool32_json in JSON mode with false value
***********************************************************************/
static void test_dio_put_bool32_json_false(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("bool_false");
  dio_put_bool32_json(&dout, loc, FALSE);

  value = json_object_get(dout.json, "bool_false");
  assert_non_null(value);
  assert_true(json_is_false(value));

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test dio_put_bool32_json in raw mode
***********************************************************************/
static void test_dio_put_bool32_raw_mode(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;

  (void) state;

  raw_output_init(&dout);

  loc = plocation_field_new("bool_value");
  dio_put_bool32_json(&dout, loc, TRUE);

  /* In raw mode, bool32 is stored as 4 bytes */
  assert_int_equal(dout.raw.current, 4);

  FC_FREE(loc);
  output_cleanup(&dout);
}

/***********************************************************************
  Test multiple field writes in JSON mode
***********************************************************************/
static void test_dio_put_multiple_fields(void **state)
{
  struct json_data_out dout;
  struct plocation *loc;
  json_t *value;

  (void) state;

  json_output_init(&dout);

  loc = plocation_field_new("field1");
  dio_put_uint8_json(&dout, loc, 10);
  FC_FREE(loc);

  loc = plocation_field_new("field2");
  dio_put_uint16_json(&dout, loc, 1000);
  FC_FREE(loc);

  loc = plocation_field_new("field3");
  dio_put_string_json(&dout, loc, "test");
  FC_FREE(loc);

  loc = plocation_field_new("field4");
  dio_put_bool8_json(&dout, loc, TRUE);
  FC_FREE(loc);

  value = json_object_get(dout.json, "field1");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 10);

  value = json_object_get(dout.json, "field2");
  assert_non_null(value);
  assert_int_equal(json_integer_value(value), 1000);

  value = json_object_get(dout.json, "field3");
  assert_non_null(value);
  assert_string_equal(json_string_value(value), "test");

  value = json_object_get(dout.json, "field4");
  assert_non_null(value);
  assert_true(json_is_true(value));

  output_cleanup(&dout);
}

/***********************************************************************
  Test plocation_field_new and plocation_elem_new functions
***********************************************************************/
static void test_plocation_functions(void **state)
{
  struct plocation *loc;

  (void) state;

  /* Test plocation_field_new */
  loc = plocation_field_new("test_field");
  assert_non_null(loc);
  assert_int_equal(loc->kind, PADR_FIELD);
  assert_string_equal(loc->name, "test_field");
  assert_null(loc->sub_location);
  FC_FREE(loc);

  /* Test plocation_elem_new */
  loc = plocation_elem_new(5);
  assert_non_null(loc);
  assert_int_equal(loc->kind, PADR_ELEMENT);
  assert_int_equal(loc->number, 5);
  assert_null(loc->sub_location);
  FC_FREE(loc);
}

/***********************************************************************
  Test dio_output_init and dio_output_used functions
***********************************************************************/
static void test_dio_output_functions(void **state)
{
  struct raw_data_out dout;
  unsigned char buffer[TEST_BUFFER_SIZE];
  size_t used;

  (void) state;

  dio_output_init(&dout, buffer, sizeof(buffer));
  assert_int_equal(dout.current, 0);
  assert_int_equal(dout.dest_size, TEST_BUFFER_SIZE);

  /* Write some data */
  dio_put_uint8_raw(&dout, 42);

  used = dio_output_used(&dout);
  assert_int_equal(used, 1);

  /* Test rewind */
  dio_output_rewind(&dout);
  assert_int_equal(dout.current, 0);
}

/***********************************************************************
  Main test runner
***********************************************************************/
int main(void)
{
  const struct CMUnitTest tests[] = {
    /* uint8 tests */
    cmocka_unit_test(test_dio_put_uint8_json_mode),
    cmocka_unit_test(test_dio_put_uint8_raw_mode),
    cmocka_unit_test(test_dio_put_uint8_json_max),
    cmocka_unit_test(test_dio_put_uint8_json_zero),

    /* uint16 tests */
    cmocka_unit_test(test_dio_put_uint16_json_mode),
    cmocka_unit_test(test_dio_put_uint16_json_max),
    cmocka_unit_test(test_dio_put_uint16_raw_mode),

    /* uint32 tests */
    cmocka_unit_test(test_dio_put_uint32_json_mode),
    cmocka_unit_test(test_dio_put_uint32_json_max),
    cmocka_unit_test(test_dio_put_uint32_raw_mode),

    /* sint8 tests */
    cmocka_unit_test(test_dio_put_sint8_json_positive),
    cmocka_unit_test(test_dio_put_sint8_json_negative),
    cmocka_unit_test(test_dio_put_sint8_json_zero),
    cmocka_unit_test(test_dio_put_sint8_json_min),
    cmocka_unit_test(test_dio_put_sint8_json_max),

    /* sint16 tests */
    cmocka_unit_test(test_dio_put_sint16_json_positive),
    cmocka_unit_test(test_dio_put_sint16_json_negative),
    cmocka_unit_test(test_dio_put_sint16_json_min),
    cmocka_unit_test(test_dio_put_sint16_json_max),

    /* sint32 tests */
    cmocka_unit_test(test_dio_put_sint32_json_positive),
    cmocka_unit_test(test_dio_put_sint32_json_negative),
    cmocka_unit_test(test_dio_put_sint32_json_min),
    cmocka_unit_test(test_dio_put_sint32_json_max),

    /* string tests */
    cmocka_unit_test(test_dio_put_string_json_mode),
    cmocka_unit_test(test_dio_put_string_json_empty),
    cmocka_unit_test(test_dio_put_string_json_special),
    cmocka_unit_test(test_dio_put_string_json_unicode),
    cmocka_unit_test(test_dio_put_string_raw_mode),

    /* bool tests */
    cmocka_unit_test(test_dio_put_bool8_json_true),
    cmocka_unit_test(test_dio_put_bool8_json_false),
    cmocka_unit_test(test_dio_put_bool8_raw_mode),
    cmocka_unit_test(test_dio_put_bool32_json_true),
    cmocka_unit_test(test_dio_put_bool32_json_false),
    cmocka_unit_test(test_dio_put_bool32_raw_mode),

    /* complex tests */
    cmocka_unit_test(test_dio_put_multiple_fields),

    /* plocation tests */
    cmocka_unit_test(test_plocation_functions),

    /* output function tests */
    cmocka_unit_test(test_dio_output_functions),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}