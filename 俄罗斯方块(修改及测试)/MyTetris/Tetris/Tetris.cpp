// Tetris.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tetris.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HMENU diff;//定义菜单句柄变量
HMENU lay;//定义菜单句柄变量
HMENU lay1;//定义菜单句柄变量
HMENU rotate;//定义菜单句柄变量
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance,//
	HINSTANCE hPrevInstance,//定义HINSTANCE型变量
	LPWSTR    lpCmdLine,//定义WCHAR型变量
	int       nCmdShow)//定义int型变量
{
	init_game();//游戏初始化
	UNREFERENCED_PARAMETER(hPrevInstance);//引用宏定义
	UNREFERENCED_PARAMETER(lpCmdLine);//引用宏定义

									  // 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);//选择难度

							   //  判断是否初始化成功
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));//初始化加速器

	MSG msg;

	
	while (1)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))//
		{
			TranslateMessage(&msg);//翻译信息
			DispatchMessage(&msg);//传送信息
			if (msg.message == WM_QUIT)//判断是否选择退出
			{
				break;
			}
		}
		else
		{
			if ((GAME_STATE & 2) != 0)
			{
				tCur = GetTickCount();//读取玩家账户
				if (tCur - tPre > g_speed)//
				{
					int flag = CheckValide(curPosX, curPosY + 1, bCurTeris);//查看矩阵是否合法
					if (flag == 1)//矩阵合法
					{
						curPosY++;//纵坐标+1
						tPre = tCur;//
						HWND hWnd = GetActiveWindow();
						//这里两次调用实现了双缓冲的作用，改善了视觉效果
						InvalidateRect(hWnd, &rc_left, FALSE);//
						InvalidateRect(hWnd, &rc_right_top, FALSE);//系统不会像窗口发送WM_PAINT消息，来重绘
					}
					else if (flag == -2)//
					{
						g_speed = t_speed;//掉落速度改变
						fillBlock();//到达底部填充矩阵
						checkComplite(); //检查是否能消除
						setRandomT();//获取随即方块
						curPosX = (NUM_X - 4) >> 1;//赋值方块横坐标
						curPosY = 0;//赋值方块纵坐标
						HWND hWnd = GetActiveWindow();//
						InvalidateRect(hWnd, &rc_main, FALSE);//
					}
					else if (flag == -3)//矩阵不合法
					{
						HWND hWnd = GetActiveWindow();//获取玩家选择
						if (MessageBox(hWnd, L"胜败乃兵家常事，菜鸡请重新来过", L":时光机", MB_YESNO) == IDYES)//
						{
							init_game();//初始化游戏
						}
						else
						{
							break;//退出游戏
						}
					}
				}
			}
		}
	}
	
	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TETRIS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;//包含某应用程序用来绘制他所拥有的窗口客户区所需要的信息
	HDC hdc;//
	int nWinx, nWiny, nClientX, nClientY;
	int posX, posY;
	RECT rect;//
	HMENU hSysmenu;//
    switch (message)
    {
	case WM_CREATE://
		GetWindowRect(hWnd, &rect);
		//
		nWinx = 530;//
		nWiny = 680;//
		posX = GetSystemMetrics(SM_CXSCREEN);//获取横坐标
		posY = GetSystemMetrics(SM_CYSCREEN);//获取纵坐标
		posX = (posX - nWinx) >> 1;//
		posY = (posY - nWiny) >> 1;//
		GetClientRect(hWnd, &rect);//
		nClientX = rect.right - rect.left;//调整玩家操作后坐标
		nClientY = rect.bottom - rect.top;//调整玩家操作后坐标

		MoveWindow(hWnd, posX, posY, 530, 680, TRUE);//
		hSysmenu = GetMenu(hWnd);//
		AppendMenu(hSysmenu, MF_SEPARATOR, 0, NULL);//
													//此处的菜单句柄要全局向前初始化
		diff = CreatePopupMenu();//
		AppendMenu(hSysmenu, MF_POPUP, (UINT_PTR)diff, L"难度选择");//展示难度选择菜单
		AppendMenu(diff, MF_STRING, ID_dif1, L"难度1");//选择难度
		AppendMenu(diff, MF_STRING, ID_dif2, L"难度2");//
		AppendMenu(diff, MF_STRING, ID_dif3, L"难度3");//
		rotate = CreatePopupMenu();//
		AppendMenu(hSysmenu, MF_POPUP, (UINT_PTR)rotate, L"旋转选择");//
		AppendMenu(rotate, MF_STRING, ID_ROATE1, L"顺时针");//
		AppendMenu(rotate, MF_STRING, ID_ROATE2, L"逆时针");//
		SetMenu(hWnd, rotate);//
		lay = CreatePopupMenu();//
		AppendMenu(hSysmenu, MF_POPUP, (UINT_PTR)lay, L"布局选择");//展示布局选择菜单
		AppendMenu(lay, MF_STRING, ID_LAYOUT1, L"布局1");//
		AppendMenu(lay, MF_STRING, ID_LAYOUT2, L"布局2");//
		lay1= CreatePopupMenu();//
		AppendMenu(hSysmenu, MF_POPUP, (UINT_PTR)lay1, L"游戏选择");//展示布局选择菜单
		AppendMenu(lay1, MF_STRING, ID_MENU1, L"开始游戏");//
		AppendMenu(lay1, MF_STRING, ID_MENU2, L"结束游戏");//
		SetMenu(hWnd, hSysmenu);//设置选择菜单
		SetMenu(hWnd, diff);//设置加速菜单
		SetMenu(hWnd, lay);//
		SetMenu(hWnd, lay1);//
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);//
            // Parse the menu selections:
		
            switch (wmId)
            {
            case IDM_ABOUT:
				 DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				DestroyWindow(hWnd);
                break;
			case ID_dif1:
			selectDiffculty(hWnd, 1);//选择难度
				break;
			case ID_dif2:
				selectDiffculty(hWnd, 2);//
				break;
			case ID_dif3:
				selectDiffculty(hWnd, 3);//
				break;
			case ID_LAYOUT1:
				selectLayOut(hWnd, 1);//选择布局
				break;
			case ID_LAYOUT2:
				selectLayOut(hWnd, 2);//
				break;
			case ID_ROATE1:
				selectRotate(hWnd, 0);//顺时针
				break;
			case ID_ROATE2:
				selectRotate(hWnd, 1);//逆时针
				break;
			case ID_MENU1:
				GAME_STATE |= 2;
				init_game();
				break;
			case ID_MENU2:
				PostQuitMessage(0);

				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN://键盘消息
		hdc = GetDC(hWnd);
		InvalidateRect(hWnd, NULL, false);
		switch (wParam)
		{
		case VK_LEFT:
			curPosX--;
			if (CheckValide(curPosX, curPosY, bCurTeris) != 1)
			{
				curPosX++;//右移一格
			}
			break;
		case VK_RIGHT:
			curPosX++;
			if (CheckValide(curPosX, curPosY, bCurTeris) != 1)
			{
				curPosX--;//左移一格
			}
			break;
		case VK_UP:
			RotateTeris(bCurTeris);//旋转方块
			break;
		case VK_DOWN://
			if (g_speed == t_speed)
				g_speed = 10;
			else
				g_speed = t_speed;
			//outPutBoxInt(g_speed);
			break;
		case 'W'://同 VK_UP
			RotateTeris(bCurTeris);
			break;
		case 'A'://同 VK_LEFT
			curPosX--;
			if (CheckValide(curPosX, curPosY, bCurTeris) != 1)
			{
				curPosX++;
			}
			break;
		case 'D'://同 VK_RIGHT
			curPosX++;
			if (CheckValide(curPosX, curPosY, bCurTeris) != 1)
			{
				curPosX--;
			}
			break;

		case 'S'://同 VK_DOWN
			if (g_speed == t_speed)
				g_speed = 10;
			else
				g_speed = t_speed;
			//outPutBoxInt(g_speed);
			break;
		default:
			break;
		}

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			DrawBackGround(hdc);//绘制背景
			drawNext(hdc);
			drawScore(hdc);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
		

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
int selectRotate(HWND hWnd, int direction)
{
	GAME_ROTATE=direction;
	return GAME_ROTATE;
}void drawBlocked(HDC mdc)//绘制当前已经存在砖块的区域
{
	int i, j;
	//
	HBRUSH hBrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 0));//选择实心绘制刷

	SelectObject(mdc, hBrush);//选择目标

	for (i = 0; i < NUM_Y; i++)
	{
		for (j = 0; j < NUM_X; j++)
		{
			if (g_hasBlocked[i][j])
			{
				Rectangle(mdc, BORDER_X + j * BLOCK_SIZE, BORDER_Y + i * BLOCK_SIZE,
					BORDER_X + (j + 1)*BLOCK_SIZE, BORDER_Y + (i + 1)*BLOCK_SIZE
				);//绘制方块
			}
		}
	}
	DeleteObject(hBrush);//删除目标
}

