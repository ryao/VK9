# Based on DXVK build system - https://github.com/doitsujin/dxvk/blob/master/wine_utils/meson.build
conf = configuration_data()
conf.set('dlldir', get_option('prefix')+'/'+get_option('bindir'))
conf.set('arch', target_machine.cpu_family())
configure_file(
  configuration : conf,
  input         : 'setup_vk9.sh.in',
  output        : 'setup_vk9.sh',
  install_dir   : get_option('bindir')
)
