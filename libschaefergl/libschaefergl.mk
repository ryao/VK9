##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=libschaefergl
ConfigurationName      :=Debug
WorkspacePath          := "/home/chris/Documents/SchaeferGL"
ProjectPath            := "/home/chris/Documents/SchaeferGL/libschaefergl"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=08/23/14
CodeLitePath           :="/home/chris/.codelite"
LinkerName             :=/usr/bin/g++ 
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
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
ObjectsFileList        :="libschaefergl.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch)/home/cschaefer/Documents/mesa/src/gallium/include/ $(IncludeSwitch)/home/cschaefer/Documents/mesa/include/ $(IncludeSwitch)/home/cschaefer/Documents/mesa/src/gallium/auxiliary/ $(IncludeSwitch). $(IncludeSwitch)/home/chris/Documents/mesa/src/gallium/include/ $(IncludeSwitch)/home/chris/Documents/mesa/src/gallium/auxiliary/ $(IncludeSwitch)/home/chris/Documents/mesa/include/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++ 
CC       := /usr/bin/gcc 
CXXFLAGS :=  -g -fPIC $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/i_unknown.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_device9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_resource9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_surface9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_query9.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_texture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3d_volume9.cpp$(ObjectSuffix) $(IntermediateDirectory)/i_direct3_d9.cpp$(ObjectSuffix) $(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix) $(IntermediateDirectory)/galliumd3_d9.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/galliumd3d_device9.cpp$(ObjectSuffix) $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(ObjectSuffix) $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(ObjectSuffix) $(IntermediateDirectory)/GalliumD3DResource9.cpp$(ObjectSuffix) $(IntermediateDirectory)/guid.c$(ObjectSuffix) 



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
	@$(MakeDirCommand) "/home/chris/Documents/SchaeferGL/.build-debug"
	@echo rebuilt > "/home/chris/Documents/SchaeferGL/.build-debug/libschaefergl"

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/i_unknown.cpp$(ObjectSuffix): i_unknown.cpp $(IntermediateDirectory)/i_unknown.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_unknown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_unknown.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_unknown.cpp$(DependSuffix): i_unknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_unknown.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_unknown.cpp$(DependSuffix) -MM "i_unknown.cpp"

$(IntermediateDirectory)/i_unknown.cpp$(PreprocessSuffix): i_unknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_unknown.cpp$(PreprocessSuffix) "i_unknown.cpp"

$(IntermediateDirectory)/i_direct3d_device9.cpp$(ObjectSuffix): i_direct3d_device9.cpp $(IntermediateDirectory)/i_direct3d_device9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_device9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_device9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_device9.cpp$(DependSuffix): i_direct3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_device9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_device9.cpp$(DependSuffix) -MM "i_direct3d_device9.cpp"

$(IntermediateDirectory)/i_direct3d_device9.cpp$(PreprocessSuffix): i_direct3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_device9.cpp$(PreprocessSuffix) "i_direct3d_device9.cpp"

$(IntermediateDirectory)/i_direct3d_resource9.cpp$(ObjectSuffix): i_direct3d_resource9.cpp $(IntermediateDirectory)/i_direct3d_resource9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_resource9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_resource9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_resource9.cpp$(DependSuffix): i_direct3d_resource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_resource9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_resource9.cpp$(DependSuffix) -MM "i_direct3d_resource9.cpp"

$(IntermediateDirectory)/i_direct3d_resource9.cpp$(PreprocessSuffix): i_direct3d_resource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_resource9.cpp$(PreprocessSuffix) "i_direct3d_resource9.cpp"

$(IntermediateDirectory)/i_direct3d_surface9.cpp$(ObjectSuffix): i_direct3d_surface9.cpp $(IntermediateDirectory)/i_direct3d_surface9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_surface9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_surface9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_surface9.cpp$(DependSuffix): i_direct3d_surface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_surface9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_surface9.cpp$(DependSuffix) -MM "i_direct3d_surface9.cpp"

$(IntermediateDirectory)/i_direct3d_surface9.cpp$(PreprocessSuffix): i_direct3d_surface9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_surface9.cpp$(PreprocessSuffix) "i_direct3d_surface9.cpp"

$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(ObjectSuffix): i_direct3d_swap_chain9.cpp $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_swap_chain9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(DependSuffix): i_direct3d_swap_chain9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(DependSuffix) -MM "i_direct3d_swap_chain9.cpp"

