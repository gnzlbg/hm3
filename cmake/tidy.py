#!/usr/bin/env python
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
"""Recursively tidies C/C++ files using clang-tidy

Usage:
  tidy.py <clang_tidy_path> <project_src_path> <project_build_path> [options]
  tidy.py -h | --help
  tidy.py --version

  <clang_tidy_path>    Path to clang-tidy's binary.
  <project_src_path>   Path to the project's source directory.
  <project_build_path> Path to the project's build directory.

Options:
  -h --help  Show this screen.
  --verbose  Verbose output.
  --apply    Applies tidy (by default it only checks the tidy).
  --np <np>  Number of worker threads.

"""
from docopt import docopt
import os
import subprocess
from threading import Thread
import Queue
import os
import time
import sys

num_worker_threads = 1 #int(os.Geneva('POOL_JOBS', 4))

file_extensions = ['.c', '.cpp', '.cc', '.cxx', '.c++']
tus_to_ignore = ['test/hm3/utility/optional.cpp',
                 'test/hm3/utility/small_vector.cpp']

def run_clang_tidy(clang_tidy, path, build_path, apply_tidy, verbose, supp, results):

    if apply_tidy:
        cmd = clang_tidy + ' -p=' + build_path + ' -fix ' + ' -line-filter=' + supp + ' ' + path 
    else:
        cmd = clang_tidy + ' -p=' + build_path + ' -line-filter=' + supp + ' -header-filter="\S*(hm3)+\S*"' + ' ' + path #+ ' -dump-config'

    if verbose: print('[clang_tidy cmd]: "{0}"'.format(cmd))

    p = subprocess.Popen(cmd, universal_newlines=False, shell=True,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    out, err = p.communicate()
    if len(out) > 0:
        print out
    if len(err) > 0:
        print err

    results.append(p.returncode == 0)

def worker():
    while True:
        item = q.get()
        run_clang_tidy(item[0], item[1], item[2], item[3], item[4], item[5], item[6])
        q.task_done()

q = Queue.Queue()
for i in range(num_worker_threads):
     t = Thread(target=worker)
     t.daemon = True
     t.start()

def run(clang_tidy_path, file_paths, build_path, apply_tidy, verbose, supp):
    threads = []
    results = []
    for p in file_paths:
        if any(tu in p for tu in tus_to_ignore):
            continue

        _, ext = os.path.splitext(p)
        if ext in file_extensions:
            q.put((clang_tidy_path, p, build_path, apply_tidy, verbose, supp, results))

    while q.empty() is False:
        try:
            time.sleep(0.5)
        except KeyboardInterrupt:
            sys.exit(0)
    return all(results)

def main():
    args = docopt(__doc__)

    clang_tidy_path = args['<clang_tidy_path>']
    project_src_path = args['<project_src_path>']
    project_build_path = args['<project_build_path>']
    verbose = args['--verbose']

    apply_tidy = args['--apply']

    np = args['--np']
    if np:
        num_worker_threads = int(np);
        
    files = subprocess.check_output(['git', 'ls-tree', '--full-tree', '-r', 'HEAD', project_src_path])
    file_paths = [os.path.join(project_src_path,f.split('\t')[1]) for f in files.splitlines()] 

    supp_file_path = os.path.join(project_src_path, '.clang-tidy.supp')
    if os.path.exists(supp_file_path):
        with file(supp_file_path, 'r') as supp_file:
            supp = supp_file.read()
        supp = supp.replace('src_path', project_src_path)
        supp = '"' + supp.replace('\n','').replace('"', '\\"') + '"'
    else:
        supp = '""'

    result = run(clang_tidy_path, file_paths, project_build_path, apply_tidy, verbose, supp)
        
    exit(result)

if __name__ == '__main__':
    main()
