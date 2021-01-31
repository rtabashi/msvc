//管理者cmdで起動したときのみ権限の書き換えが成功する
//UAC前のユーザーはSeDebugPrivilegeのリストを持っていないため

#include <Windows.h>

void adjust_privilege(LPCWSTR priv_name, BOOL priv_value)
{
	HANDLE h_token;
	LUID luid;
	TOKEN_PRIVILEGES token_priv;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &h_token)) {
		if (LookupPrivilegeValueW(NULL, priv_name, &luid)) {
			token_priv.PrivilegeCount = 1;
			token_priv.Privileges[0].Luid = luid;
			if (priv_value == TRUE) {
				token_priv.Privileges[0].Attributes = 2;
			}
			AdjustTokenPrivileges(h_token, FALSE, &token_priv, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
		}
		CloseHandle(h_token);
	}
}

void main(void)
{
	adjust_privilege(L"SeDebugPrivilege", TRUE);
	system("whoami /all");
}