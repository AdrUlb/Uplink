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

void Option::SetTooltip(char* tooltip)
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

	if (fgetc(file) == 't' && LoadData(&themeNameLength, 4, file) && themeNameLength + 1 < THEME_NAME_BUFFER_MAX && fread(themeName, themeNameLength, 1, file) == 1)
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

void Options::Print() { NOTIMPL_ABORT; }

void Options::CreateDefaultOptions() { NOTIMPL_ABORT; }
void Options::RequestShutdownChange(const char* name, int value) { NOTIMPL_ABORT; }
void Options::ApplyShutdownChanges() { NOTIMPL_ABORT; }

Option* Options::GetOption(const char* name) { NOTIMPL_ABORT; }
int Options::GetOptionValue(const char* name) { NOTIMPL_ABORT; }
const char* Options::GetThemeDescription() { NOTIMPL_ABORT; }
const char* Options::GetThemeName() { NOTIMPL_ABORT; }
const char* Options::GetThemeTitle() { NOTIMPL_ABORT; }
bool Options::IsOptionEqualTo(const char* name, int value) { NOTIMPL_ABORT; }
void Options::SetOptionValue(const char* name, int value) { NOTIMPL_ABORT; }
void Options::SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible) { NOTIMPL_ABORT; }
void Options::SetThemeName(const char* name) { NOTIMPL_ABORT; }
char* Options::ThemeFilename(const char* name) { NOTIMPL_ABORT; }

LList<Option*>* Options::GetAllOptions(const char* query, bool includeInvisible) { NOTIMPL_ABORT; }
ColourOption* Options::GetColour(const char* name) { NOTIMPL_ABORT; }
