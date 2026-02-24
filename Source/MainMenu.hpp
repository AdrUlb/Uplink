#pragma once

#include "MainMenuScreen.hpp"
#include "UplinkObject.hpp"

enum class MainMenuScreenCode
{
	Unknown,
	LoginInterface,
	LoadingInterface,
	FirstTimeLoadingInterface,
	OptionsInterface,
	ObituaryInterface,
	ConnectionLostInterface,
	DisavowedInterface,
	TheTeamInterface,
	ClosingInterface,
	DemoGameOverInterface,
	RevelationWonInterface,
	RevelationLostInterface,
	WarezGameOverInterface,
	GenericOptionsInterface = 20,
	NetworkOptionsInterface,
	ThemeInterface,
	GraphicOptionsInterface,
};

class MainMenu : public UplinkObject
{
	MainMenuScreenCode _screenCode = MainMenuScreenCode::Unknown;
	MainMenuScreen* _screen = nullptr;

public:
	~MainMenu() override;
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;
	void Create();
	void Remove();
	void RunScreen(int screenCode);
	MainMenuScreen& GetMenuScreen();
	int InScreen();
	bool IsVisible();
};
