#include "Options.hpp"

#include "Redshirt.hpp"
#include "Uplink.hpp"

static constexpr auto SAVE_VERSION_CURRENT = "SAV62";
static constexpr auto SAVE_VERSION_MIN = "SAV56";

bool Option::Load(FILE* file)
{
	LoadID(file);

	if (!LoadFixedString(_name, sizeof(_name), file))
		return false;

	if (!LoadFixedString(_tooltip, sizeof(_tooltip), file))
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
	SaveFixedString(_name, file);
	SaveFixedString(_tooltip, file);
	WriteData(&_yesOrNo, 1, file);
	WriteData(&_visible, 1, file);
	WriteData(&_value, 1, file);
	SaveID_END(file);
}

void Option::Print()
{
	std::println("Option : name={}, value={}", _name, _value);
	std::println("\tYesOrNo={}, Visible={}", _yesOrNo, _visible);
}

void Option::SetName(const char* name)
{
	UplinkAssert(strlen(name) < sizeof(_name));
	UplinkStrncpy(_name, name, sizeof(_name));
}

void Option::SetTooltip(char* tooltip)
{
	UplinkAssert(strlen(tooltip) < sizeof(_tooltip));
	UplinkStrncpy(_tooltip, tooltip, sizeof(_tooltip));
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
	char path[256];
	UplinkSnprintf(path, sizeof(path), "%soptions", app->UsersPath());

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
		DeleteBTreeData(&this->_options);
		return false;
	}

	LoadID_END(file);

	char src[128];
	uint32_t ptr;

	if (fgetc(file) == 't' && LoadData(&ptr, 4, file) && ptr + 1 < sizeof(src) && fread(src, ptr, 1, file) == 1)
	{
		UplinkStrncpy(this->_themeName, src, sizeof(src));
		src[ptr] = 0;
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

void Options::Save(FILE* file) { NOTIMPL_ABORT; }
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
