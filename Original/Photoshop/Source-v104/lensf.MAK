# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=lensf - Win32 Hybrid
!MESSAGE No configuration specified.  Defaulting to lensf - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "lensf - Win32 Release" && "$(CFG)" != "lensf - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "lensf.MAK" CFG="lensf - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lensf - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lensf - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "lensf - Win32 Hybrid"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "lensf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\genesis.8bf"

CLEAN : 
	-@erase "..\..\genesis.8bf"
	-@erase ".\Release\CustomControls.obj"
	-@erase ".\Release\Flare.obj"
	-@erase ".\Release\Exception.obj"
	-@erase ".\Release\lensf.obj"
	-@erase ".\Release\WinUtilities.obj"
	-@erase ".\Release\PIUtilities.obj"
	-@erase ".\Release\WinDialogUtils.obj"
	-@erase ".\Release\LensFlares.obj"
	-@erase ".\Release\GFXfileman.obj"
	-@erase ".\Release\Fractal.obj"
	-@erase ".\Release\FColour.obj"
	-@erase ".\Release\NoiseStuff.obj"
	-@erase ".\Release\PhotoMain.obj"
	-@erase ".\Release\lensf.res"
	-@erase ".\Release\genesis.lib"
	-@erase ".\Release\genesis.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O2 /I "c:\msdev\include\sys" /I ".\zlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /I "c:\msdev\include\sys" /I ".\zlib" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lensf.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lensf.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib zlib.lib /nologo /entry:"DLLInit" /subsystem:windows /dll /machine:I386 /out:"d:\wilson\genesis.8bf"
# SUBTRACT LINK32 /profile /incremental:yes
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib zlib.lib\
 /nologo /entry:"DLLInit" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/genesis.pdb" /machine:I386 /def:".\lensf.def"\
 /out:"d:\wilson\genesis.8bf" /implib:"$(OUTDIR)/genesis.lib" 
DEF_FILE= \
	".\lensf.def"
LINK32_OBJS= \
	"$(INTDIR)/CustomControls.obj" \
	"$(INTDIR)/Flare.obj" \
	"$(INTDIR)/Exception.obj" \
	"$(INTDIR)/lensf.obj" \
	"$(INTDIR)/WinUtilities.obj" \
	"$(INTDIR)/PIUtilities.obj" \
	"$(INTDIR)/WinDialogUtils.obj" \
	"$(INTDIR)/LensFlares.obj" \
	"$(INTDIR)/GFXfileman.obj" \
	"$(INTDIR)/Fractal.obj" \
	"$(INTDIR)/FColour.obj" \
	"$(INTDIR)/NoiseStuff.obj" \
	"$(INTDIR)/PhotoMain.obj" \
	"$(INTDIR)/lensf.res"

"$(OUTDIR)\genesis.8bf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lensf"
# PROP BASE Intermediate_Dir "lensf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Hybrid"
# PROP Intermediate_Dir "Hybrid"
# PROP Target_Dir ""
OUTDIR=.\Hybrid
INTDIR=.\Hybrid

ALL : "$(OUTDIR)\gentest.8bf"

CLEAN : 
	-@erase ".\Hybrid\vc40.pdb"
	-@erase ".\Hybrid\vc40.idb"
	-@erase "C:\Program Files\Micrografx\Picture Publisher 8\Plugins\gentest.8bf"
	-@erase ".\Hybrid\WinUtilities.obj"
	-@erase ".\Hybrid\Exception.obj"
	-@erase ".\Hybrid\WinDialogUtils.obj"
	-@erase ".\Hybrid\PhotoMain.obj"
	-@erase ".\Hybrid\LensFlares.obj"
	-@erase ".\Hybrid\Flare.obj"
	-@erase ".\Hybrid\PIUtilities.obj"
	-@erase ".\Hybrid\NoiseStuff.obj"
	-@erase ".\Hybrid\lensf.obj"
	-@erase ".\Hybrid\Fractal.obj"
	-@erase ".\Hybrid\FColour.obj"
	-@erase ".\Hybrid\CustomControls.obj"
	-@erase ".\Hybrid\GFXfileman.obj"
	-@erase ".\Hybrid\lensf.res"
	-@erase "C:\Program Files\Micrografx\Picture Publisher 8\Plugins\gentest.ilk"
	-@erase ".\Hybrid\gentest.lib"
	-@erase ".\Hybrid\gentest.exp"
	-@erase ".\Hybrid\gentest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "c:\msdev\include\sys" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "c:\msdev\include\sys" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/lensf.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/lensf.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\exe\stdplugs\lensf.flt"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib zlib.lib /nologo /entry:"DLLInit" /subsystem:windows /dll /debug /machine:I386 /out:"C:\Program Files\Micrografx\Picture Publisher 8\Plugins\gentest.8bf"
