#include <windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>

#include "opengl_view.hpp"

extern CCamera Camera;
extern CString ModuleDirectory;
extern CString ErrorLog;

extern int gl_max_texture_size;
extern int gl_max_texture_max_anisotropy_ext;
extern COpenGLRenderer OpenGLRenderer;

void GetModuleDirectory();

bool COpenGLView::Init(HINSTANCE hInstance, char *Title, int Width, int Height, int Samples)
{
    this->Title = Title;
    this->Width = Width;
    this->Height = Height;

    WNDCLASSEX WndClassEx;

    memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

    WndClassEx.cbSize = sizeof(WNDCLASSEX);
    WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WndClassEx.lpfnWndProc = WndProc;
    WndClassEx.hInstance = hInstance;
    WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClassEx.lpszClassName = "Win32OpenGLWindowClass";

    if (RegisterClassEx(&WndClassEx) == 0)
    {
        ErrorLog.Set("RegisterClassEx failed!");
        return false;
    }

    DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, Title, Style, 0, 0, Width, Height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        ErrorLog.Set("CreateWindowEx failed!");
        return false;
    }

    HDC hDC = GetDC(hWnd);

    if (hDC == NULL)
    {
        ErrorLog.Set("GetDC failed!");
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;

    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int PixelFormat = ChoosePixelFormat(hDC, &pfd);

    if (PixelFormat == 0)
    {
        ErrorLog.Set("ChoosePixelFormat failed!");
        return false;
    }

    static int MSAAPixelFormat = 0;

    if (SetPixelFormat(hDC, MSAAPixelFormat == 0 ? PixelFormat : MSAAPixelFormat, &pfd) == FALSE)
    {
        ErrorLog.Set("SetPixelFormat failed!");
        return false;
    }

    hGLRC = wglCreateContext(hDC);

    if (hGLRC == NULL)
    {
        ErrorLog.Set("wglCreateContext failed!");
        return false;
    }

    if (wglMakeCurrent(hDC, hGLRC) == FALSE)
    {
        ErrorLog.Set("wglMakeCurrent failed!");
        return false;
    }

    if (glewInit() != GLEW_OK)
    {
        ErrorLog.Set("glewInit failed!");
        return false;
    }

    if (!GLEW_VERSION_2_1)
    {
        ErrorLog.Set("OpenGL 2.1 not supported!");
        return false;
    }

    if (MSAAPixelFormat == 0 && Samples > 0)
    {
        if (GLEW_ARB_multisample && WGLEW_ARB_pixel_format)
        {
            while (Samples > 0)
            {
                UINT NumFormats = 0;

                int PFAttribs[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                    WGL_COLOR_BITS_ARB, 32,
                    WGL_DEPTH_BITS_ARB, 24,
                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                    WGL_SAMPLES_ARB, Samples,
                    0
                };

                if (wglChoosePixelFormatARB(hDC, PFAttribs, NULL, 1, &MSAAPixelFormat, &NumFormats) == TRUE && NumFormats > 0) break;

                Samples--;
            }

            wglDeleteContext(hGLRC);
            DestroyWindow(hWnd);
            UnregisterClass(WndClassEx.lpszClassName, hInstance);

            return Init(hInstance, Title, Width, Height, Samples);
        }
        else
        {
            Samples = 0;
        }
    }

    this->Samples = Samples;

    GetModuleDirectory();

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

    if (GLEW_EXT_texture_filter_anisotropic)
    {
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);
    }

    if (WGLEW_EXT_swap_control)
    {
        wglSwapIntervalEXT(0);
    }

    return OpenGLRenderer.Init();
}

void COpenGLView::Show(bool Maximized)
{
    RECT dRect, wRect, cRect;

    GetWindowRect(GetDesktopWindow(), &dRect);
    GetWindowRect(hWnd, &wRect);
    GetClientRect(hWnd, &cRect);

    wRect.right += Width - cRect.right;
    wRect.bottom += Height - cRect.bottom;
    wRect.right -= wRect.left;
    wRect.bottom -= wRect.top;
    wRect.left = dRect.right / 2 - wRect.right / 2;
    wRect.top = dRect.bottom / 2 - wRect.bottom / 2;

    MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

    ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}

