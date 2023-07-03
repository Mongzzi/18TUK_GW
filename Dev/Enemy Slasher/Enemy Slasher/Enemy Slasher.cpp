// Enemy Slasher.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Enemy Slasher.h"

#define FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 480

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE                       ghAppInstance;                                // 현재 인스턴스입니다.
TCHAR                           szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
TCHAR                           szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
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

    MSG msg;
    HACCEL hAccelTable;

    // 전역 문자열을 초기화합니다.
    ::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    ::LoadString(hInstance, IDC_ENEMYSLASHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENEMYSLASHER));

    // 기본 메시지 루프입니다:
    while (1)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else {
            // GameLoop
        }
    }

    return ((int) msg.wParam);
}


//  창 클래스를 등록합니다.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENEMYSLASHER));
    wcex.hCursor        = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_ENEMYSLASHER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return (::RegisterClassEx(&wcex));
}


//  인스턴스 핸들을 저장하고 주 창을 만듭니다.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   ghAppInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
   AdjustWindowRect(&rc, dwStyle, FALSE);

   HWND hMainWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

   if (!hMainWnd) return FALSE;

   // on create gameframework

   ::ShowWindow(hMainWnd, nCmdShow);
   ::UpdateWindow(hMainWnd);

   return (TRUE);
}


//  주 창의 메시지를 처리합니다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_SIZE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
    case WM_KEYUP:
        break;
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

            switch (wmId)
            {
            case IDM_ABOUT:
                ::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return (::DefWindowProc(hWnd, message, wParam, lParam));
            }
        
        break;
    case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    default:
        return (::DefWindowProc(hWnd, message, wParam, lParam));
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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
