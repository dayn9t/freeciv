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
  # City module tests
  'test_city.c',
  # Player module tests
  'test_player.c',
  # Game module tests
  'test_game.c',
  # Combat module tests
  'test_combat.c',
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
