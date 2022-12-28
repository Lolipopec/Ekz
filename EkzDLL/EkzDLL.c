#include "../header.h"

BOOL WINAPI DllMain(HINSTANCE hlnstDll, DWORD dwReason, LPVOID IpReserved)
{
	BOOL bAllWentWell = TRUE;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	if (bAllWentWell)
		return TRUE;
	else
		return FALSE;
}
#pragma region FirstMod
LPWSTR Reading(LPWSTR path)
{
	DWORD d = 0;
	HINSTANCE FileHandle = CreateFile(PATH, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	LPWSTR ReadString = calloc(100500, sizeof(WCHAR));
	ReadFile(FileHandle, ReadString, 100500 * sizeof(WCHAR), &d, NULL);
	CloseHandle(FileHandle);
	ReadString[d / 2] = '\n';
	ReadString[d / 2 + 1] = '\0';
	return ReadString;
}
__declspec(dllexport) U* CreateUserArray(LPWSTR path, LPDWORD n)
{
	LPWSTR str = Reading(path);
	DWORD count = CountUsers(str);
	U* users = calloc(count, sizeof(U));
	DWORD poz = 0, zap = 0, ind = 0;
	LPWSTR strvozr = calloc(3, sizeof(TCHAR));
	//i с одного для того что бы пропустить символ 65279 стоящий в начале файла
	for (size_t i = 1; i < wcslen(str); i++)
	{
		if (str[i] == '\n')
		{
			strvozr = calloc(3, sizeof(TCHAR));
			zap++;
			poz = 0;
			ind = 0;
		}
		else
		{
			if (str[i] == ';')
			{
				poz++;
				ind = 0;
			}
			else
			{
				if (poz == 0)
					users[zap].f[ind] = str[i];
				if (poz == 1)
					users[zap].i[ind] = str[i];
				if (poz == 2)
					users[zap].o[ind] = str[i];
				if (poz == 3)
				{
					if (str[i] == '\r')
					{
						strvozr[ind] = '\0';
						users[zap].vozr = LpwstrToDword(strvozr);
						free(strvozr);
					}
					else strvozr[ind] = str[i];
				}
				ind++;
			}
		}
	}
	*n = count;
	return users;
}
DWORD LpwstrToDword(LPWSTR str)
{
	DWORD dw = 0;
	for (size_t i = 0; i < wcslen(str); i++)
	{
		dw += (str[i] - '0');
		dw *= 10;
	}
	return dw / 10;
}
DWORD CountUsers(LPWSTR str)
{
	DWORD count = 0;
	for (size_t i = 0; i < wcslen(str); i++)
	{
		if (str[i] == L'\n') count++;
	}
	return count;
}
__declspec(dllexport) DWORD WriteToCSV(U* uses, LPDWORD count2)
{
	DWORD w;
	HINSTANCE FileHandle1 = CreateFile(L"SortUser.csv", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	TCHAR otvFile[512];
	WriteFile(FileHandle1, uses[0].f, wcslen(uses[0].f) * sizeof(WCHAR), &w, NULL);//Для записи символа начала строки 65279
	for (size_t i = 1; i < count2; i++)
	{
		uses[i].f[wcslen(uses[i].f)] = (WCHAR)';';
		WriteFile(FileHandle1, uses[i].f, wcslen(uses[i].f) * sizeof(WCHAR), &w, NULL);
		uses[i].i[wcslen(uses[i].i)] = (WCHAR)';';
		WriteFile(FileHandle1, uses[i].i, wcslen(uses[i].i) * sizeof(WCHAR), &w, NULL);
		uses[i].o[wcslen(uses[i].o)] = (WCHAR)';';
		WriteFile(FileHandle1, uses[i].o, wcslen(uses[i].o) * sizeof(WCHAR), &w, NULL);

		swprintf(otvFile, 512, TEXT("%d\r\n"), uses[i].vozr);
		DWORD len = 0;
		while (otvFile[len] != '\0')
		{
			len++;
		}
		DWORD dz = 0;
		WriteFile(FileHandle1,
			otvFile,
			len * (sizeof(char) * 2),
			&dz,
			NULL
		);
	}
	CloseHandle(FileHandle1);
	return 1;
}
#pragma endregion

#pragma region SecondMod
__declspec(dllexport) U* SortUserArray(U* users, LPDWORD count, LPWSTR str)
{
	DWORD ind = 0, indUser = 0, con = 0, f = 0;
	char* istr;
	DWORD countF = wcslen(str);
	for (DWORD i = 0; i < *count; i++) // Поиск количества фамилий по условию для формирования новой структуры
	{
		istr = strstr(users[i].f, str);
		if (istr != NULL)
		{
			con++;
		}
	}
	U* UsersSort = calloc(con + 1, sizeof(U));
	UsersSort[0].f[0] = (WCHAR)65279;
	indUser++;
	for (DWORD i = 0; i < *count; i++) //Поиск строк и записывание их
	{
		istr = strstr(users[i].f, str);
		if (istr != NULL)
		{
			for (DWORD j = 0; j < wcslen(users[i].f); j++)
			{
				UsersSort[indUser].f[j] = users[i].f[j];
			}
			for (DWORD j = 0; j < wcslen(users[i].i); j++)
			{
				UsersSort[indUser].i[j] = users[i].i[j];
			}
			for (DWORD j = 0; j < wcslen(users[i].o); j++)
			{
				UsersSort[indUser].o[j] = users[i].o[j];
			}
			UsersSort[indUser].vozr = users[i].vozr;
			indUser++;
		}
	}
	*count = con + 1;
	return UsersSort;
}

__declspec(dllexport) U* FilterUserArray(U* users, LPDWORD count, LPWSTR str)
{
	DWORD ind = 0, indUser = 0, con = *count, f = 0;
	U* UsersFilt = calloc(con, sizeof(U));
	UsersFilt[0].f[0] = (WCHAR)65279;
	indUser++;
	for (DWORD i = 0; i < *count; i++)//Переписал в лист
	{
		for (DWORD j = 0; j < wcslen(users[i].f); j++)
		{
			UsersFilt[indUser].f[j] = users[i].f[j];
		}
		for (DWORD j = 0; j < wcslen(users[i].i); j++)
		{
			UsersFilt[indUser].i[j] = users[i].i[j];
		}
		for (DWORD j = 0; j < wcslen(users[i].o); j++)
		{
			UsersFilt[indUser].o[j] = users[i].o[j];
		}
		UsersFilt[indUser].vozr = users[i].vozr;
		indUser++;
	}
	for (int i = 0; i < *count - 1; i++) //Сортировка
	{
		for (int j = 0; j < *count - i - 1; j++)
		{
			int err;
			char* res;
			int res_len = WideCharToMultiByte(1251, 0, UsersFilt[j].f, -1, NULL, 0, NULL, NULL);
			res = calloc(sizeof(char), res_len);
			err = WideCharToMultiByte(1251, 0, UsersFilt[j].f, -1, res, res_len, NULL, NULL);
			int err1;
			char* res1;
			int res_len1 = WideCharToMultiByte(1251, 0, UsersFilt[j+1].f, -1, NULL, 0, NULL, NULL);
			res1 = calloc(sizeof(char), res_len1);
			err1 = WideCharToMultiByte(1251, 0, UsersFilt[j+1].f, -1, res1, res_len1, NULL, NULL);
			if (strcmp(res, res1)>0)
			{
				U user = UsersFilt[j];
				UsersFilt[j] = UsersFilt[j + 1];
				UsersFilt[j + 1] = user;
			}
		}
	}
	*count = con;
	return UsersFilt;
}
#pragma endregion

#pragma region ThirdMod
__declspec(dllexport) VOID AvgAge(U* users, LPDWORD count)
{
	DWORD d = count;
	float avg = 0;
	for (DWORD i = 0; i < count; i++)
	{
		avg += users[i].vozr;
	}
	avg = avg / (d - 1);
	char array[10];
	sprintf(array, "%f", avg);
	wchar_t wc[10];
	mbstowcs(wc, array, 10);
	HKEY hKey = NULL;//дескриттор ключа реестра (он является структурой)
	if (RegOpenKey(HKEY_CURRENT_USER, NULL, &hKey) != ERROR_SUCCESS) //открываем раздел HKEY_CURRENT_USER
		return 1;
	LPWSTR ptr = wc;
	int length2 = snprintf(NULL, 0, "%s", wc);
	char* Size2 = malloc(length2 + 1);
	sprintf(Size2, "%s", wc);
	if (RegSetValueExW(hKey, L"AvgAge", NULL, REG_SZ, ptr, 22 * sizeof(WCHAR)) == ERROR_SUCCESS)
	{
	}
}
__declspec(dllexport) VOID MaxLenF(U* users, LPDWORD count)
{
	DWORD MaxLen = 0, ind = 0;
	for (int i = 0; i < count; i++)
	{
		size_t len1 = strlen(users[i].f);
		size_t len2 = strlen(users[i].i);
		size_t len3 = strlen(users[i].o);
		if ((len1 + len2 + len3) > MaxLen)
		{
			MaxLen = len1 + len2 + len3;
			ind = i;
		}
	}
	char third[512];
	swprintf(third, sizeof(third), L"%s;%s;%s", users[ind].f, users[ind].i, users[ind].o);
	HKEY hKey = NULL;//дескриттор ключа реестра (он является структурой)
	if (RegOpenKey(HKEY_CURRENT_USER, NULL, &hKey) != ERROR_SUCCESS) //открываем раздел HKEY_CURRENT_USER
		return 1;
	LPWSTR ptr = third;
	int length2 = snprintf(NULL, 0, "%s", ptr);
	char* Size2 = malloc(length2 + 1);
	sprintf(Size2, "%s", ptr);
	if (RegSetValueExW(hKey, L"MaxFIO", NULL, REG_SZ, ptr, 60 * sizeof(LPWSTR)) == ERROR_SUCCESS)
	{
	}
}
#pragma endregion

#pragma region FourthMod
__declspec(dllexport) VOID WriteToBuff(U* uses, LPDWORD count)
{
	int d = count;
	wchar_t buff[3500];
	int t = 0;
	for (int i = 1; i < d; i++)
	{
		for (int j = 0; j < strlen(uses[i].f); j++)
		{
			if (uses[i].f[j] != '\0')
			{
				buff[t] = uses[i].f[j];
				t++;
			}
		}
		buff[t] = ';';
		t++;
		for (int j = 0; j < strlen(uses[i].i); j++)
		{
			if (uses[i].i[j] != '\0')
			{
				buff[t] = uses[i].i[j];
				t++;
			}
		}
		buff[t] = ';';
		t++;
		for (int j = 0; j < strlen(uses[i].o); j++)
		{
			if (uses[i].o[j] != '\0')
			{
				buff[t] = uses[i].o[j];
				t++;
			}

		}
		buff[t] = ';';
		t++;
		int d = uses[i].vozr;
		char array[3];
		sprintf(array, "%d", d);
		for (int j = 0; j < 3; j++)
		{
			if (array[j] != '\0')
			{
				buff[t] = array[j];
				t++;
			}
		}
		buff[t] = '\r';
		t++;
		buff[t] = '\n';
		t++;
	}
	buff[t] = '\0';
	t++;
	wchar_t* str = calloc(t, sizeof(wchar_t));
	for (int i = 0; i < t; i++)
	{
		str[i] = buff[i];
	}
	str[t + 1] = '\0';
	LPWSTR buffer = str;
	DWORD len;
	HANDLE hMenu;
	len = wcslen(buffer) + 1;
	hMenu = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(LPWSTR));
	memcpy(GlobalLock(hMenu), buffer, len * sizeof(LPWSTR));
	GlobalUnlock(hMenu);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMenu);
	CloseClipboard();

}
#pragma endregion