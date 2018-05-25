shader_src = [
  'VertexBuffer_XYZ_DIFFUSE.frag',
  'VertexBuffer_XYZ_DIFFUSE.geom',
  'VertexBuffer_XYZ_DIFFUSE_TEX1.frag',
  'VertexBuffer_XYZ_DIFFUSE_TEX1.vert',
  'VertexBuffer_XYZ_DIFFUSE_TEX2.frag',
  'VertexBuffer_XYZ_DIFFUSE_TEX2.vert',
  'VertexBuffer_XYZ_DIFFUSE.vert',
  'VertexBuffer_XYZ_NORMAL_DIFFUSE.frag',
  'VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.frag',
  'VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.vert',
  'VertexBuffer_XYZ_NORMAL_DIFFUSE.vert',
  'VertexBuffer_XYZ_NORMAL.frag',
  'VertexBuffer_XYZ_NORMAL_TEX1.frag',
  'VertexBuffer_XYZ_NORMAL_TEX1.vert',
  'VertexBuffer_XYZ_NORMAL_TEX2.frag',
  'VertexBuffer_XYZ_NORMAL_TEX2.vert',
  'VertexBuffer_XYZ_NORMAL.vert',
  'VertexBuffer_XYZ_TEX1.frag',
  'VertexBuffer_XYZ_TEX1.vert',
  'VertexBuffer_XYZ_TEX2.frag',
  'VertexBuffer_XYZ_TEX2.vert'
]

shader_spv = glsl_generator.process(shader_src)

# Dummy target to force shader building
shaders = custom_target('Shaders',
  input : shader_spv,
  output : 'dummy.txt',
  command : ['touch', '@BUILD_DIR@/../dummy.txt'],
  install : true,
  install_dir : 'Shaders')
