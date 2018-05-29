#!/bin/bash

brew update;
brew install eigen libtar fftw libsquish zlib ffmpeg freetype bullet \
ode wxmac opencv assimp libvorbis openssl@1.0 || true;

rm /usr/local/bin/virtualenv;
${PYTHON_INTERP:-python3} -m pip install virtualenv;

if [[ "$CXX" == "g++" ]]; then
  brew link --overwrite gcc;
fi
