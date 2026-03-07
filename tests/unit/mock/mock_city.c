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

#include "mock_city.h"

#include "utility/mem.h"
#include "utility/shared.h"
#include "common/world_object.h"

static int mock_city_id_counter = 1;

struct city *mock_city_create(struct player *owner, const char *name, int x, int y)
{
  struct city *pcity;
  struct tile *ptile;

  ptile = map_pos_to_tile(&wld.map, x, y);
  if (ptile == NULL) {
    return NULL;
  }

  pcity = fc_calloc(1, sizeof(struct city));

  pcity->id = mock_city_id_counter++;
  pcity->name = fc_strdup(name);
  pcity->owner = owner;
  pcity->tile = ptile;

  pcity->size = 1;
  pcity->happy = 0;
  pcity->content = 1;
  pcity->unhappy = 0;
  pcity->angry = 0;

  pcity->food_stock = 0;
  pcity->shield_stock = 0;

  if (owner != NULL) {
    city_list_append(owner->cities, pcity);
  }

  tile_set_worked(ptile, pcity);

  return pcity;
}

void mock_city_destroy(struct city *pcity)
{
  if (pcity == NULL) {
    return;
  }

  if (pcity->tile != NULL) {
    tile_set_worked(pcity->tile, NULL);
  }

  if (pcity->owner != NULL) {
    city_list_remove(pcity->owner->cities, pcity);
  }

  FC_FREE(pcity->name);
  FC_FREE(pcity);
}
