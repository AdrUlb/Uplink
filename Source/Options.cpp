#include "Options.hpp"

#include <fstream>
#include <spanstream>
#include "Uplink.hpp"
#include "Util.hpp"
#include "_.hpp"
#include "Redshirt/Redshirt.hpp"

static constexpr auto SAVE_VERSION_MINIMUM = "SAV56";
static constexpr auto SAVE_VERSION_LATEST = "SAV62";

static ColourOption getColourDefault { 0.0f, 0.0f, 0.0f };

Options::~Options()
{
	DeleteBTreeData(options_);

	const auto* const colours = colours_.ConvertToDArray();
	for (size_t i = 0; i < colours->Size(); i++)
	{
		if (colours->ValidIndex(i))
			delete colours->GetData(i);
	}
	delete colours;
}

bool Options::Load(FILE* file)
{
	auto* const usersDir = gApp->GetUsersDir();

	char filepath[0x100];
	UplinkSnprintf(filepath, 0x100, "%soptions", usersDir);

	std::print("Loading uplink options from {}...", filepath);

	file = nullptr;

	const auto fileEncrypted = RsFileEncryptedNoVerify(filepath);

	if ((fileEncrypted && !RsFileEncrypted(filepath)) ||
	    !((file = fileEncrypted ? RsFileOpen(filepath, "rb") : fopen(filepath, "rb"))))
	{
		std::println("failed");
		return false;
	}

	char optionsVersion[6];
	if (!FileReadData(optionsVersion, sizeof(optionsVersion), 1, file))
		optionsVersion[0] = 0;

	auto success = false;

	if (optionsVersion[0] == 0 ||
	    strcmp(optionsVersion, SAVE_VERSION_MINIMUM) < 0 ||
	    strcmp(optionsVersion, SAVE_VERSION_LATEST) > 0)
	{
		std::println("\nERROR : Could not load options due to incompatible version format");
		goto end;
	}

	std::println("success");

	if (!LoadBTree(options_, file))
	{
		DeleteBTreeData(options_);
		goto end;
	}

	success = true;

	uint32_t themeNameLength;
	static_assert(sizeof(themeNameLength) == 4);
	if (fgetc(file) != 't' ||
	    fread(&themeNameLength, sizeof(themeNameLength), 1, file) != 1 ||
	    themeNameLength >= sizeof(themeName_) ||
	    fread(themeName_, themeNameLength, 1, file) != 1)
		goto end;

	themeName_[themeNameLength] = 0;

end:
	if (fileEncrypted)
		RsFileClose(filepath, file);
	else
		fclose(file);
	return success;
}

void Options::Save(FILE* file)
{
	auto* const usersDir = gApp->GetUsersDir();

	MakeDirectory(usersDir);

	char filepath[0x100];
	UplinkSnprintf(filepath, 0x100, "%soptions", usersDir);

	std::print("Saving uplink options to {}...", filepath);
	file = fopen(filepath, "wb");
	if (!file)
	{
		std::print("failed");
		return;
	}
	std::print("success");

	fwrite("SAV62", 6, 1, file);

	SaveBTree(options_, file);

	fputc('t', file);
	const auto themeNameLength = static_cast<uint32_t>(strlen(themeName_));
	static_assert(sizeof(themeNameLength) == 4);
	fwrite(&themeNameLength, sizeof(themeNameLength), 1, file);
	fwrite(themeName_, sizeof(themeNameLength), 1, file);

	fclose(file);

	RsEncryptFile(filepath);

	TODO_ABORT;
}

void Options::Print()
{
	std::print("============== O P T I O N S ===============================");
	PrintBTree(options_);
	std::print("============================================================");
}

void Options::Update() {}

const char* Options::GetID()
{
	return "OPTIONS";
}

Option* Options::GetOption(const char* name) const
{
	return options_.LookupData(name);
}

int Options::GetOptionValue(const char* name) const
{
	const auto* option = GetOption(name);

	if (!option)
		UplinkAbort(std::format("Option {} not found", name));

	return option->GetValue();
}

