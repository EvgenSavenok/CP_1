#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className);
HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow);
HWND CreateTable(HWND hwndParent);
int RunMessageLoop();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className; 
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    return RegisterClassW(&wc);
}

HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow)
{
    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Lab_1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd != NULL)
    {
        ShowWindow(hwnd, nCmdShow);//describes how to show window (can be parametrized by digits 
        //according to the table)
        UpdateWindow(hwnd);//send WM_PAINT without staying at queue
    }

    return hwnd;
}

HWND CreateTable(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hwndListView = CreateWindowEx(
        0,
        WC_LISTVIEW,
        NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        10, 10, 500, 200,       
        hwndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );

    if (!hwndListView)
    {
        MessageBoxW(hwndParent, L"Failed to create ListView", L"Error", MB_OK | MB_ICONERROR);
        return NULL;
    }

    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.cx = 100;

    lvc.pszText = L"Column 1";
    ListView_InsertColumn(hwndListView, 0, &lvc);

    lvc.pszText = L"Column 2";
    ListView_InsertColumn(hwndListView, 1, &lvc);

    lvc.pszText = L"Column 3";
    ListView_InsertColumn(hwndListView, 2, &lvc);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    lvi.iItem = 0;
    lvi.iSubItem = 0;
    lvi.pszText = L"Item 1";
    ListView_InsertItem(hwndListView, &lvi);

    lvi.iSubItem = 1;
    lvi.pszText = L"Subitem 1.1";
    ListView_SetItem(hwndListView, &lvi);

    lvi.iSubItem = 2;
    lvi.pszText = L"Subitem 1.2";
    ListView_SetItem(hwndListView, &lvi);

    lvi.iItem = 1;
    lvi.iSubItem = 0;
    lvi.pszText = L"Item 2";
    ListView_InsertItem(hwndListView, &lvi);

    lvi.iSubItem = 1;
    lvi.pszText = L"Subitem 2.1";
    ListView_SetItem(hwndListView, &lvi);

    lvi.iSubItem = 2;
    lvi.pszText = L"Subitem 2.2";
    ListView_SetItem(hwndListView, &lvi);

    return hwndListView;
}

int RunMessageLoop()
{
    MSG msg;
    while (GetMessage(&msg/*if success then
        message writes there*/, NULL, 0, 0) > 0) //takes first mes from the head of the queue
    {
        TranslateMessage(&msg);//translate keystrokes into characters
        DispatchMessage(&msg);//calls window procedure of window by window's handle
    }                          //after calling's ending my function returns to this place

    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd/*дескриптор окна*/, UINT uMsg/*идентификатор 
    сообщения*/, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) 
    {
    case WM_CREATE:
        CreateTable(hwnd); // Create the ListView when the window is created
        break;
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

    if (!RegisterWindowClass(hInstance, className))
    {
        MessageBoxW(NULL, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateAppWindow(hInstance, className, nCmdShow);
    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    return RunMessageLoop();
}