# SUBTRACT LINK32 /profile /map
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib zlib.lib\
 /nologo /entry:"DLLInit" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/gentest.pdb" /debug /machine:I386 /def:".\lensf.def"\
 /out:"C:\Program Files\Micrografx\Picture Publisher 8\Plugins\gentest.8bf"\
 /implib:"$(OUTDIR)/gentest.lib" 
DEF_FILE= \
	".\lensf.def"
LINK32_OBJS= \
	"$(INTDIR)/WinUtilities.obj" \
	"$(INTDIR)/Exception.obj" \
	"$(INTDIR)/WinDialogUtils.obj" \
	"$(INTDIR)/PhotoMain.obj" \
	"$(INTDIR)/LensFlares.obj" \
	"$(INTDIR)/Flare.obj" \
	"$(INTDIR)/PIUtilities.obj" \
	"$(INTDIR)/NoiseStuff.obj" \
	"$(INTDIR)/lensf.obj" \
	"$(INTDIR)/Fractal.obj" \
	"$(INTDIR)/FColour.obj" \
	"$(INTDIR)/CustomControls.obj" \
	"$(INTDIR)/GFXfileman.obj" \
	"$(INTDIR)/lensf.res"

"$(OUTDIR)\gentest.8bf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "lensf - Win32 Release"
# Name "lensf - Win32 Hybrid"

!IF  "$(CFG)" == "lensf - Win32 Release"

