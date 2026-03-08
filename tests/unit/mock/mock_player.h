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
#ifndef FC_MOCK_PLAYER_H
#define FC_MOCK_PLAYER_H

#include "player.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct player *mock_player_create(int id);
void mock_player_destroy(struct player *pplayer);
void mock_player_setup_nation(struct player *pplayer, const char *nation);
struct player *mock_player_get(int id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC_MOCK_PLAYER_H */
