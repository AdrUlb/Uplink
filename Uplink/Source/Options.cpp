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

#include "FIXME.hpp"

Options::~Options() { TODO_ABORT; }
bool Options::Load(FILE* file) { TODO_ABORT; }
void Options::Save(FILE* file) { TODO_ABORT; }
void Options::Print() { TODO_ABORT; }

void Options::CreateDefaultOptions() { TODO_ABORT; }
void Options::RequestShutdownChange(const char* name, int value) { TODO_ABORT; }
void Options::ApplyShutdownChanges() { TODO_ABORT; }

Option* Options::GetOption(const char* name) { TODO_ABORT; }
int Options::GetOptionValue(const char* name) { TODO_ABORT; }
const char* Options::GetThemeDescription() { TODO_ABORT; }
const char* Options::GetThemeName() { TODO_ABORT; }
const char* Options::GetThemeTitle() { TODO_ABORT; }
bool Options::IsOptionEqualTo(const char* name, int value) { TODO_ABORT; }
void Options::SetOptionValue(const char* name, int value) { TODO_ABORT; }
void Options::SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible) { TODO_ABORT; }
void Options::SetThemeName(const char* name) { TODO_ABORT; }
char* Options::ThemeFilename(const char* name) { TODO_ABORT; }

LList<Option*>* Options::GetAllOptions(const char* query, bool includeInvisible) { TODO_ABORT; }
ColourOption* Options::GetColour(const char* name) { TODO_ABORT; }
