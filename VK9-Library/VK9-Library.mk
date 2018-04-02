##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=VK9-Library
ConfigurationName      :=Debug
WorkspacePath          := "S:\Git\VK9"
ProjectPath            := "S:\Git\VK9\VK9-Library"
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Christopher
Date                   :=09/01/14
CodeLitePath           :="S:\Applications\CodeLite"
LinkerName             :="S:/Applications/Microsoft Visual Studio 9.0/VC/bin/link.exe" -nologo
SharedObjectLinkerName :="S:/Applications/Microsoft Visual Studio 9.0/VC/bin/link.exe" -DLL -nologo
ObjectSuffix           :=.obj
DependSuffix           :=
PreprocessSuffix       :=.i
DebugSwitch            :=/Zi 
IncludeSwitch          :=/I
LibrarySwitch          := 
OutputSwitch           :=/OUT:
LibraryPathSwitch      :=/LIBPATH:
PreprocessorSwitch     :=/D
SourceSwitch           :=-c 
OutputFile             :=
Preprocessors          :=$(PreprocessorSwitch)WIN32 $(PreprocessorSwitch)_DEBUG $(PreprocessorSwitch)_WINDOWS $(PreprocessorSwitch)_USRDLL $(PreprocessorSwitch)SCHAEFERGLLIBRARY_EXPORTS $(PreprocessorSwitch)NOMINMAX
ObjectSwitch           :=/Fo
ArchiveOutputSwitch    :=/OUT:
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="VK9-Library.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=S:/Applications/MinGW-4.8.1/bin/windres.exe 
LinkOptions            :=  -O0
IncludePath            := $(IncludeSwitch)"C:/Program Files/Microsoft SDKs/Windows/v6.0A/include" $(IncludeSwitch)"S:/Applications/Microsoft Visual Studio 9.0/VC/include"  $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                :=$(LibraryPathSwitch)"C:/Program Files/Microsoft SDKs/Windows/v6.0A/lib" $(LibraryPathSwitch)"S:/Applications/Microsoft Visual Studio 9.0/VC/lib"  $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := "S:/Applications/Microsoft Visual Studio 9.0/VC/bin/lib.exe" -nologo
CXX      := "S:/Applications/Microsoft Visual Studio 9.0/VC/bin/cl.exe" -nologo -FC -EHs
CC       := "S:/Applications/Microsoft Visual Studio 9.0/VC/bin/cl.exe" -nologo -FC
CXXFLAGS :=  -g -Wall $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=S:\Applications\CodeLite
UNIT_TEST_PP_SRC_DIR:=S:\Applications\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix) $(IntermediateDirectory)/dllmain.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3D9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DBaseTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DDevice9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DIndexBuffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DPixelShader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DQuery9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DResource9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DStateBlock9.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/IDirect3DSurface9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DSwapChain9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DVertexBuffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DVertexDeclaration9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DVertexShader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DVolume9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DVolumeTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IUnknown.cpp$(ObjectSuffix) $(IntermediateDirectory)/VK9-Library.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLUnknown.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLResource9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLBaseTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLCubeTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLVolumeTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLSurface9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGL9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLVertexDeclaration9.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/COpenGLQuery9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLVertexBuffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLIndexBuffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLPixelShader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLVertexShader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLDevice9.cpp$(ObjectSuffix) $(IntermediateDirectory)/IDirect3DCubeTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/COpenGLTypes.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "S:\Git\VK9/.build-debug"
	@echo rebuilt > "S:\Git\VK9/.build-debug/VK9-Library"

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix): D3D9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/D3D9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/D3D9.cpp$(PreprocessSuffix): D3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/D3D9.cpp$(PreprocessSuffix) "D3D9.cpp"

$(IntermediateDirectory)/dllmain.cpp$(ObjectSuffix): dllmain.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/dllmain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dllmain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dllmain.cpp$(PreprocessSuffix): dllmain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dllmain.cpp$(PreprocessSuffix) "dllmain.cpp"

