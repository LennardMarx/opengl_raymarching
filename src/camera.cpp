//
// Camera - pan, zoom, and window resizing
//
#include <algorithm>
#include <SDL.h>
#include <SDL_opengles2.h>
#include "camera.h"

// Camera::Camera() {
//   mEye = glm::vec3(0.0f, 0.0f, 0.0f);
//   mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
//   mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
// }

// ================================

bool Camera::updated()
{
    bool updated = mCameraUpdated;
    mCameraUpdated = false;
    return updated;
}

bool Camera::windowResized()
{
    bool resized = mWindowResized;
    mWindowResized = false;
    return resized;
}

void Camera::setWindowSize(int width, int height)
{
    if (mWindowSize.width != width || mWindowSize.height != height)
    {
        mWindowResized = true;
        mWindowSize = {width, height};
        mViewport = {(float)width, (float)height};
        setAspect(width / (float)height);
    }
}

// Clamp val between lo and hi
float Camera::clamp (float val, float lo, float hi) 
{ 
    return std::max(lo, std::min(val, hi)); 
}

// Convert from normalized window coords (x,y) in ([0.0, 1.0], [1.0, 0.0]) to device coords ([-1.0, 1.0], [-1.0,1.0])
void Camera::normWindowToDeviceCoords (float normWinX, float normWinY, float& deviceX, float& deviceY)
{
    deviceX = (normWinX - 0.5f) * 2.0f;
    deviceY = (1.0f - normWinY - 0.5f) * 2.0f;
}

// Convert from window coords (x,y) in ([0, mWindowWidth], [mWindowHeight, 0]) to device coords ([-1.0, 1.0], [-1.0,1.0])
void Camera::windowToDeviceCoords (int winX, int winY, float& deviceX, float& deviceY)
{
    normWindowToDeviceCoords(winX / (float)mWindowSize.width,  winY / (float)mWindowSize.height, deviceX, deviceY);
}

// Convert from device coords ([-1.0, 1.0], [-1.0,1.0]) to world coords ([-inf, inf], [-inf, inf])
void Camera::deviceToWorldCoords (float deviceX, float deviceY, float& worldX, float& worldY)
{
    worldX = deviceX / mZoom - mPan.x;
    worldY = deviceY / mAspect / mZoom - mPan.y;
}

// Convert from window coords (x,y) in ([0, windowWidth], [windowHeight, 0]) to world coords ([-inf, inf], [-inf, inf])
void Camera::windowToWorldCoords(int winX, int winY, float& worldX, float& worldY)
{
    float deviceX, deviceY;
    windowToDeviceCoords(winX, winY, deviceX, deviceY);   
    deviceToWorldCoords(deviceX, deviceY, worldX, worldY);
}

// Convert from normalized window coords (x,y) in in ([0.0, 1.0], [1.0, 0.0]) to world coords ([-inf, inf], [-inf, inf])
void Camera::normWindowToWorldCoords(float normWinX, float normWinY, float& worldX, float& worldY)
{
    float deviceX, deviceY;
    normWindowToDeviceCoords(normWinX, normWinY, deviceX, deviceY);
    deviceToWorldCoords(deviceX, deviceY, worldX, worldY);
}


//======================
// Camera movement
glm::mat4 Camera::GetViewMatrix() const {
  // return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
  return glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::GetCamPos() { return mEye; }
glm::vec3 Camera::GetCamRot() { return mViewDirection; }
glm::vec2 Camera::GetMouseDelta() { return mouseDelta; }
// glm::vec2 Camera::GetRotAngle(int mouseX, int mouseY) {
//
//   return glm::vec2(glm::radians(mouseX), mouseY);
// }

void Camera::MouseLook(int mouseX, int mouseY) {
  // std::cout << "Mouse: " << mouseX << ", " << mouseX << std::endl;

  glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

  static bool firstLook = true;
  if (firstLook) {
    mOldMousePosition = currentMouse;
    firstLook = false;
  }

  // glm::vec2 mouseDelta = mOldMousePosition - currentMouse;
  mouseDelta = mOldMousePosition - currentMouse;

  mViewDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.x), mUpVector);

  glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
  mViewDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.y), rightVector);

  mOldMousePosition = currentMouse;
}

void Camera::MoveForward(float speed) { mEye -= (mViewDirection * speed); }
void Camera::MoveBackward(float speed) { mEye += (mViewDirection * speed); }
void Camera::MoveRight(float speed) {
  glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
  mEye += rightVector * speed;
}
void Camera::MoveLeft(float speed) {
  glm::vec3 leftVector = glm::cross(mViewDirection, mUpVector);
  mEye -= leftVector * speed;
}

void Camera::MoveUp(float speed) { mEye.y += speed; }
void Camera::MoveDown(float speed) { mEye.y -= speed; }
