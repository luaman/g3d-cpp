# g3dinstaller.nsi
#
# G3D Installation script
#
#
#

!include "MUI.nsh"

!define VERSION "g3d-6_05-b02"

!define MUI_COMPONENTSPAGE_NODESC

# Use the LZMA compressor
SetCompressor lzma

# The name of the installer
Name "${VERSION} Installer"

# The file to write
OutFile "${VERSION}.exe"

# The default installation directory
InstallDir "C:\libraries\${VERSION}"


#--------------------------------

# Pages

  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES

#--------------------------------

!insertmacro MUI_LANGUAGE "English"

!ifndef NOINSTTYPES ; only if not defined
  InstType "Complete"
;  InstType /NOCUSTOM
!endif

Section "G3D Core Files"

  SectionIn 1

  ; Setup basic G3D installation
  
  SetOutPath "$INSTDIR\bin"  
  File /r "install\${VERSION}\bin\*"
  SetOutPath "$INSTDIR\contrib"  
  File /r "install\${VERSION}\contrib\*"
  SetOutPath "$INSTDIR\include"  
  File /r "install\${VERSION}\include\*"
  SetOutPath "$INSTDIR\data"  
  File /r "install\${VERSION}\data\*"
  SetOutPath "$INSTDIR\demos"  
  File /r "install\${VERSION}\demos\*"
  
SectionEnd

Section "Download Data Module"

  SectionIn 1
  
SectionEnd

Section "Install MSVC6 Libraries"
  
  SectionIn 1
  
  ; Create directories
  ;CreateDirectory $INSTDIR\win32-lib

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\win32-lib"

  ; Write out MSVC6 libs
  File /r "install\${VERSION}\win32-lib\*"

SectionEnd

Section "Install MSVC7 Libraries"
  
  SectionIn 1
  
  ; Create directories
  ;CreateDirectory $INSTDIR\win32-7-lib

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\win32-7-lib"

  ; Write out MSVC7 libs
  File /r "install\${VERSION}\win32-7-lib\*"

SectionEnd

Section "Modify autoexp.dat for G3D datatypes (recommended)"

  SectionIn 1

SectionEnd

Section "Create desktop icon for documentation"

  SectionIn 1

  ; Create desktop shortcut
  CreateShortCut $DESKTOP\G3D.lnk "$INSTDIR\html\index.html" "" "" 0 SW_SHOWNORMAL "" "G3D Doc"

SectionEnd

Section "Add G3D directories to MSVC(6/7) paths (recommended)"

  SectionIn 1

SectionEnd

Section "View G3D Manual"

  SectionIn 1

  Exec "$INSTDIR\html\index.html"  

SectionEnd