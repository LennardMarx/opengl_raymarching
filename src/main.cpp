#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengles2.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "events.h"
// #include <events.h>

// Texture
// const char *cTextureFilename = "src/media/texmap.png";
// GLuint textureObj = 0;

// Vertex shader
GLint shaderPan, shaderZoom, shaderAspect;
// Time
GLint shaderTime;
// Camera
GLint shaderCamPos, shaderMouseDelta;

// ===================
bool gQuit = false;

float u_xPos = 0.0f;
float u_yPos = 0.0f;
float u_zPos = 0.0f;
glm::vec3 camPos;
// glm::vec2 mouseDelta;

static int mouseX = 0;
static int mouseY = 0;

// Function to read shader file
std::string readShaderFile(const std::string &filePath) {
  std::ifstream shaderFile(filePath);
  if (!shaderFile.is_open()) {
    std::cerr << "Failed to open shader file: " << filePath << std::endl;
    return "";
  }

  std::stringstream shaderStream;
  shaderStream << shaderFile.rdbuf();
  return shaderStream.str();
}

auto start_time = std::chrono::high_resolution_clock::now();

void passTime() {
  auto current_time = std::chrono::high_resolution_clock::now();

  GLuint milliseconds_since_start =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time -
                                                            start_time)
          .count();

  glUniform1f(shaderTime, milliseconds_since_start);
  if (shaderTime == -1) {
    std::cerr << "Could not find time_mil in memory." << std::endl;
  }
}

void updateShader(EventHandler &eventHandler) {
  Camera &camera = eventHandler.camera();

  // glUniform2fv(shaderPan, 1, camera.pan());
  // glUniform1f(shaderZoom, camera.zoom());
  glUniform1f(shaderAspect, camera.aspect());
  // glUniform1fv(shaderCamPos, 3, &camera.GetCamPos()[0]);
  glUniform1fv(shaderCamPos, 3, &camPos[0]);
  glUniform1fv(shaderMouseDelta, 3, &eventHandler.GetMousePos()[0]);
  // print mouseDelta
  // std::cout << "Mouse: " << eventHandler.GetMousePos().x << ", "
  //           << eventHandler.GetMousePos().y << std::endl;

  // print campos
  // std::cout << "Camera: " << camPos.x << ", " << camPos.y << ", " << camPos.z
  //           << std::endl;
  // passTime();
}

GLuint initShader(EventHandler &eventHandler) {
  // Get shader source code
  std::string vertexSourceStr = readShaderFile("shaders/vert.glsl");
  std::string fragmentSourceStr = readShaderFile("shaders/frag.glsl");
  const GLchar *vertexSource = vertexSourceStr.c_str();
  const GLchar *fragmentSource = fragmentSourceStr.c_str();

  // Create and compile vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  // Create and compile fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  // Link vertex and fragment shader into shader program and use it
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Get shader variables and initalize them
  // shaderPan = glGetUniformLocation(shaderProgram, "pan");
  // shaderZoom = glGetUniformLocation(shaderProgram, "zoom");
  shaderAspect = glGetUniformLocation(shaderProgram, "aspect");
  shaderTime = glGetUniformLocation(shaderProgram, "time_mil");
  shaderCamPos = glGetUniformLocation(shaderProgram, "camPos");
  shaderMouseDelta = glGetUniformLocation(shaderProgram, "mouseDelta");
  updateShader(eventHandler);

  return shaderProgram;
}

void Input(EventHandler &eventHandler) {
  // static int mouseX = gScreenWidth / 2;
  // static int mouseY = gScreenHeight / 2;
  //
  // SDL_Event e;
  // while (SDL_PollEvent(&e) != 0) {
  //   std::cout << "Event polling: " << std::endl;
  //   if (e.type == SDL_QUIT) {
  //     std::cout << "SDL Quit" << std::endl;
  //     gQuit = true;
  //   } else if (e.type == SDL_MOUSEMOTION) {
  //     mouseX += e.motion.xrel;
  //     mouseY += e.motion.yrel;
  //
  //     std::cout << "Mouse pos: " << glm::radians((float)mouseX) << ", "
  //               << glm::radians((float)mouseY) << std::endl;
  //     // gCamera.MouseLook(mouseX, mouseY);
  //   }
  // }
  // std::cout << "beep boop: " << std::endl;

  float speed = 0.15f;
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_W]) {
    // gCamera.MoveForward(speed);
    // u_zPos += speed;
    // u_zPos += cosf(-glm::radians((float)mouseX)) * speed;
    // u_xPos -= sinf(-glm::radians((float)mouseX)) * speed;
    u_zPos += cosf(-eventHandler.GetMousePos().x) * speed;
    u_xPos -= sinf(-eventHandler.GetMousePos().x) * speed;
  }
  if (state[SDL_SCANCODE_S]) {
    // gCamera.MoveBackward(speed);
    // u_zPos -= speed;
    // u_zPos -= cosf(-glm::radians((float)mouseX)) * speed;
    // u_xPos += sinf(-glm::radians((float)mouseX)) * speed;
    u_zPos -= cosf(-eventHandler.GetMousePos().x) * speed;
    u_xPos += sinf(-eventHandler.GetMousePos().x) * speed;
  }
  if (state[SDL_SCANCODE_D]) {
    std::cout << "D" << std::endl;
    // gCamera.MoveRight(speed);
    // u_xPos += speed;
    // u_zPos -= sinf(-glm::radians((float)mouseX)) * speed;
    // u_xPos -= cosf(-glm::radians((float)mouseX)) * speed;
    u_zPos -= sinf(-eventHandler.GetMousePos().x) * speed;
    u_xPos -= cosf(-eventHandler.GetMousePos().x) * speed;
  }
  if (state[SDL_SCANCODE_A]) {
    // gCamera.MoveLeft(speed);
    // u_xPos -= speed;
    // u_zPos += sinf(-glm::radians((float)mouseX)) * speed;
    // u_xPos += cosf(-glm::radians((float)mouseX)) * speed;
    u_zPos += sinf(-eventHandler.GetMousePos().x) * speed;
    u_xPos += cosf(-eventHandler.GetMousePos().x) * speed;
  }
  if (state[SDL_SCANCODE_SPACE]) {
    // gCamera.MoveUp(speed);
    u_yPos += speed;
  }
  if (state[SDL_SCANCODE_LSHIFT]) {
    // gCamera.MoveDown(speed);
    u_yPos -= speed;
  }
  if (state[SDL_SCANCODE_ESCAPE]) {
    // gCamera.MoveDown(speed);
    gQuit = true;
  }
}

