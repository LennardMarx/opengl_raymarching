# Requires Emscripten to build: https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html
set -o verbose
# emcc -std=c++11 hello_texture.cpp events.cpp camera.cpp -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="[""png""]" -s FULL_ES2=1 -s WASM=1 --preload-file media/texmap.png -o ../hello_texture.js
# emcc -std=c++11 hello_texture.cpp events.cpp camera.cpp -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="[""png""]" -s FULL_ES2=1 -s WASM=1 --preload-file media/texmap.png --preload-file shaders -o ../hello_texture.js
emcc -std=c++11 hello_texture.cpp events.cpp camera.cpp -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s FULL_ES2=1 -s WASM=1 --preload-file media/texmap.png --preload-file shaders -I ../include/glm/ -o ../hello_texture.js
