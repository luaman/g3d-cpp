#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2003-07-21
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
    else:
        compiler = 'gcc'
 
    try:
        checkVersion(compiler + ' --version', '3.1', 'Requires g++ 3.1 or later.')
	try:
	        checkVersion('automake --version', '1.6', 'Requires automake 1.6 or later.')
	except:
	        checkVersion('automake-1.7 --version', '1.6', 'Requires automake 1.6 or later.')

	try:
	        checkVersion('aclocal --version', '1.6', 'Requires aclocal 1.6 or later.')
	except:
	        checkVersion('aclocal-1.7 --version', '1.6', 'Requires aclocal 1.6 or later.')

        checkVersion('doxygen --version', '1.2', 'Requires doxygen 1.3 or later.')
        checkVersion('python -V', '2.0', 'Requires Python 2.0 or later.', 1)

    except Error, e:
        print e.value
        sys.exit(-4)


def lib():
    x = 0

    mkdir(installDir + "/lib")
    copyIfNewer('source/include', installDir + '/include')

    if (os.name == 'nt'):
        # Windows build
        x = msdev('source/graphics3D.dsw',\
                ["graphics3D - Win32 Release",\
                 "graphics3D - Win32 Debug",\
                 "GLG3D - Win32 Release",\
                 "GLG3D - Win32 Debug"])

        copyIfNewer("temp/lib/*.lib", installDir + "/lib")

    else:
        # Linux build
    	# Check version of tools

        linuxCheckVersion()

        run("./bootstrap")
        run("./configure", ['--enable-shared', '--enable-static'])
        x = run("make")

        # Copy the lib's to the right directory
        if (x == 0):
            copyIfNewer("temp/debug/g3d/.libs/libG3D_debug.a",            installDir + "/lib/libG3D_debug.a")
            copyIfNewer("temp/debug/g3d/.libs/libG3D_debug.so.0.0.0",     installDir + "/lib/libG3D_debug.so")
            copyIfNewer("temp/release/g3d/.libs/libG3D.a",                installDir + "/lib/libG3D.a")
            copyIfNewer("temp/release/g3d/.libs/libG3D.so.0.0.0",         installDir + "/lib/libG3D.so")

            copyIfNewer("temp/debug/glg3d/.libs/libGLG3D_debug.a",        installDir + "/lib/libGLG3D_debug.a")
            copyIfNewer("temp/debug/glg3d/.libs/libGLG3D_debug.so.0.0.0", installDir + "/lib/libGLG3D_debug.so")
            copyIfNewer("temp/release/glg3d/.libs/libGLG3D.a",            installDir + "/lib/libGLG3D.a")
            copyIfNewer("temp/release/glg3d/.libs/libGLG3D.so.0.0.0",     installDir + "/lib/libGLG3D.so")

    if (x != 0):
        print "*** Errors encountered during compilation.  Build process halted."
        sys.exit(x);        

    copyIfNewer("source/lib", installDir + "lib")

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
    
    if (os.name != 'nt'):
        return

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
