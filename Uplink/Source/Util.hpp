#pragma once
#include <unistd.h>

static bool DoesFileExist(const char* name)
{
	return access(name, 0) == 0;
}
