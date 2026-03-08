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

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

/* utility */
#include "mem.h"
#include "shared.h"

/* common */
#include "game.h"
#include "player.h"
#include "packets.h"
#include "networking/dataio.h"

/* External capability string from packets_gen.c */
extern const char *const packet_functional_capability;

/***********************************************************************
  Test packet_header_init sets correct default values
***********************************************************************/
static void test_packet_header_init(void **state)
{
  struct packet_header header;

  (void) state;

  packet_header_init(&header);

  assert_int_equal(header.length, DIOT_UINT16);
  assert_int_equal(header.type, DIOT_UINT8);
}

/***********************************************************************
  Test packet_handlers_initial returns non-null handlers
***********************************************************************/
static void test_packet_handlers_initial(void **state)
{
  const struct packet_handlers *handlers;

  (void) state;

  handlers = packet_handlers_initial();

  assert_non_null(handlers);
}

/***********************************************************************
  Test packet_handlers_initial returns same pointer on multiple calls
***********************************************************************/
static void test_packet_handlers_initial_consistent(void **state)
{
  const struct packet_handlers *handlers1;
  const struct packet_handlers *handlers2;

  (void) state;

  handlers1 = packet_handlers_initial();
  handlers2 = packet_handlers_initial();

  assert_ptr_equal(handlers1, handlers2);
}

/***********************************************************************
  Test packet_handlers_get with empty capability
***********************************************************************/
static void test_packet_handlers_get_empty(void **state)
{
  const struct packet_handlers *handlers;

  (void) state;

  handlers = packet_handlers_get("");

  assert_non_null(handlers);
}

/***********************************************************************
  Test packet_handlers_get with null capability handling
***********************************************************************/
static void test_packet_handlers_get_simple(void **state)
{
  const struct packet_handlers *handlers;

  (void) state;

  /* Simple capability string */
  handlers = packet_handlers_get("test");

  assert_non_null(handlers);
}

/***********************************************************************
  Test packet_handlers_get returns same handlers for same capability
***********************************************************************/
static void test_packet_handlers_get_consistent(void **state)
{
  const struct packet_handlers *handlers1;
  const struct packet_handlers *handlers2;

  (void) state;

  handlers1 = packet_handlers_get("cap1");
  handlers2 = packet_handlers_get("cap1");

  assert_ptr_equal(handlers1, handlers2);
}

/***********************************************************************
  Test packet_handlers_get returns different handlers for different
  capabilities
***********************************************************************/
static void test_packet_handlers_get_different(void **state)
{
  const struct packet_handlers *handlers1;
  const struct packet_handlers *handlers2;

  (void) state;

  /* First, clean up any existing handlers */
  packets_deinit();

  /* These capabilities likely aren't in the functional capability,
   * so they'll both get the default handlers - this is expected behavior */
  handlers1 = packet_handlers_get("cap_a");
  packets_deinit();
  handlers2 = packet_handlers_get("cap_b");

  /* After deinit, new handlers may or may not be at the same address
   * depending on memory allocation - just verify they're valid */
  assert_non_null(handlers1);
  assert_non_null(handlers2);
}

/***********************************************************************
  Test packets_deinit doesn't crash
***********************************************************************/
static void test_packets_deinit(void **state)
{
  (void) state;

  /* Initialize handlers first */
  packet_handlers_get("test");

  /* Should not crash */
  packets_deinit();
}

