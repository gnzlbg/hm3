#!/usr/bin/env python
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
"""Recursively formats C/C++ files checked into a git repository (using clang-format).

Usage:
  format.py <clang_format_path> <project_src_path> [options]
  format.py -h | --help
  format.py --version

  <clang_format_path>  Path to clang-format's binary.
  <project_src_path>   Path to the project's source directory (must be a git repo).

Options:
  -h --help  Show this screen.
  --verbose  Verbose output.
  --apply    Applies format (by default it only checks the format).
  --np <np>  Number of worker threads.

"""
from docopt import docopt
import os
import subprocess
from threading import Thread
import Queue
import os

num_worker_threads = int(os.getenv('POOL_JOBS', 8))

file_extensions = ['.c', '.h', '.cpp', '.cc', '.cxx', 
                   '.hpp', '.hh', '.hxx', '.c++', '.h++']

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
        if verbose: print("failed!")
        results.append(False)
    else:
        if verbose: print("success!")
        results.append(True)

def worker():
    while True:
        item = q.get()
        run_clang_format(item[0], item[1], item[2], item[3], item[4])
        q.task_done()

q = Queue.Queue()
for i in range(num_worker_threads):
     t = Thread(target=worker)
     t.daemon = True
     t.start()

def run(clang_format_path, file_paths, apply_format, verbose):
    threads = []
    results = []
    for p in file_paths:
        _, ext = os.path.splitext(p)
        if ext in file_extensions:
            q.put((clang_format_path, p, apply_format, verbose, results))
    q.join()
    return all(results)

def main():
    args = docopt(__doc__)

    clang_format_path = args['<clang_format_path>']
    project_src_path = args['<project_src_path>']
    verbose = args['--verbose']

    files = subprocess.check_output(['git', 'ls-tree', '--full-tree', '-r', 'HEAD', project_src_path])
    file_paths = [os.path.join(project_src_path,f.split('\t')[1]) for f in files.splitlines()] 

    apply_format = args['--apply']

    np = args['--np']
    if np:
        num_worker_threads = int(np);

    result = run(clang_format_path, file_paths, apply_format, verbose)

    if apply_format: # If format was applied: check format and use that as result
        result = run(clang_format_path, file_paths, False, verbose)

    if verbose:
        if result:
            print("finished with success!") 
        else:
            print("finished with failed!")
    if result:
        exit(0)
    else:
        exit(1)
        

if __name__ == '__main__':
    main()