void initGeometry(GLuint shaderProgram) {
  // Create vertex buffer object and copy vertex data into it
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLfloat triangleVertices[] = {
      // First triangle
      -1.0f, -1.0f, 0.0f, // Bottom-left
      1.0f, -1.0f, 0.0f,  // Bottom-right
      -1.0f, 1.0f, 0.0f,  // Top-left

      // Second triangle
      -1.0f, 1.0f, 0.0f, // Top-left
      1.0f, -1.0f, 0.0f, // Bottom-right
      1.0f, 1.0f, 0.0f   // Top-right
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices,
               GL_STATIC_DRAW);

  // Specify the layout of the shader vertex data (positions only, 3 floats)
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

// void initTexture() {
//   SDL_Surface *image = IMG_Load(cTextureFilename);
//
//   if (!image) {
//     // Create a fallback gray texture
//     printf("Failed to load %s, due to %s\n", cTextureFilename,
//     IMG_GetError()); const int w = 128, h = 128, bitsPerPixel = 24; image =
//     SDL_CreateRGBSurface(0, w, h, bitsPerPixel, 0, 0, 0, 0); if (image)
//       memset(image->pixels, 0x42, image->w * image->h * bitsPerPixel / 8);
//   }
//
//   if (image) {
//     int bitsPerPixel = image->format->BitsPerPixel;
//     printf("Image dimensions %dx%d, %d bits per pixel\n", image->w, image->h,
//            bitsPerPixel);
//
//     // Determine GL texture format
//     GLint format = -1;
//     if (bitsPerPixel == 24)
//       format = GL_RGB;
//     else if (bitsPerPixel == 32)
//       format = GL_RGBA;
//
//     if (format != -1) {
//       // Generate a GL texture object
//       glGenTextures(1, &textureObj);
//
//       // Bind GL texture
//       glBindTexture(GL_TEXTURE_2D, textureObj);
//
//       // Set the GL texture's wrapping and stretching properties
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//       // Copy SDL surface image to GL texture
//       glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format,
//                    GL_UNSIGNED_BYTE, image->pixels);
//     }
//
//     SDL_FreeSurface(image);
//   }
// }

void redraw(EventHandler &eventHandler) {
  // Clear screen
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertex buffer
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Swap front/back framebuffers
  eventHandler.swapWindow();
}

void mainLoop(void *mainLoopArg) {
  EventHandler &eventHandler = *((EventHandler *)mainLoopArg);
  eventHandler.processEvents();

  Input(eventHandler);

  // Update shader if camera changed
  // if (eventHandler.camera().updated())
  updateShader(eventHandler);
  camPos = glm::vec3(u_xPos, u_yPos, u_zPos);
  // mouseDelta =
  //     glm::vec2(glm::radians((float)mouseX), glm::radians((float)mouseY));
  passTime();

  redraw(eventHandler);
}

int main(int argc, char **argv) {
  EventHandler eventHandler("OpenGL Raymarching");

  // Initialize shader, geometry, and texture
  GLuint shaderProgram = initShader(eventHandler);
  initGeometry(shaderProgram);
  // initTexture();

  // Start the main loop
  void *mainLoopArg = &eventHandler;

#ifdef __EMSCRIPTEN__
  int fps = 0; // Use browser's requestAnimationFrame
  emscripten_set_main_loop_arg(mainLoop, mainLoopArg, fps, true);
#else
  while (true)
    mainLoop(mainLoopArg);
#endif

  // glDeleteTextures(1, &textureObj);

  return 0;
}
