#
# The build script for graphics3D
#
# @maintainer Morgan McGuire, matrix@graphics3d.com
#
# @created 2001-01-01
# @edited  2005-01-29
# Each build target is a procedure.
#

from buildlib import *

# The library version number
version = "6_05-b02"

# Setup versions for supporting programs
aclocal    = "aclocal-1.7"
autoconf   = "autoconf"
autoheader = "autoheader"
automake   = "automake-1.7"
doxygen    = "doxygen"
python     = "python2.2"

# Turn the platform into a name to put in the
# "lib" directory name

platform = 'unknown'
if (os.name == 'nt'):
    # VC 6.0; use 'win32-vc7' for .NET
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

install    Create a user installation directory (what you probably want).

lib        Build G3D, G3D-debug, GLG3D, GLG3D-debug lib, copy over other libs and headers
lib7       Same as 'lib', but use VC7 on Windows instead of VC6
fastlib    Build the lib target without reconfiguring on Linux (not recommended) 
release    Build g3d-""" + version + """.zip, g3d-src-""" + version + """.zip, g3d-data-""" + version + """.zip
source     Build g3d-src-""" + version + """.zip only
doc        Run doxygen and copy the html directory and contrib directory
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


""" On Posix, sets the permissions of the install dir to a+rx"""
def setPermissions(args):
    if (os.name == 'posix'):
        # This could be accomplished more elegantly using os.walk
        curdir = os.getcwd()
        os.chdir(installDir(args))
        print "chmod -R uag+rx *"
        os.system("chmod -R uag+rx *")
        os.chdir(curdir)

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
            checkVersion(automake + ' --version', '1.6', 'Requires automake 1.6 or later.')
        except:
            checkVersion('automake-1.7 --version', '1.6', 'Requires automake 1.6 or later.')

        try:
            checkVersion(aclocal + ' --version', '1.6', 'Requires aclocal 1.6 or later.')
        except:
            checkVersion('aclocal-1.7 --version', '1.6', 'Requires aclocal 1.6 or later.')

            checkVersion(doxygen + ' --version', '1.2', 'Requires doxygen 1.3 or later.')
            checkVersion(python + ' -V', '2.0', 'Requires Python 2.0 or later.', 1)

    except Error, e:
        print e.value
        sys.exit(-4)

def lib7(args):
    _lib(args, 'win32-7')

def lib(args, reconfigure = 1):
    _lib(args, platform, reconfigure)


"""
  Verifies that the object files were built with the same compiler as we are 
  currently using.  If they were not, deletes the object files and binaries.
  Value is cached in lastplatform.txt

  Currently used only on windows.
"""
def checkObjectFiles(libplatform, platform):
    filename = "temp/lastplatform.txt"

    last = ""

    if os.path.exists(filename):
        # the file doesn't exist.  Better delete to be safe
        f = file(filename, "r")
        last = f.read()
        f.close()

    if (last != libplatform):
        # bad match
        print 'Last compiler used does not match this one.  Deleting old object files.'

        # delete object files
        rmdir("temp/debug")
        rmdir("temp/release")
        
        # delete lib files (they are in the platform, not libplatform directory)
        rmdir("temp/" + platform)

        # write out new platform file
        mkdir("temp")
        f = file(filename, "w")
        f.write(libplatform)
        f.close()

"""
  Used internally by lib routines
"""
def _lib(args, libplatform, reconfigure = 1):
    x = 0

    copyIfNewer('source/include', installDir(args) + '/include')

    libdir = installDir(args) + "/" + libplatform + "-lib"
    mkdir(libdir)

    if (os.name == 'nt'):
        # Windows

        checkObjectFiles(libplatform, platform)

        if (libplatform == 'win32'):
            # VC6
            x = msdev('source/graphics3D.dsw',\
                    ["graphics3D - Win32 Release",\
                     "graphics3D - Win32 Debug",\
                     "GLG3D - Win32 Release",\
                     "GLG3D - Win32 Debug"])
        else:
            # VC7
            x = devenv('source/graphics3D.sln',\
                    ["graphics3D",\
                     "GLG3D"])

        copyIfNewer("temp/" + platform + "-lib", libdir)

    else:
        # Linux build
        # Check version of tools

        if reconfigure:
            linuxCheckVersion()

            run("./bootstrap",["-a " + aclocal,
                               "-h " + autoheader,
                               "-m " + automake,
                               "-c " + autoconf])
            run("./configure", ['--enable-shared', '--enable-static'])

        x = run("make")

        # Copy the lib's to the right directory
        if (x == 0):
            # strip debug symbols (don't strip debug builds)
            os.system("strip -g --strip-unneeded temp/release/g3d/libG3D.a")
            os.system("strip -g --strip-unneeded temp/release/glg3d/libGLG3D.a")

            copyIfNewer("temp/debug/g3d/libG3D_debug.a",
                        libdir + "/libG3D_debug.a")
            copyIfNewer("temp/release/g3d/libG3D.a",    
                        libdir + "/libG3D.a")
            copyIfNewer("temp/debug/glg3d/libGLG3D_debug.a",
                        libdir + "/libGLG3D_debug.a")
            copyIfNewer("temp/release/glg3d/libGLG3D.a",
                        libdir + "/libGLG3D.a")

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

    # Copy any system libraries over.  Note that this uses
    # the global platform directory and not the local one, which 
    # may be different on Win32
    copyIfNewer("source/" + platform + "-lib", libdir)
    setPermissions(args)

    print
    print "Output written to " + libdir
    print

