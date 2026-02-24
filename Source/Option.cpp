#include "Option.hpp"

#include <cstring>
#include <print>

#include "Util.hpp"

bool Option::Load(FILE* file)
{
	// Sanity checks
	static_assert(sizeof(_name) == 0x40);
	static_assert(sizeof(_tooltip) == 0x80);
	static_assert(sizeof(_yesOrNo) == 1);
	static_assert(sizeof(_visible) == 1);
	static_assert(sizeof(_value) == 4);

	if (!FileReadData(_name, sizeof(_name), 1, file))
	{
		strcpy(_name, "");
		return false;
	}
	_name[sizeof(_name) - 1] = 0;

	if (!FileReadData(_tooltip, sizeof(_tooltip), 1, file))
	{
		strcpy(_tooltip, "");
		return false;
	}
	_tooltip[sizeof(_tooltip) - 1] = 0;

	if (!FileReadData(&_yesOrNo, sizeof(_yesOrNo), 1, file))
		return false;

	if (!FileReadData(&_visible, sizeof(_visible), 1, file))
		return false;

	if (!FileReadData(&_value, sizeof(_value), 1, file))
		return false;

	return true;
}

void Option::Save(FILE* file)
{
	fwrite(_name, sizeof(_name), 1, file);
	fwrite(_tooltip, sizeof(_tooltip), 1, file);
	fwrite(&_yesOrNo, sizeof(_yesOrNo), 1, file);
	fwrite(&_visible, sizeof(_visible), 1, file);
	fwrite(&_value, sizeof(_value), 1, file);
	SaveID_END(file);
}

void Option::Print()
{
	std::print("Option : name={}, value={}\n", _name, _value);
	std::print("\tYesOrNo={}, Visible={}\n", _yesOrNo, _visible);
}

void Option::Update() {}

const char* Option::GetID()
{
	return "OPTION";
}

UplinkObjectId Option::GetOBJECTID()
{
	return UplinkObjectId::Option;
}

const char* Option::GetName() const
{
	return _name;
}

const char* Option::GetTooltip() const
{
	return _tooltip;
}

bool Option::GetYesOrNo() const
{
	return _yesOrNo;
}

bool Option::GetVisible() const
{
	return _visible;
}

int Option::GetValue() const
{
	return _value;
}

void Option::SetName(const char* name)
{
	UplinkAssert(strlen(name) < sizeof(_name));
	UplinkStrncpy(_name, name, sizeof(_name));
}

void Option::SetTooltip(const char* tooltip)
{
	UplinkAssert(strlen(tooltip) < sizeof(_tooltip));
	UplinkStrncpy(_tooltip, tooltip, sizeof(_tooltip));
}

void Option::SetYesOrNo(const bool yesOrNo)
{
	_yesOrNo = yesOrNo;
}

void Option::SetVisible(const bool visible)
{
	_visible = visible;
}

void Option::SetValue(const int value)
{
	_value = value;
}
