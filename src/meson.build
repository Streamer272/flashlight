flashlight_sources = [
  'main.c',
  'flashlight-window.c',
  'flashlight-application.c',
]

flashlight_deps = [
  dependency('gtk4'),
]

gnome = import('gnome')

flashlight_sources += gnome.compile_resources('flashlight-resources',
  'flashlight.gresource.xml',
  c_name: 'flashlight'
)

executable('flashlight', flashlight_sources,
  dependencies: flashlight_deps,
  install: true,
)
