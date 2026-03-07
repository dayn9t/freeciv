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
#ifndef FC_MOCK_GAME_H
#define FC_MOCK_GAME_H

#include "common/game.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void mock_game_init_minimal(void);
void mock_game_cleanup(void);
void mock_game_init_map(int xsize, int ysize);
void mock_game_set_year(int year);
void mock_game_set_turn(int turn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC_MOCK_GAME_H */
