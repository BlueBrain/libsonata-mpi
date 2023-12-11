import os
import subprocess
import sys

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

import mpi4py


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    user_options = build_ext.user_options + [
        ('target=', None, "specify the CMake target to build")
    ]

    def initialize_options(self):
        self.target = "sonata_mpi_python"
        super(CMakeBuild, self).initialize_options()

    def run(self):
        try:
            subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        build_type = os.environ.get("SONATA_BUILD_TYPE", "Release")
        if self.debug:
            build_type = "Debug"

        cmake_args = [
            "-DCMAKE_BUILD_TYPE={}".format(build_type),
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            "-DSONATA_MPI_TESTS=OFF",
            "-DSONATA_MPI_FROM_SUBMODULES=ON",
            "-DSONATA_MPI_PYTHON=ON",
            "-DSONATA_MPI_VERSION=" + self.distribution.get_version(),
            "-DSONATA_MPI4PY_INCLUDE_DIR=" + mpi4py.get_include(),
        ]

        build_args = ["--config", build_type,
                      "--target", self.target,
                      "--parallel",
                      ]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp
        )

        subprocess.check_call(
            ["cmake", "--build", "."] + build_args, cwd=self.build_temp
        )


install_requires = [
    "mpi4py",
    # "libsonata>=0.1.25",
]

setup_requires = [
    "setuptools_scm",
]

with open('README.rst') as f:
    README = f.read()

setup(
    name="libsonata_mpi",
    description='MPI-IO backend for libsonata',
    author="Blue Brain Project, EPFL",
    long_description=README,
    long_description_content_type='text/x-rst',
    license="LGPLv3",
    url='https://github.com/BlueBrain/libsonata_mpi',
    classifiers=[
        "License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)",
    ],
    ext_modules=[CMakeExtension("libsonata_mpi._libsonata_mpi")],
    cmdclass={'build_ext': CMakeBuild,
              },
    zip_safe=False,
    setup_requires=setup_requires,
    install_requires=install_requires,
    python_requires=">=3.8",
    use_scm_version={"local_scheme": "no-local-version",
                     },
    package_dir={"": "python"},
    packages=['libsonata_mpi',
              ],
)
