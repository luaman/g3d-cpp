#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2003-02-20
#
# Each build target is a procedure.
#

from buildlib import *

###############################################################################
#                                                                             #
#                               Help                                          #
#                                                                             #
###############################################################################

def buildHelp():
    print """
build - Automated build system for graphics3D.

Syntax: 
    build [target]*  

TARGET     DESCRIPTION

release    Build g3d-XXX

source     Build g3d-source
data       Build g3d-data
windows    Build g3d-win

install    Create a user installation directory
lib        Build G3D, G3D-debug, GLG3D, GLG3D-debug lib, copy over other libs
doc        Run doxygen and copy the html directory
clean      Delete the build, release, temp, and install directories
help       Display this message

See cpp/readme.html for detailed build information.
    """

###############################################################################
#                                                                             #
#                              lib Target                                     #
#                                                                             #
###############################################################################

def lib():
    if (os.name == 'nt'):
        x = msdev('graphics3D.dsw',\
                ["graphics3D - Win32 Release",\
                 "graphics3D - Win32 Debug",\
                 "GLG3D - Win32 Release",\
                 "GLG3D - Win32 Debug"])
    else:
        print "Don't know how to build libraries on non-Windows platforms"

    copyIfNewer("lib", "../build/lib")

###############################################################################
#                                                                             #
#                             test Target                                     #
#                                                                             #
###############################################################################

def test():
    if (os.name == 'nt'):
        x = msdev('graphics3D.dsw',\
                ["Test - Win32 Release",\
                 "Test - Win32 Debug"])
    else:
        print "Don't know how to build tests on non-Windows platforms"

    run('../temp/release/test/test', [])
    run('../temp/debug/test/test', [])

###############################################################################
#                                                                             #
#                             doc Target                                      #
#                                                                             #
###############################################################################
    
def doc():
    run('doxygen', [])
    copyIfNewer('html', '../build/html')


###############################################################################
#                                                                             #
#                             install Target                                  #
#                                                                             #
###############################################################################
    
def install(copyData=1):
    lib()
    doc()
    copyIfNewer('../build', '../install')
    copyIfNewer('include', '../install/include')
    copyIfNewer('demos', '../install/demos')
    if (copyData):
        copyIfNewer('../data', '../install/data')
    

###############################################################################
#                                                                             #
#                             source Target                                   #
#                                                                             #
###############################################################################

def source():
    copyIfNewer('../source', '../temp/cpp/source')
    copyIfNewer('../readme.html', '../temp/cpp/readme.html')
    rmdir('../temp/cpp/data')
    mkdir('../release')
    zip('../temp/cpp/*', '../release/g3d-source-M_mm.zip')


###############################################################################
#                                                                             #
#                               data Target                                   #
#                                                                             #
###############################################################################

def data():
    mkdir('../release')
    copyIfNewer('../data', '../temp/datacopy/data')
    zip('../temp/datacopy/*', '../release/g3d-data-M_mm.zip')


###############################################################################
#                                                                             #
#                              clean Target                                   #
#                                                                             #
###############################################################################

def clean():
    rmdir('../build')
    rmdir('../temp')
    rmdir('../install')
    rmdir('../release')


###############################################################################
#                                                                             #
#                            windows Target                                   #
#                                                                             #
###############################################################################

def windows():
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the Windows release on Windows.'

    # Don't zip up the data directory
    install(False)
    rmdir('../install/data')
    mkdir('../release')
    zip('../install/*', '../release/g3d-win-M_mm.zip')

###############################################################################
#                                                                             #
#                            release Target                                   #
#                                                                             #
###############################################################################

def release():
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the Windows release on Windows.'

    clean()
    windows()
    data()
    source()

###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, install, doc, test, source, data, clean, windows, release], buildHelp)
