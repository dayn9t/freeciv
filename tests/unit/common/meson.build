# Common module tests

test_sources = [
  'test_accessarea.c',
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
  # Disaster module tests
  'test_disaster.c',
  # Actres module tests
  'test_actres.c',
  # Traderoutes module tests
  'test_traderoutes.c',
  # Vision module tests
  'test_vision.c',
  # FC interface module tests
  'test_fc_interface.c',
  # Path finding module tests
  'test_path_finding.c',
  # Metaknowledge module tests
  'test_metaknowledge.c',
  # Modpack module tests
  'test_modpack.c',
  # Server settings module tests
  'test_server_settings.c',
  # Clientutils module tests
  'test_clientutils.c',
  # Mapimg module tests
  'test_mapimg.c',
  # Networking module tests
  'test_packets.c',
  # AI core module tests
  'test_aicore_aiactions.c',
  'test_aicore_aisupport.c',
  'test_aicore_caravan.c',
  'test_aicore_citymap.c',
  'test_aicore_pf_tools.c',
]

foreach test_file : test_sources
  test_name = test_file.replace('.c', '')
  # test_base needs mock library
  if test_name == 'test_base'
    test_exe = executable(test_name,
      test_file,
      include_directories: [common_inc, include_directories('..'), include_directories('../../..'), mock_inc],
      link_with: [common_lib, mock_lib],
      dependencies: [cmocka_dep]
    )
  else
    test_exe = executable(test_name,
      test_file,
      include_directories: [common_inc, include_directories('..'), include_directories('../../..')],
      link_with: [common_lib],
      dependencies: [cmocka_dep]
    )
  endif
  test(test_name, test_exe)
endforeach

# Networking module tests (requires json-protocol/jansson)
if get_option('json-protocol')
  test_dataio_json_exe = executable('test_dataio_json',
    'test_dataio_json.c',
    include_directories: [common_inc, include_directories('..'), include_directories('../../..')],
    link_with: [common_lib],
    dependencies: [cmocka_dep, jansson_dep]
  )
  test('test_dataio_json', test_dataio_json_exe)
endif
