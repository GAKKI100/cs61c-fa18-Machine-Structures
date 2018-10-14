# Script used to combine a .S file importing other .S files into one large .S
# file to be used on Venus. This script strips any whitespace but leaves
# comments in the file so long as they lie in a particular region. All code to
# be excuted must be in .text and all data must be in .data. Each of these
# regions should occur at most once in any particular file. Each file will only
# be included once and checks are made to ensure that labels are unique across
# all files. All output is placed into a single file name a.S which can then be
# uploaded to Venus and excuted properly.

from collections import deque
from enum import Enum
import sys, re

class Region(Enum):
    TEXT = 1
    DATA = 2
    NONE = 3

output_name = "a.S"
label_regex = "^\s*([a-zA-Z0-9_$@.][a-zA-Z0-9_$@]*):.*$"
filename_regex = "[^/]+"
include_regex = "^\s*(\.include)\s*\"(" + filename_regex + ")\"\s*$"
file_include_regex = "\s*(\.file)\s*\"(" + filename_regex +")\"\s*$"
label_checker = re.compile (label_regex)
include_checker = re.compile (include_regex)
file_checker = re.compile (file_include_regex)

labels = dict ()

def exit_with_failure ():
    sys.stderr.flush ()
    sys.exit (1)

def remove_comment (line):
    index = line.find ("#")
    if index == -1:
        return line
    else:
        return line[:index].strip ()

def is_comment (line):
    line_copy = line.lstrip ()
    return line_copy[0] == "#"

def is_empty (line):
    return line.strip () == ""

def is_text (line):
    copy = line.lstrip ().strip ()
    copy = remove_comment (copy)
    return copy == ".text"

def is_data (line):
    copy = line.lstrip ().strip ()
    copy = remove_comment (copy)
    return copy == ".data"

def get_include (line):
    line = remove_comment (line)
    match = include_checker.match (line)
    if match:
        return match.group (2)
    else:
        return None

def add_file (line, filedict):
    line = remove_comment (line)
    match = file_checker.match (line)
    if match:
        filename = match.group (2)
        if filename in filedict:
            return True
        try:
            f = open (filename, "rb")
            file_bytes = []
            byte = f.read (1)
            while byte:
                file_bytes.append (byte)
                byte = f.read (1)
            filedict[filename] = file_bytes
            return True
        except OSError:
            sys.stderr.write ("Unable to include file {}\n".format (curr_name))
            exit_with_failure ()
        f.close ()
        return True
    else:
        return False

def check_labels (line, linenum, filename):
    line = remove_comment (line)
    match = label_checker.match (line)
    if match:
        label = match.group (1)
        if label in labels:
            old_linenum, old_filename = labels[label]
            sys.stderr.write ("Error repeated label. Label is first defined on line {} of {} and is redefined in line {} of {}\n".format (old_linenum, old_filename, linenum, filename))
            exit_with_failure ()
        else:
            labels[label] = (linenum, filename)


def combine_files (filename):
    include_set = set() #Set of files included by the file. There may be many/shared dependancies
    include_set.add (filename)
    text_lines = [] #List of lines to be included in the .text section
    data_lines = [] #List of lines to be included in the .data section
    filedict = {} #Dictionary mapping file names to contents
    files_list = deque () #create a queue of files to process
    files_list.append (filename)
    try:
        while (True):
            curr_name = files_list.pop ()
            try:
                current_file = open (curr_name, "r")
            except OSError:
                sys.stderr.write ("Unable to open file {}\n".format (curr_name))
                exit_with_failure ()
            region = Region.NONE
            data_seen = False #Make sure the .data appears at most once per file
            text_seen = False #Make sure the .text appears at most once per file
            for num, line in enumerate (current_file):
                if is_empty (line):
                    continue
                if add_file (line, filedict):
                    continue
                newfile = get_include (line) #get the name of any file to include if it exists
                if newfile: #check if the line is an import statement, one per line
                    if newfile not in include_set:
                        include_set.add (newfile)
                        files_list.append (newfile)
                elif is_text (line): #Check if the line is .text
                    if text_seen:
                        sys.stderr.write ("Duplicate .text region on line {} of {}\n".format (num + 1, curr_name))
                        exit_with_failure ()
                    else:
                        text_seen = True
                        region = Region.TEXT
                elif is_data (line): #Check if the line is .data
                    if data_seen:
                        sys.stderr.write ("Duplicate .data region on line {} of {}\n".format (num + 1, curr_name))
                        exit_with_failure ()
                    else:
                        data_seen = True
                        region = Region.DATA
                elif region == Region.TEXT: #Check if the current line is in the text section
                    check_labels (line, num + 1, curr_name)
                    text_lines.append (line)
                elif region == Region.DATA: #Check if the current line is in the data section
                    check_labels (line, num + 1, curr_name)
                    data_lines.append (line)
                else: #Line should be either a comment or blank
                    if not is_comment (line):
                        sys.stderr.write ("Invalid entry. Line {} in {} is not a comment or empty line and does not fall in a recognized region\n".format (num + 1, curr_name))
                        exit_with_failure ()
            current_file.close ()
    except IndexError:
        output_file = open (output_name, "w+")
        output_file.write (".data\n")
        filenames = list (filedict.keys ())
        for filename in filenames:
            output_file.write (".{}:\n".format (filename))
            output_file.write (".byte ")
            for byte in filedict[filename]:
                byte_value = byte.hex ()
                output_file.write ("0x{} ".format (byte_value))
            output_file.write ("\n.{0}_name: .asciiz \"{0}\"\n".format (filename))
        output_file.write (".files:\n")
        output_file.write (".word {} ".format (len (filenames)))
        for filename in filenames:
            output_file.write ("0 0 -1 ".format (filename))
        output_file.write ("\n")
        for line in data_lines:
            output_file.write (line)
        output_file.write (".text\n")
        output_file.write ("la t0 .files\n")
        counter = 4
        for filename in filenames:
            output_file.write ("la t1 .{}_name\n".format (filename))
            output_file.write ("sw t1 {}(t0)\n".format (counter))
            output_file.write ("la t1 .{}\n".format (filename))
            output_file.write ("sw t1 {}(t0)\n".format (counter + 4))
            counter += 12
        for line in text_lines:
            output_file.write (line)
        output_file.flush ()
        output_file.close ()

def main():
    if len (sys.argv) != 2:
        sys.stderr.write ("Usage: python combine.py <filename>\n")
        exit_with_failure ()
    combine_files (sys.argv[1])

if __name__ == "__main__":
    main ()
