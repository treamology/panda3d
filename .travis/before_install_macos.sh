#!/bin/bash

brew install eigen libtar fftw libsquish zlib ffmpeg freetype bullet \
ode wxmac opencv assimp libvorbis openssl@1.0 || true;
${PYTHON_INTERP:-python3} -m pip install virtualenv;
