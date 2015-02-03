#pragma once
#include <windows.h>

struct FileLocation {
	TCHAR Path[_MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Directory[_MAX_DIR];
	TCHAR Name[_MAX_FNAME];
	TCHAR Extension[_MAX_EXT];
};