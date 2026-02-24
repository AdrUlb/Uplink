#include "MainMenuScreen.hpp"

#include "Uplink.hpp"
#include "_.hpp"

void MainMenuScreen::RegisterButton(int x, int y, int width, int height, const char* caption, const char* name)
{
	TODO_ABORT;
}

void MainMenuScreen::RegisterButton(int x, int y, int width, int height, const char* caption, const char* tooltip, const char* name)
{
	TODO_ABORT;
}

MainMenuScreen::~MainMenuScreen()
{
	DeleteDArrayData(buttons_);
	delete buttons_;
	buttons_ = nullptr;
}

void MainMenuScreen::Create()
{
	//const auto screenWidth = gApp->GetOptions().GetOptionValue("graphics_screenwidth");
	//const auto screenHeight = gApp->GetOptions().GetOptionValue("graphics_screenwidth");
	RegisterButton(GetScaledXPosition(320) - 170, 75, 425, 60, "", "mainmenu_background");
	button_assignbitmap("mainmenu_background", "mainmenu/uplinklogo.tif");
	return EclRegisterButtonCallbacks("mainmenu_background", imagebutton_draw, nullptr, nullptr, nullptr);
}

void MainMenuScreen::Remove()
{
	int32_t result;

	for (size_t i = 0; i < buttons_->Size(); i++)
	{
		if (!buttons_->ValidIndex(i))
			continue;

		const auto* button = buttons_->GetData(i);

		if (button && button[0] != 0)
			EclRemoveButton(button);
	}
}

void MainMenuScreen::Update()
{
	TODO_ABORT;
}

bool MainMenuScreen::IsVisible()
{
	TODO_ABORT;
}

int MainMenuScreen::ScreenID()
{
	TODO_ABORT;
}

bool MainMenuScreen::ReturnKeyPressed()
{
	TODO_ABORT;
}
