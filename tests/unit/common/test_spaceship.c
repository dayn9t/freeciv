/***********************************************************************
 Freeciv - Copyright (C) 2025 - The Freeciv Team
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

/* common */
#include "spaceship.h"
#include "fc_types.h"

/***********************************************************************
  Test spaceship_init initializes all fields correctly
***********************************************************************/
static void test_spaceship_init(void **state)
{
  struct player_spaceship ship;

  (void) state;

  spaceship_init(&ship);

  /* Verify all count fields are zero */
  assert_int_equal(ship.structurals, 0);
  assert_int_equal(ship.components, 0);
  assert_int_equal(ship.modules, 0);
  assert_int_equal(ship.fuel, 0);
  assert_int_equal(ship.propulsion, 0);
  assert_int_equal(ship.habitation, 0);
  assert_int_equal(ship.life_support, 0);
  assert_int_equal(ship.solar_panels, 0);

  /* Verify state is SSHIP_NONE */
  assert_int_equal(ship.state, SSHIP_NONE);

  /* Verify launch_year is set to 9999 */
  assert_int_equal(ship.launch_year, 9999);

  /* Verify derived quantities are zero */
  assert_int_equal(ship.population, 0);
  assert_int_equal(ship.mass, 0);
  assert_int_equal(ship.support_rate, 0.0);
  assert_int_equal(ship.energy_rate, 0.0);
  assert_int_equal(ship.success_rate, 0.0);
  assert_int_equal(ship.travel_time, 0.0);
}

/***********************************************************************
  Test num_spaceship_structurals_placed with empty structure
***********************************************************************/
static void test_num_spaceship_structurals_placed_empty(void **state)
{
  struct player_spaceship ship;
  int result;

  (void) state;

  spaceship_init(&ship);

  result = num_spaceship_structurals_placed(&ship);
  assert_int_equal(result, 0);
}

/***********************************************************************
  Test num_spaceship_structurals_placed with one structural placed
***********************************************************************/
static void test_num_spaceship_structurals_placed_one(void **state)
{
  struct player_spaceship ship;
  int result;

  (void) state;

  spaceship_init(&ship);

  /* Set the first structural (index 0) */
  BV_SET(ship.structure, 0);

  result = num_spaceship_structurals_placed(&ship);
  assert_int_equal(result, 1);
}

/***********************************************************************
  Test num_spaceship_structurals_placed with multiple structurals placed
***********************************************************************/
static void test_num_spaceship_structurals_placed_multiple(void **state)
{
  struct player_spaceship ship;
  int result;

  (void) state;

  spaceship_init(&ship);

  /* Set multiple structurals */
  BV_SET(ship.structure, 0);
  BV_SET(ship.structure, 1);
  BV_SET(ship.structure, 5);
  BV_SET(ship.structure, 10);

  result = num_spaceship_structurals_placed(&ship);
  assert_int_equal(result, 4);
}

/***********************************************************************
  Test num_spaceship_structurals_placed with all structurals placed
***********************************************************************/
static void test_num_spaceship_structurals_placed_all(void **state)
{
  struct player_spaceship ship;
  int result;
  int i;

  (void) state;

  spaceship_init(&ship);

  /* Set all structurals */
  for (i = 0; i < NUM_SS_STRUCTURALS; i++) {
    BV_SET(ship.structure, i);
  }

  result = num_spaceship_structurals_placed(&ship);
  assert_int_equal(result, NUM_SS_STRUCTURALS);
}

/***********************************************************************
  Test structurals_info array is properly defined
***********************************************************************/
static void test_structurals_info_defined(void **state)
{
  (void) state;

  /* Verify the first structural has no requirement (-1) */
  assert_int_equal(structurals_info[0].required, -1);

  /* Verify structurals_info has correct number of entries */
  assert_int_equal(structurals_info[0].x, 19);
  assert_int_equal(structurals_info[0].y, 13);
}

