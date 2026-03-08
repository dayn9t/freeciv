# Server module tests

# Note: Add server test files here when available
test_sources = [
  'test_score.c',
  'test_mood.c',
  'test_setcompat.c',
  'test_srv_signal.c',
  # 'test_fcdb.c',  # Disabled - requires complex Lua initialization
]

# Math library dependency
m_dep = c_compiler.find_library('m', required: false)

# Readline library dependency (required for server module)
readline_dep = c_compiler.find_library('readline', required: false)

foreach test_file : test_sources
  test_name = test_file.replace('.c', '')
  deps = [cmocka_dep]
  if m_dep.found()
    deps += m_dep
  endif
  if readline_dep.found()
    deps += readline_dep
  endif

  test_exe = executable(test_name,
    test_file,
    include_directories: [common_inc, mock_inc, server_inc, include_directories('..'), include_directories('../../..')],
    link_with: [common_lib, mock_lib, server_lib, ais],
    dependencies: deps
  )
  test(test_name, test_exe)
endforeach
