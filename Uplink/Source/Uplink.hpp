#pragma once
#include "App.hpp"

extern App* app;

[[noreturn]] void hSignalSIGSEGV(int sig);
[[noreturn]] void hSignalSIGFPE(int sig);
[[noreturn]] void hSignalSIGPIPE(int sig);

void RunUplink(int argc, char** argv);
