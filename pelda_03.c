#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#pragma comment(lib, "d2d1")
#include <time.h>

#define KEPERNYO_WIDTH 1920
#define KEPERNYO_HEIGHT 1000
D2D1_RECT_U terulet;
ID2D1Bitmap* memkeptarolo = NULL;
unsigned int kepadat[KEPERNYO_WIDTH * KEPERNYO_HEIGHT];
float zbuffer[KEPERNYO_WIDTH * KEPERNYO_HEIGHT];

typedef struct Vec3f {
	float x, y, z;
};

ID2D1Factory* pD2DFactory = NULL;
ID2D1HwndRenderTarget* pRT = NULL;

#define HIBA_00 TEXT("Error:Program initialisation process.")
HINSTANCE hInstGlob;
int SajatiCmdShow;
char szClassName[] = "WindowsApp";
HWND Form1; //Ablak kezeloje

LRESULT CALLBACK WndProc0(HWND, UINT, WPARAM, LPARAM);
void D2D_rajzolas(ID2D1HwndRenderTarget* pRT);
void create_main_buffer(void);
void cleanup_main_buffer(void);
void swap_main_buffer(void);
int getrandom(int maxnum);
void CleanUp_Zbuffer(void);
void SetPixel_Zbuffer(int x1, int y1, int z1, int szin);
void DrawLine_Zbuffer(int x0, int y0, int z0, int x1, int y1, int z1, int szin);
void FillTriangle_Zbuffer(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int szin);

