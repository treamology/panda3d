#!/bin/bash

brew install python@3 eigen libtar fftw libsquish zlib ffmpeg freetype bullet \
ode wxmac opencv assimp libvorbis openssl@1.0 || true

# We can't trust brew to make the right symlinks, so execute commands as modules
export MODULE_PREFIX="${PYTHON_INTERP:-python3} -m"
$MODULE_PREFIX pip install virtualenv

# virtualenv can't find the interpreter, so help it out
if [[ "$PYTHON_INTERP" == "python2.7" ]]; then
  export PYTHON_INTERP="/usr/bin/python2.7";
fi
