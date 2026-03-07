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
#ifndef FC_MOCK_UNIT_H
#define FC_MOCK_UNIT_H

#include "common/unit.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct unit *mock_unit_create(struct player *owner, struct unit_type *type, int x, int y);
void mock_unit_destroy(struct unit *punit);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC_MOCK_UNIT_H */
