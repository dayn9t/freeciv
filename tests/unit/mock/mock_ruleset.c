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

#include "mock_ruleset.h"

#include <string.h>
#include "utility/mem.h"
#include "utility/support.h"
#include "fc_types.h"
#include "game.h"

/* Storage for mock ruleset data */
static struct extra_type mock_extras[MAX_EXTRA_TYPES];
static int mock_extras_count = 0;

/* Storage for mock terrain data */
static struct terrain *mock_terrains = NULL;
static int mock_terrains_count = 0;
#define MAX_MOCK_TERRAINS 16

void mock_ruleset_init_minimal(void)
{
  memset(mock_extras, 0, sizeof(mock_extras));
  mock_extras_count = 0;

  /* Initialize extras with valid states */
  for (int i = 0; i < MAX_EXTRA_TYPES; i++) {
    mock_extras[i].id = i;
  }

  /* Initialize terrain storage */
  mock_terrains = NULL;
  mock_terrains_count = 0;

  /* Initialize game control num_extra_types for iteration */
  game.control.num_extra_types = MAX_EXTRA_TYPES;
}

void mock_ruleset_cleanup(void)
{
  memset(mock_extras, 0, sizeof(mock_extras));
  mock_extras_count = 0;

  if (mock_terrains != NULL) {
    free(mock_terrains);
    mock_terrains = NULL;
  }
  mock_terrains_count = 0;
}

struct extra_type *mock_extra_create(const char *name,
                                     enum extra_cause cause)
{
  if (mock_extras_count >= MAX_EXTRA_TYPES) {
    return NULL;
  }

  struct extra_type *pextra = &mock_extras[mock_extras_count];
  memset(pextra, 0, sizeof(*pextra));
  pextra->id = mock_extras_count;

  /* Set up name - use the vernacular field directly */
  if (name != NULL) {
    strncpy(pextra->name._private_vernacular_, name, MAX_LEN_NAME - 1);
    pextra->name._private_vernacular_[MAX_LEN_NAME - 1] = '\0';
  }

  /* Set the cause bit */
  pextra->causes = (1 << cause);

  mock_extras_count++;
  return pextra;
}

struct road_type *mock_road_create(const char *name)
{
  /* For now, return NULL - road_type is more complex */
  /* Basic implementation for testing road-related functions */
  return NULL;
}

struct base_type *mock_base_create(const char *name)
{
  /* For now, return NULL - base_type is more complex */
  return NULL;
}

void mock_extra_set_flag(struct extra_type *pextra, enum extra_flag_id flag)
{
  if (pextra != NULL) {
    BV_SET(pextra->flags, flag);
  }
}

void mock_extra_set_conflicts(struct extra_type *pextra,
                              struct extra_type *conflict)
{
  if (pextra != NULL && conflict != NULL) {
    BV_SET(pextra->conflicts, extra_index(conflict));
  }
}

void mock_extra_set_dependency(struct extra_type *pextra,
                               struct extra_type *dependency)
{
  /* For simplicity, we don't set up full requirement vector.
   * This is a placeholder for future implementation. */
  (void) pextra;
  (void) dependency;
}

struct terrain *mock_terrain_create(const char *name)
{
  if (mock_terrains == NULL) {
    mock_terrains = fc_calloc(MAX_MOCK_TERRAINS, sizeof(struct terrain));
    mock_terrains_count = 0;
  }

  if (mock_terrains_count >= MAX_MOCK_TERRAINS) {
    return NULL;
  }

  struct terrain *pterrain = &mock_terrains[mock_terrains_count];
  memset(pterrain, 0, sizeof(*pterrain));
  pterrain->item_number = mock_terrains_count;

  if (name != NULL) {
    strncpy(pterrain->name._private_vernacular_, name, MAX_LEN_NAME - 1);
    pterrain->name._private_vernacular_[MAX_LEN_NAME - 1] = '\0';
  }

  /* Set default values for testing */
  pterrain->movement_cost = 1;
  pterrain->defense_bonus = 100;
  pterrain->cultivate_time = 3;
  pterrain->plant_time = 3;
  pterrain->transform_time = 9;
  pterrain->cultivate_result = NULL;
  pterrain->plant_result = NULL;
  pterrain->transform_result = NULL;

  mock_terrains_count++;
  return pterrain;
}

void mock_terrain_set_results(struct terrain *pterrain,
                              struct terrain *cultivate,
                              struct terrain *plant,
                              struct terrain *transform)
{
  if (pterrain != NULL) {
    pterrain->cultivate_result = cultivate;
    pterrain->plant_result = plant;
    pterrain->transform_result = transform;
  }
}

void mock_terrain_set_activity_times(struct terrain *pterrain,
                                     int cultivate_time,
                                     int plant_time,
                                     int transform_time)
{
  if (pterrain != NULL) {
    pterrain->cultivate_time = cultivate_time;
    pterrain->plant_time = plant_time;
    pterrain->transform_time = transform_time;
  }
}

struct unit_class *mock_unit_class_create(const char *name)
{
  /* For now, return NULL - unit_class is more complex */
  return NULL;
}

struct unit_type *mock_unit_type_create(const char *name)
{
  /* For now, return NULL - unit_type is more complex */
  return NULL;
}

struct extra_type *mock_extra_get_by_cause(enum extra_cause cause, int index)
{
  int count = 0;
  for (int i = 0; i < mock_extras_count; i++) {
    if (mock_extras[i].causes & (1 << cause)) {
      if (count == index) {
        return &mock_extras[i];
      }
      count++;
    }
  }
  return NULL;
}

void mock_setup_terrain_for_activities(void)
{
  /* Create extras for irrigation and mine */
  struct extra_type *irrigation = mock_extra_create("Irrigation", EC_BASE);
  (void) irrigation; /* Used for future tests */

  struct extra_type *mine = mock_extra_create("Mine", EC_BASE);
  (void) mine; /* Used for future tests */
}
