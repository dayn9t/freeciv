/***********************************************************************
 Freeciv - Copyright (C) 2024 - The Freeciv Project
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#include "mock_player.h"

#include "utility/mem.h"
#include "utility/shared.h"
#include "city.h"

/* Define player_slot locally since it's not exposed in headers */
struct player_slot {
  struct player *player;
  int index;
};

static struct player_slot *mock_slots = NULL;
static int mock_slot_count = 0;

struct player *mock_player_create(int id)
{
  struct player *pplayer;

  if (mock_slots == NULL) {
    mock_slot_count = MAX_NUM_PLAYER_SLOTS;
    mock_slots = fc_calloc(mock_slot_count, sizeof(struct player_slot));
  }

  if (id < 0 || id >= mock_slot_count) {
    return NULL;
  }

  pplayer = fc_calloc(1, sizeof(struct player));

  pplayer->slot = &mock_slots[id];
  mock_slots[id].player = pplayer;
  mock_slots[id].index = id;

  pplayer->is_alive = TRUE;
  pplayer->is_connected = FALSE;
  pplayer->is_ready = FALSE;
  pplayer->phase_done = FALSE;

  pplayer->economic.gold = 0;  /* Default gold */
  pplayer->economic.tax = PLAYER_DEFAULT_TAX_RATE;
  pplayer->economic.science = PLAYER_DEFAULT_SCIENCE_RATE;
  pplayer->economic.luxury = PLAYER_DEFAULT_LUXURY_RATE;

  pplayer->cities = city_list_new();
  pplayer->units = unit_list_new();

  return pplayer;
}

void mock_player_destroy(struct player *pplayer)
{
  if (pplayer == NULL) {
    return;
  }

  if (pplayer->cities != NULL) {
    city_list_destroy(pplayer->cities);
  }
  if (pplayer->units != NULL) {
    unit_list_destroy(pplayer->units);
  }

  if (pplayer->slot != NULL) {
    pplayer->slot->player = NULL;
  }

  FC_FREE(pplayer);
}

void mock_player_setup_nation(struct player *pplayer, const char *nation)
{
  if (pplayer == NULL || nation == NULL) {
    return;
  }

  sz_strlcpy(pplayer->name, nation);
}