int CheckValide(int startX, int startY, BOOL bTemp[4][4])//给定一个矩阵，查看是否合法
{
	int i, j;
	for (i = 3; i >= 0; i--)
	{
		for (j = 3; j >= 0; j--)
		{
			if (bTemp[i][j])
			{
				if (j + startX < 0 || j + startX >= NUM_X)
				{
					return -1;
				}
				if (i + startY >= NUM_Y)
				{
					return -2;
				}
				if (g_hasBlocked[i + startY][j + startX])
				{
					//outPutBoxInt(j+startY);
					if (curPosY == 0)
					{
						return -3;
					}
					return -2;
				}
			}
		}
	}
	//MessageBox(NULL,L"这里",L"as",MB_OK);
	//outPutBoxInt(curPosY);
	return 1;
}

void checkComplite()//查看一行是否能消去
{
	int i, j, k, count = 0;
	for (i = 0; i < NUM_Y; i++)
	{
		bool flag = true;
		for (j = 0; j < NUM_X; j++)
		{
			if (!g_hasBlocked[i][j])
			{
				flag = false;//有一个为空时，不能消去
			}
		}
		if (flag)
		{
			count++;
			for (j = i; j >= 1; j--)//从上往下把每行下移
			{
				for (k = 0; k < NUM_X; k++)
				{
					g_hasBlocked[j][k] = g_hasBlocked[j - 1][k];
				}

			}
			drawCompleteParticle(i);//
			Sleep(300);//暂停300ms

			PlaySound(_T("coin.wav"), NULL, SND_FILENAME | SND_ASYNC);//消去一行时播放系统自带提示音
		}
	}
	GAME_SCORE += int(count*1.5);//单行+1分，两行+3分，三行+4分，四行6分，以此类推
}

