#include <cstdlib>

#include "camera.hpp"


CCamera::CCamera()
{
    ViewMatrix = NULL;
    ViewMatrixInverse = NULL;

    X = vec3(1.0f, 0.0f, 0.0f);
    Y = vec3(0.0f, 1.0f, 0.0f);
    Z = vec3(0.0f, 0.0f, 1.0f);

    Position = vec3(0.0f, 0.0f, 5.0f);
    Reference = vec3(0.0f, 0.0f, 0.0f);
}

CCamera::~CCamera()
{
}

void CCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
    this->Position = Position;
    this->Reference = Reference;

    Z = normalize(Position - Reference);
    X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
    Y = cross(Z, X);

    if (!RotateAroundReference)
    {
        this->Reference = this->Position;
        this->Position += Z * 0.05f;
    }

    CalculateViewMatrix();
}

void CCamera::Move(const vec3 &Movement)
{
    Position += Movement;
    Reference += Movement;

    CalculateViewMatrix();
}

vec3 CCamera::OnKeys(BYTE Keys, float FrameTime)
{
    float Speed = 5.0f;

    if (Keys & 0x40) Speed *= 2.0f;
    if (Keys & 0x80) Speed *= 0.5f;

    float Distance = Speed * FrameTime;

    vec3 Up(0.0f, 1.0f, 0.0f);
    vec3 Right = X;
    vec3 Forward = cross(Up, Right);

    Up *= Distance;
    Right *= Distance;
    Forward *= Distance;

    vec3 Movement;

    if (Keys & 0x01) Movement += Forward;
    if (Keys & 0x02) Movement -= Forward;
    if (Keys & 0x04) Movement -= Right;
    if (Keys & 0x08) Movement += Right;
    if (Keys & 0x10) Movement += Up;
    if (Keys & 0x20) Movement -= Up;

    return Movement;
}

void CCamera::OnMouseMove(int dx, int dy)
{
    float Sensitivity = 0.25f;

    Position -= Reference;

    if (dx != 0)
    {
        float DeltaX = (float)dx * Sensitivity;

        X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
        Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
        Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
    }

    if (dy != 0)
    {
        float DeltaY = (float)dy * Sensitivity;

        Y = rotate(Y, DeltaY, X);
        Z = rotate(Z, DeltaY, X);

        if (Y.y < 0.0f)
        {
            Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
            Y = cross(Z, X);
        }
    }

    Position = Reference + Z * length(Position);

    CalculateViewMatrix();
}

void CCamera::OnMouseWheel(float zDelta)
{
    Position -= Reference;

    if (zDelta < 0 && length(Position) < 500.0f)
    {
        Position += Position * 0.1f;
    }

    if (zDelta > 0 && length(Position) > 0.05f)
    {
        Position -= Position * 0.1f;
    }

    Position += Reference;

    CalculateViewMatrix();
}

void CCamera::SetViewMatrixPointer(float *ViewMatrix, float *ViewMatrixInverse)
{
    this->ViewMatrix = (mat4x4*)ViewMatrix;
    this->ViewMatrixInverse = (mat4x4*)ViewMatrixInverse;

    CalculateViewMatrix();
}

void CCamera::CalculateViewMatrix()
{
    if (ViewMatrix != NULL)
    {
        *ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);

        if (ViewMatrixInverse != NULL)
        {
            *ViewMatrixInverse = inverse(*ViewMatrix);
        }
    }
}
