#include <windows.h>

// Функция обработки сообщений окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    const wchar_t className[] = L"MyWindowClass"; // Используем широкую строку

    // Определение класса окна
    WNDCLASSW wc = { 0 }; // WNDCLASSW для Unicode
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Регистрация класса окна
    if (!RegisterClassW(&wc)) { // RegisterClassW для Unicode
        MessageBoxW(NULL, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Создание окна
    HWND hwnd = CreateWindowW(
        className,
        L"My Window", // Используем широкую строку
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Показать окно
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
