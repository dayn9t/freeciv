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

#include "mock_game.h"

#include "utility/mem.h"
#include "common/map.h"
#include "common/world_object.h"

void mock_game_init_minimal(void)
{
  memset(&game, 0, sizeof(game));

  game.info.gold = GAME_DEFAULT_GOLD;
  game.info.turn = 0;
  game.calendar.year = GAME_DEFAULT_START_YEAR;

  game.server.settings_gamestart_valid = FALSE;
  game.server.end_turn = GAME_DEFAULT_END_TURN;
  game.server.global_warming = GAME_DEFAULT_GLOBAL_WARMING;
  game.server.nuclear_winter = GAME_DEFAULT_NUCLEAR_WINTER;
}

void mock_game_cleanup(void)
{
  if (wld.map.tiles != NULL) {
    main_map_free();
  }

  memset(&game, 0, sizeof(game));
}

void mock_game_init_map(int xsize, int ysize)
{
  if (wld.map.tiles != NULL) {
    main_map_free();
  }

  wld.map.topology_id = MAP_DEFAULT_TOPO;
  wld.map.wrap_id = MAP_DEFAULT_WRAP;
  wld.map.xsize = xsize;
  wld.map.ysize = ysize;

  map_init_topology(&wld.map);
  main_map_allocate();
}

void mock_game_set_year(int year)
{
  game.calendar.year = year;
}

void mock_game_set_turn(int turn)
{
  game.info.turn = turn;
}
