#!/usr/bin/env python
"""Reads HM3 session files.

Usage:
  dump.py <file_path> [options]
  dump.py -h | --help
  dump.py --version

  <file_name>           HM3 session file path.

Options:
  -h --help             Show help (i.e. this screen).
  --version             Show version.

Interactive mode:
  -i                    Enables interactive mode (batch mode is default).

Batch mode:
  --block=<block_name>  Block name.
  --file=<file_idx>     File index.
  --field=<field_name>  Field name.
  --dump-array-data     Dumps field array data (headers dumped by default).


"""
from docopt import docopt
import os
import json
import subprocess
import textwrap
import struct

def valid_extension(file_extension):
    return file_extension == ".hm3_session"

def json_dump(json_object):
    print json.dumps(json_object, sort_keys=True,
                     indent=2, separators=(',', ': '))

def is_constant(field, fields):
    f = fields[field]
    return type(f) is not dict

def is_array(field, fields):
    return not is_constant(field, fields)

def field_type(field, fields):
    if is_constant(field, fields):
        return 'constant'
    else:
        return 'array of type: ' + fields[field]['type']
  
def constant_field_value(field, fields):
    assert(is_constant(field, fields))
    return fields[field]

def array_field_size(field, fields):
    assert(not is_constant(field, fields))
    return int(fields[field]["size"])

def array_field_description(field, fields):
    assert(not is_constant(field, fields))
    return field_type(field, fields) + ', size: ' + str(array_field_size(field, fields))

def field_header(field, fields):
    """Returns field name and type
       - for constant fields: returns value
       - for array fields: returns array size
    """
    if is_constant(field, fields):
        return field, field_type(field, fields), constant_field_value(field, fields)
    else:
        return field, field_type(field, fields), array_field_size(field, fields)
    
def format_field_header(field, fields):
    name, type, _ = field_header(field, fields)
    if is_constant(field, fields):
        return name + ' (' + type + '): ' + str(_)
    else:
        return name + ' (' + type + ', ' + str(_) + ')'

def format_field_type(field, fields):
    name, type, _ = field_header(field, fields)
    if is_constant(field, fields):
        return name + ' (' + type + ')'
    else:
        return name + ' (' + type + ', ' + str(_) + ')'

def number_of_files_in_block(block):
    return len(block['files'])

def array_data_info(dir, session, block_name, file_idx, field_name):
    fields = session[block_name]["files"][file_idx]["fields"]

    fp = dir + '/' + block_name + '_' + str(file_idx)
    b_from = fields[field_name]["in_file"][0]
    b_to = fields[field_name]["in_file"][1]
    size = fields[field_name]["size"]
    data_type = fields[field_name]["type"]
    return fp, b_from, b_to, size, data_type
            

def get_array_from_file(fp, b_from, b_to, size, data_type):
    data_type_size = (b_to - b_from) / size
    fh = open(fp, "rb")
    result = []
    print ("Reading " + str(size) + " " + data_type + "s (bytes: "
           + str(data_type_size) + ") from file: " + fp)
    for i in range(0, size):
        fh.seek(b_from + i * data_type_size, 0)
        v = fh.read(data_type_size)
        if (len(v) != data_type_size):
            print("error: len(fh.read(data_type_size)) is " + str(len(v)) + " != data_type_size " + str(data_type_size))
            print("dumping result:")
            print(result)
        if data_type == "integer" and data_type_size == 4:
            result.append(struct.unpack('i', v)[0])
        elif data_type == "integer" and data_type_size == 8:
            result.append(struct.unpack('q', v)[0])
        elif data_type == "number" and data_type_size == 4:
            result.append(struct.unpack('f', v)[0])
        elif data_type == "number" and data_type_size == 8:
            result.append(struct.unpack('d', v)[0])
        elif data_type == "boolean" and data_type_size == 1:
            result.append(struct.unpack('?', v)[0])
        else:
            print("unknown data type " + data_type)
            exit(0)
    return result

def pretty_print_session_blocks(formater, session):
    for block in session:
        print(formater.fill(block))

def pretty_print_session(session):
    l1 = textwrap.TextWrapper(initial_indent='  - ')
    l2 = textwrap.TextWrapper(initial_indent='    - ')
    print('Blocks:')
    pretty_print_session_blocks(l1, session)
    print('')
    for block_name in session:
        print('block \"' + block_name + '\":')
        block = session[block_name]

        # print sub fields
        for f in block:
            if f ==  "name": # skip name
                continue
            if f == "files": 
                no_files = number_of_files_in_block(block)
                print(l1.fill("#files: " + str(no_files)))
                if (no_files > 0):  # print first file field types
                    file_fields = block[f][0]["fields"]
                    no_fields = len(file_fields)
                    print(l1.fill("#fields: " + str(no_fields)))
                    if (no_fields > 0):
                        for ff in file_fields:
                            print(l2.fill(format_field_type(ff, file_fields)))
                continue

            print(l1.fill(f + u": " + block[f]))
            print("")

