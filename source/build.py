#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2003-03-29
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
data       Build g3d-data


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
    x = 0

    if (os.name == 'nt'):
        # Windows build
        x = msdev('graphics3D.dsw',\
                ["graphics3D - Win32 Release",\
                 "graphics3D - Win32 Debug",\
                 "GLG3D - Win32 Release",\
                 "GLG3D - Win32 Debug"])

    else:
        # Linux build (right now, only builds the debug release)

        # Exectute bootstrap and configure whenever the scripts change
        if (newer("bootstrap", "configure") or newer("configure.ac", "configure")):
            run("./bootstrap")

        if (newer("configure", "config.h")):
            run("./configure", ["--enable-debugging"])

        x = run("make")

    if (x != 0):
        print "*** Errors encountered during compilation.  Build process halted."
        sys.exit(x);        

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


###############################################################################
#                                                                             #
#                            release Target                                   #
#                                                                             #
###############################################################################

def release():
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the release on Windows.'

    # Make sure the linux binaries are already built

    # Don't zip up the data directory
    install(0)
    rmdir('../install/data')
    mkdir('../release')
    zip('../install/*', '../release/g3d-M_mm.zip')
    data()

###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, install, doc, test, data, clean, release], buildHelp)
