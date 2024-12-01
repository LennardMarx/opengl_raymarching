set -o verbose
emcc -std=c++11 src/main.cpp src/events.cpp src/camera.cpp -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s FULL_ES2=1 -s WASM=1 --preload-file shaders -I ./include/glm/ -o ./opengl_raymarching.js