$(IntermediateDirectory)/IDirect3D9.cpp$(ObjectSuffix): IDirect3D9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3D9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3D9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3D9.cpp$(PreprocessSuffix): IDirect3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3D9.cpp$(PreprocessSuffix) "IDirect3D9.cpp"

$(IntermediateDirectory)/IDirect3DBaseTexture9.cpp$(ObjectSuffix): IDirect3DBaseTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DBaseTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DBaseTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DBaseTexture9.cpp$(PreprocessSuffix): IDirect3DBaseTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DBaseTexture9.cpp$(PreprocessSuffix) "IDirect3DBaseTexture9.cpp"

$(IntermediateDirectory)/IDirect3DDevice9.cpp$(ObjectSuffix): IDirect3DDevice9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DDevice9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DDevice9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DDevice9.cpp$(PreprocessSuffix): IDirect3DDevice9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DDevice9.cpp$(PreprocessSuffix) "IDirect3DDevice9.cpp"

$(IntermediateDirectory)/IDirect3DIndexBuffer9.cpp$(ObjectSuffix): IDirect3DIndexBuffer9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DIndexBuffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DIndexBuffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DIndexBuffer9.cpp$(PreprocessSuffix): IDirect3DIndexBuffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DIndexBuffer9.cpp$(PreprocessSuffix) "IDirect3DIndexBuffer9.cpp"

$(IntermediateDirectory)/IDirect3DPixelShader9.cpp$(ObjectSuffix): IDirect3DPixelShader9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DPixelShader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DPixelShader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DPixelShader9.cpp$(PreprocessSuffix): IDirect3DPixelShader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DPixelShader9.cpp$(PreprocessSuffix) "IDirect3DPixelShader9.cpp"

$(IntermediateDirectory)/IDirect3DQuery9.cpp$(ObjectSuffix): IDirect3DQuery9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DQuery9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DQuery9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DQuery9.cpp$(PreprocessSuffix): IDirect3DQuery9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DQuery9.cpp$(PreprocessSuffix) "IDirect3DQuery9.cpp"

$(IntermediateDirectory)/IDirect3DResource9.cpp$(ObjectSuffix): IDirect3DResource9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DResource9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DResource9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DResource9.cpp$(PreprocessSuffix): IDirect3DResource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DResource9.cpp$(PreprocessSuffix) "IDirect3DResource9.cpp"

$(IntermediateDirectory)/IDirect3DStateBlock9.cpp$(ObjectSuffix): IDirect3DStateBlock9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DStateBlock9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DStateBlock9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DStateBlock9.cpp$(PreprocessSuffix): IDirect3DStateBlock9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DStateBlock9.cpp$(PreprocessSuffix) "IDirect3DStateBlock9.cpp"

$(IntermediateDirectory)/IDirect3DSurface9.cpp$(ObjectSuffix): IDirect3DSurface9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DSurface9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DSurface9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DSurface9.cpp$(PreprocessSuffix): IDirect3DSurface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DSurface9.cpp$(PreprocessSuffix) "IDirect3DSurface9.cpp"

$(IntermediateDirectory)/IDirect3DSwapChain9.cpp$(ObjectSuffix): IDirect3DSwapChain9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DSwapChain9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DSwapChain9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DSwapChain9.cpp$(PreprocessSuffix): IDirect3DSwapChain9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DSwapChain9.cpp$(PreprocessSuffix) "IDirect3DSwapChain9.cpp"

$(IntermediateDirectory)/IDirect3DTexture9.cpp$(ObjectSuffix): IDirect3DTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DTexture9.cpp$(PreprocessSuffix): IDirect3DTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DTexture9.cpp$(PreprocessSuffix) "IDirect3DTexture9.cpp"

$(IntermediateDirectory)/IDirect3DVertexBuffer9.cpp$(ObjectSuffix): IDirect3DVertexBuffer9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DVertexBuffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DVertexBuffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DVertexBuffer9.cpp$(PreprocessSuffix): IDirect3DVertexBuffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DVertexBuffer9.cpp$(PreprocessSuffix) "IDirect3DVertexBuffer9.cpp"

