#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2004-01-13
#
# Each build target is a procedure.
#

from buildlib import *

# The library version number
version = "6_00-b13"

# Turn the platform into a name to put in the
# "lib" directory name

platform = 'unknown'
if (os.name == 'nt'):
    platform = 'win32'
else:
    platform = {'Linux' : 'linux', \
                'Darwin' : 'osx'}[os.uname()[0]]

###############################################################################
#                                                                             #
#                               Help                                          #
#                                                                             #
###############################################################################

def buildHelp():
    print """
build - Automated build system for graphics3D.

Syntax: 
    build target [install-dir]

TARGET     DESCRIPTION

install    Create a user installation directory (what you probably want)

lib        Build G3D, G3D-debug, GLG3D, GLG3D-debug lib, copy over other libs and headers
release    Build g3d-""" + version + """.zip, g3d-src-""" + version + """.zip
doc        Run doxygen and copy the html directory
clean      Delete the build, release, temp, and install directories
help       Display this message

See cpp/readme.html for detailed build information.
    """

def installDir(args):
    if (args == []):
        # Default
        return 'install/g3d-' + version
    else:
        # Build the name from the install dir
        s = args[0]
        if ((s != "") and (s[len(s) - 1] != "/") and (s[len(s) - 1] != "\\")):
            s = s + "/"
        return s + 'g3d-' + version

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


def lib(args):
    x = 0

    copyIfNewer('source/include', installDir(args) + '/include')

    libdir = installDir(args) + "/" + platform + "-lib"
    mkdir(libdir)

    if (os.name == 'nt'):
        # Windows build
        x = msdev('source/graphics3D.dsw',\
                ["graphics3D - Win32 Release",\
                 "graphics3D - Win32 Debug",\
                 "GLG3D - Win32 Release",\
                 "GLG3D - Win32 Debug"])

        copyIfNewer("temp/win32-lib", libdir)

    else:
        # Linux build
    	# Check version of tools

        linuxCheckVersion()

        run("./bootstrap")
        run("./configure", ['--enable-shared', '--enable-static'])
        x = run("make")

        # Copy the lib's to the right directory
        if (x == 0):
            # strip first
            os.system("strip -g --strip-unneeded temp/release/g3d/.libs/libG3D.a")
            os.system("strip -g --strip-unneeded temp/release/glg3d/.libs/libGLG3D.a")

            os.system("strip --strip-unneeded temp/debug/g3d/.libs/libG3D_debug.a")
            os.system("strip --strip-unneeded temp/debug/glg3d/.libs/libGLG3D_debug.a")

            copyIfNewer("temp/debug/g3d/.libs/libG3D_debug.a",            libdir + "/libG3D_debug.a")
            copyIfNewer("temp/release/g3d/.libs/libG3D.a",                libdir + "/libG3D.a")
            copyIfNewer("temp/debug/glg3d/.libs/libGLG3D_debug.a",        libdir + "/libGLG3D_debug.a")
            copyIfNewer("temp/release/glg3d/.libs/libGLG3D.a",            libdir + "/libGLG3D.a")

            # Dynamic libs (not currently used)
            #copyIfNewer("temp/debug/glg3d/.libs/libGLG3D_debug.so.0.0.0", installDir + "/lib/libGLG3D_debug.so")
            #copyIfNewer("temp/debug/g3d/.libs/libG3D_debug.so.0.0.0",     installDir + "/lib/libG3D_debug.so")
            #copyIfNewer("temp/release/g3d/.libs/libG3D.so.0.0.0",         installDir + "/lib/libG3D.so")
            #copyIfNewer("temp/release/glg3d/.libs/libGLG3D.so.0.0.0",     installDir + "/lib/libGLG3D.so")

            #os.system("ln -s " + installDir(args) + "/lib/libG3D_debug.so "   + installDir(args) + "/lib/libG3D_debug.so.0")
            #os.system("ln -s " + installDir + "/lib/libG3D.so "         + installDir(args) + "/lib/libG3D.so.0")
            #os.system("ln -s " + installDir + "/lib/libGLG3D_debug.so " + installDir(args) + "/lib/libGLG3D_debug.so.0")
            #os.system("ln -s " + installDir + "/lib/libGLG3D.so "       + installDir(args) + "/lib/libGLG3D.so.0")

    if (x != 0):
        print "*** Errors encountered during compilation.  Build process halted."
        sys.exit(x);        

    # Copy any system libraries over
    copyIfNewer("source/" + platform + "-lib", libdir)

###############################################################################
#                                                                             #
#                             test Target                                     #
#                                                                             #
###############################################################################

def test(args):
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
    
def doc(args):
    os.chdir("source")
    run('doxygen', [])
    os.chdir("..")
    copyIfNewer('source/html', installDir(args) + '/html')
    copyIfNewer('temp/html', installDir(args) + '/html')


###############################################################################
#                                                                             #
#                             install Target                                  #
#                                                                             #
###############################################################################

""" If copyData is true, this also copies the data module (the source/data
    directory is always copied) """
def install(args, copyData=1):
    lib(args)
    doc(args)
    
    # Copy the demos
    copyIfNewer('source/demos', installDir(args) + '/demos')

    # Most of this will be overwritten if copyData == 1, but we want
    # to be sure to have all files since source/data might
    # have a few that aren't in the data module.
    copyIfNewer('source/data', installDir(args) + '/data')

    if (copyData):
        copyIfNewer('../data', installDir(args) + '/data')

###############################################################################
#                                                                             #
#                              clean Target                                   #
#                                                                             #
###############################################################################

def clean(args):
    rmdir('build')
    rmdir('temp')
    rmdir(installDir(args))
    rmdir('release')


###############################################################################
#                                                                             #
#                            release Target                                   #
#                                                                             #
###############################################################################

def release(args):
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the release on Windows.'

    rmdir('release')

    rmdir('../temp')
    copyIfNewer('../cpp', '../temp/sourcecopy')
    rmdir('install')

    mkdir('release')

    # TODO: Make sure the linux binaries are already built

    # TODO: Build the Windows demos

    # Install to the 'install' directory
    install([], 0)
    zip('install/*', 'release/g3d-' + version + '.zip')

    # Source zip
    zip('../temp/sourcecopy/*', 'release/g3d-src-' + version + '.zip')

    # Make a separate zipfile for the data
    copyIfNewer('../data', '../temp/datacopy/data')
    zip('../temp/datacopy/*', 'release/g3d-data-' + version + '.zip')



###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, install, doc, test, clean, release], buildHelp)
