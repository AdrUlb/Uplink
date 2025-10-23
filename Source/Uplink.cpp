#include "Uplink.hpp"

#include "Util.hpp"
#include "_.hpp"
#include "Redshirt/Redshirt.hpp"

FILE* file_stdout = nullptr;
App* gApp = nullptr;

static char* vmg57670648335164_br_find_exe(uint32_t* outErrorCode)
{
	auto* buffer = static_cast<char*>(malloc(0x1080));

	if (!buffer)
	{
		if (outErrorCode)
			*outErrorCode = 0;
		return nullptr;
	}

	const auto procExeLength = readlink("/proc/self/exe", buffer, 0x1080);
	if (procExeLength != -1)
	{
		buffer[procExeLength] = 0;
		return buffer;
	}

	auto* mapsFile = fopen("/proc/self/maps", "r");

	if (!mapsFile)
	{
		free(buffer);

		if (outErrorCode)
			*outErrorCode = 1;
		return nullptr;
	}

	if (!fgets(buffer, 0x1080, mapsFile))
	{
		fclose(mapsFile);
		free(buffer);

		if (outErrorCode)
			*outErrorCode = 2;
		return nullptr;
	}

	const auto mapsLength = strlen(buffer);
	if (mapsLength)
	{
		if (buffer[mapsLength - 1] == '\n')
			buffer[mapsLength - 1] = 0;

		const char* mapsPathname = strchr(buffer, '/');
		const char* perms = strstr(buffer, " r-xp ");

		if (perms && mapsPathname)
		{
			auto* const ret = strdup(mapsPathname);
			free(buffer);
			fclose(mapsFile);
			return ret;
		}
	}

	fclose(mapsFile);
	free(buffer);

	if (!outErrorCode)
		return nullptr;

	*outErrorCode = 3;
	return nullptr;
}

static void Init_App(char const* exePath)
{
	gApp = new App();
	gApp->Set(GetFilePath(exePath).c_str(), "1.55", "RELEASE", std::format("{} at {}", "Aug 29 2011", "21:29:24").c_str(), "Uplink");

	std::println("=============================");
	std::println("=                           =");
	std::println("=        U P L I N K        =");
	std::println("=                           =");
	std::println("=        Version {:<10} =", gApp->GetVersion());
	std::println("=     - R E L E A S E -     =");
	std::println("=                           =");
	std::println("=============================");
	std::println();
	std::println("{}", gApp->GetBuild());

	MakeDirectory(gApp->GetUsersDir());
	MakeDirectory(gApp->GetUsersTempDir());
	MakeDirectory(gApp->GetUsersOldDir());

	EmptyDirectory(gApp->GetUsersTempDir());
	const auto debugLogFilePath = std::format("{}debug.log", gApp->GetUsersDir());

	file_stdout = nullptr;
	const auto fd = dup(fileno(stdout));
	if (fd != -1)
		file_stdout = fdopen(fd, "a");

	if (!freopen(debugLogFilePath.c_str(), "a", stdout))
		std::println("WARNING : Failed to open {} for writing stdout", debugLogFilePath);

	if (!freopen(debugLogFilePath.c_str(), "a", stderr))
		std::println("WARNING : Failed to open {} for writing stderr", debugLogFilePath);

	const auto currentTime = time(nullptr);
	const auto* const localTime = localtime(&currentTime);
	std::println("\n");
	std::println("===============================================");
	std::println("NEW GAME     {}:{}, {}/{}/{}", localTime->tm_hour, localTime->tm_min, localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
	std::println("===============================================");
	std::println("Version : {}", gApp->GetVersion());
	std::println("RELEASE");
	std::println("Linux Build");
	std::println("{}", gApp->GetBuild());
	std::println("Path : {}", gApp->GetPath());
	RsInitialise(gApp->GetPath());
	gApp->Initialise();
}

void Init_Options(const int argc, char* argv[]) { TODO_ABORT; }
bool VerifyLegitAndCodeCardCheck() { TODO_ABORT; }
bool Load_Data() { TODO_ABORT; }
bool Init_Game() { TODO_ABORT; }
bool Init_Graphics() { TODO_ABORT; }
bool Init_OpenGL(const int argc, char* argv[]) { TODO_ABORT; }
bool Init_Fonts() { TODO_ABORT; }
bool Init_Sound() { TODO_ABORT; }
bool Init_Music() { TODO_ABORT; }
bool Run_MainMenu() { TODO_ABORT; }
bool Run_Game() { TODO_ABORT; }
void Cleanup_Uplink() { TODO_ABORT; }

void RunUplink(const int argc, char* argv[])
{
	if (argc > 1)
	{
		char* arg1 = argv[1];

		if (arg1[0] == '-' && arg1[1] == 'v' && arg1[2] == 0)
			return std::println("{}", UPLINK_VERSION);
	}

	const auto* edx_1 = "/opt/uk.co.introversion.uplink-full/data.dat";

	if (!DoesFileExist("/opt/uk.co.introversion.uplink-full/data.dat"))
		edx_1 = vmg57670648335164_br_find_exe(nullptr);

	Init_App(edx_1);
	Init_Options(argc, argv);

	if (VerifyLegitAndCodeCardCheck() && Load_Data())
	{
		Init_Game();
		Init_Graphics();
		Init_OpenGL(argc, argv);
		Init_Fonts();
		Init_Sound();
		Init_Music();
		Run_MainMenu();
		Run_Game();
	}

	Cleanup_Uplink();
}
