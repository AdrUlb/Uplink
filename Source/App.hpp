#pragma once

#include "MainMenu.hpp"
#include "Network.hpp"
#include "Options.hpp"
#include "PhoneDialler.hpp"
#include "UplinkObject.hpp"
#include "Tosser/DArray.hpp"

class App : public UplinkObject
{
	char path_[0x100] = "c:/";
	char usersDir_[0x100] = "c:/";
	char usersTempDir_[0x100] = "c:/";
	char usersOldDir_[0x100] = "c:/";
	char version_[0x20] = "1.31c";
	char type_[0x20] = "RELEASE";
	char date_[0x20] = "01/01/97";
	char title_[0x40] = "NewApp";
	char build_[0x100] = "Version 1.0 (RELEASE), Compiled on 01/01/97";
	int32_t initTime_ = 0;
	bool closed_ = false;
	Options* options_ = nullptr;
	Network* network_ = nullptr;
	MainMenu* mainMenu_ = nullptr;
	PhoneDialler* phoneDialler_ = nullptr;
	char* nextLoadGame_ = nullptr;
	bool enableCodeCard_ = false;

public:
	[[nodiscard]] static DArray<char*>* ListExistingGames();

	[[nodiscard]] const char* GetPath() const { return path_; }
	[[nodiscard]] const char* GetUsersDir() const { return usersDir_; }
	[[nodiscard]] const char* GetUsersTempDir() const { return usersTempDir_; }
	[[nodiscard]] const char* GetUsersOldDir() const { return usersOldDir_; }
	[[nodiscard]] const char* GetVersion() const { return version_; }
	[[nodiscard]] const char* GetType() const { return type_; }
	[[nodiscard]] const char* GetDate() const { return date_; }
	[[nodiscard]] const char* GetTitle() const { return title_; }
	[[nodiscard]] const char* GetBuild() const { return build_; }
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
	void Initialise();
	void Set(const char* path, const char* version, const char* type, const char* date, const char* title);
	bool Closed();
	void CloseGame();
	void Close();
	void CoreDump();

	void LoadGame(const char* user);
	void SetNextLoadGame(const char* user);
	void LoadGame();
	void SaveGame(const char* user);
	void RetireGame(const char* user);

	void RegisterPhoneDialler(PhoneDialler* dialler);
	void UnRegisterPhoneDialler(PhoneDialler* dialler);
};
