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

#include "mock_map.h"

#include "utility/mem.h"
#include "common/world_object.h"

void mock_map_init(int xsize, int ysize)
{
  if (wld.map.tiles != NULL) {
    mock_map_free();
  }

  wld.map.topology_id = MAP_DEFAULT_TOPO;
  wld.map.wrap_id = MAP_DEFAULT_WRAP;
  wld.map.xsize = xsize;
  wld.map.ysize = ysize;

  map_init_topology(&wld.map);
  main_map_allocate();
}

void mock_map_free(void)
{
  if (wld.map.tiles != NULL) {
    main_map_free();
  }

  memset(&wld.map, 0, sizeof(wld.map));
}

struct tile *mock_map_get_tile(int x, int y)
{
  return map_pos_to_tile(&wld.map, x, y);
}

void mock_map_set_terrain(int x, int y, struct terrain *pterrain)
{
  struct tile *ptile = map_pos_to_tile(&wld.map, x, y);

  if (ptile != NULL) {
    ptile->terrain = pterrain;
  }
}
