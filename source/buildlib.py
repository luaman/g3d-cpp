#
# Common routines for python build scripts
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2003-02-15
# @edited  2003-02-20
#
# This script is written in Python.  Be aware that
# whitespaces (particularly, indentation and newlines) 
# are significant in Python when editing the file.
#

import sys
import string
import os
import os.path
import fileinput
import tempfile
import shutil
import re

###############################################################################

"""
Reads the command line arguments (sys.argv) and dispatches to the appropriate
build target by name.

validTargets - A list of functions that are build targets.
help         - A build target to execute if no target is specified
"""
def dispatchOnTarget(validTargets, help):

    # Execute the appropriate target:
    if len(sys.argv) == 1:
        # No arguments; print help
        help()
        sys.exit(-1)

    targets = sys.argv[1:]

    for target in targets:
        found = False
        for v in validTargets:
            if (target == v.func_name):
                v()
                found = 1
                break

        if (not found):
            print '"' + target + '" is not a valid build target (type "build help" to see all targets).'
            sys.exit(-1)

    sys.exit(0)


###############################################################################

"""Regular explression patterns that will be excluded from copying by 
    copyIfNewer."""
_excludePatterns =\
    ['\.ncb$', \
    '\.opt$', \
    '\.ilk$', \
    '\.pdb$', \
    '\.bsc$', \
    '\.o$', \
    '\.obj$', \
    '\.pyc$', \
    '\.plg$', \
    '^#.*#$', \
    '~$', \
    '\.old$' \
    '^log.txt$', \
    '^stderr.txt$', \
    '^stdout.txt$', \
    '\.log$', \
    '^graveyard$', \
    '^CVS$', \
    '^Debug$', \
    '^Release$']

"""
A regular expression matching files that should be excluded from copying.
"""
excludeFromCopying   = re.compile(string.join(_excludePatterns, '|'))

###############################################################################
"""Create a directory if it does not exist."""
def mkdir(path):
    if (not os.path.exists(path)):
        print 'mkdir ' + path
        os.makedirs(path)

def rmdir(path):
    if (os.path.exists(path)):
        print 'rmdir ' + path
        shutil.rmtree(path, 1)

###############################################################################

def removeTrailingSlash(s):
    if (s[len(s)-1] == '/'):
        s = s[:len(s)-2]
    if (s[len(s)-1] == '\\'):
        s = s[:len(s)-2]
    return s

""" Recursively copies all contents of source to dest 
(including source itself) that are out of date.  Does 
not copy files matching the excludeFromCopying patterns.
"""
def copyIfNewer(source, dest):
    dest = removeTrailingSlash(dest)

    if (not os.path.isdir(source) and newer(source, dest)):
        print 'cp ' + source + ' ' + dest
        shutil.copyfile(source, dest)
        
    else:

        # Walk is a special iterator that visits all of the
        # children and executes the 2nd argument on them.  

        os.path.walk(source, _copyIfNewerVisit, [len(source), dest])
    

"""Helper for copyIfNewer.

args is a list of the length of the source prefix in 
sourceDirname and the root of the destination tree.

"""
def _copyIfNewerVisit(args, sourceDirname, names):

    prefixLen      = args[0]
    destDirname = args[1] + sourceDirname[prefixLen:]

    # Create the corresponding destination dir if necessary
    mkdir(destDirname)

    # Iterate through the contents of this directory   
    for name in names:
        source = sourceDirname + '/' + name

        if (excludeFromCopying.search(name) != None):
            # Remove excluded dirs and files
            names.remove(name)

        elif (not os.path.isdir(source)):
            # Copy files if newer
            dest = destDirname + '/' + name
            if (newer(source, dest)):
                print 'cp ' + source + ' ' + dest
                shutil.copyfile(source, dest)

###############################################################################
"""Determine if a target is out of date.

Returns nonzero if file1 is newer than file2.
Throws an error if file1 does not exist, returns
nonzero if file2 does not exist."""
def newer(file1, file2):
   time1 = os.path.getmtime(file1)
   time2 = 0
   try:
       time2 = os.path.getmtime(file2)
   except OSError:
       time2 = 0
       
   return time1 >= time2


###############################################################################

"""
Finds an executable on Windows."""
def _findBinary(program):     
    # Paths that may contain the program

    PATH = os.getenv('PATH', '').split(';') + \
          ['.',\
           'C:/Program Files/Microsoft Visual Studio/Common/MSDev98/Bin',\
           'C:/python',\
           'C:/doxygen/bin',\
           'C:\Program Files\PKZIP']

    for path in PATH:
        filename = path + '/' + program + '.exe'
        if (os.path.exists(filename)):
            return filename
            break

        filename = path + '/' + program + '.com'
        if (os.path.exists(filename)):
            return filename
            break

    raise 'Error', 'Cannot find "' + program + '"'
    return program

###############################################################################

"""Runs MSDEV on the given dsw filename and builds the 
specified configs.  configs is a list of strings
"""
def msdev(filename, configs):
    binary = 'msdev'

    logfile = tempfile.mktemp()
    args = [filename]

    for config in configs:
        args.append('/MAKE')
        args.append('"' + config + '"')

    args.append('/OUT')
    args.append(logfile)

    x = run(binary, args)
  
    # Print the output to standard out
    for line in fileinput.input(logfile):
        print line.rstrip('\n')
 
    return x

###############################################################################
"""
 Recursively zips the source into zipfile
"""
def zip(source, zipfile):
    print 'Creating ' + zipfile

    if (os.name == 'nt'):
        run('pkzip25', ['-add', '-rec', '-lev=9', '-path=relative', zipfile, source])
    else:
        run('zip', [])

###############################################################################
"""Convert path separators to local style from Unix style.

s is a string that contains a path name."""
def to_local_path(s):
    return string.replace(s, '/', os.sep)

###############################################################################

def removeQuotes(s):
    if (s[1] == '"'):
        s = s[2:]
    if (s[(len(s)-2):] == '"'):
        s = s[:len(s)-2]
    return s


"""Run a program with command line arguments.

args must be a list.
Switches the slashes from unix to dos style in program.
Blocks until shell returns, then returns the exit code of the program.
"""

def run(program, args = []):
    program = removeQuotes(to_local_path(program))

    # Windows doesn't support spawnvp, so we have to locate the binary
    if (os.name == 'nt'):
        program = _findBinary(program)

    # spawn requires specification of argv[0]
    # Because the program name may contain spaces, we
    # add quotes around it.
    args.insert(0, '"' + program + '"')
    print string.join(args) + '\n'

    print '\n'

    if (os.name == 'nt'):
        # Windows doesn't support spawnvp
        exitcode = os.spawnv(os.P_WAIT, program, args)
    else:
        exitcode = os.spawnvp(os.P_WAIT, program, args)

    # Since we mutated the list, remove the element
    # that was inserted.
    args.pop(0)

    return exitcode
