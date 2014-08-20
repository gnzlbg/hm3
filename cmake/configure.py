#!/usr/bin/env python

"""Configures HM3's build.

Usage:
  configure.py [<mode>] [options] [<build_path>]
  configure.py -h | --help
  configure.py --version

  <mode>                Build mode [default: release (simulation ready)].
  <build_path>          Build directory [default: repository/build].

Build modes:
  asan                  Builds with Address Sanitizer.
  coverage              Builds with code-coverage analysis.
  debug                 Builds in optimized debug mode.
  debug-unopt           Builds in unoptimized debug mode.
  release               Builds in release mode.
  release-assertions    Builds in release mode with assertions.

Build options:
  -h --help             Show help (i.e. this screen).
  --version             Show version.
  --verbose             Verbose output.

  --asan                With Address Sanitizer.
  --assertions          With assertions.
  --debug               Debug mode.
  --coverage            Generates code coverage information.
  --debug-info          With debug information.
  --msan                With Memory Sanitizer.
  --warnings-as-errors  Warnings are treated as errors.
  --profile             With profile information.
  --with-stack-tracer   With stack-tracer.
  --with-vis            With visualization tools.
  --with-vtk            With VTK library.

  --libcxx_i=<include_path>     Include path of libc++.
  --libcxx_l=<library_path>     Library path of libc++.
  --boost_i=<include_path>      Include path of boost.
  --boost_root=<root_path>      Root path of boost libraries.
  --with-paraview=<build_path>  Build path of ParaView.
  --without-libcxx              Use the default std lib of the system.

HM3 options:
  --no-likely     Disable LIKELY/UNLIKELY branches hints.
  --no-restrict   Disable restrict


"""
from docopt import docopt
import os
import platform
import shutil
import subprocess
import copy

flags = {'--asan'               : 'HM3_WITH_ASAN',
         '--assertions'         : 'HM3_WITH_ASSERTIONS',
         '--coverage'           : 'HM3_WITH_COVERAGE',
         '--debug-info'         : 'HM3_WITH_DEBUG_INFO',
         '--msan'               : 'HM3_WITH_MSAN',
         '--warnings-as-errors' : 'HM3_WITH_WARNINGS_AS_ERRORS',
         '--verbose'            : 'HM3_VERBOSE_CONFIGURE',
         '--profile'            : 'HM3_WITH_PROFILE',
         '--with-stack-tracer'  : 'HM3_WITH_STACK_TRACER',
         '--with-vis'           : 'HM3_WITH_VISUALIZATION_TOOLS',
         '--with-vtk'           : 'HM3_WITH_VTK',
         '--with-polly'         : 'HM3_WITH_POLLY',
         '--no-likely'          : 'HM3_NO_LIKELY',
         '--no-restrict'        : 'HM3_NO_RESTRICT'
      }

# Release mode flags:
release = { '--asan'               : False,
            '--assertions'         : False,
            '--debug'              : False,
            '--coverage'           : False,
            '--debug-info'         : False,
            '--warnings-as-errors' : False,
            '--verbose'            : False,
         }

release_assertions = copy.deepcopy(release)
release_assertions['--assertions'] = True

# Debug mode flags:
debug = { '--asan'               : False,
          '--assertions'         : True,
          '--debug'              : False,
          '--coverage'           : False,
          '--debug-info'         : True,
          '--warnings-as-errors' : False,
          '--verbose'            : False,
       }

# Debug unopt mode flags:
debug_unopt = copy.deepcopy(debug)
debug_unopt['--debug'] = True

# Asan mode flags:
asan = copy.deepcopy(release)
asan['--asan'] = True

# Msan mode flags:
msan = copy.deepcopy(release)
msan['--msan'] = True

# Commit mode flags:
commit = copy.deepcopy(asan)
commit['--warning-as-errors'] = True

# Coverage mode flags:
coverage = copy.deepcopy(debug_unopt)
coverage['--coverage'] = True

mode_flags = { 'asan'               : asan,
               'commit'             : commit,
               'coverage'           : coverage,
               'debug'              : debug,
               'debug-unopt'        : debug_unopt,
               'msan'               : msan,
               'release'            : release,
               'release-assertions' : release_assertions
}

def value(k, v, mode):
   if mode is None:
      return 'On' if v else 'Off'
   else:
      return 'On' if v or mode_flags.get(mode, 'release').get(k, v) else 'Off'