void COpenGLView::MessageLoop()
{
    MSG Msg;

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
}

void COpenGLView::Destroy()
{
    if (GLEW_VERSION_2_1) {
        OpenGLRenderer.Destroy();
    }

    wglDeleteContext(hGLRC);
    DestroyWindow(hWnd);
}

void COpenGLView::OnKeyDown(UINT Key)
{
    switch (Key)
    {
    case VK_F1:
        OpenGLRenderer.WireFrame = !OpenGLRenderer.WireFrame;
        break;

    case '1':
        OpenGLRenderer.DropRadius = 4.0f / 256.0f;
        break;

    case '2':
        OpenGLRenderer.DropRadius = 4.0f / 128.0f;
        break;

    case '3':
        OpenGLRenderer.DropRadius = 4.0f / 64.0f;
        break;

    case '4':
        OpenGLRenderer.DropRadius = 4.0f / 32.0f;
        break;

    case '5':
        OpenGLRenderer.DropRadius = 4.0f / 16.0f;
        break;

    case VK_SPACE:
        OpenGLRenderer.Pause = !OpenGLRenderer.Pause;
        break;
    }
}

void COpenGLView::OnLButtonDown(int X, int Y)
{
    OpenGLRenderer.AddDropByMouseClick(X, Y);
}

void COpenGLView::OnMouseMove(int X, int Y)
{
    if (GetKeyState(VK_RBUTTON) & 0x80)
    {
        Camera.OnMouseMove(LastX - X, LastY - Y);

        LastX = X;
        LastY = Y;
    }
}

void COpenGLView::OnMouseWheel(short zDelta)
{
    Camera.OnMouseWheel(zDelta);
}

void COpenGLView::OnPaint()
{
    static DWORD LastFPSTime = GetTickCount(), LastFrameTime = LastFPSTime, FPS = 0;

    PAINTSTRUCT ps;

    HDC hDC = BeginPaint(hWnd, &ps);

    DWORD Time = GetTickCount();

    float FrameTime = (Time - LastFrameTime) * 0.001f;

    LastFrameTime = Time;

    if (Time - LastFPSTime > 1000)
    {
        CString Text = Title;

        if (OpenGLRenderer.Text[0] != 0)
        {
            Text.Append(" - " + OpenGLRenderer.Text);
        }

        Text.Append(" - %dx%d", Width, Height);
        Text.Append(", ATF %dx", gl_max_texture_max_anisotropy_ext);
        Text.Append(", MSAA %dx", Samples);
        Text.Append(", FPS: %d", FPS);
        Text.Append(" - %s", glGetString(GL_RENDERER));

        SetWindowText(hWnd, Text);

        LastFPSTime = Time;
        FPS = 0;
    }
    else
    {
        FPS++;
    }

    BYTE Keys = 0x00;

    if (GetKeyState('W') & 0x80) Keys |= 0x01;
    if (GetKeyState('S') & 0x80) Keys |= 0x02;
    if (GetKeyState('A') & 0x80) Keys |= 0x04;
    if (GetKeyState('D') & 0x80) Keys |= 0x08;
    if (GetKeyState('R') & 0x80) Keys |= 0x10;
    if (GetKeyState('F') & 0x80) Keys |= 0x20;

    if (GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
    if (GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;

    if (Keys & 0x3F)
    {
        Camera.Move(Camera.OnKeys(Keys, FrameTime));
    }

    OpenGLRenderer.Render(FrameTime);

    SwapBuffers(hDC);

    EndPaint(hWnd, &ps);

    InvalidateRect(hWnd, NULL, FALSE);
}

void COpenGLView::OnRButtonDown(int X, int Y)
{
    LastX = X;
    LastY = Y;
}

void COpenGLView::OnSize(int Width, int Height)
{
    this->Width = Width;
    this->Height = Height;

    OpenGLRenderer.Resize(Width, Height);
}

void GetModuleDirectory()
{
    char *moduledirectory = new char[256];
    GetModuleFileName(GetModuleHandle(NULL), moduledirectory, 256);
    *(strrchr(moduledirectory, '\\') + 1) = 0;
    ModuleDirectory = moduledirectory;
    delete[] moduledirectory;
}
