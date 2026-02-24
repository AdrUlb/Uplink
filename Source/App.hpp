#pragma once

#include "MainMenu.hpp"
#include "Network.hpp"
#include "Options.hpp"
#include "PhoneDialler.hpp"
#include "UplinkObject.hpp"
#include "Tosser/DArray.hpp"

class App : public UplinkObject
{
	std::string _path = "c:/";
	std::string _usersDir = "c:/";
	std::string _usersTempDir = "c:/";
	std::string _usersOldDir = "c:/";
	std::string _version = "1.31c";
	std::string _type = "RELEASE";
	std::string _date = "01/01/97";
	std::string _title = "NewApp";
	std::string _build = "Version 1.0 (RELEASE), Compiled on 01/01/97";
	int _initTime = 0;
	bool _closed = false;
	Options* _options = nullptr;
	Network* _network = nullptr;
	MainMenu* _mainMenu = nullptr;
	PhoneDialler* _phoneDialler = nullptr;
	char* _nextLoadGame = nullptr;
	bool _enableCodeCard = false;

public:
	[[nodiscard]] static DArray<char*>* ListExistingGames();
	static void CoreDump();

	[[nodiscard]] const char* GetPath() const { return _path.c_str(); }
	[[nodiscard]] const char* GetUsersDir() const { return _usersDir.c_str(); }
	[[nodiscard]] const char* GetUsersTempDir() const { return _usersTempDir.c_str(); }
	[[nodiscard]] const char* GetUsersOldDir() const { return _usersOldDir.c_str(); }
	[[nodiscard]] const char* GetVersion() const { return _version.c_str(); }
	[[nodiscard]] const char* GetType() const { return _type.c_str(); }
	[[nodiscard]] const char* GetDate() const { return _date.c_str(); }
	[[nodiscard]] const char* GetTitle() const { return _title.c_str(); }
	[[nodiscard]] const char* GetBuild() const { return _build.c_str(); }
	[[nodiscard]] int32_t GetInitTime() const { return _initTime; }
	[[nodiscard]] bool IsCodeCardEnabled() const { return _enableCodeCard; }
	void SetCodeCardEnabled(const bool enabled) { _enableCodeCard = enabled; }

	~App() override;
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;

	MainMenu& GetMainMenu();
	Network& GetNetwork();
	Options& GetOptions();
	[[nodiscard]] Options* GetOptionsOrNull() const { return _options; }
	void Initialise();
	void Set(const char* path, const char* version, const char* type, const char* date, const char* title);
	bool Closed();
	void CloseGame();
	void Close();

	void LoadGame(const char* user);
	void SetNextLoadGame(const char* user);
	void LoadGame();
	void SaveGame(const char* user);
	void RetireGame(const char* user);

	void RegisterPhoneDialler(PhoneDialler* dialler);
	void UnRegisterPhoneDialler(PhoneDialler* dialler);
};
