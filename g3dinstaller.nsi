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
Name "graphics3D (${VERSION})"

# The file to write
OutFile "release\${VERSION}_win32.exe"

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


;;
;; Main installation section
;;
Section "-graphics3D Main Install"

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
  SetOutPath "$INSTDIR\html"  
  File /r "install\${VERSION}\html\*"

  ;; Visual Studio 6 libraries

  ; Create directories
  ;CreateDirectory $INSTDIR\win32-lib

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\win32-lib"

  ; Write out MSVC6 libs
  File /r "install\${VERSION}\win32-lib\*"


  ;; Visual Studio 7 libraries
  
  ; Create directories
  ;CreateDirectory $INSTDIR\win32-7-lib

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\win32-7-lib"

  ; Write out MSVC7 libs
  File /r "install\${VERSION}\win32-7-lib\*"
SectionEnd




;;
;; Add the include/library G3D directories to the MSVC6 paths if they don't already exist
;;
Section "Add G3D Directories to MSVC6"

  SectionIn 1

  ReadRegStr $R0 HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Build System\Components\Platforms\Win32 (x86)\Directories" "Include Dirs"
  IfErrors No_include

  StrCpy $R1 $R0
  Push $R0
  Push "$INSTDIR\include"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_include
  Goto Check_lib
Add_include:
  StrCpy $R1 "$R1;$INSTDIR\include"
  WriteRegStr HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Build System\Components\Platforms\Win32 (x86)\Directories" "Include Dirs" $R1 

Check_lib:
  ReadRegStr $R0 HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Build System\Components\Platforms\Win32 (x86)\Directories" "Library Dirs"
  IfErrors No_include

  StrCpy $R1 $R0
  Push $R0
  Push "$INSTDIR\win32-lib"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_lib
  Goto No_include
Add_lib:
  StrCpy $R1 "$R1;$INSTDIR\win32-lib"
  WriteRegStr HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Build System\Components\Platforms\Win32 (x86)\Directories" "Library Dirs" $R1

No_include:

SectionEnd




;;
;; Setup Visual Studio 6 syntax highlighter and autoexp.dat
;;
Section "Show G3D Classes in MSVC6 Debugger"

  SectionIn 1

  ReadRegStr $R0 HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Text Editor\Tabs/Language Settings\C/C++" "FileExtensions"
  IfErrors No_syntax_settings

  StrCpy $R1 $R0
  Push $R0
  Push "gls"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_gls
  Goto Check_frg
Add_gls:
  StrCpy $R1 "$R1;gls"
Check_frg:
  Push $R0
  Push "frg"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_frg
  Goto Check_pix
Add_frg:
  StrCpy $R1 "$R1;frg"
Check_pix:
  Push $R0
  Push "pix"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_pix
  Goto Check_vrt
Add_pix:
  StrCpy $R1 "$R1;pix"
Check_vrt:
  Push $R0
  Push "vrt"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_vrt
  Goto Finish_check
Add_vrt:
  StrCpy $R1 "$R1;vrt"

Finish_check:
  WriteRegStr HKEY_CURRENT_USER "SOFTWARE\Microsoft\DevStudio\6.0\Text Editor\Tabs/Language Settings\C/C++" "FileExtensions" $R1
  
No_syntax_settings:  


;; Setup autoexp.dat for G3D types for debugging

  ; Search for MSVC6
  ReadRegStr $R0 HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  IfErrors No_autoexp
  
  FileOpen $0 "$R0\MSDev98\Bin\AUTOEXP.DAT" a
  IfErrors No_autoexp
Read_autoexp_loop:    
  FileRead $0 $R1
  IfErrors Finished_autoexp_read
  StrCmp $R1 "G3D::Quat=Quat(<x>,<y>,<z>,<w>)$\n" Autoexp_settings_exists Read_autoexp_loop

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




;;
;; Add the include/library G3D directories to the MSVC7 paths if they don't already exist
;;
Section "Add G3D Directories to MSVC7"

  SectionIn 1

  ; put 7.0 location into $R0
  Push "$PROFILE\Local Settings\Application Data\Microsoft\VisualStudio\7.0\VCComponents.dat"
  Pop $R0
  FileOpen $0 $R0 r
  IfErrors No_70_dat Read_vccomponents_loop