def cmake_options(args):
   mode = args['<mode>']
   options=''
   for k,v in args.iteritems():
      try:
         options += ' -D' + flags[k] + '=' + value(k, v, mode)
      except Exception:
         pass
   return options

if __name__ == '__main__':
   # Read environment variables
   src_dir = os.getcwd() + '/'
   version = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])

   # Read cmd line arguments
   args = docopt(__doc__, version='HM3 ' + version)
   verbose = args['--verbose']

   debug = args['--debug']
   if debug:
      del args['--debug']

   print('Configuring HM3...')

   if verbose:
      print('Detected platform: ' + platform.system())

   # Set up the build dir: use either user provided path or default path
   if args['<build_path>'] is None:
      build_dir = src_dir + 'build'
   else:
      build_dir = args['<build_path>']
   if not os.path.exists(build_dir):
      os.makedirs(build_dir)
   else:
      shutil.rmtree(build_dir)
      os.makedirs(build_dir)

   # Set up libc++
   if not args['--without-libcxx']:
      if args['--libcxx_i'] is None:
         if 'LIBCXX_INCLUDE' in os.environ:
            libcxx_i = os.environ['LIBCXX_INCLUDE']
      else:
         libcxx_i = args['--libcxx_i']

      if args['--libcxx_l'] is None:
         if 'LIBCXX_LIB' in os.environ:
            libcxx_l = os.environ['LIBCXX_LIB']
      else:
         libcxx_l = args['--libcxx_l']

   if args['--with-vis']:
      if args['--with-paraview'] is not None:
         paraview_dir = args['--with-paraview']
      elif 'PARAVIEW_SRC_DIR' in os.environ:
         paraview_dir = os.environ['PARAVIEW_SRC_DIR']
      else:
         print('[WARNING]: --with-vis enabled but neither:\n'
               + '    - --with-paraview=\'SRC_DIR\', nor\n'
               + '    - the \'PARAVIEW_SRC_DIR\' environment variable\n'
               + ' are provided. Use one of these if CMake cannot find ParaView.')

   if args['--asan'] and args['--msan']:
      print('[ERROR]: cannot enable multiple sanitizers at the same time')
      exit(1)


   # Move to the build dir:
   os.chdir(build_dir)

   # Build up the cmake command:
   cmake_cmd = ('cmake ' + src_dir)

   if debug:
      cmake_cmd = cmake_cmd + ' -DCMAKE_BUILD_TYPE=Debug'
   else:
      cmake_cmd = cmake_cmd + ' -DCMAKE_BUILD_TYPE=Release'

   if args['--boost_i'] is not None:
      cmake_cmd = cmake_cmd + ' -DBOOST_INCLUDEDIR=\"' + args['--boost_i'] + '\" '
   if args['--boost_root'] is not None:
      cmake_cmd = cmake_cmd + ' -DBOOST_ROOT=\"' + args['--boost_root'] + '\" '
   if verbose:
      cmake_cmd = cmake_cmd + ' -DBoost_DEBUG=On '

   # Standard library: use libcxx unless opt-out
   if not args['--without-libcxx']:
      cmake_cmd = cmake_cmd + (' -DCMAKE_CXX_COMPILE_FLAGS="-nostdinc++" '
                               + ' -DCMAKE_CXX_FLAGS="-stdlib=libc++ -I' + libcxx_i + '"'
                               + ' -DCMAKE_EXE_LINKER_FLAGS="-L' + libcxx_l + '" ')

   # Platform specific:
   if platform.system() == 'MacOSX':
      cmake_cmd = cmake_cmd + ' -DCMAKE_MACOSX_RPATH=On '

   if 'CXX' in os.environ:
      cmake_cmd = cmake_cmd + ' -DCMAKE_CXX_COMPILER="' + os.environ['CXX'] + '" '

   try:
      if args['--with-vis']:
         cmake_cmd = cmake_cmd + ' -DParaView_DIR="' + paraview_dir + '" '
   except NameError:
      print("[WARNING]: ParaView_DIR not defined (define it if CMake can't find ParaView)")

   cmake_cmd = cmake_cmd + cmake_options(args)

   # Run cmake
   if verbose:
      print('[I] CMake CMD: ' + cmake_cmd)
   p = subprocess.Popen(cmake_cmd, universal_newlines=False, shell=True,
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)

   out, err = p.communicate('foo\nfoofoo\n')
   if verbose:
      print out
      print err
   if p.returncode == 0:
      print('... done!')
   else:
      print('[E] Configuring failed!')
   exit(p.returncode)
