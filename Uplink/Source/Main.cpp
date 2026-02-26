#include <csignal>

#include "Uplink.hpp"

#include "Tosser/DArray.hpp"

int main(const int argc, char** argv)
{
	signal(SIGSEGV, hSignalSIGSEGV);
	signal(SIGFPE, hSignalSIGFPE);
	signal(SIGPIPE, hSignalSIGPIPE);
	RunUplink(argc, argv);
}