LList<Option*>* Options::GetAllOptions(const char* query, const bool getHidden) const
{
	auto* const result = new LList<Option*>();

	const auto* const options = options_.ConvertToDArray();
	for (size_t i = 0; i < options->Size(); i += 1)
	{
		if (options->ValidIndex(i))
		{
			auto* const option = options->GetData(i);
			UplinkAssert(option);

			if (!option->GetVisible() && !getHidden)
				continue;

			if (!query || strstr(option->GetName(), query))
				result->PutData(option);
		}
	}
	delete options;

	return result;
}

const ColourOption& Options::GetColour(const char* name) const
{
	const auto colour = colours_.LookupData(name);

	if (colour == nullptr)
		return getColourDefault;

	return *colour;
}

const char* Options::GetThemeName() const
{
	return themeName_;
}

const char* Options::GetThemeTitle() const
{
	return themeTitle_;
}

const char* Options::GetThemeDescription() const
{
	return themeDescription_;
}

bool Options::IsOptionEqualTo(const char* name, const int value) const
{
	const auto option = GetOption(name);

	if (!option)
		return false;

	return option->GetValue() == value;
}

void Options::SetOptionValue(const char* name, const int value) const
{
	const auto node = options_.LookupTree(name);

	if (!node)
	{
		std::println("Tried to set unrecognized option: {}", name);
		return;
	}

	auto* const option = node->GetData();
	UplinkAssert(option);
	option->SetValue(value);
}

void Options::SetOptionValue(const char* name, const int value, const char* tooltip, const bool yesOrNo, const bool visible)
{
	const auto node = options_.LookupTree(name);

	if (!node)
	{
		auto* option = new Option();
		option->SetName(name);
		option->SetValue(value);
		option->SetTooltip(tooltip);
		option->SetYesOrNo(yesOrNo);
		option->SetVisible(visible);
		options_.PutData(name, option);
		return;
	}

	auto* const option = node->GetData();
	UplinkAssert(option);
	option->SetValue(value);
	option->SetTooltip(tooltip);
	option->SetYesOrNo(yesOrNo);
	option->SetVisible(visible);
}

void Options::SetThemeName(const char* name)
{
	UplinkStrncpy(themeName_, name, sizeof(themeName_));

	const char* filePath = ThemeFilename("theme.txt");
	const auto* const rsFilePath = RsArchiveFileOpen(filePath);

	if (!rsFilePath)
	{
		RsArchiveFileClose(filePath, nullptr);
		delete filePath;
	}

	std::ifstream filestr(rsFilePath);

	char temp[0x40];

	std::print("REMOVE THIS");

	filestr >> temp >> std::ws;
	filestr.getline(themeTitle_, sizeof(themeTitle_));

	filestr >> temp >> std::ws;
	filestr.getline(themeAuthor_, sizeof(themeAuthor_));

	filestr >> temp >> std::ws;
	filestr.getline(themeDescription_, sizeof(themeDescription_));

	std::print("Loaded theme: {} by {}\nDescription: {}\n", themeTitle_, themeAuthor_, themeDescription_);

	while (!filestr.eof())
	{
		char line[0x100];
		filestr >> std::ws;
		filestr.getline(line, sizeof(line));
		if (line[0] == 0 || line[0] == ';')
			continue;

		char colorname[0x40];
		float red, green, blue;
		std::ispanstream linestr(line);
		linestr >> std::ws >> colorname >> red >> green >> blue;
		std::print("Color {}: R={} G={} B={}\n", colorname, red, green, blue);

		auto* node = colours_.LookupTree(colorname);
		if (!node)
		{
			colours_.PutData(colorname, new ColourOption { red, green, blue });
		}
		else
		{
			delete node->GetData();
			node->SetData(new ColourOption { red, green, blue });
		}
	}

	RsArchiveFileClose(filePath, nullptr);
	delete filePath;
}