$(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(PreprocessSuffix): i_direct3d_swap_chain9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(PreprocessSuffix) "i_direct3d_swap_chain9.cpp"

$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(ObjectSuffix): i_direct3d_base_texture9.cpp $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_base_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(DependSuffix): i_direct3d_base_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(DependSuffix) -MM "i_direct3d_base_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(PreprocessSuffix): i_direct3d_base_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(PreprocessSuffix) "i_direct3d_base_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(ObjectSuffix): i_direct3d_cube_texture9.cpp $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_cube_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(DependSuffix): i_direct3d_cube_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(DependSuffix) -MM "i_direct3d_cube_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(PreprocessSuffix): i_direct3d_cube_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(PreprocessSuffix) "i_direct3d_cube_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(ObjectSuffix): i_direct3d_index_buffer9.cpp $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_index_buffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(DependSuffix): i_direct3d_index_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(DependSuffix) -MM "i_direct3d_index_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(PreprocessSuffix): i_direct3d_index_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(PreprocessSuffix) "i_direct3d_index_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(ObjectSuffix): i_direct3d_pixel_shader9.cpp $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_pixel_shader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(DependSuffix): i_direct3d_pixel_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(DependSuffix) -MM "i_direct3d_pixel_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(PreprocessSuffix): i_direct3d_pixel_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(PreprocessSuffix) "i_direct3d_pixel_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_query9.cpp$(ObjectSuffix): i_direct3d_query9.cpp $(IntermediateDirectory)/i_direct3d_query9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_query9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_query9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_query9.cpp$(DependSuffix): i_direct3d_query9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_query9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_query9.cpp$(DependSuffix) -MM "i_direct3d_query9.cpp"

$(IntermediateDirectory)/i_direct3d_query9.cpp$(PreprocessSuffix): i_direct3d_query9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_query9.cpp$(PreprocessSuffix) "i_direct3d_query9.cpp"

$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(ObjectSuffix): i_direct3d_state_block9.cpp $(IntermediateDirectory)/i_direct3d_state_block9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_state_block9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(DependSuffix): i_direct3d_state_block9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(DependSuffix) -MM "i_direct3d_state_block9.cpp"

$(IntermediateDirectory)/i_direct3d_state_block9.cpp$(PreprocessSuffix): i_direct3d_state_block9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_state_block9.cpp$(PreprocessSuffix) "i_direct3d_state_block9.cpp"

$(IntermediateDirectory)/i_direct3d_texture9.cpp$(ObjectSuffix): i_direct3d_texture9.cpp $(IntermediateDirectory)/i_direct3d_texture9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_texture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_texture9.cpp$(DependSuffix): i_direct3d_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_texture9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_texture9.cpp$(DependSuffix) -MM "i_direct3d_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_texture9.cpp$(PreprocessSuffix): i_direct3d_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_texture9.cpp$(PreprocessSuffix) "i_direct3d_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(ObjectSuffix): i_direct3d_vertex_buffer9.cpp $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_buffer9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(DependSuffix): i_direct3d_vertex_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(DependSuffix) -MM "i_direct3d_vertex_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(PreprocessSuffix): i_direct3d_vertex_buffer9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(PreprocessSuffix) "i_direct3d_vertex_buffer9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(ObjectSuffix): i_direct3d_vertex_declaration9.cpp $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_declaration9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(DependSuffix): i_direct3d_vertex_declaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(DependSuffix) -MM "i_direct3d_vertex_declaration9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(PreprocessSuffix): i_direct3d_vertex_declaration9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(PreprocessSuffix) "i_direct3d_vertex_declaration9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(ObjectSuffix): i_direct3d_vertex_shader9.cpp $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_vertex_shader9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(DependSuffix): i_direct3d_vertex_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(DependSuffix) -MM "i_direct3d_vertex_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(PreprocessSuffix): i_direct3d_vertex_shader9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(PreprocessSuffix) "i_direct3d_vertex_shader9.cpp"

$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(ObjectSuffix): i_direct3d_volume_texture9.cpp $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_volume_texture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(DependSuffix): i_direct3d_volume_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(DependSuffix) -MM "i_direct3d_volume_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(PreprocessSuffix): i_direct3d_volume_texture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(PreprocessSuffix) "i_direct3d_volume_texture9.cpp"

$(IntermediateDirectory)/i_direct3d_volume9.cpp$(ObjectSuffix): i_direct3d_volume9.cpp $(IntermediateDirectory)/i_direct3d_volume9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3d_volume9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3d_volume9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3d_volume9.cpp$(DependSuffix): i_direct3d_volume9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3d_volume9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3d_volume9.cpp$(DependSuffix) -MM "i_direct3d_volume9.cpp"

$(IntermediateDirectory)/i_direct3d_volume9.cpp$(PreprocessSuffix): i_direct3d_volume9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3d_volume9.cpp$(PreprocessSuffix) "i_direct3d_volume9.cpp"

$(IntermediateDirectory)/i_direct3_d9.cpp$(ObjectSuffix): i_direct3_d9.cpp $(IntermediateDirectory)/i_direct3_d9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/i_direct3_d9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/i_direct3_d9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/i_direct3_d9.cpp$(DependSuffix): i_direct3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/i_direct3_d9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/i_direct3_d9.cpp$(DependSuffix) -MM "i_direct3_d9.cpp"

$(IntermediateDirectory)/i_direct3_d9.cpp$(PreprocessSuffix): i_direct3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/i_direct3_d9.cpp$(PreprocessSuffix) "i_direct3_d9.cpp"

$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix): D3D9.cpp $(IntermediateDirectory)/D3D9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/D3D9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/D3D9.cpp$(DependSuffix): D3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/D3D9.cpp$(DependSuffix) -MM "D3D9.cpp"