$(IntermediateDirectory)/IDirect3DVertexDeclaration9.cpp$(ObjectSuffix): IDirect3DVertexDeclaration9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DVertexDeclaration9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DVertexDeclaration9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DVertexDeclaration9.cpp$(PreprocessSuffix): IDirect3DVertexDeclaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DVertexDeclaration9.cpp$(PreprocessSuffix) "IDirect3DVertexDeclaration9.cpp"

$(IntermediateDirectory)/IDirect3DVertexShader9.cpp$(ObjectSuffix): IDirect3DVertexShader9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DVertexShader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DVertexShader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DVertexShader9.cpp$(PreprocessSuffix): IDirect3DVertexShader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DVertexShader9.cpp$(PreprocessSuffix) "IDirect3DVertexShader9.cpp"

$(IntermediateDirectory)/IDirect3DVolume9.cpp$(ObjectSuffix): IDirect3DVolume9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DVolume9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DVolume9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DVolume9.cpp$(PreprocessSuffix): IDirect3DVolume9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DVolume9.cpp$(PreprocessSuffix) "IDirect3DVolume9.cpp"

$(IntermediateDirectory)/IDirect3DVolumeTexture9.cpp$(ObjectSuffix): IDirect3DVolumeTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DVolumeTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DVolumeTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DVolumeTexture9.cpp$(PreprocessSuffix): IDirect3DVolumeTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DVolumeTexture9.cpp$(PreprocessSuffix) "IDirect3DVolumeTexture9.cpp"

$(IntermediateDirectory)/IUnknown.cpp$(ObjectSuffix): IUnknown.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IUnknown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IUnknown.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IUnknown.cpp$(PreprocessSuffix): IUnknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IUnknown.cpp$(PreprocessSuffix) "IUnknown.cpp"

$(IntermediateDirectory)/VK9-Library.cpp$(ObjectSuffix): VK9-Library.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/VK9-Library.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/VK9-Library.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/VK9-Library.cpp$(PreprocessSuffix): VK9-Library.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/VK9-Library.cpp$(PreprocessSuffix) "VK9-Library.cpp"

$(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix): stdafx.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/stdafx.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/stdafx.cpp$(PreprocessSuffix): stdafx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/stdafx.cpp$(PreprocessSuffix) "stdafx.cpp"

$(IntermediateDirectory)/COpenGLUnknown.cpp$(ObjectSuffix): COpenGLUnknown.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLUnknown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLUnknown.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLUnknown.cpp$(PreprocessSuffix): COpenGLUnknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLUnknown.cpp$(PreprocessSuffix) "COpenGLUnknown.cpp"

$(IntermediateDirectory)/COpenGLResource9.cpp$(ObjectSuffix): COpenGLResource9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLResource9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLResource9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLResource9.cpp$(PreprocessSuffix): COpenGLResource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLResource9.cpp$(PreprocessSuffix) "COpenGLResource9.cpp"

$(IntermediateDirectory)/COpenGLBaseTexture9.cpp$(ObjectSuffix): COpenGLBaseTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLBaseTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLBaseTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLBaseTexture9.cpp$(PreprocessSuffix): COpenGLBaseTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLBaseTexture9.cpp$(PreprocessSuffix) "COpenGLBaseTexture9.cpp"

$(IntermediateDirectory)/COpenGLTexture9.cpp$(ObjectSuffix): COpenGLTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLTexture9.cpp$(PreprocessSuffix): COpenGLTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLTexture9.cpp$(PreprocessSuffix) "COpenGLTexture9.cpp"

$(IntermediateDirectory)/COpenGLCubeTexture9.cpp$(ObjectSuffix): COpenGLCubeTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLCubeTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLCubeTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLCubeTexture9.cpp$(PreprocessSuffix): COpenGLCubeTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLCubeTexture9.cpp$(PreprocessSuffix) "COpenGLCubeTexture9.cpp"

$(IntermediateDirectory)/COpenGLVolumeTexture9.cpp$(ObjectSuffix): COpenGLVolumeTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLVolumeTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLVolumeTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLVolumeTexture9.cpp$(PreprocessSuffix): COpenGLVolumeTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLVolumeTexture9.cpp$(PreprocessSuffix) "COpenGLVolumeTexture9.cpp"

