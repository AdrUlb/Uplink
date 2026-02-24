#include "Uplink.hpp"

#include <cstdlib>

#include "Util.hpp"
#include "Redshirt/Redshirt.hpp"

#include "_.hpp"

FILE* file_stdout = nullptr;
App* gApp = nullptr;

static float windowScaleY = 1;
static float windowScaleX = 1;
static int tooltipanimindex = -1;

static std::optional<std::string> FindExe()
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

static void SetWindowScaleFactor(const float x, const float y)
{
	windowScaleX = x;
	windowScaleY = y;
}

static bool TestRsLoadArchive(const char* id)
{
	if (RsLoadArchive(id))
		return true;

	std::println("\nAn error occured in Uplink");
	std::println("Files integrity is not verified");
	std::println("Failed loading '{}'", id);

	if (file_stdout)
	{
		std::println(file_stdout, "\nAn Uplink Error has occured");
		std::println(file_stdout, "Files integrity is not verified");
		std::println(file_stdout, "Failed loading '{}'", id);
	}

	return false;
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

void Init_Options(const int argc, char* argv[])
{
	for (auto i = 1; i < argc; i++)
	{
		const auto* arg = argv[i];

		switch (arg[0])
		{
			case '+':
				gApp->GetOptions().SetOptionValue(arg + 1, 1);
				break;
			case '-':
				gApp->GetOptions().SetOptionValue(arg + 1, 0);
				break;
			case '!':
				if (i + 1 >= argc)
				{
					std::println("Error parsing command line option : {}", argv[i]);
					break;
				}

				i++;

				gApp->GetOptions().SetOptionValue(arg + 1, static_cast<int>(strtol(argv[i], nullptr, 10)));
				break;
			default:
				// NOTE: changed case from '\0' to default since the original code would only print an error for an empty argument
				std::println("Error parsing command line option : {}", arg);
				break;
		}
	}

	if (gApp->GetOptions().GetOptionValue("graphics_safemode") == 1)
	{
		gApp->GetOptions().SetOptionValue("graphics_fullscreen", 0);
		gApp->GetOptions().SetOptionValue("graphics_screenrefresh", -1);
		gApp->GetOptions().SetOptionValue("graphics_screendepth", -1);
		gApp->GetOptions().SetOptionValue("graphics_softwaremouse", 1);
	}

	putchar('\n');
	const auto width = gApp->GetOptions().GetOptionValue("graphics_screenwidth");
	const auto height = gApp->GetOptions().GetOptionValue("graphics_screenheight");
	SetWindowScaleFactor(static_cast<float>(width) / 640.0f, static_cast<float>(height) / 480.0f);

	if (gApp->GetOptions().IsOptionEqualTo("game_debugstart", 1))
		puts("=====DEBUGGING INFORMATION ENABLED=====");
}

bool VerifyLegitAndCodeCardCheck()
{
	TODO_PRINT;
	return true;
}

bool Load_Data()
{
	const auto debug = gApp->GetOptions().IsOptionEqualTo("game_debugstart", 1);

	if (debug)
		puts("Loading application data");

	if (TestRsLoadArchive("data.dat") &&
	    TestRsLoadArchive("graphics.dat") &&
	    TestRsLoadArchive("loading.dat") &&
	    TestRsLoadArchive("sounds.dat") &&
	    TestRsLoadArchive("music.dat") &&
	    TestRsLoadArchive("fonts.dat") &&
	    TestRsLoadArchive("patch.dat") &&
	    TestRsLoadArchive("patch2.dat") &&
	    TestRsLoadArchive("patch3.dat"))
	{
		if (debug)
			puts("Finished loading application data");

		return true;
	}

	return false;
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
