#pragma once

#include "MainMenu.hpp"
#include "Network.hpp"
#include "Options.hpp"
#include "PhoneDialler.hpp"
#include "UplinkObject.hpp"
#include "Tosser/DArray.hpp"

class App : public UplinkObject
{
	std::string path_ = "c:/";
	std::string usersDir_ = "c:/";
	std::string usersTempDir_ = "c:/";
	std::string usersOldDir_ = "c:/";
	std::string version_ = "1.31c";
	std::string type_ = "RELEASE";
	std::string date_ = "01/01/97";
	std::string title_ = "NewApp";
	std::string build_ = "Version 1.0 (RELEASE), Compiled on 01/01/97";
	int initTime_ = 0;
	bool closed_ = false;
	Options* options_ = nullptr;
	Network* network_ = nullptr;
	MainMenu* mainMenu_ = nullptr;
	PhoneDialler* phoneDialler_ = nullptr;
	char* nextLoadGame_ = nullptr;
	bool enableCodeCard_ = false;

public:
	[[nodiscard]] static DArray<char*>* ListExistingGames();
	static void CoreDump();

	[[nodiscard]] const char* GetPath() const { return path_.c_str(); }
	[[nodiscard]] const char* GetUsersDir() const { return usersDir_.c_str(); }
	[[nodiscard]] const char* GetUsersTempDir() const { return usersTempDir_.c_str(); }
	[[nodiscard]] const char* GetUsersOldDir() const { return usersOldDir_.c_str(); }
	[[nodiscard]] const char* GetVersion() const { return version_.c_str(); }
	[[nodiscard]] const char* GetType() const { return type_.c_str(); }
	[[nodiscard]] const char* GetDate() const { return date_.c_str(); }
	[[nodiscard]] const char* GetTitle() const { return title_.c_str(); }
	[[nodiscard]] const char* GetBuild() const { return build_.c_str(); }
	[[nodiscard]] int32_t GetInitTime() const { return initTime_; }
	[[nodiscard]] bool IsCodeCardEnabled() const { return enableCodeCard_; }
	void SetCodeCardEnabled(const bool enabled) { enableCodeCard_ = enabled; }

	~App() override;
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;

	MainMenu& GetMainMenu();
	Network& GetNetwork();
	Options& GetOptions();
	[[nodiscard]] Options* GetOptionsOrNull() const { return options_; }
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
