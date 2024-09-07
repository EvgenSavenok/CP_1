﻿#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <codecvt>
#include <iostream>
#include <unordered_set>
#include <filesystem>

#pragma comment(lib, "Comctl32.lib")

#define IDC_SEARCHBTN 2

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className);
HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow);

void CreateTables(HWND hwndParent);
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
        ShowWindow(hwnd, 3);//describes how to show window (can be parametrized by digits 
        //according to the table)
        UpdateWindow(hwnd);//send WM_PAINT without staying at queue
    }

    return hwnd;
}

int CalculateWindowWidth(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    return rcClient.right - rcClient.left;
}

int CalculateWindowHeight(HWND hwndParent)
{
    RECT rcClient;
    GetClientRect(hwndParent, &rcClient);

    return rcClient.bottom - rcClient.top;
}

HWND CreateStartBtn(HWND hwndParent)
{
    int windowWidth = CalculateWindowWidth(hwndParent);
    int windowHeight = CalculateWindowHeight(hwndParent);

    HWND hwndButton = CreateWindowEx(
        0,              
        L"BUTTON",      
        L"Sort",  
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
         windowWidth - 120, windowHeight - 100, 60, 50,
        hwndParent,     
        (HMENU)IDC_SEARCHBTN,       
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
        NULL            
    );
    return hwndButton;
}

//HWND AllocateHeaderTableMemory(HWND hwndListView, const int WINDOW_WIDTH)
//{
//    for (int i = 0; i < 2; ++i)
//    {
//        LVCOLUMN lvc;
//        ZeroMemory(&lvc, sizeof(LVCOLUMN));
//        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
//        lvc.cx = WINDOW_WIDTH / 2;      
//        ListView_InsertColumn(hwndListView, i, &lvc);  
//    }
//
//    std::vector<std::vector<std::wstring>> rows = {
//        { L"C:\\*", L"Result" }
//    };
//
//    for (int i = 0; i < rows.size(); ++i)
//    {
//        LVITEM lvItem;
//        ZeroMemory(&lvItem, sizeof(LVITEM));
//
//        lvItem.mask = LVIF_TEXT;
//        lvItem.iItem = i; 
//        lvItem.iSubItem = 0; 
//
//        lvItem.pszText = const_cast<LPWSTR>(rows[i][0].c_str());
//        ListView_InsertItem(hwndListView, &lvItem);
//
//        ListView_SetItemText(hwndListView, i, 1, const_cast<LPWSTR>(rows[i][1].c_str()));
//    }
//    return hwndListView;
//}

HWND AllocateDirectoriesTableMemory(HWND hwndListView, const int WINDOW_WIDTH)
{
    std::vector<std::vector<std::wstring>> rows = {
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"1", L"C:\hentai", L"13 kb", L"1", L"C:\hentai", L"15 kb" },
        { L"2", L"C:\hentai", L"19 kb", L"2", L"C:\hentai", L"11 kb" }
    };

    int numOfColumns = rows[0].size();

    for (int i = 0; i < numOfColumns; ++i)
    {
        LVCOLUMN lvc;
        ZeroMemory(&lvc, sizeof(LVCOLUMN));
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
        lvc.cx = WINDOW_WIDTH / numOfColumns;
        ListView_InsertColumn(hwndListView, i, &lvc);
    }

    for (int i = 0; i < rows.size(); ++i)
    {
        LVITEM lvItem;
        ZeroMemory(&lvItem, sizeof(LVITEM));

        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;
        lvItem.iSubItem = 0;

        lvItem.pszText = const_cast<LPWSTR>(rows[i][0].c_str());
        ListView_InsertItem(hwndListView, &lvItem);

        for (int j = 1; j < numOfColumns; ++j)
        {
            ListView_SetItemText(hwndListView, i, j, const_cast<LPWSTR>(rows[i][j].c_str()));
        }
    }

    return hwndListView;
}

HWND CreateEditBox(HWND hwndParent)
{
    int xpos = 8;            
    int ypos = 10;            
    int nwidth = 200;         
    int nheight = 25;         

    HWND hwndEditBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"C:\\",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)1001, GetModuleHandle(NULL), NULL);

    return hwndEditBox;
}


HWND CreateTable(HWND hwndParent, int WIDTH, int HEIGHT, 
    int xPos, int yPos, int ID_LISTVIEW, bool isCanEditLabels)
{
    DWORD style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER | WS_BORDER;

    if (isCanEditLabels)
        style |= LVS_EDITLABELS;  

    HWND hwndListView = CreateWindowEx(
        0,
        WC_LISTVIEW,
        NULL,
        style,
        xPos, yPos, WIDTH, HEIGHT,
        hwndParent,
        (HMENU)ID_LISTVIEW,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );
    ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_GRIDLINES);

    return hwndListView;
}