VOID outPutBoxInt(int num)//自定义的弹窗函数  用于调试，已经被注释掉了
{
	TCHAR szBuf[1024];
	LPCTSTR str = TEXT("%d");
	wsprintf(szBuf, str, num);//
	MessageBox(NULL, szBuf, L"aasa", MB_OK);//
}

VOID outPutBoxString(TCHAR str[1024])//自定义的弹窗函数  用于调试，已经被注释掉了
{
	TCHAR szBuf[1024];
	LPCTSTR cstr = TEXT("%s");
	wsprintf(szBuf, cstr, str);
	MessageBox(NULL, szBuf, L"aasa", MB_OK);
}



//设置随机方块形状
void setRandomT()
{
	int rd_start = RandomInt(0, sizeof(state_teris) / sizeof(state_teris[0]));
	int rd_next = RandomInt(0, sizeof(state_teris) / sizeof(state_teris[0]));
	//outPutBoxInt(rd_start);
	//outPutBoxInt(rd_next);
	//outPutBoxInt(rd_start);
	if (GAME_STATE == 0)
	{
		GAME_STATE = GAME_STATE | 0x0001;
		//outPutBoxInt(GAME_STATE);
		memcpy(bCurTeris, state_teris[rd_start], sizeof(state_teris[rd_start]));
		memcpy(bNextCurTeris, state_teris[rd_next], sizeof(state_teris[rd_next]));
	}
	else
	{
		memcpy(bCurTeris, bNextCurTeris, sizeof(bNextCurTeris));
		memcpy(bNextCurTeris, state_teris[rd_next], sizeof(state_teris[rd_next]));
	}

}

