#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include <windows.h>

#include "mathematics.hpp"

class CCamera
{
public:
    vec3 X, Y, Z, Position, Reference;

    CCamera();
    ~CCamera();

    void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
    void Move(const vec3 &Movement);
    vec3 OnKeys(BYTE Keys, float FrameTime);
    void OnMouseMove(int dx, int dy);
    void OnMouseWheel(float zDelta);
    void SetViewMatrixPointer(float *ViewMatrix, float *ViewMatrixInverse = NULL);

protected:
    mat4x4 *ViewMatrix, *ViewMatrixInverse;

private:
    void CalculateViewMatrix();
};


#endif /* End of 'camera.hpp' file */