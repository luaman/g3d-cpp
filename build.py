#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2003-06-17
#
# Each build target is a procedure.
#

from buildlib import *

# The library version number
version = "6_00"

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
release    Build g3d-""" + version + """.zip, g3d-src-""" + version + """.zip
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

   
def linuxCheckVersion():
    print 'Checking tool versions...'
    print

    compiler = ''
    if os.environ.has_key('CXX'):
        compiler = os.environ['CXX']
    elif os.environ.has_key('CC'):
        compiler = os.environ['CC']
    else:
        print '**Error: you need to set the CXX and CC environment variables.**'
        print 'See readme.html'
        sys.exit(-1)
    

    checkVersion(compiler + ' --version', '3.1', 'Requires g++ 3.1 or later.')
    checkVersion('automake-1.7 --version', '1.7', 'Requires automake 1.7 or later.')
    checkVersion('aclocal-1.7 --version', '1.7', 'Requires aclocal 1.7 or later.')
    checkVersion('doxygen --version', '1.2', 'Requires doxygen 1.3 or later.')
    checkVersion('python -V', '2.0', 'Requires Python 2.0 or later.', 1)


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
        # Linux build (right now, only builds the debug release, doesn't 
        # copy files)

	# Check version of tools

        linuxCheckVersion()

        run("./bootstrap")
        run("./configure", ['--enable-shared', '--enable-static'])
        x = run("make")

        # Copy the lib's to the right directory
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

    if (os.name != 'nt'):
        return

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

    mkdir('release')

    # Put everything but CVS, temp, and binaries and
    rmdir('../temp')
    copyIfNewer('../cpp', '../temp/copy/cpp')
    rmdir('../temp/copy/cpp/temp')
    rmdir('../temp/copy/cpp/install')
    rmdir('../temp/copy/cpp/release')
    zip('../temp/copy/*', 'release/g3d-src-' + version + '.zip')

    # TODO: Make sure the linux binaries are already built

    install()
    zip('install/*', 'release/g3d-' + version + '.zip')



###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, install, doc, test, clean, release], buildHelp)
