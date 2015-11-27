{
  'variables': {
    'extension_build_type%': '<(extension_build_type)',
    'extension_build_type%': 'Debug',
  },
  'target_defaults': {
    'conditions': [
      ['extension_build_type == "Release"', {
        'defines': ['NDEBUG', ],
        'cflags': [
          '-O2',
          # Don't emit the GCC version ident directives, they just end up
          # in the .comment section taking up binary size.
          '-fno-ident',
          # Put data and code in their own sections, so that unused symbols
          # can be removed at link time with --gc-sections.
          '-fdata-sections',
          '-ffunction-sections',
        ],
      }],
    ],
    'includes': [
      'xwalk_js2c.gypi',
      'pkg-config.gypi',
    ],
    'cflags': [
      '-std=c++0x',
      '-fPIC',
      '-fvisibility=hidden',
    ],
  },
}
