#include <csignal>
#include <cstdio>
#include <print>
#include "Uplink.hpp"
#include "_.hpp"
#include "Gucci/Gucci.hpp"

static FILE* file_stdout = nullptr;

[[noreturn]] static void RunUplinkExceptionHandling()
{
	if (gApp &&
	    gApp->GetOptionsOrNull() &&
	    gApp->GetOptions().GetOptionOrNull("crash_graphicsinit") &&
	    gApp->GetOptions().GetOptionValue("crash_graphicsinit"))
	{
		std::println("\nAn Uplink Internal Error has occured during graphics initialization");

		if (file_stdout)
		{
			std::println(file_stdout, "\nAn Uplink Internal Error has occured during graphics initialization");
			fflush(file_stdout);
		}
	}

	std::println("\nAn (unrecognised) Uplink Internal Error has occured");
	std::println("===================================================");
	App::CoreDump();

	if (file_stdout)
	{
		std::println(file_stdout, "\nAn (unrecognised) Uplink Internal Error has occured");
		std::println(file_stdout, "===================================================");

		std::println(file_stdout,
		             "See the {}debug.log file for more informations on the error",
		             strcmp(gApp->GetUsersDir(), "c:/") != 0 ? gApp->GetUsersDir() : "");

		fflush(file_stdout);
	}

	GciRestoreScreenSize();
	fflush(nullptr);
	exit(255);
}

#define DEFINE_SIGNAL_HANDLER(name, message) \
[[noreturn]] static void name(int signum) \
{ \
	std::println("\nAn Uplink Internal Error has occured: " message); \
	if (file_stdout) \
	{ \
		std::println(file_stdout, "\nAn Uplink Internal Error has occured: " message); \
		fflush(file_stdout); \
	} \
	RunUplinkExceptionHandling(); \
}

DEFINE_SIGNAL_HANDLER(hSignalSIGSEGV, "segmentation violation (SIGSEGV)");
DEFINE_SIGNAL_HANDLER(hSignalSIGFPE, "erroneous arithmetic operation (SIGFPE)");
DEFINE_SIGNAL_HANDLER(hSignalSIGPIPE, "write to pipe with no one reading (SIGPIPE)");

#undef DEFINE_SIGNAL_HANDLER

int main(const int argc, char* argv[])
{
	signal(SIGSEGV, hSignalSIGSEGV);
	signal(SIGFPE, hSignalSIGFPE);
	signal(SIGPIPE, hSignalSIGPIPE);
	RunUplink(argc, argv);
	return 0;
}
