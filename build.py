#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2003-04-03
#
# Each build target is a procedure.
#

from buildlib import *

# The library version number
version = "5_01"

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

install    Create a user installation directory (what you probably want)
release    Build g3d-cpp-""" + version + """.zip, g3d-data-""" + version + """.zip

lib        Build G3D, G3D-debug, GLG3D, GLG3D-debug lib, copy over other libs
doc        Run doxygen and copy the html directory
clean      Delete the build, release, temp, and install directories
help       Display this message

See cpp/readme.html for detailed build information.
    """

installDir = 'install/g3d-' + version

###############################################################################
#                                                                             #
#                              lib Target                                     #
#                                                                             #
###############################################################################

def lib():
    x = 0

    if (os.name == 'nt'):
        # Windows build
        x = msdev('source/graphics3D.dsw',\
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
            run("./configure")

        x = run("make")

        # Copy the lib's to the right directory (temporary; the makefile
        # should really be putting them here)
        if (x != 0):
            mkdir("temp/lib")
            copyIfNewer("source/G3Dcpp/.libs/libG3D.a",                  "temp/lib/libG3D.a")
            copyIfNewer("source/G3Dcpp/.libs/libG3D_debug.a",            "temp/lib/libG3D_debug.a")
            copyIfNewer("source/G3Dcpp/.libs/libG3D.so.0.0.0",           "temp/lib/libG3D.so")
            copyIfNewer("source/G3Dcpp/.libs/libG3D_debug.so.0.0.0",     "temp/lib/libG3D_debug.so")
            
            copyIfNewer("source/GLG3Dcpp/.libs/libGLG3D.a",              "temp/lib/libGLG3D.a")
            copyIfNewer("source/GLG3Dcpp/.libs/libGLG3D_debug.a",        "temp/lib/libGLG3D_debug.a")
            copyIfNewer("source/GLG3Dcpp/.libs/libGLG3D.so.0.0.0",       "temp/lib/libGLG3D.so")
            copyIfNewer("source/GLG3Dcpp/.libs/libGLG3D_debug.so.0.0.0", "temp/lib/libGLG3D_debug.so")

    if (x != 0):
        print "*** Errors encountered during compilation.  Build process halted."
        sys.exit(x);        

    copyIfNewer("source/lib", "temp/lib")

###############################################################################
#                                                                             #
#                             test Target                                     #
#                                                                             #
###############################################################################

def test():
    if (os.name == 'nt'):
        x = msdev('source/graphics3D.dsw',\
                ["Test - Win32 Release",\
                 "Test - Win32 Debug"])
    else:
        print "Don't know how to build tests on non-Windows platforms"

    run('temp/release/test/test', [])
    run('temp/debug/test/test', [])

###############################################################################
#                                                                             #
#                             doc Target                                      #
#                                                                             #
###############################################################################
    
def doc():
    os.chdir("source")
    run('doxygen', [])
    os.chdir("..")
    copyIfNewer('source/html', installDir + '/html')
    copyIfNewer('temp/html', installDir + '/html')


###############################################################################
#                                                                             #
#                             install Target                                  #
#                                                                             #
###############################################################################
    
def install(copyData=1):
    lib()
    doc()
    copyIfNewer('temp/lib', installDir + '/lib')
    copyIfNewer('source/include', installDir + '/include')
    if (copyData):
        copyIfNewer('source/demos', installDir + '/demos')
        copyIfNewer('data', installDir + '/data')

###############################################################################
#                                                                             #
#                              clean Target                                   #
#                                                                             #
###############################################################################

def clean():
    rmdir('build')
    rmdir('temp')
    rmdir('install')
    rmdir('release')


###############################################################################
#                                                                             #
#                            release Target                                   #
#                                                                             #
###############################################################################

def release():
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the release on Windows.'

    # TODO: Make sure the linux binaries are already built
    install(0)
    mkdir('release')
    mkdir('temp/datacopy/g3d-' + version + '/demos')
    copyIfNewer('data', 'temp/datacopy/g3d-' + version + '/data')
    copyIfNewer('source/demos', 'temp/datacopy/g3d-' + version + '/demos')
    zip('temp/datacopy/*', 'release/g3d-data-' + version + '.zip')

    # Don't zip up the data or demos directories
    rmdir(installDir + '/data')
    rmdir(installDir + '/demos')
    mkdir('release')
    zip('install/*', 'release/g3d-cpp-' + version + '.zip')


###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, install, doc, test, clean, release], buildHelp)
