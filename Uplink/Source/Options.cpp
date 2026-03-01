#include "Options.hpp"

#include "Redshirt.hpp"
#include "Uplink.hpp"

static constexpr auto SAVE_VERSION_CURRENT = "SAV62";
static constexpr auto SAVE_VERSION_MIN = "SAV56";

bool Option::Load(FILE* file)
{
	LoadID(file);

	if (!LoadFixedString(_name, NAME_BUFFER_MAX, file))
		return false;

	if (!LoadFixedString(_tooltip, TOOLTIP_BUFFER_MAX, file))
		return false;

	if (!LoadData(&_yesOrNo, 1, file))
		return false;

	if (!LoadData(&_visible, 1, file))
		return false;

	if (!LoadData(&_value, 1, file))
		return false;

	LoadID_END(file);
	return true;
}

void Option::Save(FILE* file)
{
	SaveID(file);
	SaveFixedString(_name, NAME_BUFFER_MAX, file);
	SaveFixedString(_tooltip, TOOLTIP_BUFFER_MAX, file);
	SaveData(&_yesOrNo, 1, file);
	SaveData(&_visible, 1, file);
	SaveData(&_value, 1, file);
	SaveID_END(file);
}

void Option::Print()
{
	std::println("Option : name={}, value={}", _name, _value);
	std::println("\tYesOrNo={}, Visible={}", _yesOrNo, _visible);
}

void Option::SetName(const char* name)
{
	UplinkAssert(strlen(name) < NAME_BUFFER_MAX);
	UplinkStrncpy(_name, name, NAME_BUFFER_MAX);
}

void Option::SetTooltip(const char* tooltip)
{
	UplinkAssert(strlen(tooltip) < TOOLTIP_BUFFER_MAX);
	UplinkStrncpy(_tooltip, tooltip, TOOLTIP_BUFFER_MAX);
}

Options::~Options()
{
	DeleteBTreeData(&_options);

	const auto colours = _colours.ConvertToDArray();
	for (auto i = 0; i < colours->Size(); i++)
		if (colours->ValidIndex(i))
			delete colours->GetData(i);

	delete colours;
}

#include "NOTIMPL.hpp"

bool Options::Load(FILE* file)
{
	char path[PATH_MAX];
	UplinkSnprintf(path, PATH_MAX, "%soptions", app->UsersPath());

	std::print("Loading uplink options from {}...", path);

	const auto encrypted = RsFileEncryptedNoVerify(path);

	auto ret = false;
	file = nullptr;
	if (encrypted)
	{
		if (!RsFileEncrypted(path))
			goto end;

		file = RsFileOpen(path, "rb");
	}
	else
	{
		file = fopen(path, "rb");
	}

	if (!file)
		goto end;

	char version[6];
	if (!LoadFixedString(version, sizeof(version), file) || version[0] == 0 ||
	    strcmp(version, SAVE_VERSION_MIN) < 0 ||
	    strcmp(version, SAVE_VERSION_CURRENT) > 0)
	{
		std::print("\nERROR : Could not load options due to incompatible version format");

		if (encrypted)
			RsFileClose(path, file);
		else
			fclose(file);

		return false;
	}

	std::print("success");

	LoadID(file);

	if (!LoadBTree(&_options, file))
	{
		DeleteBTreeData(&_options);
		return false;
	}

	LoadID_END(file);

	uint32_t themeNameLength;
	char themeName[THEME_NAME_BUFFER_MAX];

	if (fgetc(file) == 't' && LoadData(&themeNameLength, 4, file) && themeNameLength + 1 < THEME_NAME_BUFFER_MAX && fread(themeName, themeNameLength, 1, file)
	    == 1)
	{
		UplinkStrncpy(_themeName, themeName, THEME_NAME_BUFFER_MAX);
		themeName[themeNameLength] = 0;
	}

	ret = true;

end:
	if (file)
	{
		if (encrypted)
			RsFileClose(path, file);
		else
			fclose(file);
	}

	if (!ret)
		std::print("failed");

	return ret;
}

void Options::Save(FILE* file)
{
	MakeDirectory(app->UsersPath());

	char path[PATH_MAX];
	UplinkSnprintf(path, PATH_MAX, "%soptions", app->UsersPath());

	printf("Saving uplink options to %s...", path);
	file = fopen(path, "wb");

	if (!file)
	{
		std::print("failed");
		return;
	}

	std::print("success");

	fwrite(SAVE_VERSION_CURRENT, 6, 1, file);

	SaveID(file);
	SaveBTree(&_options, file);
	SaveID_END(file);

	fputc('t', file);
	const uint32_t themeNameSize = strlen(this->_themeName);
	SaveData(&themeNameSize, 4, file);
	SaveFixedString(_themeName, themeNameSize, file);

	fwrite(_themeName, themeNameSize, 1, file);
	fclose(file);
	RsEncryptFile(path);
}

void Options::Print()
{
	std::println("============== O P T I O N S ===============================");
	PrintBTree(&_options);
	std::println("============================================================");
}

