#!/usr/bin/env python
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
"""Creates a module.modulemap file containing one module per header file.

Usage:
  modularize.py <project_include_path> [options]
  modularize.py -h | --help
  modularize.py --version

  <project_include_path>   Path to the project's include directory.

Options:
  -h --help  Show this screen.

"""
from docopt import docopt
import os
import subprocess
from threading import Thread

file_extensions = ['.hpp']

def run_clang_format(clang_format, path, apply_format, verbose, results):
    if apply_format:
        cmd = clang_format + ' -style=file -i ' + path
    else:
        cmd = clang_format + ' -style=file ' + path + ' | diff -u ' + path + ' - '

    if verbose: print('[clang_format cmd]: "{0}"'.format(cmd))

    p = subprocess.Popen(cmd, universal_newlines=False, shell=True,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    out, err = p.communicate()
    if len(out) > 0:
        print out
    if len(err) > 0:
        print err

    if not p.returncode == 0 or len(err) > 0 or len(out) > 0:
        if verbose: print('failed!')
        results.append(False)
    else:
        if verbose: print('success!')
        results.append(True)

def run(clang_format_path, file_paths, apply_format, verbose):
    threads = []
    results = []
    for p in file_paths:
        _, ext = os.path.splitext(p)
        if ext in file_extensions:
            threads.append(Thread(target=run_clang_format, args=(clang_format_path, p, apply_format, verbose, results)))
    [t.start() for t in threads]
    [t.join() for t in threads]
    return all(results)

def main():
    args = docopt(__doc__)

    project_include_path = args['<project_include_path>']

    module_files = []
    module_names =[]

    for dir_, dirs_, files_ in os.walk(project_include_path):
        for fileName in files_:
            _, ext = os.path.splitext(fileName)
            if ext not in file_extensions:
                continue
            relDir = os.path.relpath(dir_, project_include_path)
            relFile = os.path.join(relDir, fileName)
            module_files.append(relFile)
            modname = relFile.replace('/', '_').replace('.hpp', '').replace('-', '_')
            module_names.append(modname)

    for n, f in zip(module_names, module_files):
        print 'module ' + n + ' { header "' + f + '" export * }'

if __name__ == '__main__':
    main()
