#include "Options.hpp"

bool Option::Load(FILE* file)
{
	LoadID(file);
	Load_StringBuf(_name, file);
	Load_StringBuf(_tooltip, file);
	Load_Field(_yesOrNo, file);
	Load_Field(_visible, file);
	Load_Field(_value, file);
	LoadID_END(file);
	return true;
}

void Option::Save(FILE* file)
{
	SaveID(file);
	Save_StringBuf(_name, file);
	Save_StringBuf(_tooltip, file);
	Save_Field(_yesOrNo, file);
	Save_Field(_visible, file);
	Save_Field(_value, file);
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

#include "NOTIMPL.hpp"

Options::~Options()
{
	DeleteBTreeData(&_options);

	const auto colours = _colours.ConvertToDArray();
	for (auto i = 0; i < colours->Size(); i++)
		if (colours->ValidIndex(i))
			delete colours->GetData(i);

	delete colours;
}

bool Options::Load(FILE* file) { NOTIMPL_ABORT; }
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
