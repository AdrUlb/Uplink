#include "App.hpp"

#include "_.hpp"

DArray<char*>* App::ListExistingGames()
{
	TODO_ABORT;
}

void App::CoreDump()
{
	TODO_ABORT;
}

App::~App()
{
	if (!Closed())
		Close();
}

bool App::Load(FILE* file) { TODO_ABORT; }

void App::Save(FILE* file) { TODO_ABORT; }

void App::Print() { TODO_ABORT; }

void App::Update() { TODO_ABORT; }

const char* App::GetID() { TODO_ABORT; }

MainMenu& App::GetMainMenu() { TODO_ABORT; }
Network& App::GetNetwork() { TODO_ABORT; }
Options& App::GetOptions() { TODO_ABORT; }
void App::Initialise() { TODO_ABORT; }
void App::Set(const char* path, const char* version, const char* type, const char* date, const char* title) { TODO_ABORT; }
bool App::Closed() { TODO_ABORT; }
void App::CloseGame() { TODO_ABORT; }
void App::Close() { TODO_ABORT; }

void App::LoadGame(const char* user) { TODO_ABORT; }
void App::SetNextLoadGame(const char* user) { TODO_ABORT; }
void App::LoadGame() { TODO_ABORT; }
void App::SaveGame(const char* user) { TODO_ABORT; }
void App::RetireGame(const char* user) { TODO_ABORT; }

void App::RegisterPhoneDialler(PhoneDialler* dialler) { TODO_ABORT; }
void App::UnRegisterPhoneDialler(PhoneDialler* dialler) { TODO_ABORT; }
