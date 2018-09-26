# Based on DXVK build system - https://github.com/doitsujin/dxvk/blob/master/meson.build
project('vk9', ['c', 'cpp'])

vk9_compiler = meson.get_compiler('cpp')
if vk9_compiler.get_id() == 'msvc'
  add_global_arguments('-DNOMINMAX', language : 'cpp')
  vk9_cpp_std='c++latest'
else
  vk9_cpp_std='c++17'
endif

add_global_arguments( ['-DUNICODE', '-D_UNICODE'], language : 'cpp')

boost_dep = dependency('boost', modules : ['system', 'filesystem', 'thread', 'program_options', 'log', 'log_setup'])
vulkan_dep= dependency('vulkan-1')
eigen_dep = dependency('eigen')

vulkan_defs = ['-DVK_USE_PLATFORM_WIN32_KHR', '-DVK_PROTOTYPES', '-DVULKAN_HPP_NO_EXCEPTIONS',
	'-DVULKAN_HPP_NO_SMART_HANDLE', '-DVK_KHR_push_descriptor', '-DVK_EXT_debug_report']

glsl_compiler = find_program('glslc')
glsl_generator = generator(glsl_compiler,
  output    : [ '@PLAINNAME@.h' ],
  arguments : [ '-mfmt=c', '@INPUT@', '-o', '@OUTPUT@' ])

ms_d3d9_dep = declare_dependency(link_args : ['-ld3d9'])
ms_d3dx9_dep = declare_dependency(link_args : ['-ld3dx9'])

subdir('VK9-Library')

enable_tests = get_option('enable_tests')

if enable_tests
  subdir('VK9-Tests')
endif

if vk9_compiler.get_id() != 'msvc'
  subdir('wine_utils')
endif
