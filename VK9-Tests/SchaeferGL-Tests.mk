##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=VK9-Tests
ConfigurationName      :=Debug
WorkspacePath          := "S:\Git\VK9"
ProjectPath            := "S:\Git\VK9\VK9-Tests"
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
Preprocessors          :=$(PreprocessorSwitch)WIN32 $(PreprocessorSwitch)_DEBUG $(PreprocessorSwitch)_WINDOWS 
ObjectSwitch           :=/Fo
ArchiveOutputSwitch    :=/OUT:
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="VK9-Tests.txt"
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
Objects0=$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix) $(IntermediateDirectory)/VK9-Tests.rc$(ObjectSuffix) 



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
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Main.cpp$(ObjectSuffix): Main.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Tests/Main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Main.cpp$(PreprocessSuffix): Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Main.cpp$(PreprocessSuffix) "Main.cpp"

$(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix): stdafx.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "S:/Git/VK9/VK9-Tests/stdafx.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/stdafx.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/stdafx.cpp$(PreprocessSuffix): stdafx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/stdafx.cpp$(PreprocessSuffix) "stdafx.cpp"

$(IntermediateDirectory)/VK9-Tests.rc$(ObjectSuffix): VK9-Tests.rc
	$(RcCompilerName) -i "S:/Git/VK9/VK9-Tests/VK9-Tests.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/VK9-Tests.rc$(ObjectSuffix) $(RcIncludePath)
##
## Clean
##
clean:
	$(RM) $(ConfigurationName)/*$(ObjectSuffix)
	$(RM) $(ConfigurationName)/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "../.build-debug/VK9-Tests"


