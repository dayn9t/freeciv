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

#include "mock_unit.h"

#include "utility/mem.h"
#include "utility/shared.h"
#include "world_object.h"
#include "unitlist.h"
#include "map.h"
#include "tile.h"
#include "fc_types.h"

static int mock_unit_id_counter = 1;

struct unit *mock_unit_create(struct player *owner, struct unit_type *type, int x, int y)
{
  struct unit *punit;
  struct tile *ptile;

  ptile = map_pos_to_tile(&wld.map, x, y);
  if (ptile == NULL) {
    return NULL;
  }

  punit = fc_calloc(1, sizeof(struct unit));

  punit->id = mock_unit_id_counter++;
  punit->utype = type;
  punit->owner = owner;
  punit->tile = ptile;
  punit->nationality = owner;

  punit->hp = 10;
  punit->moves_left = 3;  /* Simplified: fixed move points */
  punit->veteran = 0;

  punit->activity = ACTIVITY_IDLE;
  punit->ssa_controller = SSA_NONE;

  punit->homecity = 0;
  punit->battlegroup = BATTLEGROUP_NONE;

  if (owner != NULL) {
    unit_list_prepend(owner->units, punit);
  }

  unit_list_prepend(ptile->units, punit);

  return punit;
}

void mock_unit_destroy(struct unit *punit)
{
  if (punit == NULL) {
    return;
  }

  if (punit->tile != NULL) {
    unit_list_remove(punit->tile->units, punit);
  }

  if (punit->owner != NULL) {
    unit_list_remove(punit->owner->units, punit);
  }

  FC_FREE(punit);
}