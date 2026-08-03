/**
 * @file Camera.cpp
 * @brief Implementation of the Camera class
 */

#include "Datastructs/Camera.h"

Camera::Camera(glm::vec2 _resolution)
: m_Resolution(_resolution)
, m_Position(0.0, 0.0, 3.0)
, m_Rotation(1.0f, 0.0f, 0.0f, 0.0f)
, m_fov(0.5f)
, m_LastPosition(0.0, 0.0, 3.0)
, m_LastForward(0.0f, 0.0f, -1.0f)
, m_LastRight(1.0f, 0.0f, 0.0f)
, m_LastUp(0.0f, 1.0f, 0.0f)
, m_LastFov(0.5f)
{
}

Camera::~Camera()
{
}

void Camera::StoreLastFrameState()
{
    m_LastPosition = m_Position;
    m_LastForward = m_Forward;
    m_LastRight = m_Right;
    m_LastUp = m_Up;
    m_LastFov = m_fov;
}

void Camera::Update(Input _inputMap, float _deltaTime)
{
    Move(_inputMap.forward * m_Forward * 3.0f * _deltaTime);
    Move(_inputMap.right * m_Right * 3.0f * _deltaTime);
    Move(_inputMap.up * m_Up * 3.0f * _deltaTime);

    if(abs(_inputMap.deltaMouseX) > 0.0f && abs(_inputMap.deltaMouseX) < 1.0f)
    {
        Rotate(_inputMap.deltaMouseX * 10.0f * _deltaTime, m_Up);
    }

    if(abs(_inputMap.deltaMouseY) > 0.0f && abs(_inputMap.deltaMouseY) < 1.0f)
    {
        Rotate(_inputMap.deltaMouseY * 10.0f * _deltaTime, m_Right);
    }
}

void Camera::UpdateShader(ComputeShader& _shader)
{
    // Set last frame camera uniforms
    _shader.SetUniform("u_lastFrameCamera.position", m_LastPosition);
    _shader.SetUniform("u_lastFrameCamera.forward", m_LastForward);
    _shader.SetUniform("u_lastFrameCamera.right", m_LastRight);
    _shader.SetUniform("u_lastFrameCamera.up", m_LastUp);
    _shader.SetUniform("u_lastFrameCamera.fov", m_LastFov);

    // Set current camera uniforms
    _shader.SetUniform("u_camera.position", m_Position);
    _shader.SetUniform("u_camera.forward", m_Forward);
    _shader.SetUniform("u_camera.up", m_Up);
    _shader.SetUniform("u_camera.right", m_Right);
    _shader.SetUniform("u_camera.fov", m_fov);
}

void Camera::ExportState(ShaderInfo* _shaderInfo)
{
    ShaderProperty* posProp = _shaderInfo->GetProperty("u_camera.position");

    if(posProp)
    {
        posProp->value.vec3 = m_Position;
        _shaderInfo->Shader()->SetUniform("u_camera.position", m_Position);
    }

    posProp = _shaderInfo->GetProperty("u_camera.forward");

    if(posProp)
    {
        posProp->value.vec3 = m_Forward;
        _shaderInfo->Shader()->SetUniform("u_camera.forward", m_Forward);
    }

    posProp = _shaderInfo->GetProperty("u_camera.right");
    if(posProp)
    {
        posProp->value.vec3 = m_Right;
        _shaderInfo->Shader()->SetUniform("u_camera.right", m_Right);
    }

    posProp = _shaderInfo->GetProperty("u_camera.up");
    if(posProp)
    {
        posProp->value.vec3 = m_Up;
        _shaderInfo->Shader()->SetUniform("u_camera.up", m_Up);
    }

    posProp = _shaderInfo->GetProperty("u_camera.fov");
    if(posProp)
    {
        posProp->value.f = m_fov;
        _shaderInfo->Shader()->SetUniform("u_camera.fov", m_fov);
    }
}

void Camera::ExportLastFrameState(ShaderInfo* _shaderInfo)
{
    ShaderProperty* posProp = _shaderInfo->GetProperty("u_lastFrameCamera.position");

    if(posProp)
    {
        posProp->value.vec3 = m_LastPosition;
        _shaderInfo->Shader()->SetUniform("u_lastFrameCamera.position", m_LastPosition);
    }

    posProp = _shaderInfo->GetProperty("u_lastFrameCamera.forward");
    if(posProp)
    {
        posProp->value.vec3 = m_LastForward;
        _shaderInfo->Shader()->SetUniform("u_lastFrameCamera.forward", m_LastForward);
    }

    posProp = _shaderInfo->GetProperty("u_lastFrameCamera.right");
    if(posProp)
    {
        posProp->value.vec3 = m_LastRight;
        _shaderInfo->Shader()->SetUniform("u_lastFrameCamera.right", m_LastRight);
    }

    posProp = _shaderInfo->GetProperty("u_lastFrameCamera.up");
    if(posProp)
    {
        posProp->value.vec3 = m_LastUp;
        _shaderInfo->Shader()->SetUniform("u_lastFrameCamera.up", m_LastUp);
    }

    posProp = _shaderInfo->GetProperty("u_lastFrameCamera.fov");
    if(posProp)
    {
        posProp->value.f = m_LastFov;
        _shaderInfo->Shader()->SetUniform("u_lastFrameCamera.fov", m_LastFov);
    }
}

void Camera::Rotate(float _angle, glm::vec3 _axis)
{
    glm::quat rotation = glm::angleAxis(_angle, glm::normalize(_axis));

    m_Rotation = rotation * m_Rotation;
    m_Rotation = glm::normalize(m_Rotation);

    m_Forward = glm::normalize(m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
}

void Camera::Move(glm::vec3 _posChange)
{
    m_Position += _posChange;
}

void Camera::Position(glm::vec3 _newPos)
{
    m_Position = _newPos;
}

glm::vec3& Camera::Position()
{
    return m_Position;
}

glm::vec3& Camera::Forward()
{
    return m_Forward;
}

glm::vec3& Camera::Right()
{
    return m_Right;
}

glm::vec3& Camera::Up()
{
    return m_Up;
}

glm::quat Camera::Rotation()
{
    return m_Rotation;
}

void Camera::fov(float _f)
{
    m_fov = _f;
}

float Camera::fov()
{
    return m_fov;
}