$(IntermediateDirectory)/COpenGLSurface9.cpp$(ObjectSuffix): COpenGLSurface9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLSurface9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLSurface9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLSurface9.cpp$(PreprocessSuffix): COpenGLSurface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLSurface9.cpp$(PreprocessSuffix) "COpenGLSurface9.cpp"

$(IntermediateDirectory)/COpenGL9.cpp$(ObjectSuffix): COpenGL9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGL9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGL9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGL9.cpp$(PreprocessSuffix): COpenGL9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGL9.cpp$(PreprocessSuffix) "COpenGL9.cpp"

$(IntermediateDirectory)/COpenGLVertexDeclaration9.cpp$(ObjectSuffix): COpenGLVertexDeclaration9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLVertexDeclaration9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLVertexDeclaration9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLVertexDeclaration9.cpp$(PreprocessSuffix): COpenGLVertexDeclaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLVertexDeclaration9.cpp$(PreprocessSuffix) "COpenGLVertexDeclaration9.cpp"

$(IntermediateDirectory)/COpenGLQuery9.cpp$(ObjectSuffix): COpenGLQuery9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLQuery9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLQuery9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLQuery9.cpp$(PreprocessSuffix): COpenGLQuery9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLQuery9.cpp$(PreprocessSuffix) "COpenGLQuery9.cpp"

$(IntermediateDirectory)/COpenGLVertexBuffer9.cpp$(ObjectSuffix): COpenGLVertexBuffer9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLVertexBuffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLVertexBuffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLVertexBuffer9.cpp$(PreprocessSuffix): COpenGLVertexBuffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLVertexBuffer9.cpp$(PreprocessSuffix) "COpenGLVertexBuffer9.cpp"

$(IntermediateDirectory)/COpenGLIndexBuffer9.cpp$(ObjectSuffix): COpenGLIndexBuffer9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLIndexBuffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLIndexBuffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLIndexBuffer9.cpp$(PreprocessSuffix): COpenGLIndexBuffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLIndexBuffer9.cpp$(PreprocessSuffix) "COpenGLIndexBuffer9.cpp"

$(IntermediateDirectory)/COpenGLPixelShader9.cpp$(ObjectSuffix): COpenGLPixelShader9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLPixelShader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLPixelShader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLPixelShader9.cpp$(PreprocessSuffix): COpenGLPixelShader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLPixelShader9.cpp$(PreprocessSuffix) "COpenGLPixelShader9.cpp"

$(IntermediateDirectory)/COpenGLVertexShader9.cpp$(ObjectSuffix): COpenGLVertexShader9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLVertexShader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLVertexShader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLVertexShader9.cpp$(PreprocessSuffix): COpenGLVertexShader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLVertexShader9.cpp$(PreprocessSuffix) "COpenGLVertexShader9.cpp"

$(IntermediateDirectory)/COpenGLDevice9.cpp$(ObjectSuffix): COpenGLDevice9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLDevice9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLDevice9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLDevice9.cpp$(PreprocessSuffix): COpenGLDevice9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLDevice9.cpp$(PreprocessSuffix) "COpenGLDevice9.cpp"

$(IntermediateDirectory)/IDirect3DCubeTexture9.cpp$(ObjectSuffix): IDirect3DCubeTexture9.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/IDirect3DCubeTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/IDirect3DCubeTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/IDirect3DCubeTexture9.cpp$(PreprocessSuffix): IDirect3DCubeTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/IDirect3DCubeTexture9.cpp$(PreprocessSuffix) "IDirect3DCubeTexture9.cpp"

$(IntermediateDirectory)/COpenGLTypes.cpp$(ObjectSuffix): COpenGLTypes.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Library/COpenGLTypes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/COpenGLTypes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/COpenGLTypes.cpp$(PreprocessSuffix): COpenGLTypes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/COpenGLTypes.cpp$(PreprocessSuffix) "COpenGLTypes.cpp"

##
## Clean
##
clean:
	$(RM) $(ConfigurationName)/*$(ObjectSuffix)
	$(RM) $(ConfigurationName)/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/VK9-Library"


