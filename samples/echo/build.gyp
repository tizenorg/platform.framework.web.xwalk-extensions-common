{
  'targets': [
    {
      'target_name': 'echo',
      'type': 'shared_library',
      'sources': [
        'echo_api.js',
        'echo_extension.h',
        'echo_extension.cc',
      ],
      'variables': {
          'packages': [
            'xwalk-extensions-common',
          ],
        },
    },
  ],
}
