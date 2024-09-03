#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd/*дескриптор окна*/, UINT uMsg/*идентификатор 
    сообщения*/, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) 
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY://after disappearing of window, but before destroying queue
        PostQuitMessage(0);//for breaking queue (puts WM_QUIT mes to the queue)
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, 
    int nCmdShow)
{
    const wchar_t className[] = L"MyWindowClass"; 

    WNDCLASSW wc = { 0 }; 
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className; //defines the window's class name
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassW(&wc)/*because zero is returned when error appear*/)
    { 
        MessageBoxW(NULL, L"Failed to register window class", L"Error", 
            MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Lab_1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) 
    {
        MessageBoxW(NULL, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        GetMessage(&msg/*if success then
        message writes there*/, NULL, 0, 0);//take first mes from the head of the queue
        TranslateMessage(&msg);//translate keystrokes into characters
        DispatchMessage(&msg);//calls window procedure of window by window's handle
    }                          //after calling's ending my function returns to this place

    return (int)msg.wParam;
}
