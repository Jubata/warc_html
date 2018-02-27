project('warc_html', 'cpp', version: '0.9.0', default_options : ['cpp_std=c++14'])

sources = ['main.cpp', 'html_parser.h', 'html_parser.cpp', 'stats.h', 'stats.cpp']

# libwarc dependency
libwarc_sp = subproject('libwarc') #default_options
libwarc = libwarc_sp.get_variable('libwarc_dep')

# libhttp dependency
http = run_command('make', ['-f', 
  join_paths(meson.current_source_dir(), 'subprojects/http-parser/Makefile'),
  '-C',
  join_paths(meson.current_source_dir(), 'subprojects/http-parser'),
  'package'])

if http.returncode() == 0
  http_inc = include_directories('subprojects/http-parser')
  http_lib = meson.get_compiler('cpp').find_library('http_parser', dirs: join_paths(meson.current_source_dir(), 'subprojects/http-parser'))
else
  error(http.stderr())
endif

http_dep = declare_dependency(dependencies : http_lib,
  include_directories : http_inc)

# netsurf dependencies
buildsystem = run_command('make', ['-f', 
  join_paths(meson.current_source_dir(), 'subprojects/buildsystem/Makefile'),
  '-C',
  join_paths(meson.current_source_dir(), 'subprojects/buildsystem'),
  'PREFIX=' + join_paths(meson.current_build_dir(), 'netsurf'),
  'install'])

if buildsystem.returncode() != 0
  error(buildsystem.stderr())
endif

libparserutils = run_command('make', ['-f', 
  join_paths(meson.current_source_dir(), 'subprojects/libparserutils/Makefile'),
  '-C',
  join_paths(meson.current_source_dir(), 'subprojects/libparserutils'),
  'PREFIX=' + join_paths(meson.current_build_dir(), 'netsurf'),
  'install'])

if libparserutils.returncode() != 0
  error(libparserutils.stderr())
endif

libhubbub = run_command('make', ['-f', 
  join_paths(meson.current_source_dir(), 'subprojects/libhubbub/Makefile'),
  '-C',
  join_paths(meson.current_source_dir(), 'subprojects/libhubbub'),
  'PREFIX=' + join_paths(meson.current_build_dir(), 'netsurf'),
  'install'])

if libhubbub.returncode() != 0
  error(libhubbub.stderr())
endif

hubbub_inc = include_directories(['subprojects/libhubbub/include', 'subprojects/libhubbub/src/tokeniser', 'subprojects/libparserutils/include'])
hubbub_lib = meson.get_compiler('cpp').find_library('hubbub', dirs: join_paths(meson.current_build_dir(), 'netsurf/lib'))
hubbub_dep = declare_dependency(dependencies : hubbub_lib,
  include_directories : hubbub_inc)

parseutils_inc = include_directories(['subprojects/libparserutils/include'])
parseutils_lib = meson.get_compiler('cpp').find_library('parserutils', dirs: join_paths(meson.current_build_dir(), 'netsurf/lib'))
parseutils_dep = declare_dependency(dependencies : parseutils_lib,
  include_directories : parseutils_inc)

#yuarel
yuarel = run_command('make', ['-f', 
  join_paths(meson.current_source_dir(), 'subprojects//libyuarel/Makefile'),
  '-C',
  join_paths(meson.current_source_dir(), 'subprojects/libyuarel'),
  'yuarel'])

if yuarel.returncode() == 0
  yuarel_inc = include_directories(['subprojects/libyuarel'])
  yuarel_lib = meson.get_compiler('cpp').find_library('yuarel', dirs: join_paths(meson.current_source_dir(), 'subprojects/libyuarel'))
else
  error(yuarel.stderr())
endif

yuarel_dep = declare_dependency(dependencies : yuarel_lib,
  include_directories : yuarel_inc)

# jsoncpp dependency
jsoncpp_sp = subproject('jsoncpp') #default_options
jsoncpp = jsoncpp_sp.get_variable('jsoncpp_dep')

#text-csv
textcsv_dep = declare_dependency( include_directories : include_directories(['subprojects/text-csv/include/text']) )

# target executable
executable('warc_html', sources, 
	dependencies: [libwarc, http_dep, hubbub_dep, parseutils_dep, yuarel_dep, jsoncpp, textcsv_dep] )

executable('join_csv', ['join_csv.cpp', 'stats.h', 'stats.cpp'], 
	dependencies: [libwarc, http_dep, hubbub_dep, parseutils_dep, yuarel_dep, jsoncpp,
                 dependency('threads'), textcsv_dep] )
