#include "Uplink.hpp"

#include "Util.hpp"
#include "_.hpp"
#include "Redshirt/Redshirt.hpp"

FILE* file_stdout = nullptr;
App* gApp = nullptr;

std::optional<std::string> FindExe()
{
	std::string procExe;
	procExe.resize(0xFFF);

	const auto procExeLength = readlink("/proc/self/exe", procExe.data(), 0xFFF);
	if (procExeLength > 0)
	{
		procExe.resize(procExeLength);
		return procExe;
	}

	auto* mapsFile = fopen("/proc/self/maps", "r");

	if (!mapsFile)
		return { };

	std::string maps;
	maps.resize(0x1080);
	if (!fgets(maps.data(), 0x1080, mapsFile))
	{
		fclose(mapsFile);
		return { };
	}
	maps.resize(strlen(maps.data()));

	if (!maps.empty())
	{
		if (maps[maps.size() - 1] == '\n')
			maps.resize(maps.size() - 1);

		const auto pathnameIndex = maps.find('/');
		const auto permsIndex = maps.find(" r-xp ");

		if (pathnameIndex != std::string::npos && permsIndex != std::string::npos)
		{
			fclose(mapsFile);
			return maps.substr(pathnameIndex);
		}
	}

	fclose(mapsFile);

	return { };
}

static void Init_App(char const* exePath)
{
	gApp = new App();
	gApp->Set(GetFilePath(exePath).c_str(), "1.55", UPLINK_TYPE, std::format("{} at {}", __DATE__, __TIME__).c_str(), "Uplink");

	std::println("=============================");
	std::println("=                           =");
	std::println("=        U P L I N K        =");
	std::println("=                           =");
	std::println("=        Version {:<10} =", gApp->GetVersion());
#if defined(UPLINK_TYPE_RELEASE)
	std::println("=     - R E L E A S E -     =");
#elif defined(UPLINK_TYPE_DEBUG)
	std::println("=       - D E B U G -       =");
#else
#error Chnage me!
#endif
	std::println("=                           =");
	std::println("=============================");
	std::println();
	std::println("{}", gApp->GetBuild());

	MakeDirectory(gApp->GetUsersDir());
	MakeDirectory(gApp->GetUsersTempDir());
	MakeDirectory(gApp->GetUsersOldDir());

	EmptyDirectory(gApp->GetUsersTempDir());

	file_stdout = nullptr;
#ifdef UPLINK_TYPE_RELEASE
	const auto debugLogFilePath = std::format("{}debug.log", gApp->GetUsersDir());

	const auto stdoutDesc = dup(fileno(stdout));
	if (stdoutDesc != -1)
		file_stdout = fdopen(stdoutDesc, "a");

	if (!freopen(debugLogFilePath.c_str(), "a", stdout))
		std::println("WARNING : Failed to open {} for writing stdout", debugLogFilePath);

	if (!freopen(debugLogFilePath.c_str(), "a", stderr))
		std::println("WARNING : Failed to open {} for writing stderr", debugLogFilePath);

	setvbuf(stdout, nullptr, _IONBF, 0);
	setvbuf(stderr, nullptr, _IONBF, 0);
#endif

	const auto currentTime = time(nullptr);
	const auto* const localTime = localtime(&currentTime);
	std::println("\n");
	std::println("===============================================");
	std::println("NEW GAME     {}:{}, {}/{}/{}", localTime->tm_hour, localTime->tm_min, localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
	std::println("===============================================");
	std::println("{}", gApp->GetBuild());
	std::println("Path : {}", gApp->GetPath());
	RsInitialise(gApp->GetPath());
	gApp->Initialise();
}

void Init_Options(const int argc, char* argv[]) { TODO_PRINT; }

bool VerifyLegitAndCodeCardCheck()
{
	TODO_PRINT;
	return true;
}

bool Load_Data()
{
	TODO_PRINT;
	return true;
}

bool Init_Game()
{
	TODO_PRINT;
	return true;
}

bool Init_Graphics()
{
	TODO_PRINT;
	return true;
}

bool Init_OpenGL(const int argc, char* argv[])
{
	TODO_PRINT;
	return true;
}

bool Init_Fonts()
{
	TODO_PRINT;
	return true;
}

bool Init_Sound()
{
	TODO_PRINT;
	return true;
}

bool Init_Music()
{
	TODO_PRINT;
	return true;
}

bool Run_MainMenu()
{
	TODO_PRINT;
	return true;
}

bool Run_Game()
{
	TODO_PRINT;
	return true;
}

void Cleanup_Uplink() { TODO_PRINT; }

void RunUplink(const int argc, char* argv[])
{
	if (argc > 1)
	{
		char* arg1 = argv[1];

		if (arg1[0] == '-' && arg1[1] == 'v' && arg1[2] == 0)
			return std::println("{}", UPLINK_VERSION);
	}

	auto path = FindExe();
	assert(path);

	Init_App(path->c_str());
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
