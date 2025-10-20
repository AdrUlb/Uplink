#include "Option.hpp"

#include <cstring>
#include <print>

#include "Util.hpp"

bool Option::Load(FILE* file)
{
	// Sanity checks
	static_assert(sizeof(name_) == 0x40);
	static_assert(sizeof(tooltip_) == 0x80);
	static_assert(sizeof(yesOrNo_) == 1);
	static_assert(sizeof(visible_) == 1);
	static_assert(sizeof(value_) == 4);

	if (!FileReadData(name_, sizeof(name_), 1, file))
	{
		strcpy(name_, "");
		return false;
	}
	name_[sizeof(name_) - 1] = 0;

	if (!FileReadData(tooltip_, sizeof(tooltip_), 1, file))
	{
		strcpy(tooltip_, "");
		return false;
	}
	tooltip_[sizeof(tooltip_) - 1] = 0;

	if (!FileReadData(&yesOrNo_, sizeof(yesOrNo_), 1, file))
		return false;

	if (!FileReadData(&visible_, sizeof(visible_), 1, file))
		return false;

	if (!FileReadData(&value_, sizeof(value_), 1, file))
		return false;

	return true;
}

void Option::Save(FILE* file)
{
	fwrite(name_, sizeof(name_), 1, file);
	fwrite(tooltip_, sizeof(tooltip_), 1, file);
	fwrite(&yesOrNo_, sizeof(yesOrNo_), 1, file);
	fwrite(&visible_, sizeof(visible_), 1, file);
	fwrite(&value_, sizeof(value_), 1, file);
	SaveID_END(file);
}

void Option::Print()
{
	std::print("Option : name={}, value={}\n", name_, value_);
	std::print("\tYesOrNo={}, Visible={}\n", yesOrNo_, visible_);
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
	return name_;
}

const char* Option::GetTooltip() const
{
	return tooltip_;
}

bool Option::GetYesOrNo() const
{
	return yesOrNo_;
}

bool Option::GetVisible() const
{
	return visible_;
}

int Option::GetValue() const
{
	return value_;
}

void Option::SetName(const char* name)
{
	UplinkAssert(strlen(name) < sizeof(name_));
	UplinkStrncpy(name_, name, sizeof(name_));
}

void Option::SetTooltip(const char* tooltip)
{
	UplinkAssert(strlen(tooltip) < sizeof(tooltip_));
	UplinkStrncpy(tooltip_, tooltip, sizeof(tooltip_));
}

void Option::SetYesOrNo(const bool yesOrNo)
{
	yesOrNo_ = yesOrNo;
}

void Option::SetVisible(const bool visible)
{
	visible_ = visible;
}

void Option::SetValue(const int value)
{
	value_ = value;
}