!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\lensf.rc
DEP_RSC_LENSF=\
	".\Dialog\Register\RegOk.bmp"\
	".\Dialog\Master\logo.bmp"\
	".\Dialog\Master\MasterHelp.BMP"\
	".\Dialog\Master\complain.BMP"\
	".\Dialog\help\Small_Genesis.bmp"\
	".\Dialog\Master\Master.bmp"\
	".\Dialog\Master\ElementNew.BMP"\
	".\Dialog\Master\ElementCopy.BMP"\
	".\Dialog\Master\ElementDelete.BMP"\
	".\Dialog\Master\SystemNew.BMP"\
	".\Dialog\Master\SystemLoad.BMP"\
	".\Dialog\Master\SystemSave.BMP"\
	".\Dialog\Master\SystemConfig.BMP"\
	".\Dialog\Master\LockUp.BMP"\
	".\Dialog\Master\LockDown.BMP"\
	".\Dialog\Master\ANimationShapeUp.BMP"\
	".\Dialog\Master\ANimationShape.BMP"\
	".\Dialog\Master\ANimationPosnUp.BMP"\
	".\Dialog\Master\ANimationPosn.BMP"\
	".\Dialog\Master\ShapeNoise.BMP"\
	".\Dialog\Master\ShapeMorph.BMP"\
	".\Dialog\Master\ShapeMake.BMP"\
	".\Dialog\Master\ShapeEdit.BMP"\
	".\Dialog\Master\ShapeSave.BMP"\
	".\Dialog\Master\ShapeLoad.BMP"\
	".\Dialog\Master\UpArrow.BMP"\
	".\Dialog\Master\DownArrow.BMP"\
	".\Dialog\Master\MasterOk.BMP"\
	".\Dialog\Master\MasterCancel.BMP"\
	".\Dialog\Edit\Edit.BMP"\
	".\Dialog\Edit\Exp2.BMP"\
	".\Dialog\Edit\Exp.BMP"\
	".\Dialog\Edit\Hyp.BMP"\
	".\Dialog\Edit\Lin.BMP"\
	".\Dialog\Edit\Sin.BMP"\
	".\Dialog\Edit\Noise.BMP"\
	".\dialog\Edit\sin_PUSHED.bmp"\
	".\dialog\Edit\noise_pushed.bmp"\
	".\dialog\Edit\lin_pushed.bmp"\
	".\dialog\Edit\hyp_pushed.bmp"\
	".\dialog\Edit\exp2_pushed.bmp"\
	".\dialog\Edit\exp_pushed.bmp"\
	".\Dialog\Edit\Build.BMP"\
	".\Dialog\Edit\Normalise.BMP"\
	".\Dialog\Edit\Reset.BMP"\
	".\Dialog\Edit\ResetCurve.BMP"\
	".\Dialog\Edit\Swap.BMP"\
	".\Dialog\Edit\Undo.BMP"\
	".\Dialog\Edit\LockUp.BMP"\
	".\Dialog\Edit\LockDown.BMP"\
	".\Dialog\Make\Make.BMP"\
	".\Dialog\Make\Button1.BMP"\
	".\Dialog\Make\Button2.BMP"\
	".\Dialog\Make\Button3.BMP"\
	".\Dialog\Make\Button4.BMP"\
	".\Dialog\Make\Button5.BMP"\
	".\Dialog\Make\Button6.BMP"\
	".\Dialog\Make\Button7.BMP"\
	".\Dialog\Make\Button8.BMP"\
	".\Dialog\Make\Button2_1.BMP"\
	".\Dialog\Make\Button2_2.BMP"\
	".\Dialog\Make\Button2_3.BMP"\
	".\Dialog\Make\Button2_4.BMP"\
	".\Dialog\Make\Button1_UnPushed.bmp"\
	".\Dialog\Make\Button2_UnPushed.bmp"\
	".\Dialog\Make\Button3_UnPushed.bmp"\
	".\Dialog\Make\Button4_UnPushed.bmp"\
	".\Dialog\Make\Button5_UnPushed.bmp"\
	".\Dialog\Make\Button6_UnPushed.bmp"\
	".\Dialog\Make\Button7_UnPushed.bmp"\
	".\Dialog\Make\Button8_UnPushed.bmp"\
	".\Dialog\Make\Button2_1_UnPushed.bmp"\
	".\Dialog\Make\Button2_2_UnPushed.bmp"\
	".\Dialog\Make\Button2_3_UnPushed.bmp"\
	".\dialog\master\bitmap1.bmp"\
	".\Dialog\Morph\Morph.BMP"\
	".\Dialog\Noise\Noise.BMP"\
	".\Dialog\Noise\RegularActive.BMP"\
	".\Dialog\Noise\TurbActive.bmp"\
	".\Dialog\Noise\TurbActive_Down.BMP"\
	".\Dialog\Noise\RegularActive_Down.BMP"\
	".\Dialog\Noise\RegularLock_Up.BMP"\
	".\Dialog\Noise\RegularLock_Down.BMP"\
	".\Dialog\Noise\TurbLock_Up.BMP"\
	".\Dialog\Noise\TurbLock_Down.BMP"\
	".\Dialog\Render\render.BMP"\
	".\Dialog\Render\centroid.BMP"\
	".\Dialog\Render\colorise.BMP"\
	".\Dialog\Render\colour.BMP"\
	".\Dialog\Render\external.BMP"\
	".\Dialog\Render\high.BMP"\
	".\Dialog\Render\inside.bmp"\
	".\Dialog\Render\internal.BMP"\
	".\Dialog\Render\maskext.BMP"\
	".\Dialog\Render\maskint.BMP"\
	".\Dialog\Render\medium.BMP"\
	".\Dialog\Render\multi.BMP"\
	".\Dialog\Render\nomask.BMP"\
	".\Dialog\Render\none.BMP"\
	".\Dialog\Render\normal.BMP"\
	".\Dialog\Render\outside.bmp"\
	".\Dialog\Render\resize.bmp"\
	".\Dialog\Render\single.BMP"\
	".\Dialog\Render\renderexit.bmp"\
	".\Dialog\Render\close.BMP"\
	

!IF  "$(CFG)" == "lensf - Win32 Release"


