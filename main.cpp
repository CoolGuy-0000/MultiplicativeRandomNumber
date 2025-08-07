#include <windows.h>
#include <stdint.h>
#include <tchar.h>

uint64_t g_mBase = 9;
uint64_t g_mM = 7;
uint64_t g_mStartNum = 1234;
uint64_t g_mLimit = 0;
uint64_t g_Divisor = 81;
COLORREF* g_colors = NULL;
uint32_t g_randomSeed = 0; // 랜덤 시드 추가

// 윈도우 컨트롤 ID
#define IDC_EDIT_MBASE 100
#define IDC_EDIT_MM 101
#define IDC_EDIT_STARTNUM 102
#define IDC_BUTTON_CALCULATE 103
#define IDC_EDIT_MLIMIT 104
#define IDC_BUTTON_APPLY_MBASE 105
#define IDC_LISTBOX_COLORS 107
#define IDC_EDIT_DIVISOR 108
#define IDC_EDIT_RANDOM_SEED 109 
#define IDC_BUTTON_CHANGE_COLOR 110 
#define IDC_EDIT_RED 111
#define IDC_EDIT_GREEN 112
#define IDC_EDIT_BLUE 113
#define IDC_BUTTON_APPLY_COLOR 114
#define IDC_BUTTON_SAVE 115

// 윈도우 클래스 이름
const TCHAR g_szClassName[] = TEXT("MyWindowClass");

HBITMAP g_hBitmap = NULL;
COLORREF* g_pixelData = NULL;
BOOL g_bDataReady = FALSE;
int64_t g_bitmapWidth = 320;
int64_t g_bitmapHeight = 320;
int g_selectedColorIndex = -1; // 현재 선택된 색상의 인덱스

