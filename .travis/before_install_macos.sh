#!/bin/bash

brew install python@3 eigen libtar fftw libsquish zlib ffmpeg freetype bullet \
ode wxmac opencv assimp libvorbis openssl@1.0 || true

export MODULE_PREFIX="${PYTHON_INTERP:-python3} -m"
$MODULE_PREFIX pip install virtualenv