//游戏初始化
void init_game()
{
	GAME_SCORE = 0;
	setRandomT();//
	curPosX = (NUM_X - 4) >> 1;//设置初始坐标
	curPosY = 0;
	//memset，是计算机语言中的函数。起功能是将s所指向的某一块内存中的每个字节的内容全部设置为ch指定的ASCII值，最后行清零
	memset(g_hasBlocked, 0, sizeof(g_hasBlocked));
	rc_left.left = 0;
	rc_left.right = SCREEN_LEFT_X;
	rc_left.top = 0;
	rc_left.bottom = SCREEN_Y;

	rc_right.left = rc_left.right + BORDER_X;
	rc_right.right = 180 + rc_right.left;
	rc_right.top = 0;
	rc_right.bottom = SCREEN_Y;

	rc_main.left = 0;
	rc_main.right = SCREEN_X;
	rc_main.top = 0;
	rc_main.bottom = SCREEN_Y;

	rc_right_top.left = rc_right.left;
	rc_right_top.top = rc_right.top;
	rc_right_top.right = rc_right.right;
	rc_right_top.bottom = (rc_right.bottom) / 2;

	rc_right_bottom.left = rc_right.left;
	rc_right_bottom.top = rc_right_top.bottom + BORDER_Y;
	rc_right_bottom.right = rc_right.right;
	rc_right_bottom.bottom = rc_right.bottom;

	g_speed = t_speed = 1000 - GAME_DIFF * 280;
}

void fillBlock()//到达底部后填充矩阵
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (bCurTeris[i][j])
			{
				g_hasBlocked[curPosY + i][curPosX + j] = 1;
			}
		}
	}
}

void RotateTeris(BOOL bTeris[4][4])
{
	BOOL bNewTeris[4][4];
	int x, y;
	for (x = 0; x < 4; x++)
	{
		for (y = 0; y < 4; y++)
		{//旋转角度
			if (GAME_ROTATE == 0)
			bNewTeris[x][y] = bTeris[3 - y][x];
			//逆时针：
			bNewTeris[x][y] = bTeris[y][3 - x];
		}
	}
	if (CheckValide(curPosX, curPosY, bNewTeris) == 1)
	{
		memcpy(bTeris, bNewTeris, sizeof(bNewTeris));//成功则将变换后的形状保存
	}

}

int RandomInt(int _min, int _max)
{
	srand((rd_seed++) % 65532 + GetTickCount());
	return _min + rand() % (_max - _min);
}

VOID DrawTeris(HDC mdc)
{

	int i, j;
	HPEN hPen = (HPEN)GetStockObject(BLACK_PEN);//
	HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	SelectObject(mdc, hPen);//SelectObject是计算机编程语言函数，该函数选择一对象到指定的设备上下文环境中，新对象替换先前的相同类型的对象
	SelectObject(mdc, hBrush);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (bCurTeris[i][j])
			{
				Rectangle(mdc, (j + curPosX)*BLOCK_SIZE + BORDER_X, (i + curPosY)*BLOCK_SIZE + BORDER_Y, (j + 1 + curPosX)*BLOCK_SIZE + BORDER_X, (i + 1 + curPosY)*BLOCK_SIZE + BORDER_Y);
			}
		}
	}
	drawBlocked(mdc);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

VOID DrawBackGround(HDC hdc)//绘制背景
{

	HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
	//适用于支持光栅操作的设备，应用程序可以通过调用GetDeviceCaps函数来确定一个设备是否支持这些操作
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, SCREEN_X, SCREEN_Y);

	SelectObject(mdc, hBrush);
	SelectObject(mdc, hBitmap);

	HBRUSH hBrush2 = (HBRUSH)GetStockObject(WHITE_BRUSH);
	FillRect(mdc, &rc_main, hBrush2);
	Rectangle(mdc, rc_left.left + BORDER_X, rc_left.top + BORDER_Y, rc_left.right, rc_left.bottom);
	Rectangle(mdc, rc_right.left + BORDER_X, rc_right.top + BORDER_Y, rc_right.right, rc_right.bottom);
	DrawTeris(mdc);
	drawNext(mdc);
	drawScore(mdc);
	::BitBlt(hdc, 0, 0, SCREEN_X, SCREEN_Y, mdc, 0, 0, SRCCOPY);
	DeleteObject(hBrush);
	DeleteDC(mdc);
	DeleteObject(hBitmap);
	DeleteObject(hBrush2);


	//  int x,y;
	//  HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
	//  HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
	//  SelectObject(hdc,hPen);
	//  SelectObject(hdc,hBrush);
	//  for (x = 0;x<NUM_X;x++)
	//  {
	//      for(y=0;y<NUM_Y;y++)
	//      {
	//          Rectangle(hdc,BORDER_X+x*BLOCK_SIZE,BORDER_Y+y*BLOCK_SIZE,
	//              BORDER_X+(x+1)*BLOCK_SIZE,
	//              BORDER_Y+(y+1)*BLOCK_SIZE);
	//      }
	//  }
}

