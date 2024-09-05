#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

#pragma comment(lib, "Comctl32.lib")

#define IDC_EDIT 1
#define IDC_SEARCHBTN 2
#define IDC_LABEL 3
#define ID_LISTVIEW 1001

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className);
HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow);

HWND CreateTable(HWND hwndParent);
HWND CreateStartBtn(HWND hwndParent);

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
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    const int WINDOW_WIDTH = rcClient.right - rcClient.left;

     HWND hwndButton = CreateWindowEx(
        0,              
        L"BUTTON",      
        L"Sort",  
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
         WINDOW_WIDTH - 120, 500, 60, 30,
        hwndParent,     
        (HMENU)IDC_SEARCHBTN,       
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
        NULL            
    );
     return hwndButton;
}

HWND AllocateTableMemory(HWND hwndListView, const int WINDOW_WIDTH)
{
    for (int i = 0; i < 2; ++i)
    {
        LVCOLUMN lvc;
        ZeroMemory(&lvc, sizeof(LVCOLUMN));
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
        lvc.cx = WINDOW_WIDTH / 2;      
        ListView_InsertColumn(hwndListView, i, &lvc);  
    }

    std::vector<std::vector<std::wstring>> rows = {
        { L"Directory", L"Result" }
    };

    for (int i = 0; i < rows.size(); ++i)
    {
        LVITEM lvItem;
        ZeroMemory(&lvItem, sizeof(LVITEM));

        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i; 
        lvItem.iSubItem = 0; 

        lvItem.pszText = const_cast<LPWSTR>(rows[i][0].c_str());
        ListView_InsertItem(hwndListView, &lvItem);

        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(rows[i][1].c_str()));
    }

    return hwndListView;
}

HWND CreateTable(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    const int WINDOW_WIDTH = rcClient.right - rcClient.left;

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    int tableWidth = WINDOW_WIDTH;
    int tableHeight = 200; 
    int xPos = 0; 
    int yPos = 0; 

    HWND hwndListView = CreateWindowEx(
        0,
        WC_LISTVIEW,
        NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_NOCOLUMNHEADER,
        xPos, yPos, tableWidth, tableHeight,
        hwndParent,
        (HMENU)ID_LISTVIEW,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );

    AllocateTableMemory(hwndListView, WINDOW_WIDTH);

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
    HWND hwndListView = {};

    switch (uMsg) 
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_SEARCHBTN)
        {
            //Start sorting...
        }
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmhdr = (LPNMHDR)lParam;
        if (pnmhdr->idFrom == ID_LISTVIEW)
        {
            switch (pnmhdr->code)
            {
            case LVN_BEGINLABELEDIT:
            {
                NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
                //HWND hEdit = (HWND)SendMessage(pnmhdr->hwndFrom, LVM_GETEDITCONTROL, 0, 0);
                if (plvdi->item.iSubItem != 0)
                {
                    return true;
                }
                break;
            }
            case LVN_ENDLABELEDIT:
            {
                NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
                if (plvdi->item.pszText != NULL)
                {
                    ListView_SetItemText(pnmhdr->hwndFrom, plvdi->item.iItem, 0, plvdi->item.pszText);
                }
                break;
            }
            }
        }
        break;
    }
    case WM_CREATE:
        CreateTable(hwnd);
        CreateStartBtn(hwnd);
        
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
