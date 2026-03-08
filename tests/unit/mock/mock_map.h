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
#ifndef FC_MOCK_MAP_H
#define FC_MOCK_MAP_H

#include "map.h"
#include "terrain.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void mock_map_init(int xsize, int ysize);
void mock_map_free(void);
struct tile *mock_map_get_tile(int x, int y);
void mock_map_set_terrain(int x, int y, struct terrain *pterrain);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC_MOCK_MAP_H */
