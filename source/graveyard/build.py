# The build script for graphics3d
#
# morgan@graphics3d.com
# @edited 2003-02-10
#

import sys
import time
import os
import tools
import shutil
import string
import gzip
from buildlib import *
from os.path import split

contributorList = """
<BR>Seth Block seth_block@brown.edu
   <BR>Nick Capens nicolas@capens.net
   <BR>John Chisholm jchishol@hotmail.com
   <BR>Jauhn Dabz jauhn@yahoo.com
   <BR>Dave Eberly eberly@magic-software.com
   <BR>Kevin Egan ktegan@cs.brown.edu
   <BR>Cass Everitt cass@r3.nu
   <BR>Jakob Fischer pizzadude@pizzadude.dk
   <BR>Harishabd Khalsa hkhalsa@cs.brown.edu
   <BR>Michael Herf
   <BR>Peter Hunt p222h@aol.com
   <BR>Robert Hunter rob@cs.brown.edu
   <BR>Ed Johnson odin@alum.mit.edu
   <BR>Benjamin Jurke bjurke@gmx.de
   <BR>Chris Kern ekern@cs.brown.edu
   <BR>Aaron Orenstein a@orenstein.name
   <BR>Independent JPEG Group <A HREF="http://www.ijg.org/">http://www.ijg.org/</A>
   <BR>Darius Jazayeri jazayeri@MIT.EDU
   <BR>Ben Landon brl@world.std.com
   <BR>Thomas G. Lane
   <BR>Ray Larabie drowsy@cheerful.com
   <BR>Jukka Liimatta jukka@liimatta.org
   <BR>Max McGuire mmcguire@ironlore.com
   <BR>Morgan McGuire matrix@graphics3d.com
   <BR>Justin Miller jcmiller@alum.mit.edu
   <BR>Kurt Miller kurt@flipcode.com
   <BR>Nate Miller nathanm@uci.edu
   <BR>Edward Resnick cgp@gdnmail.net
   <BR>SGI<A HREF="http://oss.sgi.com/projects/FreeB">SGI</A>
   <BR>Matthew Welch daffy-duck@worldnet.att.net
   <BR>Laura Wollstadt laura3d@yahoo.com
"""

source_dir = 'd:/graphics3d/book/cpp/source'
distrib_dir = 'd:/graphics3d/book/cpp/distrib'
data_distrib_dir = 'd:/graphics3d/book/cpp/data-distrib'

###############################################################################

def current_time():
	return time.strftime('%I:%M:%S%p', time.localtime(time.time()))

###############################################################################
#                                                                             #
#                               Help                                          #
#                                                                             #
###############################################################################

def help():
    print """
build - Automated build system for Graphics3D.

TARGET     DESCRIPTION
           source_dir        = """ + source_dir + """
           distrib_dir       = """ + distrib_dir + """
           data_distrib_dir  = """ + data_distrib_dir + """

headers    Copies the headers.

doc        Builds documentation.

demos      Copies the demos over

data       Copies the data directory over

release    Build all targets above (assumes you have built the libs from MSVC)

clean      Deletes the release dir.
    """
    sys.exit()

###############################################################################

def doc():
    print current_time() + ' Building documentation'

    tools.makedirs_if_they_do_not_exist(distrib_dir)
    shell("c:/doxygen/bin/doxygen", ["Doxyfile"])

    doc_dir = '/html'

    files = recursive_listdir(source_dir + doc_dir, '*')
    for file in files:
         (p, f) = split(file)
         tools.makedirs_if_they_do_not_exist(distrib_dir + doc_dir + '/' + p)
         sfilename = source_dir + doc_dir + '/' + file
         dfilename = distrib_dir + doc_dir + '/' + file
         if tools.newer(sfilename, dfilename):
             print '  Copying ' + sfilename + ' to ' + dfilename
             shutil.copyfile(sfilename, dfilename)

    print current_time() + ' Done building documentation'