void CreateTables(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    int windowWidth = CalculateWindowWidth(hwndParent);
    int windowHeight = CalculateWindowHeight(hwndParent);

    //AllocateHeaderTableMemory(hwndListView, windowWidth);
    HWND hwndListView = CreateTable(hwndParent, windowWidth, windowHeight - 170,
        8, 50, 1002, false);
    AllocateDirectoriesTableMemory(hwndListView, windowWidth);
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

bool ProcessNotify(LPARAM lParam)
{
    const int ID_HEAD_LISTVIEW = 1001;
    LPNMHDR pnmhdr = (LPNMHDR)lParam;
    if (pnmhdr->idFrom == ID_HEAD_LISTVIEW)
    {
        switch (pnmhdr->code)
        {
        case LVN_BEGINLABELEDIT:
        {
            NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
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
                ListView_SetItemText(pnmhdr->hwndFrom, plvdi->item.iItem, 0, 
                    plvdi->item.pszText);
            }
            break;
        }
        }
    }
    return false;
}

TCHAR* OnButtonClick(HWND hwndParent) {
    HWND hWndEdit = GetDlgItem(hwndParent, 1001);
    int length = GetWindowTextLengthW(hWndEdit) + 1;
    TCHAR* buffer = new TCHAR[length];
    if (buffer)
    {
        GetDlgItemTextW(hwndParent, 1001, (LPWSTR)buffer, length);
    }
    return buffer;
}

#define MASK_FOUND L"*"
#define SIZE_BUF 260

std::string GetFileNameFromPath(const std::string& fullPath) 
{
    auto pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) 
    {
        return fullPath.substr(pos + 1);
    }
    return fullPath;
}

std::string GetUniqueFileName(const std::string& fullPath, 
    std::unordered_set<std::string>& fileNames) 
{
    std::string fileName = GetFileNameFromPath(fullPath);
    std::string directoryPath = fullPath.substr(0, fullPath.find_last_of("\\/"));
    std::string uniqueName = fileName;
    int counter = 1;
    bool isUnique = false;
    while (fileNames.find(uniqueName) != fileNames.end()) 
    {
        uniqueName = fileName + "(" + std::to_string(counter++) + ")";
        isUnique = true;
    }
    fileNames.insert(uniqueName);
    if (isUnique)
        return directoryPath + "\\" + uniqueName;
    else
        return fullPath;
}

void LogError(const std::wstring& filePath, int& countOfErrors) 
{
    DWORD errorCode = GetLastError();  
    std::wcerr << L"Failed to access file: " << filePath
        << L" Error code: " << errorCode << std::endl;
    countOfErrors++;
}

LONGLONG GetMyFileSize(const WIN32_FIND_DATA& findFileData) 
{
    LARGE_INTEGER fileSize;
    fileSize.LowPart = findFileData.nFileSizeLow;  
    fileSize.HighPart = findFileData.nFileSizeHigh;  
    return fileSize.QuadPart;  
}

void ScanNext(const wchar_t* directory, std::unordered_set<std::string>& fileNames, 
    std::vector<std::string>& filesPath, std::vector<std::string>& filesSizes,
    int& countOfErrors) {
    std::wstring searchPath = directory;
    searchPath.append(L"\\*");
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        LogError(searchPath, countOfErrors);
        return;
    }
    do 
    {
        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0)
            continue;
        std::wstring fullPath = directory;
        fullPath.append(L"\\");
        fullPath.append(findFileData.cFileName);
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            ScanNext(fullPath.c_str(), fileNames, filesPath,
                filesSizes, countOfErrors);
        }
        else 
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string fileName = converter.to_bytes(fullPath);
            std::string uniqueFileName = GetUniqueFileName(fileName, fileNames);
            filesPath.push_back(uniqueFileName);
            LONGLONG fileSize = GetMyFileSize(findFileData);
            std::string sizeStr = std::to_string(fileSize);
            filesSizes.push_back(sizeStr);
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);
    if (GetLastError() != ERROR_NO_MORE_FILES) 
    {
        LogError(searchPath, countOfErrors);
    }
    FindClose(hFind);
}

struct FileInfo {
    std::string filePath;
    LONGLONG fileSize;
};

void StartSorting(std::vector<std::string>& filesPath,
    std::vector<std::string>& filesSizes)
{

}

void StartScanning(HWND hwnd) 
{
    wchar_t* directory = OnButtonClick(hwnd);  
    std::unordered_set<std::string> fileNames; 
    std::vector<std::string> filesPath;
    std::vector<std::string> filesSizes;
    int countOfErrors = 0;
    ScanNext(directory, fileNames, filesPath, filesSizes, countOfErrors);
    std::cout << "Number of failure readings: " << countOfErrors << "\n";
    StartSorting(filesPath, filesSizes);
}

LRESULT CALLBACK WndProc(HWND hwnd/*дескриптор окна*/, UINT uMsg/*идентификатор 
    сообщения*/, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) 
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (LOWORD(wParam) == IDC_SEARCHBTN) 
        {
            StartScanning(hwnd);
        }
        break;
    }
    case WM_NOTIFY:
    {
        return ProcessNotify(lParam);
    }
    case WM_CREATE:
        CreateTables(hwnd);
        CreateEditBox(hwnd);
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

void CreateConsole() {
    AllocConsole();  
    FILE* pCout;  
    freopen_s(&pCout, "CONOUT$", "w", stdout);  
    FILE* pCerr;  
    freopen_s(&pCerr, "CONOUT$", "w", stderr);  
    std::cout << "Console has been created.\n";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
    int nCmdShow)
{
    CreateConsole();
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