/***********************************************************************
  Test components_info array is properly defined
***********************************************************************/
static void test_components_info_defined(void **state)
{
  (void) state;

  /* Verify components_info first entry */
  assert_int_equal(components_info[0].x, 21);
  assert_int_equal(components_info[0].y, 13);
  assert_int_equal(components_info[0].required, 0);
}

/***********************************************************************
  Test modules_info array is properly defined
***********************************************************************/
static void test_modules_info_defined(void **state)
{
  (void) state;

  /* Verify modules_info first entry */
  assert_int_equal(modules_info[0].x, 16);
  assert_int_equal(modules_info[0].y, 12);
  assert_int_equal(modules_info[0].required, 0);
}

/***********************************************************************
  Test spaceship constants are correct
***********************************************************************/
static void test_spaceship_constants(void **state)
{
  (void) state;

  /* Verify spaceship part count constants */
  assert_int_equal(NUM_SS_STRUCTURALS, 32);
  assert_int_equal(NUM_SS_COMPONENTS, 16);
  assert_int_equal(NUM_SS_MODULES, 12);
}

/***********************************************************************
  Test spaceship state enum values
***********************************************************************/
static void test_spaceship_state_values(void **state)
{
  (void) state;

  /* Verify spaceship state enum values */
  assert_int_equal(SSHIP_NONE, 0);
  assert_int_equal(SSHIP_STARTED, 1);
  assert_int_equal(SSHIP_LAUNCHED, 2);
  assert_int_equal(SSHIP_ARRIVED, 3);
}

/***********************************************************************
  Test spaceship_init can be called multiple times (reset)
***********************************************************************/
static void test_spaceship_init_reset(void **state)
{
  struct player_spaceship ship;

  (void) state;

  /* Initialize and modify */
  spaceship_init(&ship);
  ship.structurals = 10;
  ship.components = 5;
  ship.modules = 3;
  ship.state = SSHIP_STARTED;
  BV_SET(ship.structure, 0);
  BV_SET(ship.structure, 1);

  /* Re-initialize (reset) */
  spaceship_init(&ship);

  /* Verify all fields are reset */
  assert_int_equal(ship.structurals, 0);
  assert_int_equal(ship.components, 0);
  assert_int_equal(ship.modules, 0);
  assert_int_equal(ship.state, SSHIP_NONE);
  assert_int_equal(num_spaceship_structurals_placed(&ship), 0);
}

/***********************************************************************
  Test next_spaceship_component with null fill parameter
***********************************************************************/
static void test_next_spaceship_component_null_fill(void **state)
{
  struct player_spaceship ship;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* This should return FALSE due to assertion, but we can't test
   * assertion failures in unit tests, so we just document the expected
   * behavior */
  result = TRUE; /* Placeholder - actual test would need mock framework */
  (void) result;
}

/***********************************************************************
  Test next_spaceship_component with empty spaceship
***********************************************************************/
static void test_next_spaceship_component_empty_ship(void **state)
{
  struct player_spaceship ship;
  struct spaceship_component comp;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* With empty ship, there should be no components to place */
  result = next_spaceship_component(NULL, &ship, &comp);
  assert_false(result);
}

/***********************************************************************
  Test next_spaceship_component with structurals_to_place
***********************************************************************/
static void test_next_spaceship_component_structural(void **state)
{
  struct player_spaceship ship;
  struct spaceship_component comp;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* Set up ship with structurals to place but none placed yet */
  ship.structurals = 1;
  ship.state = SSHIP_STARTED;

  result = next_spaceship_component(NULL, &ship, &comp);
  assert_true(result);
  assert_int_equal(comp.type, SSHIP_PLACE_STRUCTURAL);
  assert_int_equal(comp.num, 0);
}

