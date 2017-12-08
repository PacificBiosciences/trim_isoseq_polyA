#############
# libraries #
#############

trima_cpp_sources = files([
  'hmm_model.cpp',
  'polyA_hmm_model.cpp',])

trima_lib_install = (not meson.is_subproject()) or (get_option('default_library') == 'shared')

trima_lib = library(
  'trima',
  trima_cpp_sources,
  install : true,
  soversion : meson.project_version(),
  version : meson.project_version(),
  include_directories : trima_include_directories,
  cpp_args : trima_flags)


###############
# executables #
###############

# trima
if not meson.is_subproject()
  # boost
  trima_boost_dep = dependency('boost', modules : ['system', 'iostreams'], required : true)
  boost_po_dep = cpp.find_library('boost_program_options')

  trima_mains = executable(
  'trima', [
    'main.cpp'],
  install : true,
  dependencies : [trima_lib_deps, trima_boost_dep, boost_po_dep],
  include_directories : trima_include_directories,
  link_with : trima_lib,
  cpp_args : trima_flags)
endif