void Options::CreateDefaultOptions()
{
	if (!GetOption("game_debugstart"))
		SetOptionValue("game_debugstart", 1, "z", true, false);

	if (!GetOption("game_firsttime"))
	{
		const auto* const existingGames = App::ListExistingGames();

		const auto existingGamesCount = existingGames->Size();

		SetOptionValue("game_firsttime", existingGamesCount == 0, "z", true, false);
		for (size_t i = 0; i < existingGamesCount; i++)
		{
			if (existingGames->ValidIndex(i))
				delete[] existingGames->GetData(i);
		}

		delete existingGames;
	}

	float version;
	std::ispanstream(UPLINK_VERSION) >> version;
	version *= 100.0f;

	if (!GetOption("game_version"))
		SetOptionValue("game_version", static_cast<int>(version), "z", false, false);

	if (!GetOption("graphics_screenwidth"))
		SetOptionValue("graphics_screenwidth", 1024, "Sets the width of the screen", false, false);

	if (!GetOption("graphics_screenheight"))
		SetOptionValue("graphics_screenheight", 768, "Sets the height of the screen", false, false);

	if (!GetOption("graphics_screendepth"))
		SetOptionValue("graphics_screendepth", -1, "Sets the colour depth. -1 Means use desktop colour depth.", false, false);

	if (!GetOption("graphics_screenrefresh"))
		SetOptionValue("graphics_screenrefresh", -1, "Sets the refresh rate. -1 Means use desktop refresh.", false, false);

	if (!GetOption("graphics_fullscreen"))
		SetOptionValue("graphics_fullscreen", true, "Sets the game to run fullscreen or in a window", true, true);

	if (!GetOption("graphics_buttonanimations"))
		SetOptionValue("graphics_buttonanimations", true, "Enables or disables button animations", true, true);

	if (!GetOption("graphics_safemode"))
		SetOptionValue("graphics_safemode", false, "Enables graphical safemode for troubleshooting", true, true);

	if (!GetOption("graphics_softwaremouse"))
		SetOptionValue("graphics_softwaremouse", false, "Render a software mouse.  Use to correct mouse problems.", true, true);

	if (!GetOption("graphics_fasterbuttonanimations"))
		SetOptionValue("graphics_fasterbuttonanimations", false, "Increase the speed of button animations.", true, true);

	if (!GetOption("graphics_defaultworldmap"))
		SetOptionValue("graphics_defaultworldmap", false, "Create agents with the default world map.", true, true);

	if (!GetOption("graphics_softwarerendering"))
		SetOptionValue("graphics_softwarerendering", false, "Enable software rendering.", true, false);

	if (!GetOption("sound_musicenabled"))
		SetOptionValue("sound_musicenabled", true, "Enables or disables music", true, true);

	GetOption("graphics_softwarerendering")->SetVisible(false);
	GetOption("graphics_screenwidth")->SetVisible(false);
	GetOption("graphics_screenheight")->SetVisible(false);
	GetOption("graphics_screendepth")->SetVisible(false);
	GetOption("graphics_screenrefresh")->SetVisible(false);
}

void Options::RequestShutdownChange(const char* name, const int value)
{
	auto* const optionChange = new OptionChange();
	UplinkStrncpy(optionChange->name, name, sizeof(optionChange->name));
	optionChange->value = value;
	shutdownChanges_.PutData(optionChange);
	TODO_ABORT;
}

void Options::ApplyShutdownChanges()
{
	while (true)
	{
		const auto* const data = shutdownChanges_.GetData(0);
		if (!data)
			break;

		shutdownChanges_.RemoveData(0);
		SetOptionValue(data->name, data->value);
		delete data;
		TODO_ABORT;
	}
}

char* Options::ThemeFilename(const char* filename)
{
	auto* result = new char[0x100];

	if (strcmp(themeName_, "graphics") == 0)
	{
		UplinkSnprintf(result, 0x100, "graphics/%s", filename);
		return result;
	}

	char filepath[0x100];
	UplinkSnprintf(filepath, 0x100, "%s%s/%s", gApp->GetPath(), themeName_, filename);

	if (DoesFileExist(filepath))
	{
		UplinkSnprintf(result, 0x100, "%s/%s", themeName_, filename);
	}
	else
	{
		UplinkSnprintf(result, 0x100, "graphics/%s", filename);
	}

	return result;
}
