project(
  'trima',
  'cpp',
  version : '0.0.1',
  default_options : [
    'buildtype=release',
    'default_library=static',
    'warning_level=3',
    'cpp_std=c++14'],
  license : 'BSD-3',
  meson_version : '>= 0.41.1')

############
# CXXFLAGS #
############

trima_flags = []
cpp = meson.get_compiler('cpp')
foreach cflag: [
  '-Wno-terminate',
  '-Wno-missing-field-initializers']
    if cpp.has_argument(cflag)
      trima_flags += cflag
    endif
endforeach

################
# dependencies #
################

# threads
trima_thread_dep = dependency('threads', required : true)

# boost
trima_boost_dep = dependency('boost', modules : ['system', 'iostreams'], required : true)
boost_po_dep = cpp.find_library('boost_program_options')

trima_lib_deps = [trima_thread_dep, trima_boost_dep, boost_po_dep]

# compression
trima_zlib_dep = dependency('zlib', required : false, fallback : ['zlib', 'zlib_dep'])
trima_libbz2_dep = dependency('libbz2', required : false, fallback : ['libbz2', 'bz2_dep'])

if (trima_zlib_dep.found() and trima_libbz2_dep.found())
  trima_flags += '-DTO_SUPPORT_COMPRESSED_INPUT'
  trima_lib_deps += [trima_zlib_dep, trima_libbz2_dep]
endif

if cpp.has_function('clock_gettime')
  dep_clock = []
else
  dep_clock = cpp.find_library('rt')
endif
trima_lib_deps += dep_clock

###########
# headers #
###########

subdir('include')

#####################
# sources + library #
#####################

subdir('src')

#########
# tests #
#########

if not meson.is_subproject()
  # tests
  if get_option('enable-tests')
    subdir('tests')
  endif
endif

###################
# dependency info #
###################

if not meson.is_subproject()
  import('pkgconfig').generate(
    libraries : trima_lib,
    version : meson.project_version(),
    name : 'trima',
    filebase : 'trima',
    description : 'tool to trim poly A tails from PacBio data')
endif

trima_dep = declare_dependency(
  include_directories : trima_include_directories,
  link_with : trima_lib,
  dependencies : dep_clock,
  version : meson.project_version(),
  compile_args : trima_flags)
