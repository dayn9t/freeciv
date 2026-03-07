# Common module tests

test_sources = [
  'test_calendar.c',
  'test_rgbcolor.c',
  'test_sex.c',
  'test_version.c',
  'test_capstr.c',
  'test_base.c',
  'test_borders.c',
  'test_effects.c',
  'test_requirements.c',
  'test_oblig_reqs.c',
  'test_counters.c',
  'test_culture.c',
  'test_citizens.c',
  # Data structure tests
  'test_worklist.c',
  'test_unitlist.c',
  'test_victory.c',
  'test_workertask.c',
  'test_idex.c',
  'test_tile.c',
  'test_map.c',
  # Unit module tests
  'test_unit.c',
  'test_unittype.c',
  # City module tests
  'test_city.c',
  # Player module tests
  'test_player.c',
  # Nation module tests
  'test_nation.c',
  # Game module tests
  'test_game.c',
  # Terrain module tests
  'test_terrain.c',
  # Combat module tests
  'test_combat.c',
  # Research module tests
  'test_research.c',
  # Tech module tests
  'test_tech.c',
  # Improvement module tests
  'test_improvement.c',
  # Government module tests
  'test_government.c',
  # Specialist module tests
  'test_specialist.c',
  # Extras module tests
  'test_extras.c',
  # Achievement module tests
  'test_achievements.c',
  # Road module tests
  'test_road.c',
  # Style module tests
  'test_style.c',
  # Featured text module tests
  'test_featured_text.c',
  # Actions module tests
  'test_actions.c',
  # Events module tests
  'test_events.c',
  # Movement module tests
  'test_movement.c',
  # Team module tests
  'test_team.c',
  # Reqtext module tests
  'test_reqtext.c',
  # Multipliers module tests
  'test_multipliers.c',
  # Spaceship module tests
  'test_spaceship.c',
  # Diplomatic treaty module tests
  'test_diptreaty.c',
]

foreach test_file : test_sources
  test_name = test_file.replace('.c', '')
  test_exe = executable(test_name,
    test_file,
    include_directories: [common_inc, include_directories('..'), include_directories('../../..')],
    link_with: [common_lib],
    dependencies: [cmocka_dep]
  )
  test(test_name, test_exe)
endforeach