"$(INTDIR)\lensf.res" : $(SOURCE) $(DEP_RSC_LENSF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"


"$(INTDIR)\lensf.res" : $(SOURCE) $(DEP_RSC_LENSF) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lensf.cpp

!IF  "$(CFG)" == "lensf - Win32 Release"

DEP_CPP_LENSF_=\
	".\Exception.h"\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\Utils.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\lensf.H"\
	".\GFXfileman.h"\
	".\PhotoRender.cpp"\
	".\Inferno.cpp"\
	".\FlareBuild.cpp"\
	".\FlareEdit.cpp"\
	".\FlaresEdit.cpp"\
	".\Morph.cpp"\
	".\Render.cpp"\
	".\FileBin.h"\
	".\zlib.h"\
	".\FlaresEdit_File.cpp"\
	

"$(INTDIR)\lensf.obj" : $(SOURCE) $(DEP_CPP_LENSF_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

DEP_CPP_LENSF_=\
	".\Exception.h"\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\Utils.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\lensf.H"\
	".\GFXfileman.h"\
	".\PhotoRender.cpp"\
	".\Inferno.cpp"\
	".\FlareBuild.cpp"\
	".\FlareEdit.cpp"\
	".\FlaresEdit.cpp"\
	".\Morph.cpp"\
	".\Render.cpp"\
	".\FileBin.h"\
	".\Fractal.h"\
	".\version.h"\
	".\zlib.h"\
	".\zlib\zconf.h"\
	".\FlaresEdit_File.cpp"\
	

"$(INTDIR)\lensf.obj" : $(SOURCE) $(DEP_CPP_LENSF_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lensf.def

!IF  "$(CFG)" == "lensf - Win32 Release"

!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Flare.cpp

!IF  "$(CFG)" == "lensf - Win32 Release"

DEP_CPP_FLARE=\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\FileBin.h"\
	".\Utils.h"\
	

"$(INTDIR)\Flare.obj" : $(SOURCE) $(DEP_CPP_FLARE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

DEP_CPP_FLARE=\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\FileBin.h"\
	".\Utils.h"\
	".\Fractal.h"\
	".\version.h"\
	

"$(INTDIR)\Flare.obj" : $(SOURCE) $(DEP_CPP_FLARE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LensFlares.cpp

!IF  "$(CFG)" == "lensf - Win32 Release"

DEP_CPP_LENSFL=\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\Utils.h"\
	".\FileBin.h"\
	

"$(INTDIR)\LensFlares.obj" : $(SOURCE) $(DEP_CPP_LENSFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

DEP_CPP_LENSFL=\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\Utils.h"\
	".\FileBin.h"\
	".\Fractal.h"\
	".\version.h"\
	

"$(INTDIR)\LensFlares.obj" : $(SOURCE) $(DEP_CPP_LENSFL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FColour.cpp
DEP_CPP_FCOLO=\
	".\FColour.h"\
	".\FileBin.h"\
	

"$(INTDIR)\FColour.obj" : $(SOURCE) $(DEP_CPP_FCOLO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Fractal.cpp
DEP_CPP_FRACT=\
	".\Fractal.h"\
	".\FileBin.h"\
	

"$(INTDIR)\Fractal.obj" : $(SOURCE) $(DEP_CPP_FRACT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CustomControls.cpp
DEP_CPP_CUSTO=\
	".\CustomControls.h"\
	".\PhotoUtil.h"\
	

"$(INTDIR)\CustomControls.obj" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\photoshop\PIUtilities.c
DEP_CPP_PIUTI=\
	".\photoshop\PIUtilities.h"\
	"C:\MSDEV\INCLUDE\sys\TYPES.H"\
	".\photoshop\PITypes.h"\
	".\photoshop\PIGeneral.h"\
	".\photoshop\PIActions.h"\
	".\photoshop\PIAbout.h"\
	".\photoshop\DialogUtilities.h"\
	".\photoshop\WinUtilities.h"\
	".\photoshop\WinDialogUtils.h"\
	
NODEP_CPP_PIUTI=\
	".\photoshop\LowMem.h"\
	".\photoshop\Types.r"\
	".\photoshop\AEUserTermTypes.r"\
	".\photoshop\AppleEvents.r"\
	".\photoshop\AERegistry.h"\
	

"$(INTDIR)\PIUtilities.obj" : $(SOURCE) $(DEP_CPP_PIUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\photoshop\WinDialogUtils.c
DEP_CPP_WINDI=\
	".\photoshop\WinDialogUtils.h"\
	".\photoshop\PITypes.h"\
	".\photoshop\PIAbout.h"\
	".\photoshop\PIUtilities.h"\
	"C:\MSDEV\INCLUDE\sys\TYPES.H"\
	".\photoshop\PIGeneral.h"\
	".\photoshop\PIActions.h"\
	".\photoshop\DialogUtilities.h"\
	".\photoshop\WinUtilities.h"\
	
NODEP_CPP_WINDI=\
	".\photoshop\LowMem.h"\
	".\photoshop\Types.r"\
	".\photoshop\AEUserTermTypes.r"\
	".\photoshop\AppleEvents.r"\
	".\photoshop\AERegistry.h"\
	

"$(INTDIR)\WinDialogUtils.obj" : $(SOURCE) $(DEP_CPP_WINDI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\photoshop\WinUtilities.c
DEP_CPP_WINUT=\
	".\photoshop\WinUtilities.h"\
	".\photoshop\PITypes.h"\
	".\photoshop\WinDialogUtils.h"\
	"C:\MSDEV\INCLUDE\sys\TYPES.H"\
	".\photoshop\PIAbout.h"\
	".\photoshop\PIUtilities.h"\
	".\photoshop\PIGeneral.h"\
	".\photoshop\PIActions.h"\
	".\photoshop\DialogUtilities.h"\
	
NODEP_CPP_WINUT=\
	".\photoshop\LowMem.h"\
	".\photoshop\Types.r"\
	".\photoshop\AEUserTermTypes.r"\
	".\photoshop\AppleEvents.r"\
	".\photoshop\AERegistry.h"\
	

"$(INTDIR)\WinUtilities.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\NoiseStuff.cpp

"$(INTDIR)\NoiseStuff.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PhotoMain.cpp

!IF  "$(CFG)" == "lensf - Win32 Release"

DEP_CPP_PHOTO=\
	".\CustomControls.h"\
	".\Utils.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\version.h"\
	".\lensf.H"\
	".\PhotoMain.h"\
	".\Exception.h"\
	".\PhotoUtil.cpp"\
	".\FileBin.h"\
	".\photoshop\PIFilter.h"\
	".\photoshop\PIUtilities.h"\
	".\photoshop\PIProperties.h"\
	".\photoshop\PIActions.h"\
	".\photoshop\PIGeneral.h"\
	".\photoshop\PITypes.h"\
	"C:\MSDEV\INCLUDE\sys\TYPES.H"\
	".\photoshop\PIAbout.h"\
	".\photoshop\DialogUtilities.h"\
	".\photoshop\WinUtilities.h"\
	".\photoshop\WinDialogUtils.h"\
	
NODEP_CPP_PHOTO=\
	".\photoshop\Types.r"\
	".\photoshop\AEUserTermTypes.r"\
	".\photoshop\AppleEvents.r"\
	".\photoshop\AERegistry.h"\
	".\photoshop\LowMem.h"\
	

"$(INTDIR)\PhotoMain.obj" : $(SOURCE) $(DEP_CPP_PHOTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

DEP_CPP_PHOTO=\
	".\CustomControls.h"\
	".\Utils.h"\
	".\PhotoUtil.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\version.h"\
	".\lensf.H"\
	".\PhotoMain.h"\
	".\Exception.h"\
	".\PhotoUtil.cpp"\
	".\FileBin.h"\
	".\Fractal.h"\
	".\photoshop\PIFilter.h"\
	

"$(INTDIR)\PhotoMain.obj" : $(SOURCE) $(DEP_CPP_PHOTO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Exception.cpp
DEP_CPP_EXCEP=\
	".\Exception.h"\
	

"$(INTDIR)\Exception.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GFXfileman.cpp

!IF  "$(CFG)" == "lensf - Win32 Release"

DEP_CPP_GFXFI=\
	".\Exception.h"\
	".\PhotoUtil.h"\
	".\Utils.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\lensf.H"\
	".\GFXfileman.h"\
	".\FileBin.h"\
	".\zlib.h"\
	

"$(INTDIR)\GFXfileman.obj" : $(SOURCE) $(DEP_CPP_GFXFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "lensf - Win32 Hybrid"

DEP_CPP_GFXFI=\
	".\Exception.h"\
	".\PhotoUtil.h"\
	".\Utils.h"\
	".\FColour.h"\
	".\flare.h"\
	".\LensFlares.h"\
	".\lensf.H"\
	".\GFXfileman.h"\
	".\FileBin.h"\
	".\Fractal.h"\
	".\version.h"\
	".\zlib.h"\
	".\zlib\zconf.h"\
	

"$(INTDIR)\GFXfileman.obj" : $(SOURCE) $(DEP_CPP_GFXFI) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
