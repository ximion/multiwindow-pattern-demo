project('Multiwindow Pattern Demo', 'cpp',
  meson_version: '>=0.64',
  default_options: [
      'cpp_std=gnu++20',
  ],
  version: '0.1',
)

cpp_compiler = meson.get_compiler('cpp')

# Dependencies
qt = import('qt6')
qt_core_dep = dependency('qt6', modules: ['Core'], version: '>= 6.2')
qt_widgets_dep = dependency('qt6', modules: ['Widgets'])

# General compiler flags
add_project_arguments(
  cpp_compiler.get_supported_arguments([
    '-Werror=empty-body',
    '-Werror=pointer-arith',
    '-Werror=missing-declarations',
    '-Werror=return-type',
    '-Werror=misleading-indentation',
    '-Werror=format-security',
    '-Werror=suggest-override',

    '-Wno-missing-field-initializers',
    '-Wno-error=missing-field-initializers',
    '-Wno-unused-parameter',
    '-Wno-error=unused-parameter',
  ]),
  language: 'cpp'
)

# Sources
mwsdi_hdr = [
    'toolbarwindow.h',
    'snapwindows.h',
]

mwsdi_src = [
    'main.cpp',
    'utils.cpp',
    'snapwindows.cpp',
    'toolbarwindow.cpp',
]

# Preprocessing
mwsdi_moc = qt.preprocess(
    moc_sources: mwsdi_src,
    moc_headers: mwsdi_hdr,
    moc_extra_arguments: ['--no-notes']
)

# Targets
executable('multiwindow-pattern-demo',
    [mwsdi_hdr,
     mwsdi_src,
     mwsdi_moc],
    dependencies: [
        qt_core_dep,
        qt_widgets_dep,
    ]
)