###############################################################################

# Returns true if none of the prefixes are in file
#
def noPrefix(prefList, file):
    for prefix in prefList:
        if (file.lower().startswith(prefix.lower())):
            return False
    return 1

def headers():
    print current_time() + ' Building distribution headers'

    include_dir = distrib_dir + '/include'

    files = recursive_listdir('source', '*.h')
    files = files + recursive_listdir('source', '*.inl')
    for file in files:
        if (noPrefix(['graveyard/', 'G3D/IJG/', 'Test/', 'Demos/'], file)):
             (p, f) = split(file)
             tools.makedirs_if_they_do_not_exist(include_dir + '/' + p)
             sfilename = 'source/' + file
             dfilename = include_dir + '/' + file
             if tools.newer(sfilename, dfilename):
                 print '  Copying ' + sfilename + ' to ' + dfilename
                 shutil.copyfile(sfilename, dfilename)

    print current_time() + ' Done building distribution headers'

###############################################################################

def demo():
    print current_time() + ' Copying demos'

    tools.makedirs_if_they_do_not_exist(data_distrib_dir)
	
    files = recursive_listdir(source_dir + '/demos', '*.*')
    for file in files:
        if (file[:10] != 'graveyard/'):
             (p, f) = split(file)
             tools.makedirs_if_they_do_not_exist(data_distrib_dir + '/demos/' + p)
             sfilename = source_dir + '/demos/' + file
             dfilename = data_distrib_dir + '/demos/' + file
             if tools.newer(sfilename, dfilename):
                 print '  Copying ' + sfilename + ' to ' + dfilename
                 shutil.copyfile(sfilename, dfilename)

    print current_time() + ' Done copying demos'

###############################################################################

def data():
    print current_time() + ' Copying data'

    tools.makedirs_if_they_do_not_exist(data_distrib_dir)

    files = recursive_listdir(source_dir + '/data', '*.*')
    for file in files:
        if (file[:10] != 'graveyard/'):
             (p, f) = split(file)
             tools.makedirs_if_they_do_not_exist(data_distrib_dir + '/data/' + p)
             sfilename = source_dir + '/data/' + file
             dfilename = data_distrib_dir + '/data/' + file
             if tools.newer(sfilename, dfilename):
                 print '  Copying ' + sfilename + ' to ' + dfilename
                 shutil.copyfile(sfilename, dfilename)

    print current_time() + ' Done copying data'

###############################################################################

def release():
    print current_time() + ' Beginning release build'
    doc()
    data()
    demo()
    headers()
    #args = ['c:\\Program Files\\pkzip\\PKZIP25.EXE', '-add', '-rec', '-lev=9', '-path=relative', 'data-distrib', 'data.zip', 'data-distrib/*']
    #command = 'c:\\Program Files\\pkzip\\PKZIP25.EXE'

    print current_time() + ' Done release buiild'

###############################################################################

def clean():
    print current_time() + ' Cleaning'

    dest = distrib_dir
    assert(dest != '')
    assert(dest != source_dir)
    os.removedir(dest)

    dest = data_distrib_dir
    assert(dest != '')
    assert(dest != source_dir)
    os.removedir(dest)

    print current_time() + ' Done cleaning'

###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

# Execute the appropriate target:
if len(sys.argv) == 1:
   print 'Type "build --help" to see all targets.'
   sys.exit()

for target in sys.argv[1:]:   
    if target == '--help':
       help()
    elif target == 'doc':
	   doc()
    elif target == 'headers':
	   headers()
    elif target == 'clean':
	   clean()
    elif target == 'demos':
	   demo()
    elif target == 'data':
	   data()
    elif target == 'release':
       release()
    else:
       print 'Unknown target: "' + target + '". (Type "build --help" to see all targets.)'
       sys.exit(-1)

print '\n"' + string.join(sys.argv[1:]) + '" build succeeded.'