//*********************************
//A windows program belépési pontja
//*********************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("StdWinClassName");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass0;
	SajatiCmdShow = iCmdShow;
	hInstGlob = hInstance;

	//*********************************
	//Ablak osztálypéldány elokészítése
	//*********************************
	wndclass0.style = CS_HREDRAW | CS_VREDRAW;
	wndclass0.lpfnWndProc = WndProc0;
	wndclass0.cbClsExtra = 0;
	wndclass0.cbWndExtra = 0;
	wndclass0.hInstance = hInstance;
	wndclass0.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass0.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass0.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass0.lpszMenuName = NULL;
	wndclass0.lpszClassName = TEXT("WIN0");

	//*********************************
	//Ablak osztálypéldány regisztrációja
	//*********************************
	if (!RegisterClass(&wndclass0))
	{
		MessageBox(NULL, HIBA_00, TEXT("Program Start"), MB_ICONERROR);
		return 0;
	}

	//*********************************
	//Ablak létrehozása
	//*********************************
	Form1 = CreateWindow(TEXT("WIN0"),
		TEXT("Form1"),
		(WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
		50,
		50,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	//*********************************
	//Ablak megjelenítése
	//*********************************
	ShowWindow(Form1, SajatiCmdShow);
	UpdateWindow(Form1);

	//*********************************
	//Ablak üzenetkezelésének aktiválása
	//*********************************
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//*********************************
//Az ablak callback függvénye: eseménykezelés
//*********************************
LRESULT CALLBACK WndProc0(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		//*********************************
		//Ablak létrehozásakor közvetlenül
		//*********************************
	case WM_CREATE:
		/*Init*/;
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd, D2D1::SizeU(800, 600)),
			&pRT);
		srand((unsigned)time(NULL));
		create_main_buffer();
		return 0;
		//*********************************
		//Ablak kliens területének újrarajzolása
		//*********************************
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		D2D_rajzolas(pRT);
		return 0;
		//*********************************
		//Ablak bezárása, erõforrások felszabadítása
		//*********************************
	case WM_CLOSE:
		pRT->Release();
		pD2DFactory->Release();
		DestroyWindow(hwnd);
		return 0;
		//*********************************
		//Ablak megsemmisítése
		//*********************************
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void cleanup_main_buffer(void)
{
	memset(kepadat, 255, KEPERNYO_HEIGHT * KEPERNYO_WIDTH * sizeof(unsigned int));
}

void swap_main_buffer(void)
{
	terulet.left = 0;
	terulet.top = 0;
	terulet.right = KEPERNYO_WIDTH - 1;
	terulet.bottom = KEPERNYO_HEIGHT - 1;

	memkeptarolo->CopyFromMemory(&terulet, kepadat, KEPERNYO_WIDTH * sizeof(unsigned int));

	pRT->BeginDraw();
	pRT->DrawBitmap(memkeptarolo, D2D1::RectF(0.0f, 0.0f, KEPERNYO_WIDTH - 1, KEPERNYO_HEIGHT - 1), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
	pRT->EndDraw();
}

void D2D_rajzolas(ID2D1HwndRenderTarget* pRT)
{
	int i;
	cleanup_main_buffer();
	CleanUp_Zbuffer();
	for (i = 0; i < 10; i++)
	{
		SetPixel_Zbuffer(getrandom(800), getrandom(600), getrandom(12), RGB(getrandom(250), getrandom(250), getrandom(250)));
		DrawLine_Zbuffer(getrandom(800), getrandom(600), getrandom(12), getrandom(800), getrandom(600), getrandom(12), RGB(getrandom(250), getrandom(250), getrandom(250)));
		FillTriangle_Zbuffer(getrandom(800), getrandom(600), getrandom(12), getrandom(800), getrandom(600), getrandom(12), getrandom(800), getrandom(600), getrandom(12), RGB(getrandom(250), getrandom(250), getrandom(250)));
	}
	swap_main_buffer();
}

int getrandom(int maxnum)
{
	return (double)rand() / (RAND_MAX + 1) * maxnum;
}

void create_main_buffer(void)
{
	pRT->CreateBitmap(D2D1::SizeU(KEPERNYO_WIDTH, KEPERNYO_HEIGHT),
		D2D1::BitmapProperties(D2D1::PixelFormat
		(DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE)), &memkeptarolo);
}

void CleanUp_Zbuffer(void)
{
	int i, j;
	for (i = 0; i < KEPERNYO_WIDTH; ++i)
		for (j = 0; j < KEPERNYO_HEIGHT; ++j)
		{
			zbuffer[(j * KEPERNYO_WIDTH) + i] = 9999999;
		}
}

void SetPixel_Zbuffer(int x1, int y1, int z1, int szin)
{
	int aktoffset = (y1 * KEPERNYO_WIDTH) + x1;

	if (zbuffer[aktoffset] > z1)
	{
		zbuffer[aktoffset] = z1;
		kepadat[aktoffset] = szin;
	}
}

void DrawLine_Zbuffer(int x0, int y0, int z0, int x1, int y1, int z1, int szin)
{
	bool flip = false;
	int csere, aktoffset;
	float melysegertek;
	if (y1 < 0 || y0 < 0) return;

	if (abs(x0 - x1) < abs(y0 - y1))
	{
		csere = x0;
		x0 = y0;
		y0 = csere;

		csere = x1;
		x1 = y1;
		y1 = csere;
		flip = true;
	}
	if (x0 > x1)
	{
		csere = x0;
		x0 = x1;
		x1 = csere;

		csere = y0;
		y0 = y1;
		y1 = csere;
	}
	int dx = x1 - x0;
	int dy = y1 - y0;

	int jelzo1 = abs(dy) * 2;
	int jelzo2 = 0;
	int y = y0, x;

	for (x = x0; x <= x1; ++x)
	{
		if (z0 == z1) melysegertek = z0;
		else
		{
			int s1 = abs(x1 - x0);
			int s2 = abs(z0 - z1);
			melysegertek = (float)z1 + (float)((((float)x - (float)x0) / (float)s1) * (float)s2);
		}
		if (flip)
		{
			aktoffset = (x * KEPERNYO_WIDTH);
			if (zbuffer[aktoffset + y] > melysegertek)
			{
				zbuffer[aktoffset + y] = melysegertek;
				kepadat[aktoffset + y] = szin;
			}
		}
		else
		{
			aktoffset = (y * KEPERNYO_WIDTH);
			if (zbuffer[aktoffset + x] > melysegertek)
			{
				zbuffer[aktoffset + x] = melysegertek;
				kepadat[aktoffset + x] = szin;
			}
		}
		jelzo2 += jelzo1;
		if (jelzo2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			jelzo2 -= dx * 2;
		}
	}
}

void FillTriangle_Zbuffer(int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int szin)
{
	int Px, Py, melysegertek, boxminx = KEPERNYO_WIDTH - 1, boxminy = KEPERNYO_HEIGHT - 1, boxmaxx = 0, boxmaxy = 0;
	int aktoffset;
	Vec3f interpolal, segedvektor;

	if (y1 == y2 && y1 == y3) return;
	if (x1 == x2 && x1 == x3) return;
	boxminx = __min(x1, x2); boxminx = __min(boxminx, x3);
	boxminy = __min(y1, y2); boxminy = __min(boxminy, y3);
	boxmaxx = __max(x1, x2); boxmaxx = __max(boxmaxx, x3);
	boxmaxy = __max(y1, y2); boxmaxy = __max(boxmaxy, y3);

	for (Px = boxminx; Px <= boxmaxx; ++Px)
	{
		for (Py = boxminy; Py <= boxmaxy; ++Py)
		{
			aktoffset = Px + (Py * KEPERNYO_WIDTH);
			segedvektor.x = (x2 - x1) * (y1 - Py) - (x1 - Px) * (y2 - y1);
			segedvektor.y = (x1 - Px) * (y3 - y1) - (x3 - x1) * (y1 - Py);
			segedvektor.z = (x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1);
			if (abs((int)segedvektor.z) < 1) { interpolal.x = -1; interpolal.y = 0; interpolal.z = 0; }
			else
			{
				interpolal.x = 1.f - (segedvektor.x + segedvektor.y) / segedvektor.z;
				interpolal.y = segedvektor.y / segedvektor.z;
				interpolal.z = segedvektor.x / segedvektor.z;
			}

			if (interpolal.x < 0 || interpolal.y < 0 || interpolal.z < 0) continue;
			melysegertek = (z1 * interpolal.x) + (z2 * interpolal.y) + (z3 * interpolal.z);
			if (zbuffer[aktoffset] > melysegertek)
			{
				zbuffer[aktoffset] = melysegertek;
				kepadat[aktoffset] = szin;
			}
		}
	}
}
