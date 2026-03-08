/***********************************************************************
 Freeciv - Copyright (C) 2026 - The Freeciv Project
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#ifndef FC_MOCK_RULESET_H
#define FC_MOCK_RULESET_H

#include "extras.h"
#include "terrain.h"
#include "unittype.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Initialize minimal ruleset data for testing */
void mock_ruleset_init_minimal(void);

/* Cleanup ruleset data */
void mock_ruleset_cleanup(void);

/* Create a mock extra_type for testing */
struct extra_type *mock_extra_create(const char *name,
                                     enum extra_cause cause);

/* Create a mock base_type for testing */
struct base_type *mock_base_create(const char *name);

/* Create a mock road_type for testing */
struct road_type *mock_road_create(const char *name);

/* Create a mock terrain for testing */
struct terrain *mock_terrain_create(const char *name);

/* Create a mock unit_type for testing */
struct unit_type *mock_unit_type_create(const char *name);

/* Create a mock unit_class for testing */
struct unit_class *mock_unit_class_create(const char *name);

/* Get extra by cause - mock implementation */
struct extra_type *mock_extra_get_by_cause(enum extra_cause cause, int index);

/* Setup terrain with extras for testing tile activities */
void mock_setup_terrain_for_activities(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FC_MOCK_RULESET_H */
