#include "Uplink.hpp"

#include <climits>
#include <cstring>
#include <print>
#include <unistd.h>
#include <sys/stat.h>

#include "App.hpp"
#include "NOTIMPL.hpp"
#include "Util.hpp"

// NOTIMPL:impl

static FILE* file_stdout = nullptr;
static App* app = nullptr;

#pragma region Signal Handlers and Exception Handling

[[noreturn]] static void RunUplinkExceptionHandling()
{
	NOTIMPL_ABORT;
}

#define X(signame) \
void hSignal##signame(int sig) \
{ \
	std::println("\nAn Uplink Internal Error has occured: segmentation violation (" #signame ")"); \
	\
	if (file_stdout) \
	{ \
		std::println(file_stdout, "\nAn Uplink Internal Error has occured: segmentation violation (" #signame ")"); \
		fflush(file_stdout); \
	} \
	\
	RunUplinkExceptionHandling(); \
}

X(SIGSEGV)
X(SIGFPE)
X(SIGPIPE)
#undef X

#pragma endregion

// Adapted from BinReloc by Hongli Lai
static char* br_find_exe()
{
	/* Read from /proc/self/exe (symlink) */
	size_t buf_size = PATH_MAX - 1;

	auto path = static_cast<char*>(malloc(buf_size));

	if (path == nullptr)
	{
		/* Cannot allocate memory. */
		return nullptr;
	}

	const auto path2 = static_cast<char*>(malloc(buf_size));
	if (path2 == nullptr)
	{
		/* Cannot allocate memory. */
		free(path);
		return nullptr;
	}

	strncpy(path2, "/proc/self/exe", buf_size - 1);

	while (true)
	{
		const auto size = readlink(path2, path, buf_size - 1);
		if (size == -1)
		{
			/* Error. */
			free(path2);
			break;
		}

		/* readlink() success. */
		path[size] = '\0';

		/* Check whether the symlink's target is also a symlink.
		 * We want to get the final target. */
		struct stat stat_buf { };
		if (stat(path, &stat_buf) == -1)
		{
			/* Error. */
			free(path2);
			break;
		}

		/* stat() success. */
		if (!S_ISLNK(stat_buf.st_mode))
		{
			/* path is not a symlink. Done. */
			free(path2);
			return path;
		}

		/* path is a symlink. Continue loop and resolve this. */
		strncpy(path, path2, buf_size - 1);
	}

	/* readlink() or stat() failed; this can happen when the program is
	 * running in Valgrind 2.2. Read from /proc/self/maps as fallback. */

	buf_size = PATH_MAX + 128;
	const auto line = static_cast<char*>(realloc(path, buf_size));
	if (line == nullptr)
	{
		/* Cannot allocate memory. */
		free(path);
		return nullptr;
	}

	const auto f = fopen("/proc/self/maps", "r");
	if (f == nullptr)
	{
		free(line);
		return nullptr;
	}

	/* The first entry should be the executable name. */
	if (fgets(line, static_cast<int>(buf_size), f) == nullptr)
	{
		fclose(f);
		free(line);
		return nullptr;
	}

	/* Get rid of newline character. */
	buf_size = strlen(line);
	if (buf_size <= 0)
	{
		/* Huh? An empty string? */
		fclose(f);
		free(line);
		return nullptr;
	}
	if (line[buf_size - 1] == 10)
		line[buf_size - 1] = 0;

	/* Extract the filename; it is always an absolute path. */
	path = strchr(line, '/');

	/* Sanity check. */
	if (strstr(line, " r-xp ") == nullptr || path == nullptr)
	{
		fclose(f);
		free(line);
		return nullptr;
	}

	path = strdup(path);
	free(line);
	fclose(f);
	return path;
}

static void Init_App(const char* exePath) { NOTIMPL_PRINT; }
static void Init_Options(int argc, char** argv) { NOTIMPL_PRINT; }
static void Init_Game() { NOTIMPL_PRINT; }
static void Init_Graphics() { NOTIMPL_PRINT; }
static void Init_OpenGL(int argc, char** argv) { NOTIMPL_PRINT; }
static void Init_Fonts() { NOTIMPL_PRINT; }
static void Init_Sound() { NOTIMPL_PRINT; }
static void Init_Music() { NOTIMPL_PRINT; }
static void Run_MainMenu() { NOTIMPL_PRINT; }
static void Run_Game() { NOTIMPL_PRINT; }
static void Cleanup_Uplink() { NOTIMPL_PRINT; }

static bool VerifyLegitAndCodeCardCheck()
{
	NOTIMPL_PRINT;
	return true;
}

static bool Load_Data()
{
	NOTIMPL_PRINT;
	return true;
}

void RunUplink(const int argc, char** argv)
{
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v')
	{
		std::println(UPLINK_VERSION);
		return;
	}

	if (const auto defaultExePath = "/opt/uk.co.introversion.uplink-full/data.dat"; DoesFileExist(defaultExePath))
	{
		Init_App(defaultExePath);
	}
	else
	{
		const auto exePath = br_find_exe();
		// NOTE: should probably check if exePath is NULL
		Init_App(exePath);
	}

	Init_Options(argc, argv);
	if (!VerifyLegitAndCodeCardCheck() || !Load_Data())
	{
		Cleanup_Uplink();
		return;
	}

	Init_Game();
	Init_Graphics();
	Init_OpenGL(argc, argv);
	Init_Fonts();
	Init_Sound();
	Init_Music();
	Run_MainMenu();
	Run_Game();
	Cleanup_Uplink();
}