def print_array_from_file(fp, b_from, b_to, size, data_type):
    result = get_array_from_file(fp, b_from, b_to, size, data_type)

    count = 0
    for i in result:
        print(str(count) + ': ' + str(i))
        count = count + 1
    return

def interactive_mode(args):
    print('Reading session \"' + file_path + '\" in interactive mode:\n')

    session = None
    with open(file_path, 'r') as session_file:
        session = json.load(session_file)

    #json_dump(session)
    pretty_print_session(session)
    w = textwrap.TextWrapper(initial_indent='  ')
    not_done = True
    while(not_done):
        i = raw_input('\nInput required:\n  To print a field enter: <block_name> <file_idx> <field_name> [--d (prints the data)]\n  To print the session header press "b"\n  To quit press "q"\n')
        args = i.split()
        if len(args) is 1:
            if args[0] is 'q':
                not_done = False
            elif args[0] is 'b':
                pretty_print_session(session)
                continue
            else:
                print('unknown command')
                continue
        if len(args) < 3 and not len(args) is 1:
            print('try again')
            continue
        block = args[0]
        file_idx = int(args[1])
        field_name = args[2]
        if block not in session:
            print('Block \"' + block + '\" not found!')
            continue
        if field_name not in session[block]["files"][file_idx]["fields"]:
            print('Field \"' + field_name + '\" not found in block \"' + block + '\"!')
            continue
        fields = session[block]["files"][file_idx]["fields"]
        if len(args) is 3:
            print(w.fill(format_field_header(field_name, fields)))
        if len(args) is 4:
            fp, b_from, b_to, size, data_type = array_data_info(dir, session, block, file_idx, field_name)
            print_array_from_file(fp, b_from, b_to, size, data_type)

def batch_mode(args):
    block = args['--block']
    file_k = args['--file']

    field = args['--field']
    array_data = args['--dump-array-data']

    if block is None:
        print('Batch mode error: missing block name: "--block=name"')
        exit(1)
        
    if file is None:
        print('Batch mode error: missing file index: "--file=idx"')
        exit(1)

    file = int(file_k)

    if field is None:
        print('Batch mode error: missing field name: "--field=name"')
        exit(1)

    session = None
    with open(file_path, 'r') as session_file:
        session = json.load(session_file)


    if block not in session:
        print('Batch mode error: block "' + block + '" not found in session.')
        print('Avaliable blocks:')
        l1 = textwrap.TextWrapper(initial_indent='  - ')
        pretty_print_session_blocks(l1, session)
        exit(1)

    block_j = session[block]

    if file >=  number_of_files_in_block(block_j):
        print('Batch mode error: file index "' + str(file) +
              '" is out of bounds (#of files = "'
              + str(number_of_files_in_block(block_j)) + '").')
        exit(1)

    file_fields = block_j['files'][file]['fields']
    if field not in file_fields:
        print('Batch mode error: field "' + field + '" not found in block "' + block + '" file "' + str(file) + '".')
        print('The available fields on that file are:')
        l1 = textwrap.TextWrapper(initial_indent='  - ')
        for field in file_fields:
            print(l1.fill(format_field_header(field, file_fields)))
        exit(1)

    if array_data and is_constant(field, file_fields):
        print('Batch mode error: field "' + field + '" is not an array and --dump-array-data is enabled ".')
        exit(1)


    if not array_data:
        print(format_field_header(field, file_fields))
    else:
        fp, b_from, b_to, size, data_type = array_data_info(dir, session, block, file, field)
        array = get_array_from_file(fp, b_from, b_to, size, data_type)
        for a in array:
            print(a)

    return

if __name__ == '__main__':
    # Read environment variables
    src_dir = os.getcwd() + '/'
    version = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])

    # Read cmd line arguments
    args = docopt(__doc__, version='HM3 Dump ' + version)

    if '<file_path>' not in args:
        print("Error: missing session file path")
        exit(1)

    file_path = args['<file_path>']

    dir, file_name = os.path.split(file_path)
    if dir == '':
        dir ='.'

    base_file_name, file_extension = os.path.splitext(file_name)

    if not valid_extension(file_extension):
        print('[ERROR]: invalid file extension ', file_extension,
              '\n  valid file extension is \".hm3_session\"')

    

    if args['-i']:
        interactive_mode(args)
    else:
        batch_mode(args)

