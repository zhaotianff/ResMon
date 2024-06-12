// ResourceMonitor.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ResourceMonitor.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

VOID CreateTab(HWND,HFONT);
LRESULT CALLBACK WndProcInfoClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL g_bMonitor = TRUE;
HWND g_hwndInfoTabControl;
HWND g_hwndInfoClassTabControl;
HWND g_aInfoClassHwnd[2];
HFONT hFont;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RESOURCEMONITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RESOURCEMONITOR));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RESOURCEMONITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RESOURCEMONITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        EnableMenuItem(GetMenu(hWnd), IDM_STARTMONITOR, MF_BYCOMMAND | MF_GRAYED);
        CreateTab(hWnd,hFont);
        break;
    }
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case TCN_SELCHANGE:
            auto nIndex = SendMessage(g_hwndInfoClassTabControl, TCM_GETCURSEL, 0, 0);
            for (int i = 0; i < 2; i++)
            {
                ShowWindow(g_aInfoClassHwnd[i], SW_HIDE);
            }
            ShowWindow(g_aInfoClassHwnd[nIndex], SW_SHOW);
            break;
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_STARTMONITOR:
            {
                g_bMonitor = TRUE;
                EnableMenuItem(GetMenu(hWnd), IDM_STARTMONITOR, MF_BYCOMMAND | MF_GRAYED);
                EnableMenuItem(GetMenu(hWnd), IDM_STOPMONITOR, MF_BYCOMMAND | MF_ENABLED);
                break;
            }
            case IDM_STOPMONITOR:
            {
                g_bMonitor = FALSE;
                EnableMenuItem(GetMenu(hWnd), IDM_STARTMONITOR, MF_BYCOMMAND | MF_ENABLED);
                EnableMenuItem(GetMenu(hWnd), IDM_STOPMONITOR, MF_BYCOMMAND | MF_GRAYED);
            }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE:
    {
        RECT rect{};
        SetRect(&rect, 0, 0, LOWORD(lParam), HIWORD(lParam));
        SendMessage(g_hwndInfoClassTabControl, TCM_ADJUSTRECT, FALSE, (LPARAM)&rect);
        for (int i = 0; i < 2; i++)
        {
            MoveWindow(g_aInfoClassHwnd[i],
                rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
        }
        break;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        DeleteObject(hFont);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

VOID CreateTab(HWND hWnd,HFONT hFont)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_hwndInfoTabControl = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE | TCS_MULTILINE | TCS_TOOLTIPS, 0, 0, rect.right, rect.bottom, hWnd, NULL, hInst, NULL);

    TCITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    TCHAR achTemp[256];
    tie.pszText = achTemp;
    StringCchCopy(achTemp, _countof(achTemp), L"概览");
    if (TabCtrl_InsertItem(g_hwndInfoTabControl, 0, &tie) == -1)
    {
        DestroyWindow(g_hwndInfoTabControl);
        return;
    }

    TCITEM subTie;
    subTie.mask = TCIF_TEXT | TCIF_IMAGE;
    subTie.iImage = -1;
    TCHAR achSub[256];
    subTie.pszText = achSub;
    StringCchCopy(achSub, _countof(achSub), L"内存");

    RECT rectClassTab{};
    TabCtrl_AdjustRect(g_hwndInfoTabControl, FALSE, &rectClassTab);

    g_hwndInfoClassTabControl = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE | TCS_VERTICAL | TCS_TOOLTIPS, rectClassTab.left, rectClassTab.top, rect.right, rect.bottom, g_hwndInfoTabControl, NULL, hInst, NULL);

    if (TabCtrl_InsertItem(g_hwndInfoClassTabControl, 0, &subTie) == -1)
    {
        DestroyWindow(g_hwndInfoClassTabControl);
        return;
    }

    subTie.mask = TCIF_TEXT | TCIF_IMAGE;
    subTie.iImage = -1;
    subTie.pszText = achSub;
    StringCchCopy(achSub, _countof(achSub), L"CPU");

    if (TabCtrl_InsertItem(g_hwndInfoClassTabControl, 1, &subTie) == -1)
    {
        DestroyWindow(g_hwndInfoClassTabControl);
        return;
    }

    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProcInfoClass;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = TEXT("ClassChildWindow");
    wndclass.hIconSm = NULL;
    RegisterClassEx(&wndclass);
    g_aInfoClassHwnd[0] = CreateWindowEx(0, TEXT("ClassChildWindow"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, g_hwndInfoClassTabControl, NULL, hInst, NULL);
    g_aInfoClassHwnd[1] = CreateWindowEx(0, TEXT("ClassChildWindow"), NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, g_hwndInfoClassTabControl, NULL, hInst, NULL);

    auto nIndex = SendMessage(g_hwndInfoClassTabControl, TCM_GETCURSEL, 0, 0);
    for (int i = 0; i < 2; i++)
    {
        ShowWindow(g_aInfoClassHwnd[i],SW_HIDE);
    }
    ShowWindow(g_aInfoClassHwnd[nIndex], SW_SHOW);

    hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
    SendMessage(g_hwndInfoTabControl, WM_SETFONT, (WPARAM)hFont, FALSE);
    SendMessage(g_hwndInfoClassTabControl, WM_SETFONT, (WPARAM)hFont, FALSE);
}


LRESULT CALLBACK WndProcInfoClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect{};
        GetClientRect(hWnd, &rect);
        if(hWnd == g_aInfoClassHwnd[0])
            DrawText(hdc, TEXT("内存"), _tcslen(TEXT("内存")),
                &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        else if(hWnd == g_aInfoClassHwnd[1])
            DrawText(hdc, TEXT("CPU"), _tcslen(TEXT("CPU")),
                &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        EndPaint(hWnd, &ps); 
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}