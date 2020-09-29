#include <windows.h>

typedef int(WINAPI* t_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);

int main(void)
{
	HMODULE h_module;
	t_MessageBoxA p_MessageBoxA;

	h_module = LoadLibraryA("user32.dll");
	p_MessageBoxA = (t_MessageBoxA)GetProcAddress(h_module, "MessageBoxA");
	
	p_MessageBoxA(NULL, "Hello, Windows", "rtabashi", MB_OK);

	return 0;
}