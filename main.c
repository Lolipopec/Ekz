#include "header.h"
#define PATHDLL L"EkzDLL.dll"

typedef U* (_cdecl* MyFunction)(LPWSTR, LPDWORD);
typedef U* (_cdecl* MyFunction1)(U*, LPDWORD, LPWSTR);
typedef U* (_cdecl* MyFunction3)(U*, LPDWORD, LPWSTR);
typedef U* (_cdecl* MyFunction2)(LPWSTR, LPDWORD);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HINSTANCE MyDLL;
	if (!(MyDLL = LoadLibrary(PATHDLL))) return 1;
	MyFunction UserList = (MyFunction)GetProcAddress(MyDLL, "CreateUserArray");
	MyFunction1 UserSort = (MyFunction1)GetProcAddress(MyDLL, "SortUserArray");
	MyFunction3 UserFilt = (MyFunction1)GetProcAddress(MyDLL, "FilterUserArray");
	MyFunction2 WriteToCSV = (MyFunction2)GetProcAddress(MyDLL, "WriteToCSV");
	MyFunction MaxLenF = (MyFunction)GetProcAddress(MyDLL, "MaxLenF");
	MyFunction AvgAge = (MyFunction)GetProcAddress(MyDLL, "AvgAge");
	MyFunction WriteToBuff = (MyFunction)GetProcAddress(MyDLL, "WriteToBuff");
	DWORD countUser = 0, countSortUser;
	U* users = UserList(PATH, &countUser);
	LPWSTR str = L"Иванов";
	countSortUser = countUser;
	//фильтр начало
	U* uses = UserFilt(users, &countSortUser, str);
	countSortUser = countUser;
	WriteToCSV(uses, countSortUser);
	countSortUser = countUser;
	//фильтр конец
	U* uses1 = UserSort(users, &countSortUser, str);
	WriteToCSV(uses1, countSortUser);
	countSortUser = countUser;
	uses1 = UserSort(users, &countSortUser, str);
	MaxLenF(users, countUser);
	AvgAge(users, countUser);
	WriteToBuff(users, countUser-1);
	return 0;
}