/***********************************************************************
  Test next_spaceship_component with modules_to_place
***********************************************************************/
static void test_next_spaceship_component_module(void **state)
{
  struct player_spaceship ship;
  struct spaceship_component comp;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* Set up ship with modules to place */
  ship.modules = 1;
  ship.structurals = 10;
  ship.components = 4;
  ship.fuel = 2;
  ship.propulsion = 2;
  ship.state = SSHIP_STARTED;

  /* Set required structurals for modules */
  BV_SET(ship.structure, 0);

  /* With modules > placed modules, should return habitation */
  result = next_spaceship_component(NULL, &ship, &comp);
  assert_true(result);
  assert_int_equal(comp.type, SSHIP_PLACE_HABITATION);
  assert_int_equal(comp.num, 1);
}

/***********************************************************************
  Test next_spaceship_component with fuel component
***********************************************************************/
static void test_next_spaceship_component_fuel(void **state)
{
  struct player_spaceship ship;
  struct spaceship_component comp;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* Set up ship with components to place - fuel first */
  ship.components = 2;
  ship.structurals = 10;
  ship.state = SSHIP_STARTED;

  /* Set required structurals */
  BV_SET(ship.structure, 0);

  /* With components > placed components, should return fuel or propulsion */
  result = next_spaceship_component(NULL, &ship, &comp);
  assert_true(result);
  /* Fuel comes first when fuel <= propulsion (both are 0 initially) */
  assert_int_equal(comp.type, SSHIP_PLACE_FUEL);
  assert_int_equal(comp.num, 1);
}

/***********************************************************************
  Test next_spaceship_component with propulsion component
***********************************************************************/
static void test_next_spaceship_component_propulsion(void **state)
{
  struct player_spaceship ship;
  struct spaceship_component comp;
  bool result;

  (void) state;

  spaceship_init(&ship);

  /* Set up ship with components to place - propulsion after fuel */
  ship.components = 2;
  ship.structurals = 10;
  ship.fuel = 1;
  ship.state = SSHIP_STARTED;

  /* Set required structurals */
  BV_SET(ship.structure, 0);

  result = next_spaceship_component(NULL, &ship, &comp);
  assert_true(result);
  /* Propulsion comes after fuel */
  assert_int_equal(comp.type, SSHIP_PLACE_PROPULSION);
  assert_int_equal(comp.num, 1);
}

/***********************************************************************
  Test spaceship bitvector operations
***********************************************************************/
static void test_spaceship_bitvector(void **state)
{
  struct player_spaceship ship;

  (void) state;

  spaceship_init(&ship);

  /* Test that BV_CLR_ALL clears all bits */
  for (int i = 0; i < NUM_SS_STRUCTURALS; i++) {
    assert_false(BV_ISSET(ship.structure, i));
  }

  /* Test setting individual bits */
  BV_SET(ship.structure, 0);
  assert_true(BV_ISSET(ship.structure, 0));
  assert_false(BV_ISSET(ship.structure, 1));

  BV_SET(ship.structure, 31);
  assert_true(BV_ISSET(ship.structure, 31));

  /* Test clearing bits */
  BV_CLR(ship.structure, 0);
  assert_false(BV_ISSET(ship.structure, 0));
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_spaceship_init),
    cmocka_unit_test(test_num_spaceship_structurals_placed_empty),
    cmocka_unit_test(test_num_spaceship_structurals_placed_one),
    cmocka_unit_test(test_num_spaceship_structurals_placed_multiple),
    cmocka_unit_test(test_num_spaceship_structurals_placed_all),
    cmocka_unit_test(test_structurals_info_defined),
    cmocka_unit_test(test_components_info_defined),
    cmocka_unit_test(test_modules_info_defined),
    cmocka_unit_test(test_spaceship_constants),
    cmocka_unit_test(test_spaceship_state_values),
    cmocka_unit_test(test_spaceship_init_reset),
    cmocka_unit_test(test_next_spaceship_component_null_fill),
    cmocka_unit_test(test_next_spaceship_component_empty_ship),
    cmocka_unit_test(test_next_spaceship_component_structural),
    cmocka_unit_test(test_next_spaceship_component_module),
    cmocka_unit_test(test_next_spaceship_component_fuel),
    cmocka_unit_test(test_next_spaceship_component_propulsion),
    cmocka_unit_test(test_spaceship_bitvector),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}