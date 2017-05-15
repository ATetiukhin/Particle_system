#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

typedef unsigned char BYTE;

#include "mathematics.hpp"

class CCamera
{
public:
    vec3 X;
    vec3 Y;
    vec3 Z;
    vec3 Position;
    vec3 Reference;

    CCamera();
    ~CCamera() = default;

    void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
    void Move(const vec3 &Movement);
    vec3 OnKeys(BYTE Keys, float FrameTime);
    void OnMouseMove(int dx, int dy);
    void OnMouseWheel(float zDelta);
    void SetViewMatrixPointer(float * ViewMatrix, float * ViewMatrixInverse = nullptr);

protected:
    mat4x4 * ViewMatrix;
    mat4x4 * ViewMatrixInverse;

private:
    void CalculateViewMatrix();
};

#endif /* End of 'camera.hpp' file */