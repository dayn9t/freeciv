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

/* Storage for mock ruleset data */
static struct extra_type mock_extras[MAX_EXTRA_TYPES];
static int mock_extras_count = 0;

void mock_ruleset_init_minimal(void)
{
  memset(mock_extras, 0, sizeof(mock_extras));
  mock_extras_count = 0;

  /* Initialize extras with valid states */
  for (int i = 0; i < MAX_EXTRA_TYPES; i++) {
    mock_extras[i].id = i;
  }
}

void mock_ruleset_cleanup(void)
{
  memset(mock_extras, 0, sizeof(mock_extras));
  mock_extras_count = 0;
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

struct base_type *mock_base_create(const char *name)
{
  /* For now, return NULL - base_type is more complex */
  return NULL;
}

struct road_type *mock_road_create(const char *name)
{
  /* For now, return NULL - road_type is more complex */
  return NULL;
}

struct terrain *mock_terrain_create(const char *name)
{
  /* For now, return NULL - terrain is more complex */
  return NULL;
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
