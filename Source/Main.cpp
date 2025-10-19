#include <csignal>
#include "_.hpp"
#include "Tosser/BTree.hpp"

static FILE* file_stdout = nullptr;

[[noreturn]] static void RunUplinkExceptionHandling()
{
	TODO_ABORT;
}

#define DEFINE_SIGNAL_HANDLER(name, message) \
    [[noreturn]] static void name(int signum) \
    { \
        puts("\nAn Uplink Internal Error has occured: " message); \
        if (file_stdout) \
        { \
            fputs("\nAn Uplink Internal Error has occured: " message "\n", file_stdout); \
            fflush(file_stdout); \
        } \
        RunUplinkExceptionHandling(); \
    }

DEFINE_SIGNAL_HANDLER(hSignalSIGSEGV, "segmentation violation (SIGSEGV)");
DEFINE_SIGNAL_HANDLER(hSignalSIGFPE, "erroneous arithmetic operation (SIGFPE)");
DEFINE_SIGNAL_HANDLER(hSignalSIGPIPE, "write to pipe with no one reading (SIGPIPE)");

#undef DEFINE_SIGNAL_HANDLER

static void RunUplink(int argc, char* argv[])
{
	TODO_ABORT;
}

int main(const int argc, char* argv[])
{
	signal(SIGSEGV, hSignalSIGSEGV);
	signal(SIGFPE, hSignalSIGFPE);
	signal(SIGPIPE, hSignalSIGPIPE);
	RunUplink(argc, argv);
	return 0;
}
