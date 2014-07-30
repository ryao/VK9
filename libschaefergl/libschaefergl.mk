##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=libschaefergl
ConfigurationName      :=Debug
WorkspacePath          := "/home/cschaefer/Documents/SchaeferGL"
ProjectPath            := "/home/cschaefer/Documents/SchaeferGL/libschaefergl"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Christopher Joseph Dean Schaefer
Date                   :=07/29/2014
CodeLitePath           :="/home/cschaefer/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).so
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="/home/cschaefer/Documents/SchaeferGL/libschaefergl/libschaefergl.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects=$(IntermediateDirectory)/i_unknown$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "/home/cschaefer/Documents/SchaeferGL/.build-debug"
	@echo rebuilt > "/home/cschaefer/Documents/SchaeferGL/.build-debug/libschaefergl"

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/i_unknown$(ObjectSuffix): i_unknown.cpp $(IntermediateDirectory)/i_unknown$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_unknown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_unknown$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_unknown$(DependSuffix): i_unknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_unknown$(ObjectSuffix) -MF$(IntermediateDirectory)/i_unknown$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_unknown.cpp"

$(IntermediateDirectory)/i_unknown$(PreprocessSuffix): i_unknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_unknown$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_unknown.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/i_unknown$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/home/cschaefer/Documents/SchaeferGL/.build-debug/libschaefergl"


