#pragma once

#include "ColourOption.hpp"
#include "Option.hpp"
#include "OptionChange.hpp"
#include "UplinkObject.hpp"
#include "Tosser/BTree.hpp"
#include "Tosser/LList.hpp"

class Options : public UplinkObject
{
	BTree<Option*> _options;
	LList<OptionChange*> _shutdownChanges;
	char _themeName[0x80] = "graphics";
	char _themeAuthor[0x80] = "";
	char _themeTitle[0x80] = "";
	char _themeDescription[0x400] = "";
	BTree<ColourOption*> _colours;

public:
	~Options() override;
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;

	[[nodiscard]] Option* GetOptionOrNull(const char* name) const;
	[[nodiscard]] int GetOptionValue(const char* name) const;
	[[nodiscard]] LList<Option*>* GetAllOptions(const char* query, bool getHidden) const;
	[[nodiscard]] const ColourOption& GetColour(const char* name) const;
	[[nodiscard]] const char* GetThemeName() const;
	[[nodiscard]] const char* GetThemeTitle() const;
	[[nodiscard]] const char* GetThemeDescription() const;
	[[nodiscard]] bool IsOptionEqualTo(const char* name, int value) const;
	void SetOptionValue(const char* name, int value) const;
	void SetOptionValue(const char* name, int value, const char* tooltip, bool yesOrNo, bool visible);
	void SetThemeName(const char* name);
	void CreateDefaultOptions();
	void RequestShutdownChange(const char* name, int value);
	void ApplyShutdownChanges();
	[[nodiscard]] std::string ThemeFilename(const char* filename);
};
