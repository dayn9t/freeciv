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

static int mock_city_id_counter = 1;

struct city *mock_city_create(struct player *owner, const char *name, int x, int y)
{
  struct city *pcity;

  (void) x;  /* Unused for now */
  (void) y;

  pcity = fc_calloc(1, sizeof(struct city));

  pcity->id = mock_city_id_counter++;
  pcity->name = fc_strdup(name);
  pcity->owner = owner;
  pcity->tile = NULL;  /* Simplified - no tile association */

  pcity->size = 1;
  pcity->food_stock = 0;
  pcity->shield_stock = 0;

  return pcity;
}

void mock_city_destroy(struct city *pcity)
{
  if (pcity == NULL) {
    return;
  }

  FC_FREE(pcity->name);
  FC_FREE(pcity);
}
