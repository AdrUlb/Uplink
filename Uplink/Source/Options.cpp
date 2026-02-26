#include "Options.hpp"

#include "FIXME.hpp"

bool Option::Load(FILE* file)
{
	LoadID(file);
	FileReadStringBuf(_name, file);
	FileReadStringBuf(_tooltip, file);
	FileReadValue(_yesOrNo, file);
	FileReadValue(_visible, file);
	FileReadValue(_value, file);
	LoadID_END(file);
	return true;
}

void Option::Save(FILE* file) { TODO_ABORT; }
void Option::Print() { TODO_ABORT; }

void Option::SetName(const char* name) { TODO_ABORT; }
void Option::SetTooltip(char* tooltip) { TODO_ABORT; }
void Option::SetValue(int value) { TODO_ABORT; }
void Option::SetVisible(bool visible) { TODO_ABORT; }
void Option::SetYesOrNo(bool yesOrNo) { TODO_ABORT; }

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
