#include "App.hpp"

#include <dirent.h>

#include "Uplink.hpp"
#include "Util.hpp"
#include "Eclipse/Eclipse.hpp"

#include "_.hpp"

DArray<char*>* App::ListExistingGames()
{
	auto* result = new DArray<char*>();

	char name[0x100];
	UplinkStrncpy(name, gApp->_usersDir.c_str(), 0x100);

	auto* dir = opendir(name);
	if (!dir)
		return result;

	for (auto* entry = readdir(dir); entry; entry = readdir(dir))
	{
		auto* extOffset = strstr(entry->d_name, ".usr");

		if (!extOffset)
			continue;

		extOffset[0] = 0;

		auto* buffer = new char[0x100];
		UplinkStrncpy(buffer, entry->d_name, 0x100);
		result->PutData(buffer);
	}

	closedir(dir);

	return result;
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

MainMenu& App::GetMainMenu() { return *_mainMenu; }
Network& App::GetNetwork() { return *_network; }
Options& App::GetOptions() { return *_options; }

void App::Initialise()
{
	_options = new Options();
	_options->Load(nullptr);
	_options->CreateDefaultOptions();
	_initTime = static_cast<int>(EclGetAccurateTime());
	_network = new Network();
	_mainMenu = new MainMenu();
}

void App::Set(const char* path, const char* version, const char* type, const char* date, const char* title)
{
	_path = path;
	_version = version;
	_type = type;
	_date = date;
	_title = title;
	_build = std::format("Version {} ({})\nCompiled on {}\n", _version, _type, _date);

	auto* homeDir = getenv("HOME");
	if (homeDir)
	{
		_usersDir = std::string(homeDir) + "/.uplink/";
		_usersTempDir = std::string(homeDir) + "/.uplink/userstmp/";
		_usersOldDir = std::string(homeDir) + "/.uplink/usersold/";
	}
	else
	{
		_usersDir = _path + "users/";
		_usersTempDir = _path + "userstmp/";
		_usersOldDir = _path + "usersold/";
	}
}

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
