#include <windows.h>

typedef HMODULE(WINAPI* t_LoadLibraryA)(LPCSTR);
typedef FARPROC(WINAPI* t_GetProcAddress)(HMODULE, LPCSTR);
typedef int(WINAPI* t_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);

FARPROC resolve_api_by_hash(unsigned int api_hash)
{
	__asm
	{
		xor		eax, eax
		mov		eax, fs:[eax+30h]	// PEB
		mov		eax, [eax+0ch]		// Ldr
		mov		esi, [eax+14h]		// InmemoryOrderModuleList
	next_mod:
		lodsd						// next _LDR_DATA_TABLE_ENTRY
		mov		[esp+1ch], eax		// store eax
		mov		ebp, [eax+10h]		// DllBase
		mov		eax, [ebp+3ch]		// IMAGE_DOS_HEADER.e_lfanew
		mov		edx, [ebp+eax+78h]	// IMAGE_EXPORT_DIRECTORY
		add		edx, ebp
		mov		ecx, [edx+18h]		// NumberOfNames
		mov		ebx, [edx+20h]		// AddressOfNames
		add		ebx, ebp
	next_name:						// while (--NumberOfNames)
		jecxz	name_not_found
		dec		ecx
		mov		esi, [ebx+ecx*4]	// ptr = AddressOfNames[NumberOfNames]
		add		esi, ebp
		xor		edi, edi			// hash = 0
		xor		eax, eax
	compute_hash_loop:				// while ((c = *(ptr++)) != 0)
		lodsb
		test	al, al
		jz		compare_hash
		ror		edi, 0dh			// hash += ror(c, 0x0d)
		add		edi, eax
		jmp		compute_hash_loop
	name_not_found:
		mov		esi, [esp+1ch]		// update eax
		jmp		next_mod
	compare_hash:
		cmp		edi, [esp+14h]		//compare with api hash
		jnz		next_name
		mov		ebx, [edx+24h]		//AddressOfNameOrdinals
		add		ebx, ebp
		mov		cx, [ebx+ecx*2]		// y = AddressOfNameOrdinals[x]
		mov		ebx, [edx+1ch]		// AddressOfFunctions
		add		ebx, ebp
		mov		eax, [ebx+ecx*4]	//AddressOfFunctions[y]
		add		eax, ebp
	}
}

unsigned int calc_hash(char* api_name)
{
	unsigned int hash;

	hash = 0;
	while (*api_name != '\0') {
		hash = _rorx_u32(hash, 0xd) + *api_name;
		api_name++;
	}
	return hash;
}

int main(void)
{
	unsigned int hash_api00;
	unsigned int hash_api01;
	t_LoadLibraryA p_LoadLibraryA;
	t_GetProcAddress p_GetProcAddress;
	HMODULE h_module;
	t_MessageBoxA p_MessageBoxA;

	hash_api00 = calc_hash("LoadLibraryA");
	hash_api01 = calc_hash("GetProcAddress");

	p_LoadLibraryA = (t_LoadLibraryA)resolve_api_by_hash(hash_api00);
	p_GetProcAddress = (t_GetProcAddress)resolve_api_by_hash(hash_api01);

	h_module = p_LoadLibraryA("user32.dll");
	p_MessageBoxA = (t_MessageBoxA)p_GetProcAddress(h_module, "MessageBoxA");
	
	p_MessageBoxA(NULL, "Hello, Windows", "rtabashi", 0);

	return 0;
}