HINSTANCE g_hMyInstance;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateBitmapAndMemory();
void GenerateUniqueRandomColors(uint64_t count);
void UpdateColorListBox(HWND hwnd);
void UpdateMBase(uint64_t new_mBase);
void ChangeColors(HWND hwnd);
void ApplyColorChanges(HWND hwnd);
bool SaveBitmapToFile(HWND hwnd, LPCTSTR filename);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    g_hMyInstance = hInstance;
    
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))return 0;

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        TEXT("곱셈 주기성 연구"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static HWND hEditRed, hEditGreen, hEditBlue;

    switch (msg) {
    case WM_CREATE: {
        // HBITMAP 및 픽셀 메모리 생성
        CreateBitmapAndMemory();

        CreateWindow(TEXT("STATIC"), TEXT("M_BASE:"), WS_VISIBLE | WS_CHILD, 10, 10, 60, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("9"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 80, 10, 50, 20, hwnd, (HMENU)IDC_EDIT_MBASE, g_hMyInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("APPLY"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, 10, 50, 20, hwnd, (HMENU)IDC_BUTTON_APPLY_MBASE, g_hMyInstance, NULL);


        CreateWindow(TEXT("STATIC"), TEXT("M_M:"), WS_VISIBLE | WS_CHILD, 140, 10, 50, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("7"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 190, 10, 50, 20, hwnd, (HMENU)IDC_EDIT_MM, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("Start Num:"), WS_VISIBLE | WS_CHILD, 10, 40, 70, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("1"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 80, 40, 100, 20, hwnd, (HMENU)IDC_EDIT_STARTNUM, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("M_LIMIT:"), WS_VISIBLE | WS_CHILD, 190, 40, 70, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("729"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 270, 40, 100, 20, hwnd, (HMENU)IDC_EDIT_MLIMIT, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("Divisor:"), WS_VISIBLE | WS_CHILD, 190, 70, 70, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("81"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 270, 70, 100, 20, hwnd, (HMENU)IDC_EDIT_DIVISOR, g_hMyInstance, NULL);

        // "random color seed" editbox 추가
        CreateWindow(TEXT("STATIC"), TEXT("Seed:"), WS_VISIBLE | WS_CHILD, 350, 120, 80, 20, hwnd, NULL, NULL, NULL);
        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 420, 120, 80, 20, hwnd, (HMENU)IDC_EDIT_RANDOM_SEED, g_hMyInstance, NULL);

        // "change color" button 추가
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("Change Colors"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 350, 150, 100, 30, hwnd, (HMENU)IDC_BUTTON_CHANGE_COLOR, g_hMyInstance, NULL);

        // 버튼
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("계산 및 그리기"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 70, 150, 30, hwnd, (HMENU)IDC_BUTTON_CALCULATE, g_hMyInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY, g_bitmapWidth + 200, 10, 200, g_bitmapHeight + 50, hwnd, (HMENU)IDC_LISTBOX_COLORS, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("Red:"), WS_VISIBLE | WS_CHILD, g_bitmapWidth + 200, g_bitmapHeight + 70, 30, 20, hwnd, NULL, NULL, NULL);
        hEditRed = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, g_bitmapWidth + 240, g_bitmapHeight + 70, 50, 20, hwnd, (HMENU)IDC_EDIT_RED, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("Green:"), WS_VISIBLE | WS_CHILD, g_bitmapWidth + 200, g_bitmapHeight + 100, 30, 20, hwnd, NULL, NULL, NULL);
        hEditGreen = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, g_bitmapWidth + 240, g_bitmapHeight + 100, 50, 20, hwnd, (HMENU)IDC_EDIT_GREEN, g_hMyInstance, NULL);

        CreateWindow(TEXT("STATIC"), TEXT("Blue:"), WS_VISIBLE | WS_CHILD, g_bitmapWidth + 200, g_bitmapHeight + 130, 30, 20, hwnd, NULL, NULL, NULL);
        hEditBlue = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, g_bitmapWidth + 240, g_bitmapHeight + 130, 50, 20, hwnd, (HMENU)IDC_EDIT_BLUE, g_hMyInstance, NULL);

        // "색상 적용" 버튼 추가
        CreateWindowEx(0, TEXT("BUTTON"), TEXT("색상 적용"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, g_bitmapWidth + 200, g_bitmapHeight + 160, 100, 30, hwnd, (HMENU)IDC_BUTTON_APPLY_COLOR, g_hMyInstance, NULL);

        CreateWindowEx(0, TEXT("BUTTON"), TEXT("저장"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, g_bitmapWidth + 200, g_bitmapHeight + 200, 150, 30, hwnd, (HMENU)IDC_BUTTON_SAVE, g_hMyInstance, NULL);

        UpdateMBase(g_mBase);

        break;
    }

    case WM_COMMAND: {
        // 버튼 클릭 처리
        if (LOWORD(wParam) == IDC_BUTTON_APPLY_MBASE) {
            TCHAR buffer[256];
            GetWindowText(GetDlgItem(hwnd, IDC_EDIT_MBASE), buffer, sizeof(buffer));

            UpdateMBase(_ttoi(buffer));
            UpdateColorListBox(hwnd);
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CALCULATE) {
            TCHAR buffer[256];

            GetWindowText(GetDlgItem(hwnd, IDC_EDIT_MM), buffer, sizeof(buffer));
            g_mM = _ttoi64(buffer);

            GetWindowText(GetDlgItem(hwnd, IDC_EDIT_STARTNUM), buffer, sizeof(buffer));
            g_mStartNum = _ttoi64(buffer);

            GetWindowText(GetDlgItem(hwnd, IDC_EDIT_MLIMIT), buffer, sizeof(buffer));
            g_mLimit = _ttoi64(buffer);
            if (g_mLimit <= 0) {
                MessageBox(hwnd, TEXT("M_LIMIT must be a positive integer greater than 0"), TEXT("Input Error"), MB_ICONWARNING | MB_OK);
                break;
            }

            GetWindowText(GetDlgItem(hwnd, IDC_EDIT_DIVISOR), buffer, sizeof(buffer));
            g_Divisor = _ttoi64(buffer);
            if (g_Divisor == 0) {
                MessageBox(hwnd, TEXT("Divisor must be a positive integer greater than 0"), TEXT("Input Error"), MB_ICONWARNING | MB_OK);
                break;
            }
            

            uint64_t n = g_mStartNum;
            uint64_t z = 0;

            uint64_t totalPixels = g_bitmapWidth * g_bitmapHeight;
            for (uint64_t i = 0; i < totalPixels; i++) {
                z = (n / g_Divisor) % g_mBase;
                g_pixelData[i] = RGB(GetBValue(g_colors[z]), GetGValue(g_colors[z]), GetRValue(g_colors[z]));
                n *= g_mM;
                n %= g_mLimit;
            }

            g_bDataReady = TRUE;
            UpdateColorListBox(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CHANGE_COLOR) {
            ChangeColors(hwnd);
            InvalidateRect(hwnd, NULL, FALSE); // 윈도우 다시 그리기
        }
        else if (LOWORD(wParam) == IDC_BUTTON_APPLY_COLOR) {
            ApplyColorChanges(hwnd);
            UpdateColorListBox(hwnd); // 색상 목록 갱신
            InvalidateRect(hwnd, NULL, FALSE); // 윈도우 다시 그리기
        }
        // ListBox 선택 이벤트 처리
        else if (HIWORD(wParam) == LBN_SELCHANGE && LOWORD(wParam) == IDC_LISTBOX_COLORS) {
            int selectedIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0);
            if (selectedIndex != LB_ERR) {
                g_selectedColorIndex = selectedIndex;
                COLORREF color = g_colors[selectedIndex];

                // Edit 컨트롤에 색상 값 표시
                TCHAR buffer[256];
                _stprintf_s(buffer, 256, TEXT("%d"), GetRValue(color));
                SetWindowText(GetDlgItem(hwnd, IDC_EDIT_RED), buffer);

                _stprintf_s(buffer, 256, TEXT("%d"), GetGValue(color));
                SetWindowText(GetDlgItem(hwnd, IDC_EDIT_GREEN), buffer);

                _stprintf_s(buffer, 256, TEXT("%d"), GetBValue(color));
                SetWindowText(GetDlgItem(hwnd, IDC_EDIT_BLUE), buffer);
            }
        }
        else if (LOWORD(wParam) == IDC_BUTTON_SAVE) {
            TCHAR filename[MAX_PATH] = { 0 };
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = sizeof(filename) / sizeof(TCHAR);
            ofn.lpstrFilter = TEXT("BMP Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (GetSaveFileName(&ofn)) {
                // 파일 확장자에 따라 저장 함수 호출
                if (_tcsstr(filename, TEXT(".bmp"))) {
                    if (!SaveBitmapToFile(hwnd, filename)) {
                        MessageBox(hwnd, TEXT("BMP 파일 저장 실패!"), TEXT("오류"), MB_OK | MB_ICONERROR);
                    }
                }
            }
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (g_bDataReady) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hBitmap);
            BitBlt(hdc, 20, 120, g_bitmapWidth, g_bitmapHeight, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            DeleteDC(hdcMem);
        }

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY: {
        if (g_hBitmap) {
            DeleteObject(g_hBitmap);
        }
        PostQuitMessage(0);
        break;
    }

    default: {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
    return 0;
}

bool SaveBitmapToFile(HWND hwnd, LPCTSTR filename) {
    HDC hdc = NULL;
    HDC hdcMem = NULL;
    HBITMAP hBitmap = NULL;
    BITMAPINFO bmi;
    BYTE* pPixelData = NULL;
    bool bSuccess = false;

    hdc = GetDC(hwnd);
    if (!hdc) return false;

    hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem) {
        ReleaseDC(hwnd, hdc);
        return false;
    }

    // BITMAPINFO 설정
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_bitmapWidth;
    bmi.bmiHeader.biHeight = -g_bitmapHeight; // 상하 반전
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = g_bitmapWidth * g_bitmapHeight * 4;

    // DIB Section 생성
    hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pPixelData, NULL, 0);
    if (!hBitmap) {
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdc);
        return false;
    }

    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // 메모리 DC에 비트맵 복사
    if (!BitBlt(hdcMem, 0, 0, g_bitmapWidth, g_bitmapHeight, GetDC(hwnd), 20, 120, SRCCOPY)) { // Y 좌표 변경
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdc);
        return false;
    }

    SelectObject(hdcMem, hbmOld);

    // BMP 파일 헤더 생성
    BITMAPFILEHEADER bmfHeader;
    memset(&bmfHeader, 0, sizeof(BITMAPFILEHEADER));
    bmfHeader.bfType = 0x4D42; // "BM"
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + bmi.bmiHeader.biSizeImage;

    // 파일 쓰기
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdc);
        return false;
    }

    DWORD dwBytesWritten = 0;
    bSuccess = WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    if (bSuccess) {
        bSuccess = WriteFile(hFile, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        if (bSuccess) {
            DWORD imageSize = bmi.bmiHeader.biSizeImage;
            bSuccess = WriteFile(hFile, pPixelData, imageSize, &dwBytesWritten, NULL);
        }
    }

    // 정리
    CloseHandle(hFile);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);

    return bSuccess;
}

void ChangeColors(HWND hwnd) {
    TCHAR buffer[256];
    GetWindowText(GetDlgItem(hwnd, IDC_EDIT_RANDOM_SEED), buffer, sizeof(buffer));
    g_randomSeed = _ttoi(buffer);

    srand(g_randomSeed); // 랜덤 시드 설정
    GenerateUniqueRandomColors(g_mBase);
    UpdateColorListBox(hwnd); // 색상 목록 갱신
}

void UpdateMBase(uint64_t new_mBase) {
    if (new_mBase <= 1 || new_mBase > 100) {
        return;
    }
    
    if (g_colors == NULL)
        g_colors = (COLORREF*)malloc(sizeof(COLORREF) * g_mBase);

    if (new_mBase != g_mBase) {
        if (g_colors) {
            free(g_colors);
        }
        g_mBase = new_mBase;
        g_colors = (COLORREF*)malloc(sizeof(COLORREF) * g_mBase);
    }

    GenerateUniqueRandomColors(g_mBase);
}

void CreateBitmapAndMemory() {
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_bitmapWidth;
    bmi.bmiHeader.biHeight = -g_bitmapHeight; // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;            // 32-bit color
    bmi.bmiHeader.biCompression = BI_RGB;

    g_hBitmap = CreateDIBSection(
        NULL,
        &bmi,
        DIB_RGB_COLORS,
        (void**)&g_pixelData,
        NULL,
        0
    );
}

void GenerateUniqueRandomColors(uint64_t count) {
    if (g_colors == NULL || count <= 0) {
        return;
    }

    const uint64_t MIN_DIST_SQUARED = 3000; // 색상 간의 최소 유클리드 거리 (제곱)

    for (int i = 0; i < count; i++) {
        COLORREF newColor;
        BOOL isUnique;

        do {
            isUnique = TRUE;
            // 랜덤 RGB 값 생성
            newColor = RGB(rand() % 256, rand() % 256, rand() % 256);

            // 기존 색상들과의 유사성 체크
            for (int j = 0; j < i; j++) {
                int r_dist = GetRValue(newColor) - GetRValue(g_colors[j]);
                int g_dist = GetGValue(newColor) - GetGValue(g_colors[j]);
                int b_dist = GetBValue(newColor) - GetBValue(g_colors[j]);

                if ((r_dist * r_dist + g_dist * g_dist + b_dist * b_dist) < MIN_DIST_SQUARED) {
                    isUnique = FALSE;
                    break;
                }
            }
        } while (!isUnique);

        g_colors[i] = newColor;
    }
}

void ApplyColorChanges(HWND hwnd) {
    if (g_selectedColorIndex != -1) {
        TCHAR buffer[256];
        int red, green, blue;

        GetWindowText(GetDlgItem(hwnd, IDC_EDIT_RED), buffer, sizeof(buffer));
        red = _ttoi(buffer);
        GetWindowText(GetDlgItem(hwnd, IDC_EDIT_GREEN), buffer, sizeof(buffer));
        green = _ttoi(buffer);
        GetWindowText(GetDlgItem(hwnd, IDC_EDIT_BLUE), buffer, sizeof(buffer));
        blue = _ttoi(buffer);

        // 유효성 검사
        if (red >= 0 && red <= 255 && green >= 0 && green <= 255 && blue >= 0 && blue <= 255) {
            g_colors[g_selectedColorIndex] = RGB(red, green, blue);
        }
        else {
            MessageBox(hwnd, TEXT("Invalid color value (0-255)"), TEXT("Error"), MB_OK);
        }
    }
}

void UpdateColorListBox(HWND hwnd) {
    HWND hListBox = GetDlgItem(hwnd, IDC_LISTBOX_COLORS);
    SendMessage(hListBox, LB_RESETCONTENT, 0, 0); // 기존 항목 제거

    TCHAR buffer[256];
    for (int i = 0; i < g_mBase; ++i) {
        COLORREF color = g_colors[i];
        _stprintf_s(buffer, 256, TEXT("Color %d: (%d, %d, %d)"), i, GetRValue(color), GetGValue(color), GetBValue(color));
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
    }
}