No_70_dat:
  ; put 7.1 location into $R0
  Push "$PROFILE\Local Settings\Application Data\Microsoft\VisualStudio\7.1\VCComponents.dat"
  Pop $R0
  FileOpen $0 $R0 r
  IfErrors No_71_dat
Read_vccomponents_loop:    
  FileRead $0 $R1
  IfErrors Finished_vccomponents_read
  Push $R1
  Push "Include Dirs="
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Check_library_dirs Add_dir_to_include

Add_dir_to_include:
  Push $R1
  Push "$INSTDIR\include"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Finish_adding_include Add_line_to_temp
Finish_adding_include:
  StrLen $R3 $R1
  IntOp $R3 $R3 - 2
  StrCpy $R1 $R1 $R3
  StrCpy $R1 "$R1;$INSTDIR\include$\r$\n"
  Goto Add_line_to_temp

Check_library_dirs:
  Push $R1
  Push "Library Dirs="
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Add_line_to_temp
  Push $R1
  Push "$INSTDIR\win32-7-lib"
  Call StrStr
  Pop $R2
  StrLen $R3 $R2
  IntCmpU 0 $R3 Finish_adding_lib Add_line_to_temp
Finish_adding_lib:
  StrLen $R3 $R1
  IntOp $R3 $R3 - 2
  StrCpy $R1 $R1 $R3
  StrCpy $R1 "$R1;$INSTDIR\win32-7-lib$\r$\n"
  Goto Add_line_to_temp
  
Add_line_to_temp:
  StrCpy $R9 $R9$R1
  Goto Read_vccomponents_loop


Finished_vccomponents_read:
  FileClose $0
  FileOpen $0 $R0 w
  IfErrors No_71_dat
  FileWrite $0 $R9
  Fileclose $0

No_71_dat:


SectionEnd




;;
;; Setup Visual Studio 7 syntax highlighter and autoexp.dat
;;
Section "Show G3D Classes in MSVC7 Debugger"

  SectionIn 1



;; Setup autoexp.dat for G3D types for debugging

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
  StrCmp $R1 "G3D::Quat=Quat(<x>,<y>,<z>,<w>)$\n" Autoexp_settings_exists Read_autoexp_loop

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




;;
;; Create link to documentation on desktop
;;
Section "Create Desktop Link for Documentation"

  SectionIn 1

  ; Create desktop shortcut
  CreateShortCut "$DESKTOP\Graphics3D.lnk" "$INSTDIR\html\index.html" "" "$INSTDIR\html\g3d.ico"

SectionEnd




;;
;; Open browser to download "data" module
;;
Section "Download G3D 6 Data Module"

  SectionIn 1

  ExecShell open "http://prdownloads.sourceforge.net/g3d-cpp/g3d-data-6_00.zip?download"
  
SectionEnd




;; Functions

; StrStr
 ; input, top of stack = string to search for
 ;        top of stack-1 = string to search in
 ; output, top of stack (replaces with the portion of the string remaining)
 ; modifies no other variables.
 ;
 ; Usage:
 ;   Push "this is a long ass string"
 ;   Push "ass"
 ;   Call StrStr
 ;   Pop $R0
 ;  ($R0 at this point is "ass string")

 Function StrStr
   Exch $R1 ; st=haystack,old$R1, $R1=needle
   Exch    ; st=old$R1,haystack
   Exch $R2 ; st=old$R1,old$R2, $R2=haystack
   Push $R3
   Push $R4
   Push $R5
   StrLen $R3 $R1
   StrCpy $R4 0
   ; $R1=needle
   ; $R2=haystack
   ; $R3=len(needle)
   ; $R4=cnt
   ; $R5=tmp
   loop:
     StrCpy $R5 $R2 $R3 $R4
     StrCmp $R5 $R1 done
     StrCmp $R5 "" done
     IntOp $R4 $R4 + 1
     Goto loop
 done:
   StrCpy $R1 $R2 "" $R4
   Pop $R5
   Pop $R4
   Pop $R3
   Pop $R2
   Exch $R1
 FunctionEnd
