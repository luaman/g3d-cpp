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

Section "Modify MSVC6 autoexp.dat for G3D datatypes (recommended)"

  SectionIn 1

  ; Search for MSVC6
  ReadRegStr $R0 HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  IfErrors No_autoexp
  
  FileOpen $0 "$R0\MSDev98\Bin\AUTOEXP.DAT" a
  IfErrors No_autoexp
Read_autoexp_loop:    
  FileRead $0 $R1
  IfErrors Finished_autoexp_read
  StrCmp $R1 "G3D::Quat=Quat(<x>,<y>,<z>,<w>)" Autoexp_settings_exists Read_autoexp_loop

Finished_autoexp_read:
  FileSeek $0 0 END
  FileWrite $0 "$\n$\n;; graphics3D$\nG3D::Quat=Quat(<x>,<y>,<z>,<w>)$\nG3D::Vector4=Vector4(<x>,<y>,<z>,<w>)$\nG3D::Vector3=Vector3(<x>,<y>,<z>)$\nG3D::Vector2=Vector2(<x>,<y>)$\nG3D::Vector4int16=Vector4(<x>,<y>,<z>,<w>)$\nG3D::Vector3int16=Vector3(<x>,<y>,<z>)$\nG3D::Vector2int16=Vector2(<x>,<y>)$\nG3D::Color4=Color4(<r>,<g>,<b>,<a>)$\nG3D::Color3=Color3(<r>,<g>,<b>)$\nG3D::Color4uint8=Color4uint8(<r>,<g>,<b>,<a>)$\nG3D::Color3uint8=Color3uint8(<r>,<g>,<b>)$\nG3D::NetAddress=NetAddress(<addr.sin_addr.S_un.S_un_b.s_b1,u.<addr.sin_addr.S_un.S_un_b.s_b2,u.<addr.sin_addr.S_un.S_un_b.s_b3,u.<addr.sin_addr.S_un.S_un_b.s_b4,u)$\n"
  FileClose $0

Autoexp_settings_exists:
  Fileclose $0
  Return

No_autoexp:
  MessageBox MB_OK "There was no MSVC6 autoexp.dat file found!"    


SectionEnd

Section "Modify MSVC7 autoexp.dat for G3D datatypes (recommended)"

  SectionIn 1

  ; Search for MSVC7 7.0 or 7.1
  ReadRegStr $R0 HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\VisualStudio\7.0\Setup\VS" "VS7CommonDir"
  IfErrors Find_VS71 Modify_autoexp
  
Find_VS71:
  ReadRegStr $R0 HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\VisualStudio\7.1\Setup\VS" "VS7CommonDir"
  IfErrors No_autoexp

Modify_autoexp:
  FileOpen $0 "$R0\Packages\Debugger\autoexp.dat" a
  IfErrors No_autoexp
Read_autoexp_loop:    
  FileRead $0 $R1
  IfErrors Finished_autoexp_read
  StrCmp $R1 "G3D::Quat=Quat(<x>,<y>,<z>,<w>)" Autoexp_settings_exists Read_autoexp_loop

Finished_autoexp_read:
  FileSeek $0 0 END
  FileWrite $0 "$\n$\n;; graphics3D$\nG3D::Quat=Quat(<x>,<y>,<z>,<w>)$\nG3D::Vector4=Vector4(<x>,<y>,<z>,<w>)$\nG3D::Vector3=Vector3(<x>,<y>,<z>)$\nG3D::Vector2=Vector2(<x>,<y>)$\nG3D::Vector4int16=Vector4(<x>,<y>,<z>,<w>)$\nG3D::Vector3int16=Vector3(<x>,<y>,<z>)$\nG3D::Vector2int16=Vector2(<x>,<y>)$\nG3D::Color4=Color4(<r>,<g>,<b>,<a>)$\nG3D::Color3=Color3(<r>,<g>,<b>)$\nG3D::Color4uint8=Color4uint8(<r>,<g>,<b>,<a>)$\nG3D::Color3uint8=Color3uint8(<r>,<g>,<b>)$\nG3D::NetAddress=NetAddress(<addr.sin_addr.S_un.S_un_b.s_b1,u.<addr.sin_addr.S_un.S_un_b.s_b2,u.<addr.sin_addr.S_un.S_un_b.s_b3,u.<addr.sin_addr.S_un.S_un_b.s_b4,u)$\n"
  FileClose $0

Autoexp_settings_exists:
  Fileclose $0
  Return

No_autoexp:
  MessageBox MB_OK "There was no MSVC7 autoexp.dat file found!"

SectionEnd

Section "Add G3D directories to MSVC6 paths (recommended)"

  SectionIn 1


SectionEnd

Section "Add G3D directories to MSVC7 paths (recommended)"

  SectionIn 1

SectionEnd

Section "Create desktop icon for documentation"

  SectionIn 1

  ; Create desktop shortcut
  CreateShortCut "$DESKTOP\Graphics3D.lnk" "$INSTDIR\html\index.html" "" "$INSTDIR\html\g3d.ico" 0

SectionEnd


Section "View G3D Manual"

  SectionIn 1

  Exec "$INSTDIR\html\index.html"  

SectionEnd