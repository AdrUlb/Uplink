#pragma once

#include <Tosser/DArray.hpp>

#include "MainMenu.hpp"
#include "Network.hpp"
#include "Options.hpp"
#include "PhoneDialler.hpp"
#include "UplinkObject.hpp"

class App : public UplinkObject
{
	char _path[256];
	char _usersPath[256];
	char _usersTempPath[256];
	char _usersOldPath[256];
	char _version[32];
	char _type[32];
	char _date[32];
	char _title[64];
	char _build[256];
	int _startTime;
	bool _closed;
	Options* _options;
	Network* _network;
	MainMenu* _mainMenu;
	PhoneDialler* _dialler;
	char* _nextLoadGame;
	bool _enableCodeCard;

public:
	App();
	~App() override;
	void Print() override;
	void Update() override;
	const char* GetID() override { return "APP"; }

	[[nodiscard]] const char* Path() const { return _path; }
	[[nodiscard]] const char* UsersPath() const { return _usersPath; }
	[[nodiscard]] const char* UsersTempPath() const { return _usersTempPath; }
	[[nodiscard]] const char* UsersOldPath() const { return _usersOldPath; }
	[[nodiscard]] const char* Version() const { return _version; }
	[[nodiscard]] const char* Type() const { return _type; }
	[[nodiscard]] const char* Date() const { return _date; }
	[[nodiscard]] const char* Title() const { return _title; }
	[[nodiscard]] const char* Build() const { return _build; }

	[[nodiscard]] bool Closed() const { return _closed; }
	MainMenu* GetMainMenu();
	Network* GetNetwork();
	Options* GetOptions();

	void Set(const char* path, const char* version, const char* type, const char* date, const char* title);
	void Initialise();
	void Close();
	void CloseGame();

	DArray<char*>* ListExistingGames();
	void SetNextLoadGame(const char* name);
	void LoadGame(const char* name);
	void SaveGame(const char* name);
	void RetireGame(const char* name);
	void LoadGame();

	void RegisterPhoneDialler(PhoneDialler* dialler);
	void UnRegisterPhoneDialler(PhoneDialler* dialler);

	static void CoreDump();
};
