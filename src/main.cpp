#include <Windows.h>

#include "cstring.hpp"
#include "camera.hpp"
#include "opengl_renderer.hpp"
#include "opengl_view.hpp"

CCamera Camera;
COpenGLView OpenGLView;
COpenGLRenderer OpenGLRenderer;
CString ModuleDirectory, ErrorLog;
int gl_max_texture_size = 0, gl_max_texture_max_anisotropy_ext = 0;


LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        OpenGLView.OnKeyDown((UINT)wParam);
        break;

    case WM_LBUTTONDOWN:
        OpenGLView.OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_MOUSEMOVE:
        OpenGLView.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;

    case 0x020A: // WM_MOUSWHEEL
        OpenGLView.OnMouseWheel(HIWORD(wParam));
        break;

    case WM_PAINT:
        OpenGLView.OnPaint();
        break;

    case WM_RBUTTONDOWN:
        OpenGLView.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_SIZE:
        OpenGLView.OnSize(LOWORD(lParam), HIWORD(lParam));
        break;

    default:
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int iShow)
{
    char *AppName = "Water waves caustic GPU algorithm";

    if (OpenGLView.Init(hInstance, AppName, 800, 600, 0))
    {
        OpenGLView.Show();
        OpenGLView.MessageLoop();
    }
    else
    {
        MessageBox(NULL, ErrorLog, AppName, MB_OK | MB_ICONERROR);
    }

    OpenGLView.Destroy();

    return 0;
}