$(IntermediateDirectory)/D3D9.cpp$(PreprocessSuffix): D3D9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/D3D9.cpp$(PreprocessSuffix) "D3D9.cpp"

$(IntermediateDirectory)/galliumd3_d9.cpp$(ObjectSuffix): galliumd3_d9.cpp $(IntermediateDirectory)/galliumd3_d9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/galliumd3_d9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/galliumd3_d9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/galliumd3_d9.cpp$(DependSuffix): galliumd3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/galliumd3_d9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/galliumd3_d9.cpp$(DependSuffix) -MM "galliumd3_d9.cpp"

$(IntermediateDirectory)/galliumd3_d9.cpp$(PreprocessSuffix): galliumd3_d9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/galliumd3_d9.cpp$(PreprocessSuffix) "galliumd3_d9.cpp"

$(IntermediateDirectory)/galliumd3d_device9.cpp$(ObjectSuffix): galliumd3d_device9.cpp $(IntermediateDirectory)/galliumd3d_device9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/galliumd3d_device9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/galliumd3d_device9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/galliumd3d_device9.cpp$(DependSuffix): galliumd3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/galliumd3d_device9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/galliumd3d_device9.cpp$(DependSuffix) -MM "galliumd3d_device9.cpp"

$(IntermediateDirectory)/galliumd3d_device9.cpp$(PreprocessSuffix): galliumd3d_device9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/galliumd3d_device9.cpp$(PreprocessSuffix) "galliumd3d_device9.cpp"

$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(ObjectSuffix): GalliumD3DBaseTexture9.cpp $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/GalliumD3DBaseTexture9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(DependSuffix): GalliumD3DBaseTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(DependSuffix) -MM "GalliumD3DBaseTexture9.cpp"

$(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(PreprocessSuffix): GalliumD3DBaseTexture9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(PreprocessSuffix) "GalliumD3DBaseTexture9.cpp"

$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(ObjectSuffix): GalliumD3DUnknown.cpp $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/GalliumD3DUnknown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(DependSuffix): GalliumD3DUnknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(DependSuffix) -MM "GalliumD3DUnknown.cpp"

$(IntermediateDirectory)/GalliumD3DUnknown.cpp$(PreprocessSuffix): GalliumD3DUnknown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(PreprocessSuffix) "GalliumD3DUnknown.cpp"

$(IntermediateDirectory)/GalliumD3DResource9.cpp$(ObjectSuffix): GalliumD3DResource9.cpp $(IntermediateDirectory)/GalliumD3DResource9.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/GalliumD3DResource9.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GalliumD3DResource9.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GalliumD3DResource9.cpp$(DependSuffix): GalliumD3DResource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GalliumD3DResource9.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GalliumD3DResource9.cpp$(DependSuffix) -MM "GalliumD3DResource9.cpp"

$(IntermediateDirectory)/GalliumD3DResource9.cpp$(PreprocessSuffix): GalliumD3DResource9.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GalliumD3DResource9.cpp$(PreprocessSuffix) "GalliumD3DResource9.cpp"

$(IntermediateDirectory)/guid.c$(ObjectSuffix): guid.c $(IntermediateDirectory)/guid.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/chris/Documents/SchaeferGL/libschaefergl/guid.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/guid.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/guid.c$(DependSuffix): guid.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/guid.c$(ObjectSuffix) -MF$(IntermediateDirectory)/guid.c$(DependSuffix) -MM "guid.c"

$(IntermediateDirectory)/guid.c$(PreprocessSuffix): guid.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/guid.c$(PreprocessSuffix) "guid.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/i_unknown.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_unknown.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_device9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_resource9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_surface9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_swap_chain9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_base_texture9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_cube_texture9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_index_buffer9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_pixel_shader9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_query9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_state_block9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_texture9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_buffer9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_declaration9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_vertex_shader9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume_texture9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3d_volume9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/i_direct3_d9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/D3D9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/D3D9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/D3D9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3_d9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3_d9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3_d9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3d_device9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3d_device9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/galliumd3d_device9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DBaseTexture9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DUnknown.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DResource9.cpp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DResource9.cpp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/GalliumD3DResource9.cpp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/guid.c$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/guid.c$(DependSuffix)
	$(RM) $(IntermediateDirectory)/guid.c$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/libschaefergl"


