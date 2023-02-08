// gobang.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "gobang.h"

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
    LoadStringW(hInstance, IDC_GOBANG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GOBANG));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOBANG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GOBANG);
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
static chess_board board;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static COLORREF color = BLACK;//最初棋子的颜色
    switch (message){
    case WM_CREATE: {
        InitConsoler();
	    //std::cout << "初始化调试" << std::endl;
		//设置窗口大小 400 400
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 800, 800, SWP_NOMOVE);
		//设置窗口标题
		SetWindowText(hWnd, L"五子棋");
		//设置为不可修改大小
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
		//设置为不可最大化
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
		//初始化棋盘
        //获得窗口dc
		HDC hdc = GetDC(hWnd);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int x = rect.left;
        int y = rect.top;
        //棋盘大小为400*400 窗口大小为800*800 居中
        x = (rect.right - rect.left - 400) / 2;
        y = (rect.bottom - rect.top - 400) / 2;
		board.Init(hdc, x, y,400,400,20,20);
    }
    case WM_COMMAND:{
       int wmId = LOWORD(wParam);
       // 分析菜单选择:
       switch (wmId)
       {
       case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
       default:
           return DefWindowProc(hWnd, message, wParam, lParam);
       }
       break;
    }
    case WM_PAINT:{
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //绘制背景
	    board.DrawBackground();
        //绘制棋盘 
	    board.DrawChessBoard();
        //绘制棋子
	    board.DrawChessPiece();
        EndPaint(hWnd, &ps);
        break;
    }
	case WM_LBUTTONDOWN:{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		
        auto [px,py] = board.GetChessPiecePos(x, y);//转换为棋盘坐标

        LogInfo("x : %d,py :%d\n", px, py);
	
		if (board.CheckHasPiece(px, py)) break;
        
        auto newcolor = (color == BLACK) ? WHITE : BLACK;
        if (board.AddChessPiece(px, py, color)) {
            color = newcolor;
        }
        InvalidateRect(hWnd, NULL, TRUE);//
        bool IsRestart = false;
        if (WHITE ==color &&board.IsWin(px, py, BLACK)) {
            MessageBoxA(NULL, "黑棋胜", "提示", MB_OK);
            IsRestart = true;
        }
        if (BLACK ==color&&board.IsWin(px, py, WHITE)) {
            MessageBoxA(NULL, "白棋胜", "提示", MB_OK);
            IsRestart = true;
        }
        //重新开始
        if (IsRestart) {
            board.ClearChessPiece();
            InvalidateRect(hWnd, NULL, TRUE);//
        }
        break;
	}
    case WM_RBUTTONDOWN:{
        if (board.BackChessPiece()){
            LogInfo("悔棋\n");
            color=(color == BLACK) ? WHITE : BLACK;
        }
		InvalidateRect(hWnd, NULL, TRUE);//
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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