def fastlib(args):
    lib(args, 0)

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
    run(doxygen, [])
    os.chdir("..")
    # Hand-written docs
    copyIfNewer('source/html', installDir(args) + '/html')
    # Generated docs
    copyIfNewer('temp/html', installDir(args) + '/html')
    # Contrib
    copyIfNewer('source/contrib', installDir(args) + '/contrib')
    setPermissions(args)

###############################################################################
#                                                                             #
#                             install Target                                  #
#                                                                             #
###############################################################################

""" If copyData is true, this also copies the data module (the source/data
    directory is always copied) """
def install(args, copyData=1, fromRelease=False):

    if (os.name == "nt"):

        # see what compilers are installed on windows
        has6 = 0
        has7 = 0
        try:
            findBinary("msdev")
            has6 = 1
        except 'Error':
            0

        try:
            findBinary("devenv")
            has7 = 1
        except 'Error':
            0
        
        if (not has6 and not has7):
            raise 'Error', 'No version of MSVC++ found on this machine.  Cannot build.'
        else:
            if has6:
                lib(args)
                # Need to build here before MSVC7 files are build over these.
                if (fromRelease == True):
                    # Build demos for Release but before demos is copied
                    curdir = os.getcwd()
                    os.chdir('source/demos')
                    if (os.name == "nt"):
                        for dir in os.listdir('.'):
                            if os.path.isdir(dir):
                                #os.chdir(dir)
                                dspFile = ''
                                dswFile = ''
                                dirFiles = os.listdir(dir)
                                
                                for file in dirFiles:
                                    base, ext = os.path.splitext(file)
                                    if ext == '.dsw':
                                        dswFile = file
                                    elif ext == '.dsp':
                                        dspFile = base

                                if dswFile != '' and dspFile != '':
                                    msdev(dir + '/' + dswFile, [dspFile + ' - Win32 Release'])
                                
                                # Attempt to remove any 'Debug' or 'Release' directories accidentally left
                                rmdir(dir + '/Debug')
                                rmdir(dir + '/Release')
                                
                    os.chdir(curdir)    
            if has7:
                lib7(args)

    else:
        lib(args)
    doc(args)
    
    # Copy the demos
    copyIfNewer('source/demos', installDir(args) + '/demos')

    # Most of this will be overwritten if copyData == 1, but we want
    # to be sure to have all files since source/data might
    # have a few that aren't in the data module.
    copyIfNewer('source/data', installDir(args) + '/data')
    
    # Copy the source/bin directory with iCompile in it
    copyIfNewer('source/bin', installDir(args) + '/bin')

    if (copyData):
        copyIfNewer('../data', installDir(args) + '/data')

    setPermissions(args)

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
#                             source Target                                   #
#                                                                             #
###############################################################################

def source(args):
    if (os.name != 'nt'):
        raise 'Error', 'Can only build source on Windows.'
    rmdir('temp/sourcecopy')
    copyIfNewer('source', 'temp/sourcecopy')

    # Remove certain huge files
    rm('temp/sourcecopy/g3ddocs.tag')
    rm('temp/sourcecopy/html/G3D-Map.ppt')

    # Remove all executables
    files = os.listdir('temp/sourcecopy/demos')
    for f in files:
        if (f[-4:] == '.exe'):
            rm('temp/sourcecopy/demos/' + f)

    # Source zip
    zip('temp/sourcecopy/*', 'release/g3d-src-' + version + '.zip')


###############################################################################
#                                                                             #
#                            release Target                                   #
#                                                                             #
###############################################################################

def release(args):
    if (os.name != 'nt'):
        raise 'Error', 'Can only build the release on Windows.'

    rmdir('release')

    rmdir('temp')
    rmdir('install')

    mkdir('release')

    # TODO: Make sure the linux binaries are already built
    
    # Install to the 'install' directory
    install([], 0, True)

    # Copy Linux and OS X lib directories.  They should be
    # manually placed in cpp/linux-lib and cpp/osx-lib before
    # building this target.
    copyIfNewer('linux-lib', installDir([]) + '/linux-lib')
    copyIfNewer('osx-lib', installDir([]) + '/osx-lib')

    zip('install/*', 'release/g3d-' + version + '.zip')

    source(args)
    setPermissions(args)

    # Make a separate zipfile for the data
    copyIfNewer('../data', 'temp/datacopy/data')
    zip('temp/datacopy/*', 'release/g3d-data-' + version + '.zip')
    
    import _winreg
    
    try:
        NSISKey = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, "SOFTWARE\NSIS")
        (NSISLocation, NSISType) = _winreg.QueryValueEx(NSISKey, '')
                
        NSISArgs = os.getcwd() + '\g3dinstaller.nsi';
        os.chdir(NSISLocation)
        os.spawnl(os.P_WAIT, 'makensis', 'makensis "' + NSISArgs + '"')
    except EnvironmentError, e:
        print e.value


###############################################################################
#                                                                             #
#                              Targets                                        #
#                                                                             #
###############################################################################

dispatchOnTarget([lib, lib7, fastlib, install, source, doc, test, clean, release], buildHelp)