void Options::CreateDefaultOptions()
{
	if (!GetOption("game_debugstart"))
		SetOptionValue("game_debugstart", 1, "z", true, false);

	if (!GetOption("game_firsttime"))
	{
		const auto existingGames = App::ListExistingGames();
		const int existingGameCount = existingGames->Size();
		if (existingGameCount <= 0)
		{
			SetOptionValue("game_firsttime", 1, "z", true, false);
		}
		else
		{
			SetOptionValue("game_firsttime", 0, "z", true, false);
			for (int i = 0; i < existingGameCount; i++)
			{
				if (!existingGames->ValidIndex(i))
					continue;

				delete[] existingGames->GetData(i);
			}
		}

		delete existingGames;
	}

	const auto gameVersion = static_cast<int>(strtof(UPLINK_VERSION, nullptr) * 100.0f);

	if (!GetOption("game_version"))
		SetOptionValue("game_version", gameVersion, "z", false, false);

	if (!GetOption("graphics_screenwidth"))
		SetOptionValue("graphics_screenwidth", 1024, "Sets the width of the screen", false, false);

	if (!GetOption("graphics_screenheight"))
		SetOptionValue("graphics_screenheight", 768, "Sets the height of the screen", false, false);

	if (!GetOption("graphics_screendepth"))
		SetOptionValue("graphics_screendepth", -1, "Sets the colour depth. -1 Means use desktop colour depth.", false, false);

	if (!GetOption("graphics_screenrefresh"))
		SetOptionValue("graphics_screenrefresh", -1, "Sets the refresh rate. -1 Means use desktop refresh.", false, false);

	if (!GetOption("graphics_fullscreen"))
		SetOptionValue("graphics_fullscreen", 1, "Sets the game to run fullscreen or in a window", true, true);

	if (!GetOption("graphics_buttonanimations"))
		SetOptionValue("graphics_buttonanimations", 1, "Enables or disables button animations", true, true);

	if (!GetOption("graphics_safemode"))
		SetOptionValue("graphics_safemode", 0, "Enables graphical safemode for troubleshooting", true, true);

	if (!GetOption("graphics_softwaremouse"))
		SetOptionValue("graphics_softwaremouse", 0, "Render a software mouse.  Use to correct mouse problems.", true, true);

	if (!GetOption("graphics_fasterbuttonanimations"))
		SetOptionValue("graphics_fasterbuttonanimations", 0, "Increase the speed of button animations.", true, true);

	if (!GetOption("graphics_defaultworldmap"))
		SetOptionValue("graphics_defaultworldmap", 0, "Create agents with the default world map.", true, true);

	if (const auto o = GetOption("graphics_softwarerendering"); !o)
		SetOptionValue("graphics_softwarerendering", 0, "Enable software rendering.", true, false);
	else
		o->SetVisible(false);

	if (!GetOption("sound_musicenabled"))
		SetOptionValue("sound_musicenabled", 1, "Enables or disables music", true, true);

	GetOption("graphics_screenwidth")->SetVisible(false);
	GetOption("graphics_screenheight")->SetVisible(false);
	GetOption("graphics_screendepth")->SetVisible(false);
	GetOption("graphics_screenrefresh")->SetVisible(false);
}

void Options::RequestShutdownChange(const char* name, const int value)
{
	const auto change = new OptionChange();

	UplinkStrncpy(change->Name, name, OptionChange::NAME_BUFFER_MAX);
	change->Value = value;

	_changes.PutData(change);
}

void Options::ApplyShutdownChanges()
{
	while (_changes.GetData(0))
	{
		const auto change = _changes.GetData(0);
		_changes.RemoveData(0);
		SetOptionValue(change->Name, change->Value);
		delete change;
	}
}

Option* Options::GetOption(const char* name)
{
	return _options.GetData(name);
}

int Options::GetOptionValue(const char* name)
{
	const auto option = GetOption(name);
	if (!option)
	{
		char buffer[256]; // [esp+20h] [ebp-108h] BYREF
		UplinkSnprintf(buffer, 0x100u, "Option %s not found", name);
		UplinkAbort(buffer);
	}

	return option->Value();
}

bool Options::IsOptionEqualTo(const char* name, const int value)
{
	const auto option = GetOption(name);
	return option && option->Value() == value;
}

void Options::SetOptionValue(const char* name, int value)
{
	const auto tree = _options.LookupTree(name);

	if (!tree)
	{
		std::println("Tried to set unrecognised option: {}", name);
		return;
	}

	const auto option = tree->NodeData();
	UplinkAssert(option);
	option->SetValue(value);
}

void Options::SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible)
{
	const auto tree = _options.LookupTree(name);

	if (tree)
	{
		const auto option = tree->NodeData();
		UplinkAssert(option);
		option->SetValue(value);
		option->SetTooltip(tooltip);
		option->SetYesOrNo(yesOrNo);
		option->SetVisible(visible);
		return;
	}

	const auto option = new Option();
	option->SetName(name);
	option->SetValue(value);
	option->SetTooltip(tooltip);
	option->SetYesOrNo(yesOrNo);
	option->SetVisible(visible);
	_options.PutData(name, option);
}

void Options::SetThemeName(const char* name) { NOTIMPL_ABORT; }
char* Options::ThemeFilename(const char* name) { NOTIMPL_ABORT; }

LList<Option*>* Options::GetAllOptions(const char* query, bool includeInvisible) { NOTIMPL_ABORT; }
ColourOption* Options::GetColour(const char* name) { NOTIMPL_ABORT; }
