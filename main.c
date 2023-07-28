#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

//MENU
#define MENU_GAME_8X8 0x08080A
#define MENU_GAME_9X9 0x09090A
#define MENU_GAME_16X16 0x101028
#define MENU_GAME_30X16 0x1E1063

//GRID
#define GRID_WIDTH 30
#define GRID_HEIGHT 16

//GUI
#define CELL_SIZE 30

//Cell flags
#define FLAG_OPEN 0x80
#define FLAG_FLAG 0x40
#define FLAG_MINE 0x20
#define FLAG_COUNT 0x0F

//Reset button
#define RESET 9999

//HANDLES
HBITMAP images[16];
HMENU hMenu;
HWND ResetButton;
byte cells[GRID_WIDTH*GRID_HEIGHT];
int currentMode, unOpenCell;
int mines[99];
boolean playing;

//Prototypes
void AddMenus(HWND hwnd);
void AddControls(HWND hwnd);
void SetMode(HWND hwnd, WPARAM mode);
void LoadImages();
int ArrayContains(int a[], int array_count, int n);
int MineValueToBITMAPIndex(byte value);
void OpenCell(int x, int y, HWND hwnd);
void OnLose(HWND hwnd);
void OnWin(HWND hwnd);
void OnLeftClick(HWND hwnd, LPARAM lParam);
void OnRightClick(HWND hwnd, LPARAM lParam);

const char g_szClassName[] = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_COMMAND: {
            switch(wParam) {
                case MENU_GAME_8X8: {
                    SetMode(hwnd,wParam);
                    break;
                }
                case MENU_GAME_9X9: {
                    SetMode(hwnd,wParam);
                    break;
                }
                case MENU_GAME_16X16: {
                    SetMode(hwnd,wParam);
                    break;
                }
                case MENU_GAME_30X16: {
                    SetMode(hwnd,wParam);
                    break;
                }
                case RESET: {
                    SetMode(hwnd,currentMode);
                    break;
                }
            }
            break;
        }
        case WM_RBUTTONUP: {
            OnRightClick(hwnd, lParam);
            break;
        }
        case WM_LBUTTONUP: {
            OnLeftClick(hwnd, lParam);
            break;
        }
        case WM_CREATE: {
            LoadImages();
            AddMenus(hwnd);
            AddControls(hwnd);
            break;
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY: {
            FreeConsole();
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Minesweeper Copy",
        WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280,
        720,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    FreeConsole();

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetMode(hwnd, MENU_GAME_8X8);

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

void AddMenus(HWND hwnd) {
    hMenu = CreateMenu();
    HMENU hGameMenu = CreateMenu();
    HMENU hSubMenu = CreateMenu();

    AppendMenu(hSubMenu,MF_STRING,0,"Submenu Item");

    AppendMenu(hGameMenu,MF_STRING,MENU_GAME_8X8,"8x8");
    AppendMenu(hGameMenu,MF_STRING,MENU_GAME_9X9,"9x9");
    AppendMenu(hGameMenu,MF_STRING,MENU_GAME_16X16,"16x16");
    AppendMenu(hGameMenu,MF_STRING,MENU_GAME_30X16,"30x16");

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hGameMenu,"Game");

    SetMenu(hwnd,hMenu);
}

void AddControls(HWND hwnd) {
    for (WPARAM i = 0; i < GRID_HEIGHT; i++) {
        for (WPARAM j = 0; j < GRID_WIDTH; j++) {
            WPARAM id = i*GRID_WIDTH + j;
            HWND handle = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, CELL_SIZE*j, 80 + CELL_SIZE*i, CELL_SIZE, CELL_SIZE, hwnd, (HMENU)id, NULL, NULL);
            SendMessageW(handle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[11]);
        }
    }
    ResetButton = CreateWindowW(L"button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, 20, 20, 60, 60, hwnd, (HMENU)RESET, NULL, NULL);
    SendMessageW(ResetButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[13]);
}

void SetMode(HWND hwnd, WPARAM mode) {
    playing = TRUE;
    //Reset grid
    for (int i = 0; i < GRID_HEIGHT*GRID_WIDTH; i++) {
        cells[i] = 0;
        SendMessageW(GetDlgItem(hwnd, i), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[MineValueToBITMAPIndex(cells[i])]);
    }
    //Create new mines
    int mine_count = (mode & 0x0000FF);
    int width = (mode & 0xFF0000) >> 16;
    int height = (mode & 0x00FF00) >> 8;
    currentMode = mode;
    unOpenCell = width*height;
    for (int i = 0; i < mine_count; i++) {
        int y = (rand()%height);
        int x = (rand()%width);
        while (ArrayContains(mines, i+1, y*GRID_WIDTH+x)){
            y = (rand()%height);
            x = (rand()%width);
        }
        mines[i] = y * GRID_WIDTH + x;
        cells[y*GRID_WIDTH+x]  = cells[y*GRID_WIDTH+x] | FLAG_MINE;
        //
        if (y+1 < height) {cells[(y+1) * GRID_WIDTH + x] += 1;}
        if (y-1 > -1) {cells[(y-1) * GRID_WIDTH + x] += 1;}
        if (x + 1 < width) {
            cells[y * GRID_WIDTH + x + 1] += 1;
            if (y+1 < height) {cells[(y+1) * GRID_WIDTH + x + 1] += 1;}
            if (y-1 > -1) {cells[(y-1) * GRID_WIDTH + x + 1] += 1;}
        }
        if (x - 1 > -1) {
            cells[y * GRID_WIDTH + x - 1] += 1;
            if (y+1 < height) {cells[(y+1) * GRID_WIDTH + x - 1] += 1;}
            if (y-1 > -1) {cells[(y-1) * GRID_WIDTH + x - 1] += 1;}
        }
        //
    }
    RECT wr = {0,0, width*CELL_SIZE,height*CELL_SIZE + 80};
    AdjustWindowRect(&wr, WS_MINIMIZEBOX | WS_SYSMENU, TRUE);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, wr.right - wr.left + GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXEDGE) , wr.bottom - wr.top + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE | SWP_SHOWWINDOW);
    SetWindowPos(ResetButton, HWND_TOP, (wr.right - wr.left)/2 - 30, 10, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
    SendMessageW(ResetButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[13]);
}

