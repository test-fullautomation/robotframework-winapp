#!/usr/bin/env python

from os.path import abspath, dirname, join
from setuptools import setup, find_packages
import os
import platform
from distutils.core import setup
from distutils.command.install import install as _install


class _post_install(_install):
   def run(self):
      _install.run(self)
      if platform.system().lower().startswith('linux'):
         pass
         # from subprocess import call, Popen
         # try:
            # call(['sudo', 'dpkg', '-r', 'DLTConnector'], cwd=dirname(os.path.realpath(__file__)))
            # call(['sudo', 'unalias', 'DLTConnector'], cwd=dirname(os.path.realpath(__file__)))
         # except Exception:
            # pass
         # call(['sudo', 'dpkg', '-i', 'QConnectionDLTLibrary/tools/DLTConnector/linux/DLTConnector_v1.3.6.deb'], cwd=dirname(os.path.realpath(__file__)))
         # #call(['source','~/.bashrc'], cwd=dirname(os.path.realpath(__file__)))
         # Popen(['bash', '-c', 'source ~/.bashrc'])



ROOT = dirname(abspath(__file__))

version_file = join(ROOT, 'QConnectionDLTLibrary', 'version.py')
exec(compile(open(version_file).read(), version_file, 'exec'))

setup(
      name='robotframework-qconnect-winapp',
      version=VERSION,
      description='An robotframework-qconnect-baselibrary extension for DLT connection.',
      long_description=open(join(ROOT, 'README.md')).read(),
      author='Nguyen Huynh Tri Cuong, Thomas Pollerspoeck',
      author_email='cuong.nguyenhuynhtri@vn.bosch.com, thomas.pollerspoeck@de.bosch.com',
      url='https://sourcecode.socialcoding.bosch.com/projects/ROBFW/repos/robotframework-qconnect-dlt/browse',
      # license='Apache License 2.0',
      keywords='robotframework testing connection dlt',
      platforms='any',
      classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
      ],
      setup_requires=[
      ],
      install_requires=[
            'pywin32 >= 300; platform_system=="Windows"',
            'robotframework-qconnect-baselibrary >= 1.0.0'

      ],
      cmdclass={'install': _post_install},
      tests_require=[
      ],
      packages=find_packages(exclude=["demo", "docs", "tests", ]),
      include_package_data=True,
      package_data={'robotframework-qconnect-dltlibrary': ['QConnectionDLTLibrary/tools/*']}
     )
