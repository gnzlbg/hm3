#!/usr/bin/env python
"""Generate HM3's website

Usage:
  generate_website.py [<source_dir>] [options]
  generate_website.py -h | --help

  <source_dir>  Path to the HM3's source directory.

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

def md_files_in(path):
    md_files = []
    for subdir, dirn, files in os.walk(path):
        for f in files:
            if f.endswith('.md') and not 'site' in subdir:
                md_files.append(os.path.join(subdir, f))
    return md_files

def md_to_site_path(md_file_path):
    dir_name = os.path.dirname(md_file_path.replace('./', './site/').replace('include/', ''))
    file_name = os.path.basename(md_file_path).replace('readme', 'index')
    return os.path.join(dir_name, file_name)

def generate_site_file(md_file_path):
    #if file contains readme, replace with index
    site_file_path = md_to_site_path(md_file_path)

    print('mdfp: ' + md_file_path + ' -> ' + site_file_path)

    # copy file to site directory and prepend ---\n---\n to the file
    site_dir = os.path.dirname(site_file_path)
    if not os.path.exists(site_dir):
        os.makedirs(site_dir)
    with file(md_file_path, 'r') as original: data = original.read()
    with file(site_file_path, 'w') as modified: modified.write("---\n---\n" + data)

    # returns the site file path
    return site_file_path

def update_links(md_file_path, site_file_path, md_file_paths):
    # read site file
    with file(site_file_path, 'r') as original: data = original.read()
    # for every possible link
    for fp in md_file_paths:
        # get the correct link
        site_fp = md_to_site_path(fp).replace('site/', '{{ site.baseurl }}')
        # replace with correct link
        data = data.replace(fp, site_fp)
    # write back to disk
    with file(site_file_path, 'w') as modified: modified.write(data)

def init_toc(path):
    toc = {}
    for subdir, dirn, files in os.walk(path):
        if not '.git' in subdir and not 'site' in subdir and not 'CMake' in subdir:
            sd = md_to_site_path(subdir)
            if sd == './site/':
                sd = './site'
            toc[sd] = []
    return toc

def add_to_toc(site_file_path, toc_links):
    sub_dir = os.path.dirname(site_file_path)
    toc_links[sub_dir].append(site_file_path)

def generate_toc(toc_links):
    toc = ''

    # remove root directory from toc:
    toc_links.pop('./site', None)

    # remove empty keys from toc:
    cleansed = copy.deepcopy(toc_links)
    for k,v in toc_links.iteritems():
        if not v:
            cleansed.pop(k, None)
    toc_links = cleansed

    # clear the key names
    for k,v in toc_links.iteritems():
        if './site/hm3' in k:
            toc_links[k.replace('./site/hm3/', '')] = toc_links.pop(k)
        elif './site' in k:
            toc_links[k.replace('./site/', '')] = toc_links.pop(k)
    for k,v in toc_links.iteritems():
        if '/' in k:
            toc_links[k.replace('/', '::')] = toc_links.pop(k)

    # build the toc
    for k,v in sorted(toc_links.iteritems()):
        if len(v) == 1:
            toc = toc + '- [' + k + '](' + v[0].replace('./site', '{{ site.baseurl }}').replace('.md', '.html') + ')\n'
        else:
            toc = toc + '- ' + k + '\n'
            for l in v:
                toc = toc + '- [' + os.path.basename(l) + '](' + l.replace('./site', '{{ site.baseurl }}').replace('.md', '.html') + ')\n'

    # indent the toc
    new_toc = ''
    for l in toc.splitlines():
        indent = l.count('::') * 2
        new_toc = new_toc + (' ' * indent) + l + '\n'

    return new_toc

def generate_site_files(md_files, path):
    toc_links = init_toc(path)
    print('init toc links:')
    print(toc_links)

    for md_file in md_files:
        site_file = generate_site_file(md_file)
        update_links(md_file, site_file, md_files)
        add_to_toc(site_file, toc_links)
    toc = generate_toc(toc_links)
    with file('./site/_includes/directory_structure', 'w') as modified: modified.write(toc)
    print('toc:')
    print(toc)

if __name__ == '__main__':
    src_dir = os.getcwd() + '/'
    version = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])

    # Read cmd line arguments
    args = docopt(__doc__, version='HM3 ' + version)
    verbose = args['--verbose']

    # Check that the source directory is a valid hm3's source directory:
    src_dir = src_dir if args['<source_dir>'] is None else args['<source_dir>'] + '/'
    dirs = [src_dir, src_dir + '/include', src_dir + '/test', src_dir + '/site']
    for d in dirs:
        if not os.path.exists(d):
            print("[E] Generate site script not launched in HM3's source dir!")
            exit(1)

    print('Generating HM3\'s website...')

    md_files = md_files_in(src_dir)
    generate_site_files(md_files, src_dir)
    exit(0)


    # Recursively copy all *.md files into /site:
    for subdir, dirs, files in os.walk(src_dir):
        for f in files:
            if '.md' in f:
                print("md2 file: " + f)
                shutil.copyfile(subdir + '/' + f, src_dir + 'site/' + subdir + '/' + f)
        break

    for subdir, dirs, files in os.walk(src_dir):
        if not ('./include' or './test' or './vis') in subdir:
            continue
        to_dir = src_dir + 'site/' + subdir
        if './include' == subdir:
            continue
        if './include' in subdir:
            tmp = subdir.split('include')
            print tmp
            to_dir = src_dir + 'site/' + tmp[1]
        for f in files:
            if '.md' in f:
                print("md file: " + f)
                print subdir
                if not os.path.exists(to_dir):
                    os.makedirs(to_dir)
                shutil.copyfile(subdir + '/' + f, to_dir + '/' + f)

    # Within the site directory: rename all readme.md files to index.md
    for subdir, dirs, files in os.walk(src_dir + '/site'):
        for f in files:
            if 'readme.md' == f:
                src = subdir + '/' + f
                dst = subdir + '/' + 'index.md'
                print src
                print dst
                os.rename(src, dst)

    # Replace all links to .md files within /site to their corresponding .html files
    for subdir, dirs, files in os.walk(src_dir + '/site'):
        for f in files:
            if '.md' in f:
                tmp = ''
                with open (subdir + '/' + f, "r") as myfile:
                    tmp = myfile.read().replace('.md', '.html')
                with open (subdir + '/' + f, "w") as myfile:
                    myfile.write(tmp)

    # Generate directory structure
    # for all .md files in site/hm3
    # add relative path from site/hm3 to a dictionary
    # sort dictionary by relative path
    # strip site/hm3 from path
    # check if site/_include exists and otherwise create it
    # write _include/directory_structure.md file
    #  for each file in the dictionary in sorted order
       # if the file is called readme.md -> subdir.md
       # if the file is not called readme -> list them under the sub-dir
       # keep track of identation (distance from path begin


    print('...done!')
    exit(0)
