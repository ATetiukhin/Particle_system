#ifndef OPENGL_VIEW_HPP_INCLUDED
#define OPENGL_VIEW_HPP_INCLUDED


LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);


class COpenGLView
{
public:
    COpenGLView();
    ~COpenGLView();

    bool Init(HINSTANCE hInstance, char *Title, int Width, int Height, int Samples);
    void Show(bool Maximized = false);
    void MessageLoop();
    void Destroy();

    void OnKeyDown(UINT Key);
    void OnLButtonDown(int X, int Y);
    void OnMouseMove(int X, int Y);
    void OnMouseWheel(short zDelta);
    void OnPaint();
    void OnRButtonDown(int X, int Y);
    void OnSize(int Width, int Height);

protected:
    HWND hWnd;
    HGLRC hGLRC;

    int Width;
    int Height;
    int Samples;

    int LastX;
    int LastY;

    char *Title;


};

#endif /* End of 'opengl_view.hpp' file */