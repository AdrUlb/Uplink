#pragma once

#include "UplinkObject.hpp"
#include "Tosser/BTree.hpp"
#include "Tosser/LList.hpp"

struct OptionChange
{
	char Name[64];
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
	char _name[64] { };
	char _tooltip[128] = "";
	bool _yesOrNo = false;
	bool _visible = true;
	int _value = 0;

	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	const char* GetID() override { return "OPTION"; }
	UplinkObjectID GetOBJECTID() override { return UplinkObjectID::Option; }

	void SetName(const char* name);
	void SetTooltip(char* tooltip);
	void SetValue(int value);
	void SetVisible(bool visible);
	void SetYesOrNo(bool yesOrNo);
};

class Options : public UplinkObject
{
	BTree<Option*> _options;
	LList<OptionChange*> _changes;
	char _themeName[128] = "graphics";
	char _themeAuthor[128] { };
	char _themeTitle[128] { };
	char _themeDescription[1024] { };
	BTree<ColourOption*> _colours;

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
	const char* GetThemeDescription();
	const char* GetThemeName();
	const char* GetThemeTitle();
	bool IsOptionEqualTo(const char* name, int value);
	void SetOptionValue(const char* name, int value);
	void SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible);
	void SetThemeName(const char* name);
	char* ThemeFilename(const char* name);

	LList<Option*>* GetAllOptions(const char* query, bool includeInvisible);
	ColourOption* GetColour(const char* name);
};
