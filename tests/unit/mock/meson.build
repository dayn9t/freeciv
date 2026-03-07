# Mock framework library for unit testing

mock_sources = files(
  'mock_game.c',
  'mock_player.c',
  'mock_map.c',
  'mock_city.c',
  'mock_unit.c'
)

mock_inc = include_directories('.')

mock_lib = static_library('fc_test_mock',
  mock_sources,
  include_directories: [common_inc, mock_inc],
  link_with: [common_lib],
  dependencies: [cmocka_dep]
)
