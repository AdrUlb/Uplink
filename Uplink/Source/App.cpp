#include "App.hpp"

#include "NOTIMPL.hpp"

// NOTIMPL:impl

App::App() { NOTIMPL_ABORT; }

App::~App() { NOTIMPL_ABORT; }

void App::Print() { NOTIMPL_ABORT; }

void App::Update() { NOTIMPL_ABORT; }

MainMenu* App::GetMainMenu() { NOTIMPL_ABORT; }
Network* App::GetNetwork() { NOTIMPL_ABORT; }
Options* App::GetOptions() { NOTIMPL_ABORT; }

void App::Set(const char* path, const char* version, const char* type, const char* date, const char* title) { NOTIMPL_ABORT; }
void App::Initialise() { NOTIMPL_ABORT; }
void App::Close() { NOTIMPL_ABORT; }
void App::CloseGame() { NOTIMPL_ABORT; }

DArray<char*>* App::ListExistingGames() { NOTIMPL_ABORT; }
void App::SetNextLoadGame(const char* name) { NOTIMPL_ABORT; }
void App::LoadGame(const char* name) { NOTIMPL_ABORT; }
void App::SaveGame(const char* name) { NOTIMPL_ABORT; }
void App::RetireGame(const char* name) { NOTIMPL_ABORT; }
void App::LoadGame() { NOTIMPL_ABORT; }

void App::RegisterPhoneDialler(PhoneDialler* dialler) { NOTIMPL_ABORT; }
void App::UnRegisterPhoneDialler(PhoneDialler* dialler) { NOTIMPL_ABORT; }

void App::CoreDump() { NOTIMPL_ABORT; }