void drawNext(HDC hdc)//绘制下一个将要掉落的方块
{
	int i, j;
	HBRUSH hBrush = (HBRUSH)CreateSolidBrush(RGB(188, 0, 0));
	HPEN hPen = (HPEN)CreatePen(PS_SOLID, 2, RGB(0, 0, 233));
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (bNextCurTeris[i][j])
			{
				Rectangle(hdc, rc_right_top.left + BLOCK_SIZE * (j + 1), rc_right_top.top + BLOCK_SIZE * (i + 1), rc_right_top.left + BLOCK_SIZE * (j + 2), rc_right_top.top + BLOCK_SIZE * (i + 2));
			}
		}
	}
	HFONT hFont = CreateFont(30, 0, 0, 0, FW_THIN, 0, 0, 0, UNICODE, 0, 0, 0, 0, L"微软雅黑");
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetBkColor(hdc, RGB(255, 255, 0));
	SetTextColor(hdc, RGB(152, 206, 0));
	RECT rect;
	rect.left = rc_right_top.left + 40;
	rect.top = rc_right_top.bottom - 150;
	rect.right = rc_right_top.right;
	rect.bottom = rc_right_top.bottom;
	DrawTextW(hdc, TEXT("下一个"), _tcslen(TEXT("下一个")), &rect, 0);
	DeleteObject(hFont);
	DeleteObject(hBrush);
}

void drawScore(HDC hdc)
{
	HFONT hFont = CreateFont(30, 0, 0, 0, FW_THIN, 0, 0, 0, UNICODE, 0, 0, 0, 0, L"微软雅黑");
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetBkColor(hdc, RGB(255, 255, 0));
	SetTextColor(hdc, RGB(0, 200, 150));
	RECT rect;
	rect.left = rc_right_bottom.left;
	rect.top = rc_right_bottom.top;
	rect.right = rc_right_bottom.right;
	rect.bottom = rc_right_bottom.bottom;
	TCHAR szBuf[30];
	LPCTSTR cstr = TEXT("当前难度：%d");
	wsprintf(szBuf, cstr, GAME_DIFF);
	DrawTextW(hdc, szBuf, _tcslen(szBuf), &rect, DT_CENTER | DT_VCENTER);

	RECT rect2;
	rect2.left = rc_right_bottom.left;
	rect2.top = rc_right_bottom.bottom / 2 + 100;
	rect2.right = rc_right_bottom.right;
	rect2.bottom = rc_right_bottom.bottom;
	TCHAR szBuf2[30];
	LPCTSTR cstr2 = TEXT("得分：%d");
	wsprintf(szBuf2, cstr2, GAME_SCORE);
	//outPutBoxInt(sizeof(szBuf));
	SetTextColor(hdc, RGB(15, 0, 150));
	DrawTextW(hdc, szBuf2, _tcslen(szBuf2), &rect2, DT_CENTER | DT_VCENTER);

	DeleteObject(hFont);
}

int selectDiffculty(HWND hWnd, int diff)
{
	TCHAR szBuf2[30];
	LPCTSTR cstr2 = TEXT("确认选择难度 %d 吗？");
	wsprintf(szBuf2, cstr2, diff);
	if (MessageBox(hWnd, szBuf2, L"难度选择", MB_YESNO) == IDYES)
	{
		GAME_DIFF = diff;
		InvalidateRect(hWnd, &rc_right_bottom, false);
		GAME_STATE |= 2;
		init_game();
		return GAME_DIFF;
	}
	return -1;
}

int selectLayOut(HWND hWnd, int layout)//选择布局
{
	NUM_X = 10 * layout;
	NUM_Y = 20 * layout;
	BLOCK_SIZE = 30 / layout;
	GAME_STATE |= 2;
	InvalidateRect(hWnd, &rc_right_bottom, false);
	init_game();
	return layout;
}

void drawCompleteParticle(int line)
{
	HWND hWnd = GetActiveWindow();
	HDC hdc = GetDC(hWnd);
	HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
	HPEN hPen = (HPEN)CreatePen(PS_DOT, 1, RGB(255, 255, 0));
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	Rectangle(hdc, BORDER_X,
		BORDER_Y + line * BLOCK_SIZE,
		BORDER_X + NUM_X * BLOCK_SIZE,
		BORDER_Y + BLOCK_SIZE * (1 + line));
	DeleteObject(hBrush);
	DeleteObject(hPen);
}