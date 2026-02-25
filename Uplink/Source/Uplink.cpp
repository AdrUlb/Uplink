#include "Uplink.hpp"
#include "FIXME.hpp"

#include <print>

static FILE* file_stdout = nullptr;
static App* app = nullptr;

#pragma region Signal Handlers and Exception Handling

[[noreturn]] static void RunUplinkExceptionHandling()
{
	TODO_ABORT;
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

void RunUplink(int argc, char** argv)
{
	TODO_ABORT;
}
