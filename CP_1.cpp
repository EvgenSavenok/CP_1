#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <codecvt>
#include <iostream>
#include <unordered_set>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <iostream>

#pragma comment(lib, "Comctl32.lib")

#define IDC_SEARCHBTN 2
#define IDC_EDITBOX 1001
#define IDC_LISTVIEW 1002
#define MASK_FOUND L"*"
#define SIZE_BUF 260

BOOL RegisterWindowClass(HINSTANCE hInstance, const wchar_t* className);
HWND CreateAppWindow(HINSTANCE hInstance, const wchar_t* className, int nCmdShow);

void CreateTables(HWND hwndParent);
HWND CreateStartBtn(HWND hwndParent);

int RunMessageLoop();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    return RegisterClassW(&wc);
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
         windowWidth - 120, windowHeight - 135, 60, 50,
        hwndParent,     
        (HMENU)IDC_SEARCHBTN,       
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
        NULL            
    );
    return hwndButton;
}

HWND AllocateDirectoriesTableMemory(HWND hwndListView, const int WINDOW_WIDTH)
{
    const int numOfColumns = 6;

    LVCOLUMN lvc;
    ZeroMemory(&lvc, sizeof(LVCOLUMN));
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    const wchar_t* headers[] = { L"Number", L"File Path", L"File Size", L"Number", L"Sorted File Path", L"Sorted File Size" };

    for (int i = 0; i < numOfColumns; ++i)
    {
        lvc.iSubItem = i;
        lvc.pszText = const_cast<LPWSTR>(headers[i]);
        lvc.cx = WINDOW_WIDTH / numOfColumns;
        ListView_InsertColumn(hwndListView, i, &lvc);
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
        xpos, ypos, nwidth, nheight, hwndParent, (HMENU)IDC_EDITBOX, GetModuleHandle(NULL), NULL);

    return hwndEditBox;
}


HWND CreateTable(HWND hwndParent, int WIDTH, int HEIGHT, 
    int xPos, int yPos, int ID_LISTVIEW, bool isCanEditLabels)
{
    DWORD style = WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER;

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

    HWND hwndListView = CreateTable(hwndParent, windowWidth, windowHeight - 200,
        8, 50, IDC_LISTVIEW, false);
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

TCHAR* OnButtonClick(HWND hwndParent) 
{
    HWND hwndListView = GetDlgItem(hwndParent, IDC_LISTVIEW);
    ListView_DeleteAllItems(hwndListView);
    HWND hWndEdit = GetDlgItem(hwndParent, IDC_EDITBOX);
    int length = GetWindowTextLengthW(hWndEdit) + 1;
    TCHAR* buffer = new TCHAR[length];
    if (buffer)
    {
        GetDlgItemTextW(hwndParent, IDC_EDITBOX, (LPWSTR)buffer, length);
    }
    return buffer;
}

std::wstring GetFileNameFromPath(const std::wstring& fullPath)
{
    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
        return fullPath.substr(pos + 1);
    return fullPath;
}

std::wstring GetUniqueFileName(const std::wstring& fullPath,
    std::unordered_map<std::wstring, int>& nameCount)
{
    std::wstring fileName = GetFileNameFromPath(fullPath);
    std::wstring directoryPath = fullPath.substr(0, fullPath.find_last_of(L"\\/"));
    int count = nameCount[fileName];
    std::wstring uniqueName;
    if (count > 0) 
        uniqueName = fileName + L"(" + std::to_wstring(count) + L")";
    else 
        uniqueName = fileName;
    nameCount[fileName] = count + 1;
    return directoryPath + L"\\" + uniqueName;
}

struct ErrorInfo {
    std::wstring filePath;
    DWORD errorCode;
};

std::vector<ErrorInfo> errorBuffer;

void LogError(const std::wstring& filePath, int& countOfErrors)
{
    ErrorInfo err{ filePath, GetLastError() };
    errorBuffer.push_back(err);
    countOfErrors++;
}

void FlushErrors(int& countOfErrors, wchar_t* directory)
{
    std::wstring searchPath = L"C:\\Downloads";
    searchPath += L"\\ErrorLogs";
    if (!CreateDirectoryW(searchPath.c_str(), NULL))
        return;
    
    std::wofstream errorLogFile(searchPath + L"\\ErrorLogs.txt", std::ios::out
        | std::ios::trunc);
    if (!errorLogFile.is_open()) 
        return;
    for (const auto& error : errorBuffer) 
    {
        errorLogFile << L"Failed to access file: " << error.filePath
            << L" Error code: " << error.errorCode << std::endl;
    }
    errorLogFile << L"Number of failure readings: " << countOfErrors 
        << std::endl;
    errorLogFile.close();
}


LONGLONG GetMyFileSize(const WIN32_FIND_DATA& findFileData) 
{
    LARGE_INTEGER fileSize;
    fileSize.LowPart = findFileData.nFileSizeLow;  
    fileSize.HighPart = findFileData.nFileSizeHigh;  
    return fileSize.QuadPart;  
}

void ScanNext(const wchar_t* directory, std::unordered_map<std::wstring, int>& fileNames,
    std::vector<std::wstring>& filesPath, std::vector<std::wstring>& filesSizes,
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
            std::wstring uniqueFileName = GetUniqueFileName(fullPath, fileNames);
            filesPath.push_back(uniqueFileName);  
            filesSizes.push_back(std::to_wstring(GetMyFileSize(findFileData)));
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);
    if (GetLastError() != ERROR_NO_MORE_FILES) 
        LogError(searchPath, countOfErrors);
    FindClose(hFind);
}

