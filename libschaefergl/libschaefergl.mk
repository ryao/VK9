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
Objects=$(IntermediateDirectory)/i_unknown$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_device9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_resource9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_surface9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_swap_chain9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_base_texture9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_cube_texture9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_index_buffer9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_pixel_shader9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_query9$(ObjectSuffix) \
	$(IntermediateDirectory)/i_direct3d_state_block9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_texture9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_shader9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_volume_texture9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_volume9$(ObjectSuffix) $(IntermediateDirectory)/i_direct3_d9$(ObjectSuffix) $(IntermediateDirectory)/D3D9$(ObjectSuffix) 

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

$(IntermediateDirectory)/i_direct3d_swap_chain9$(ObjectSuffix): i_direct3d_swap_chain9.cpp $(IntermediateDirectory)/i_direct3d_swap_chain9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_swap_chain9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_swap_chain9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_swap_chain9$(DependSuffix): i_direct3d_swap_chain9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_swap_chain9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_swap_chain9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_swap_chain9.cpp"

$(IntermediateDirectory)/i_direct3d_swap_chain9$(PreprocessSuffix): i_direct3d_swap_chain9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_swap_chain9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_swap_chain9.cpp"

$(IntermediateDirectory)/i_direct3d_base_texture9$(ObjectSuffix): i_direct3d_base_texture9.cpp $(IntermediateDirectory)/i_direct3d_base_texture9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_base_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_base_texture9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_base_texture9$(DependSuffix): i_direct3d_base_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_base_texture9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_base_texture9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_base_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_base_texture9$(PreprocessSuffix): i_direct3d_base_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_base_texture9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_base_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_cube_texture9$(ObjectSuffix): i_direct3d_cube_texture9.cpp $(IntermediateDirectory)/i_direct3d_cube_texture9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_cube_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_cube_texture9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_cube_texture9$(DependSuffix): i_direct3d_cube_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_cube_texture9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_cube_texture9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_cube_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_cube_texture9$(PreprocessSuffix): i_direct3d_cube_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_cube_texture9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_cube_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_index_buffer9$(ObjectSuffix): i_direct3d_index_buffer9.cpp $(IntermediateDirectory)/i_direct3d_index_buffer9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_index_buffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_index_buffer9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_index_buffer9$(DependSuffix): i_direct3d_index_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_index_buffer9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_index_buffer9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_index_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_index_buffer9$(PreprocessSuffix): i_direct3d_index_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_index_buffer9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_index_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_pixel_shader9$(ObjectSuffix): i_direct3d_pixel_shader9.cpp $(IntermediateDirectory)/i_direct3d_pixel_shader9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_pixel_shader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_pixel_shader9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_pixel_shader9$(DependSuffix): i_direct3d_pixel_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_pixel_shader9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_pixel_shader9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_pixel_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_pixel_shader9$(PreprocessSuffix): i_direct3d_pixel_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_pixel_shader9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_pixel_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_query9$(ObjectSuffix): i_direct3d_query9.cpp $(IntermediateDirectory)/i_direct3d_query9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_query9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_query9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_query9$(DependSuffix): i_direct3d_query9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_query9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_query9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_query9.cpp"

$(IntermediateDirectory)/i_direct3d_query9$(PreprocessSuffix): i_direct3d_query9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_query9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_query9.cpp"

$(IntermediateDirectory)/i_direct3d_state_block9$(ObjectSuffix): i_direct3d_state_block9.cpp $(IntermediateDirectory)/i_direct3d_state_block9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_state_block9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_state_block9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_state_block9$(DependSuffix): i_direct3d_state_block9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_state_block9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_state_block9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_state_block9.cpp"

$(IntermediateDirectory)/i_direct3d_state_block9$(PreprocessSuffix): i_direct3d_state_block9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_state_block9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_state_block9.cpp"

$(IntermediateDirectory)/i_direct3d_texture9$(ObjectSuffix): i_direct3d_texture9.cpp $(IntermediateDirectory)/i_direct3d_texture9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_texture9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_texture9$(DependSuffix): i_direct3d_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_texture9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_texture9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_texture9$(PreprocessSuffix): i_direct3d_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_texture9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(ObjectSuffix): i_direct3d_vertex_buffer9.cpp $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_buffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(DependSuffix): i_direct3d_vertex_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_buffer9$(PreprocessSuffix): i_direct3d_vertex_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(ObjectSuffix): i_direct3d_vertex_declaration9.cpp $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_declaration9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(DependSuffix): i_direct3d_vertex_declaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_declaration9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_declaration9$(PreprocessSuffix): i_direct3d_vertex_declaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_declaration9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_shader9$(ObjectSuffix): i_direct3d_vertex_shader9.cpp $(IntermediateDirectory)/i_direct3d_vertex_shader9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_shader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_shader9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_shader9$(DependSuffix): i_direct3d_vertex_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_shader9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_shader9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_shader9$(PreprocessSuffix): i_direct3d_vertex_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_shader9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_volume_texture9$(ObjectSuffix): i_direct3d_volume_texture9.cpp $(IntermediateDirectory)/i_direct3d_volume_texture9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_volume_texture9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_volume_texture9$(DependSuffix): i_direct3d_volume_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_volume_texture9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_volume_texture9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_volume_texture9$(PreprocessSuffix): i_direct3d_volume_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_volume_texture9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_volume9$(ObjectSuffix): i_direct3d_volume9.cpp $(IntermediateDirectory)/i_direct3d_volume9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_volume9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_volume9$(DependSuffix): i_direct3d_volume9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_volume9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_volume9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume9.cpp"

$(IntermediateDirectory)/i_direct3d_volume9$(PreprocessSuffix): i_direct3d_volume9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_volume9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3d_volume9.cpp"

$(IntermediateDirectory)/i_direct3_d9$(ObjectSuffix): i_direct3_d9.cpp $(IntermediateDirectory)/i_direct3_d9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3_d9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3_d9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3_d9$(DependSuffix): i_direct3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3_d9$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3_d9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3_d9.cpp"

$(IntermediateDirectory)/i_direct3_d9$(PreprocessSuffix): i_direct3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3_d9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/i_direct3_d9.cpp"

$(IntermediateDirectory)/D3D9$(ObjectSuffix): D3D9.cpp $(IntermediateDirectory)/D3D9$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/D3D9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/D3D9$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/D3D9$(DependSuffix): D3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/D3D9$(ObjectSuffix) -MF$(IntermediateDirectory)/D3D9$(DependSuffix) -MM "/home/cschaefer/Documents/SchaeferGL/libschaefergl/D3D9.cpp"

$(IntermediateDirectory)/D3D9$(PreprocessSuffix): D3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/D3D9$(PreprocessSuffix) "/home/cschaefer/Documents/SchaeferGL/libschaefergl/D3D9.cpp"


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
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/D3D9$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/D3D9$(DependSuffix)
	$(RM) $(IntermediateDirectory)/D3D9$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/home/cschaefer/Documents/SchaeferGL/.build-debug/libschaefergl"


