# Based on DXVK build system -https://github.com/doitsujin/dxvk/blob/master/src/d3d11/meson.build
executable('VK9-Tests', files('Main.cpp'),
  dependencies        : [ d3d9_dep, ms_d3d9_dep, ms_d3dx9_dep ],
  cpp_args            : vulkan_defs,
  install             : true,
  override_options    : ['cpp_std='+vk9_cpp_std])