struct FileInfo {
    std::wstring filePath;
    LONGLONG fileSize;
};

void Merge(std::vector<std::wstring>& files, std::vector<std::wstring>& sizes, 
    int left, int mid, int right) 
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<std::wstring> leftFiles(n1), rightFiles(n2);
    std::vector<std::wstring> leftSizes(n1), rightSizes(n2);

    for (int i = 0; i < n1; i++) 
    {
        leftFiles[i] = files[left + i];
        leftSizes[i] = sizes[left + i];
    }
    for (int i = 0; i < n2; i++) 
    {
        rightFiles[i] = files[mid + 1 + i];
        rightSizes[i] = sizes[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) 
    {
        if (leftSizes[i].length() < rightSizes[j].length() ||
            (leftSizes[i].length() == rightSizes[j].length() && leftSizes[i] <= rightSizes[j]))
        {
            sizes[k] = leftSizes[i];
            files[k] = leftFiles[i];
            i++;
        }
        else 
        {
            sizes[k] = rightSizes[j];
            files[k] = rightFiles[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        sizes[k] = leftSizes[i];
        files[k] = leftFiles[i];
        i++;
        k++;
    }
    while (j < n2) {
        sizes[k] = rightSizes[j];
        files[k] = rightFiles[j];
        j++;
        k++;
    }
}

void MergeSort(std::vector<std::wstring>& files, 
    std::vector<std::wstring>& sizes, int left, int right) 
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;//to avoid overflow
        MergeSort(files, sizes, left, mid);
        MergeSort(files, sizes, mid + 1, right);
        Merge(files, sizes, left, mid, right);
    }
}

void StartSorting(std::vector<std::wstring>& filesPath,
    std::vector<std::wstring>& filesSizes)
{
    MergeSort(filesPath, filesSizes, 0, filesSizes.size() - 1);
}

void PrintResults(std::vector<std::wstring>& filesPath, std::vector<std::wstring>& filesSizes,
    std::vector<std::wstring>& sortedFilesPath, std::vector<std::wstring>& sortedFilesSizes,
    HWND hwndParent)
{
    HWND hwndListView = GetDlgItem(hwndParent, IDC_LISTVIEW);
    const int numOfColumns = 6;
    if (filesPath.size() != filesSizes.size() || sortedFilesPath.size() != sortedFilesSizes.size())
        return;
    SendMessage(hwndListView, WM_SETREDRAW, FALSE, 0);
    LVITEM lvItem;
    ZeroMemory(&lvItem, sizeof(LVITEM));
    lvItem.mask = LVIF_TEXT;
    std::wstring index;
    for (int i = 0; i < filesPath.size(); ++i)
    {
        index = std::to_wstring(i + 1);
        lvItem.iItem = i;
        lvItem.iSubItem = 0;
        lvItem.pszText = (LPWSTR)index.c_str();  
        ListView_InsertItem(hwndListView, &lvItem);
        ListView_SetItemText(hwndListView, i, 1, (LPWSTR)filesPath[i].c_str());
        ListView_SetItemText(hwndListView, i, 2, (LPWSTR)filesSizes[i].c_str());
        ListView_SetItemText(hwndListView, i, 3, (LPWSTR)index.c_str());
        ListView_SetItemText(hwndListView, i, 4, (LPWSTR)sortedFilesPath[i].c_str());
        ListView_SetItemText(hwndListView, i, 5, (LPWSTR)sortedFilesSizes[i].c_str());
    }
    SendMessage(hwndListView, WM_SETREDRAW, TRUE, 0);
    RedrawWindow(hwndListView, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void StartScanning(HWND hwndParent) 
{
    wchar_t* directory = OnButtonClick(hwndParent);  
    std::unordered_map<std::wstring, int> nameCount;
    std::vector<std::wstring> filesPath;
    std::vector<std::wstring> filesSizes;
    int countOfErrors = 0;
    ScanNext(directory, nameCount, filesPath, filesSizes, countOfErrors);
    std::vector<std::wstring> sortedFilesPath = filesPath;
    std::vector<std::wstring> sortedFilesSizes = filesSizes;
    StartSorting(sortedFilesPath, sortedFilesSizes);
    PrintResults(filesPath, filesSizes, sortedFilesPath, sortedFilesSizes, hwndParent);
    FlushErrors(countOfErrors, directory);
}

HWND CreateAppWindow(HINSTANCE hInstance, int nCmdShow)
{
    HWND hwnd = CreateWindowEx(
        0,
        L"MainClass",
        L"Lab_1",
        WS_OVERLAPPEDWINDOW,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL
    );
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style);
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, 3);//describes how to show window (can be parametrized by digits 
        //according to the table)
        UpdateWindow(hwnd);//send WM_PAINT without staying at queue
    }

    return hwnd;
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
    int nCmdShow)
{
    if (!RegisterWindowClass(hInstance))
    {
        MessageBoxW(NULL, L"Failed to register window class", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateAppWindow(hInstance, nCmdShow);
    if (hwnd == NULL)
    {
        MessageBoxW(NULL, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    return RunMessageLoop();
}
