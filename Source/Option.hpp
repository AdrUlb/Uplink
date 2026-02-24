#pragma once

#include "UplinkObject.hpp"

class Option : public UplinkObject
{
	char _name[0x40] = "";
	char _tooltip[0x80] = "";
	bool _yesOrNo = false;
	bool _visible = true;
	int _value = 0;

public:
	~Option() override {} // NOLINT(*-use-equals-default)
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;
	UplinkObjectId GetOBJECTID() override;

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
