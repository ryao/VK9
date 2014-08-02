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
Date                   :=08/02/2014
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
Objects=$(IntermediateDirectory)/i_unknown$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix) 

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

$(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix): i_direct3d_device9.cpp $(IntermediateDirectory)/i_direct3d_device9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_device9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_device9$(DependSuffix): i_direct3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_device9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_device9.cpp"

$(IntermediateDirectory)/i_direct3d_device9$(PreprocessSuffix): i_direct3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_device9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_device9.cpp"

$(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix): i_direct3d_resource9.cpp $(IntermediateDirectory)/i_direct3d_resource9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_resource9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_resource9$(DependSuffix): i_direct3d_resource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_resource9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_resource9.cpp"

$(IntermediateDirectory)/i_direct3d_resource9$(PreprocessSuffix): i_direct3d_resource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_resource9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_resource9.cpp"

$(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix): i_direct3d_surface9.cpp $(IntermediateDirectory)/i_direct3d_surface9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_surface9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_surface9$(DependSuffix): i_direct3d_surface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_surface9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_surface9.cpp"

$(IntermediateDirectory)/i_direct3d_surface9$(PreprocessSuffix): i_direct3d_surface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_surface9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_surface9.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/i_unknown$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/home/cschaefer/Documents/SchaeferGL/.build-debug/libschaefergl"