void LoadImages() {
    images[0] = (HBITMAP)LoadImageW(NULL, L"bitmaps/zero.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[1] = (HBITMAP)LoadImageW(NULL, L"bitmaps/one.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[2] = (HBITMAP)LoadImageW(NULL, L"bitmaps/two.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[3] = (HBITMAP)LoadImageW(NULL, L"bitmaps/three.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[4] = (HBITMAP)LoadImageW(NULL, L"bitmaps/four.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[5] = (HBITMAP)LoadImageW(NULL, L"bitmaps/five.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[6] = (HBITMAP)LoadImageW(NULL, L"bitmaps/six.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[7] = (HBITMAP)LoadImageW(NULL, L"bitmaps/seven.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[8] = (HBITMAP)LoadImageW(NULL, L"bitmaps/eight.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[9] = (HBITMAP)LoadImageW(NULL, L"bitmaps/mine.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[10] = (HBITMAP)LoadImageW(NULL, L"bitmaps/flag.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[11] = (HBITMAP)LoadImageW(NULL, L"bitmaps/close.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[12] = (HBITMAP)LoadImageW(NULL, L"bitmaps/redmine.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[13] = (HBITMAP)LoadImageW(NULL, L"bitmaps/default.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[14] = (HBITMAP)LoadImageW(NULL, L"bitmaps/win.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
    images[15] = (HBITMAP)LoadImageW(NULL, L"bitmaps/lose.bmp", IMAGE_BITMAP, CELL_SIZE, CELL_SIZE, LR_LOADFROMFILE);
}

int ArrayContains(int a[], int array_count,int n) {
    if (array_count == 1) {return 0;}
    for (int i = 0; i <= array_count; i++) {
        if (a[i] == n) {
            return 1;
        }
    }
    return 0;
}

int MineValueToBITMAPIndex(byte value) {
    if ((value & FLAG_FLAG)) {
        return 10;
    } 
    if (!(value & FLAG_OPEN)) {
        return 11;
    }
    if ((value & FLAG_MINE)) {
        return 9;
    }
    return value & FLAG_COUNT;
}

void OnRightClick(HWND hwnd, LPARAM lParam) {
    if (playing) {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        if (yPos >= 80) {
            int id = ((yPos-80)/CELL_SIZE) * GRID_WIDTH + (xPos/CELL_SIZE);
            if (!(cells[id]&FLAG_OPEN)) {
                cells[id] = (cells[id]&~FLAG_FLAG) | (((cells[id]&FLAG_FLAG)^FLAG_FLAG)&FLAG_FLAG);
                SendMessageW(GetDlgItem(hwnd, id), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[MineValueToBITMAPIndex(cells[id])]);
            }
        }
    }
}

void OnLeftClick(HWND hwnd, LPARAM lParam) {
    if (playing) {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        if (yPos >= 80) {
            OpenCell(xPos/CELL_SIZE,(yPos-80)/CELL_SIZE,hwnd);
        }
    }
}

void OpenCell(int x, int y, HWND hwnd) {
    int mine_count = (currentMode & 0x0000FF);
    int width = (currentMode & 0xFF0000) >> 16;
    int height = (currentMode & 0x00FF00) >> 8;
    if (x > -1 && x < width && y > -1 && y < height) {
        int id = y*GRID_WIDTH+x;
        if (!(cells[id]&FLAG_OPEN) && !(cells[id]&FLAG_FLAG)) {
            if ((cells[id]&FLAG_MINE)) {
                OnLose(hwnd);
                SendMessageW(GetDlgItem(hwnd, id), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[12]);
            } else if (playing) {
                cells[id] = cells[id] | FLAG_OPEN;
                SendMessageW(GetDlgItem(hwnd, id), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[MineValueToBITMAPIndex(cells[id])]);
                unOpenCell -= 1;
                if (unOpenCell==mine_count) {
                    OnWin(hwnd);
                }
                if (MineValueToBITMAPIndex(cells[id])==0) {
                    OpenCell(x+1, y, hwnd);
                    OpenCell(x-1, y, hwnd);
                    OpenCell(x, y+1, hwnd);
                    OpenCell(x+1, y+1, hwnd);
                    OpenCell(x-1, y+1, hwnd);
                    OpenCell(x, y-1, hwnd);
                    OpenCell(x+1, y-1, hwnd);
                    OpenCell(x-1, y-1, hwnd);
                }
            }
        }
    }
}

void OnLose(HWND hwnd) {
    playing = FALSE;
    int mine_count = (currentMode & 0x0000FF);
    SendMessageW(ResetButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[15]);
    for (int i = 0; i < mine_count; i++) {
        SendMessageW(GetDlgItem(hwnd, mines[i]), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[9]);
    }
}

void OnWin(HWND hwnd) {
    playing = FALSE;
    SendMessageW(ResetButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)images[14]);
}