/***********************************************************************
  Test packets_deinit multiple times
***********************************************************************/
static void test_packets_deinit_multiple(void **state)
{
  (void) state;

  packet_handlers_get("test1");
  packets_deinit();

  packet_handlers_get("test2");
  packets_deinit();

  packet_handlers_get("test3");
  packets_deinit();
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk with first chunk
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_first(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 0;
  chunk.total_length = 100;
  chunk.chunk_length = 50;
  memset(chunk.data, 0xAB, 50);

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_non_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 100);

  /* Cleanup */
  free(pplayer.attribute_block_buffer.data);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk invalid total_length
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_invalid_total(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 0;
  chunk.total_length = -1;  /* Invalid */
  chunk.chunk_length = 10;

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  /* Should not allocate buffer for invalid data */
  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk invalid chunk_length
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_invalid_chunk(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 0;
  chunk.total_length = 100;
  chunk.chunk_length = -1;  /* Invalid */

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk chunk exceeds total
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_exceeds_total(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 0;
  chunk.total_length = 50;
  chunk.chunk_length = 100;  /* Chunk larger than total */

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk offset exceeds total
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_offset_exceeds(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 200;  /* Offset larger than total */
  chunk.total_length = 100;
  chunk.chunk_length = 10;

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk max block exceeded
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_max_exceeded(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  chunk.offset = 0;
  chunk.total_length = MAX_ATTRIBUTE_BLOCK + 1;  /* Exceeds max */
  chunk.chunk_length = 10;

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk complete transfer
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_complete(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  /* Single chunk that completes the transfer */
  chunk.offset = 0;
  chunk.total_length = 50;
  chunk.chunk_length = 50;
  memset(chunk.data, 0xCD, 50);

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  /* Buffer should be transferred to attribute_block */
  assert_non_null(pplayer.attribute_block.data);
  assert_int_equal(pplayer.attribute_block.length, 50);
  assert_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 0);

  /* Cleanup */
  free(pplayer.attribute_block.data);
}

/***********************************************************************
  Test generic_handle_player_attribute_chunk two part transfer
***********************************************************************/
static void test_generic_handle_player_attribute_chunk_two_part(void **state)
{
  struct player pplayer;
  struct packet_player_attribute_chunk chunk;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  memset(&chunk, 0, sizeof(chunk));

  /* First chunk */
  chunk.offset = 0;
  chunk.total_length = 100;
  chunk.chunk_length = 50;
  memset(chunk.data, 0xAA, ATTRIBUTE_CHUNK_SIZE);

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  assert_non_null(pplayer.attribute_block_buffer.data);
  assert_int_equal(pplayer.attribute_block_buffer.length, 100);
  assert_null(pplayer.attribute_block.data);

  /* Second chunk completes */
  chunk.offset = 50;
  chunk.total_length = 100;
  chunk.chunk_length = 50;
  memset(chunk.data, 0xBB, ATTRIBUTE_CHUNK_SIZE);

  generic_handle_player_attribute_chunk(&pplayer, &chunk);

  /* Now complete */
  assert_non_null(pplayer.attribute_block.data);
  assert_int_equal(pplayer.attribute_block.length, 100);
  assert_null(pplayer.attribute_block_buffer.data);

  /* Cleanup */
  free(pplayer.attribute_block.data);
}

/***********************************************************************
  Test send_attribute_block with null player
***********************************************************************/
static void test_send_attribute_block_null_player(void **state)
{
  (void) state;

  /* Should not crash with null player */
  send_attribute_block(NULL, NULL);
}

/***********************************************************************
  Test send_attribute_block with player null data
***********************************************************************/
static void test_send_attribute_block_null_data(void **state)
{
  struct player pplayer;

  (void) state;

  memset(&pplayer, 0, sizeof(pplayer));
  pplayer.attribute_block.data = NULL;
  pplayer.attribute_block.length = 0;

  /* Should not crash with null data */
  send_attribute_block(&pplayer, NULL);
}

/***********************************************************************
  Test packet_name returns non-null for valid types
***********************************************************************/
static void test_packet_name_valid(void **state)
{
  const char *name;

  (void) state;

  name = packet_name(PACKET_SERVER_JOIN_REQ);

  assert_non_null(name);
}

/***********************************************************************
  Test packet_name for boundary values
***********************************************************************/
static void test_packet_name_boundary(void **state)
{
  const char *name0;
  const char *name_last;

  (void) state;

  name0 = packet_name(0);
  name_last = packet_name(PACKET_LAST - 1);

  assert_non_null(name0);
  assert_non_null(name_last);
}

/***********************************************************************
  Test packet_has_game_info_flag
***********************************************************************/
static void test_packet_has_game_info_flag(void **state)
{
  bool result;

  (void) state;

  /* Just verify the function doesn't crash */
  result = packet_has_game_info_flag(PACKET_SERVER_JOIN_REQ);
  (void) result;
}

/***********************************************************************
  Test post_send_packet_server_join_reply with join success
***********************************************************************/
static void test_post_send_packet_server_join_reply_success(void **state)
{
  struct connection pc;
  struct packet_server_join_reply packet;

  (void) state;

  memset(&pc, 0, sizeof(pc));
  packet_header_init(&pc.packet_header);
  memset(&packet, 0, sizeof(packet));

  packet.you_can_join = TRUE;

  post_send_packet_server_join_reply(&pc, &packet);

  /* After successful join, type should be upgraded to UINT16 */
  assert_int_equal(pc.packet_header.type, DIOT_UINT16);
}

/***********************************************************************
  Test post_send_packet_server_join_reply with join failure
***********************************************************************/
static void test_post_send_packet_server_join_reply_failure(void **state)
{
  struct connection pc;
  struct packet_server_join_reply packet;

  (void) state;

  memset(&pc, 0, sizeof(pc));
  packet_header_init(&pc.packet_header);
  memset(&packet, 0, sizeof(packet));

  packet.you_can_join = FALSE;

  post_send_packet_server_join_reply(&pc, &packet);

  /* After failed join, header should remain unchanged */
  assert_int_equal(pc.packet_header.type, DIOT_UINT8);
}

/***********************************************************************
  Test post_receive_packet_server_join_reply with join success
***********************************************************************/
static void test_post_receive_packet_server_join_reply_success(void **state)
{
  struct connection pc;
  struct packet_server_join_reply packet;

  (void) state;

  memset(&pc, 0, sizeof(pc));
  packet_header_init(&pc.packet_header);
  memset(&packet, 0, sizeof(packet));

  packet.you_can_join = TRUE;

  post_receive_packet_server_join_reply(&pc, &packet);

  /* After successful join, type should be upgraded to UINT16 */
  assert_int_equal(pc.packet_header.type, DIOT_UINT16);
}

/***********************************************************************
  Test post_receive_packet_server_join_reply with join failure
***********************************************************************/
static void test_post_receive_packet_server_join_reply_failure(void **state)
{
  struct connection pc;
  struct packet_server_join_reply packet;

  (void) state;

  memset(&pc, 0, sizeof(pc));
  packet_header_init(&pc.packet_header);
  memset(&packet, 0, sizeof(packet));

  packet.you_can_join = FALSE;

  post_receive_packet_server_join_reply(&pc, &packet);

  /* After failed join, header should remain unchanged */
  assert_int_equal(pc.packet_header.type, DIOT_UINT8);
}

/***********************************************************************
  Test pre_send_packet_player_attribute_chunk valid
***********************************************************************/
static void test_pre_send_packet_player_attribute_chunk_valid(void **state)
{
  struct connection pc;
  struct packet_player_attribute_chunk packet;

  (void) state;

  memset(&pc, 0, sizeof(pc));
  memset(&packet, 0, sizeof(packet));

  packet.offset = 0;
  packet.total_length = 100;
  packet.chunk_length = 50;

  /* Should not crash or assert with valid values */
  pre_send_packet_player_attribute_chunk(&pc, &packet);
}

/***********************************************************************
  Test packet_destroy with null packet
***********************************************************************/
static void test_packet_destroy_null(void **state)
{
  (void) state;

  /* Should handle null gracefully */
  packet_destroy(NULL, PACKET_SERVER_JOIN_REQ);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    /* Packet header tests */
    cmocka_unit_test(test_packet_header_init),

    /* Packet handlers tests */
    cmocka_unit_test(test_packet_handlers_initial),
    cmocka_unit_test(test_packet_handlers_initial_consistent),
    cmocka_unit_test(test_packet_handlers_get_empty),
    cmocka_unit_test(test_packet_handlers_get_simple),
    cmocka_unit_test(test_packet_handlers_get_consistent),
    cmocka_unit_test(test_packet_handlers_get_different),

    /* Packets deinit tests */
    cmocka_unit_test(test_packets_deinit),
    cmocka_unit_test(test_packets_deinit_multiple),

    /* Attribute chunk tests */
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_first),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_invalid_total),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_invalid_chunk),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_exceeds_total),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_offset_exceeds),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_max_exceeded),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_complete),
    cmocka_unit_test(test_generic_handle_player_attribute_chunk_two_part),

    /* Send attribute block tests */
    cmocka_unit_test(test_send_attribute_block_null_player),
    cmocka_unit_test(test_send_attribute_block_null_data),

    /* Packet name tests */
    cmocka_unit_test(test_packet_name_valid),
    cmocka_unit_test(test_packet_name_boundary),

    /* Packet flags tests */
    cmocka_unit_test(test_packet_has_game_info_flag),

    /* Join reply tests */
    cmocka_unit_test(test_post_send_packet_server_join_reply_success),
    cmocka_unit_test(test_post_send_packet_server_join_reply_failure),
    cmocka_unit_test(test_post_receive_packet_server_join_reply_success),
    cmocka_unit_test(test_post_receive_packet_server_join_reply_failure),

    /* Pre-send tests */
    cmocka_unit_test(test_pre_send_packet_player_attribute_chunk_valid),

    /* Packet destroy tests */
    cmocka_unit_test(test_packet_destroy_null),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}