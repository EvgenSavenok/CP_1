#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

#pragma comment(lib, "Comctl32.lib")

#define IDC_EDIT 1
#define IDC_SEARCHBTN 2
#define IDC_LABEL 3

COLORREF backgroundColor = RGB(255, 255, 255);

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className);
HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow);

HWND CreateTable(HWND hwndParent);
HWND CreateStartBtn(HWND hwndParent);
HWND CreateEditBox(HWND hwndParent);
HWND CreateResultLabel(HWND hwndParent);

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
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
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

HWND CreateStartBtn(HWND hwndParent)
{
     HWND hwndButton = CreateWindowEx(
        0,              
        L"BUTTON",      
        L"Sort",  
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
        500, 500, 60, 30,             
        hwndParent,     
        (HMENU)IDC_SEARCHBTN,       
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
        NULL            
    );
     return hwndButton;
}

HWND CreateResulLabel(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    const int WINDOW_WIDTH = rcClient.right - rcClient.left;

    HWND hwndLabel = CreateWindowEx(
        0,                  
        L"STATIC",           
        L"Result",  
        WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
        WINDOW_WIDTH / 2,
        0,                 
        WINDOW_WIDTH / 2,
        20,                 
        hwndParent,               
        (HMENU)IDC_LABEL,   
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
        NULL                
    );
    return hwndLabel;
}

HWND CreateEditBox(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    const int WINDOW_WIDTH = rcClient.right - rcClient.left;

    HWND hwndEdit = CreateWindowEx(
        0,                  
        L"EDIT",            
        L"Directory",               
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER, 
        0,                
        0,                 
        WINDOW_WIDTH / 2,
        20,                 
        hwndParent,         
        (HMENU)IDC_EDIT,    
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL               
    );
    return hwndEdit;
}

HWND CreateTable(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    const int WINDOW_WIDTH = rcClient.right - rcClient.left;
    const int NUM_OF_COLS = 6;

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    int tableWidth = WINDOW_WIDTH;
    int tableHeight = 200; 
    int xPos = 0; 
    int yPos = 20; 

    HWND hwndListView = CreateWindowEx(
        0,
        WC_LISTVIEW,
        NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        xPos, yPos, tableWidth, tableHeight,
        hwndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );

    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.cx = (WINDOW_WIDTH) / NUM_OF_COLS;

    std::wstring wstr1 = L"Num";
    std::wstring wstr2 = L"Directory";
    std::wstring wstr3 = L"Dynamic Column 3";
    std::wstring wstr4 = L"Num";
    std::wstring wstr5 = L"Dynamic Column 5";
    std::wstring wstr6 = L"Dynamic Column 6";

    std::vector<std::wstring> columns = { wstr1, wstr2, wstr3, wstr4, wstr5, wstr6 };

    for (int i = 0; i < columns.size(); ++i)
    {
        LVCOLUMN lvc;
        ZeroMemory(&lvc, sizeof(LVCOLUMN)); // Инициализация структуры нулями

        lvc.mask = LVCF_TEXT | LVCF_WIDTH; // Указываем, что устанавливаем текст и ширину
        lvc.cx = 100; // Ширина столбца (можете настроить по своему)

        // Выделяем память для текстовой строки
        LPWSTR lpwstr = new wchar_t[columns[i].size() + 1];
        std::copy(columns[i].begin(), columns[i].end(), lpwstr);
        lpwstr[columns[i].size()] = L'\0'; // Устанавливаем нулевой терминатор

        lvc.pszText = lpwstr; // Устанавливаем текст столбца
        ListView_InsertColumn(hwndListView, i, &lvc); // Вставляем столбец

        // Освобождаем память после вставки столбца
        delete[] lpwstr;
    }

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    /*lvi.iItem = 0;
    lvi.iSubItem = 0;
    lvi.pszText = L"Item 1";
    ListView_InsertItem(hwndListView, &lvi);

    lvi.iSubItem = 1;
    lvi.pszText = L"Subitem 1.1";
    ListView_SetItem(hwndListView, &lvi);*/

    return hwndListView;
}

int RunMessageLoop()
{
    MSG msg;
    while (GetMessage(&msg/*if success then
        message writes there*/, NULL, 0, 0)) //takes first mes from the head of the queue
    {
        TranslateMessage(&msg);//translate keystrokes into characters
        DispatchMessage(&msg);//calls window procedure of window by window's handle
    }                          //after calling's ending my function returns to this place

    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd/*дескриптор окна*/, UINT uMsg/*идентификатор 
    сообщения*/, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush;

    switch (uMsg) 
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_SEARCHBTN)
        {
            //Start sorting...
        }
        if (wmId == IDC_EDIT) {
            if (HIWORD(wParam) == EN_CHANGE) {
                // Текст в текстовом поле изменился
                HWND hwndEdit = GetDlgItem(hwnd, IDC_EDIT);
                int length = GetWindowTextLength(hwndEdit);
                wchar_t* buffer = new wchar_t[length + 1];
                GetWindowText(hwndEdit, buffer, length + 1);
                // Теперь buffer содержит текст из поля редактирования
                delete[] buffer;
            }
        }
        break;
    }
    case WM_CREATE:
        CreateTable(hwnd); 
        CreateStartBtn(hwnd);
        CreateEditBox(hwnd);
        CreateResulLabel(hwnd);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
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
