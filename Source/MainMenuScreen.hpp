#pragma once

#include "Tosser/DArray.hpp"

class MainMenuScreen
{
	DArray<char*>* _buttons = new DArray<char*>();

public:
	virtual void RegisterButton(int x, int y, int width, int height, const char* caption, const char* name);
	virtual void RegisterButton(int x, int y, int width, int height, const char* caption, const char* tooltip, const char* name);
	virtual ~MainMenuScreen();
	virtual void Create();
	virtual void Remove();
	virtual void Update();
	virtual bool IsVisible();
	virtual int ScreenID();
	virtual bool ReturnKeyPressed();
};
