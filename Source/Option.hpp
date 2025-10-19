#pragma once

#include "UplinkObject.hpp"

class Option : public UplinkObject
{
	char name_[0x40] = "";
	char tooltip_[0x80] = "";
	bool yesOrNo_ = false;
	bool visible_ = true;
	int value_ = 0;

public:
	~Option() override {} // NOLINT(*-use-equals-default)
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;
	UplinkObjectID GetOBJECTID() override;

	[[nodiscard]] const char* GetName() const;
	[[nodiscard]] const char* GetTooltip() const;
	[[nodiscard]] bool GetYesOrNo() const;
	[[nodiscard]] bool GetVisible() const;
	[[nodiscard]] int GetValue() const;

	void SetName(const char* name);
	void SetTooltip(const char* tooltip);
	void SetYesOrNo(bool yesOrNo);
	void SetVisible(bool visible);
	void SetValue(int value);
};
