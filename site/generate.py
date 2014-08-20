#!/usr/bin/env python
"""Generate HM3's website

Usage:
  generate_website.py <source_dir> <build_dir> [options]
  generate_website.py -h | --help

  <source_dir>  Path to the HM3's source directory.
  <build_dir>   Path to the HM3's build directory.

Options:
  -h --help     Show this screen.
  --verbose     Verbose output.

  --serve       Starts a server to view the site locally.

"""
from docopt import docopt
import subprocess
import os
import shutil
import copy
import json


# Path of an md file from tree to /site/src
def path_in_site(md_file_path):
    dir_name = os.path.dirname(os.path.join('site/src/',md_file_path).replace('include/', ''))
    file_name = os.path.basename(md_file_path).replace('readme', 'index')
    return os.path.join(dir_name, file_name)

# Copies a file from the tree to the site/src directory and updates its links
def copy_to_site(md_file_path, md_file_paths):
    site_path = path_in_site(md_file_path)

    print('    - ' + md_file_path + ' -> ' + site_path)

    # copy file to site directory and prepend ---\n---\n to the file
    site_dir = os.path.dirname(site_path)
    if not os.path.exists(site_dir):
        os.makedirs(site_dir)
    with file(md_file_path, 'r') as original: data = original.read()

    # Correct links to other md files
    for fp in md_file_paths:
        # get the correct link
        corrected_fp = path_in_site(fp).replace('site/src/', '{{ site.baseurl }}')
        # replace with correct link
        data = data.replace(fp, corrected_fp)

    with file(site_path, 'w') as modified: modified.write("---\n---\n" + data)

def generate_toc(starting_point):
    toc = ''
    start_count = starting_point.count(os.path.sep)

    def indent(topdir, count = 0):
        return ' ' * 2 * (topdir.count(os.path.sep) + count - start_count) + '- '
    def link(topdir, name, path, count = 0):
        return indent(topdir, count) + '[' + name + ']({{ site.baseurl }}/' + path + ')\n'

    for topdir, subdirs, files in os.walk(starting_point):
        dn = os.path.basename(topdir)
        if os.path.basename(topdir) == 'hm3':
            toc = toc + link(topdir, dn, 'documentation')
        elif 'index.md' in sorted(files):
            toc = toc + link(topdir, dn, topdir)
        else:
            toc = toc + indent(topdir) + dn + '\n'
        for f in sorted(files):
            if f == 'index.md':
                continue
            m = f.split('.md')[0] + '.html'
            p = os.path.join(topdir, m)
            toc = toc + link(topdir, m, p, 1)
    return toc

if __name__ == '__main__':
    # Read cmd line arguments
    version = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])
    args = docopt(__doc__, version='HM3 ' + version)
    verbose = args['--verbose']

    print('Generating HM3\'s website...')
    source_dir = args['<source_dir>']
    build_dir = args['<build_dir>']
    print('  - source_dir: ' + source_dir)
    print('  - build_dir: ' + build_dir)
    site_dir = os.path.join(source_dir, 'site/src')
    print('  - site_dir: ' + site_dir)

    # Get all the site files (TODO: remove this, not necessary)
    site_files = subprocess.check_output(['git', 'ls-files', site_dir]).strip().split('\n')
    print('  - site source files:')
    for f in site_files:
        print('    - ' + f)

    # Get all the md files in tree
    md_files = [f for f in subprocess.check_output(['git', 'ls-files', source_dir]).strip().split('\n') if os.path.splitext(f)[1] == '.md' and not 'site' in f]

    print('  - copying md files in tree to site:')
    for f in md_files:
        copy_to_site(f, md_files)

    def toc_link(f):
        return ' {{  site.baseurl }}/' + f.replace('md', 'html')

    print(' - toc files:')
    cd = os.getcwd()
    tocd = os.path.join(cd, 'site/src')
    os.chdir(tocd)
    toc = generate_toc('hm3')
    os.chdir(cd)
    print(toc)

    includes_dir = os.path.join(site_dir, '_includes')
    if not os.path.exists(includes_dir):
        os.makedirs(includes_dir)
    toc_path = os.path.join(includes_dir, 'directory_structure')
    with file(toc_path, 'w') as toc_file:
        toc_file.write(toc)

    print('...done!')
    exit(0)
