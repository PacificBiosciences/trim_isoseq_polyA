subdir('src')

trima_h_config = configuration_data()
trima_h_config.set('TrimIsoseqPolyA_TestsDir', meson.current_source_dir())

trima_h = configure_file(
  input : files('src/TestData.h.in'),
  output : 'TestData.h',
  configuration : trima_h_config)

trima_test_cpp_sources += trima_h

trima_gmock_main_dep = dependency('gmock_main', fallback : ['gtest', 'gmock_main_dep'])

# boost
trima_boost_dep = dependency('boost', modules : ['system', 'iostreams'], required : true)
boost_po_dep = cpp.find_library('boost_program_options')

trima_test = executable(
  'trima_test',
  trima_test_cpp_sources,
  dependencies : [trima_lib_deps, trima_gmock_main_dep, trima_boost_dep, boost_po_dep],
  include_directories : trima_include_directories,
  link_with : trima_lib,
  cpp_args : trima_flags,
  install : false)

test('trima testsuite', trima_test, args : ['--gtest_output=xml:uny-unit.xml'])
