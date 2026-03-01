#pragma once

#include "UplinkObject.hpp"
#include "Tosser/BTree.hpp"
#include "Tosser/LList.hpp"

struct OptionChange
{
	static constexpr size_t NAME_BUFFER_MAX = 0x40;

	char Name[NAME_BUFFER_MAX];
	int Value;
};

struct ColourOption
{
	float Red;
	float Green;
	float Blue;
};

class Option : public UplinkObject
{
	static constexpr size_t NAME_BUFFER_MAX = 0x40;
	static constexpr size_t TOOLTIP_BUFFER_MAX = 0x20;

	char _name[NAME_BUFFER_MAX] { };
	char _tooltip[TOOLTIP_BUFFER_MAX] = "";
	bool _yesOrNo = false;
	bool _visible = true;
	int _value = 0;

public:
	[[nodiscard]] const char* Name() const { return _name; }
	[[nodiscard]] const char* Tooltip() const { return _tooltip; }
	[[nodiscard]] bool YesOrNo() const { return _yesOrNo; }
	[[nodiscard]] bool Visible() const { return _visible; }
	[[nodiscard]] int Value() const { return _value; }

	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	const char* GetID() override { return "OPTION"; }
	UplinkObjectID GetOBJECTID() override { return UplinkObjectID::Option; }

	void SetName(const char* name);
	void SetTooltip(const char* tooltip);
	void SetValue(const int value) { _value = value; }
	void SetVisible(const bool visible) { _visible = visible; }
	void SetYesOrNo(const bool yesOrNo) { _yesOrNo = yesOrNo; }
};

class Options : public UplinkObject
{
	static constexpr size_t THEME_NAME_BUFFER_MAX = 128;
	static constexpr size_t THEME_AUTHOR_BUFFER_MAX = 128;
	static constexpr size_t THEME_TITLE_BUFFER_MAX = 128;
	static constexpr size_t THEME_DESCRIPTION_BUFFER_MAX = 1024;

	BTree<Option*> _options;
	LList<OptionChange*> _changes;
	char _themeName[THEME_NAME_BUFFER_MAX] = "graphics";
	char _themeAuthor[THEME_AUTHOR_BUFFER_MAX] { };
	char _themeTitle[THEME_TITLE_BUFFER_MAX] { };
	char _themeDescription[THEME_DESCRIPTION_BUFFER_MAX] { };
	BTree<ColourOption*> _colours;

public:
	~Options() override;
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	const char* GetID() override { return "OPTIONS"; }

	void CreateDefaultOptions();
	void RequestShutdownChange(const char* name, int value);
	void ApplyShutdownChanges();

	Option* GetOption(const char* name);
	int GetOptionValue(const char* name);
	[[nodiscard]] const char* GetThemeName() const { return _themeName; }
	[[nodiscard]] const char* GetThemeTitle() const { return _themeTitle; }
	[[nodiscard]] const char* GetThemeDescription() const { return _themeDescription; }
	bool IsOptionEqualTo(const char* name, int value);
	void SetOptionValue(const char* name, int value);
	void SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible);
	void SetThemeName(const char* name);
	char* ThemeFilename(const char* name);

	LList<Option*>* GetAllOptions(const char* query, bool includeInvisible);
	ColourOption* GetColour(const char* name);
};
