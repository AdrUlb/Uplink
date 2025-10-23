#include "Date.hpp"

#include "Util.hpp"
#include "_.hpp"

static char tempdate[0x40];

bool Date::Load(FILE* file)
{
	static_assert(sizeof(second_) == 4);
	static_assert(sizeof(minute_) == 4);
	static_assert(sizeof(hour_) == 4);
	static_assert(sizeof(day_) == 4);
	static_assert(sizeof(month_) == 4);
	static_assert(sizeof(year_) == 4);
	static_assert(sizeof(active_) == 1);

	if (!FileReadData(&second_, sizeof(second_), 1, file))
		return false;

	if (!FileReadData(&minute_, sizeof(minute_), 1, file))
		return false;

	if (!FileReadData(&hour_, sizeof(hour_), 1, file))
		return false;

	if (!FileReadData(&day_, sizeof(day_), 1, file))
		return false;

	if (!FileReadData(&month_, sizeof(month_), 1, file))
		return false;

	if (!FileReadData(&year_, sizeof(year_), 1, file))
		return false;

	if (!FileReadData(&active_, sizeof(active_), 1, file))
		return false;

	return true;
}

void Date::Save(FILE* file)
{
	fwrite(&second_, sizeof(second_), 1, file);
	fwrite(&minute_, sizeof(minute_), 1, file);
	fwrite(&hour_, sizeof(hour_), 1, file);
	fwrite(&day_, sizeof(day_), 1, file);
	fwrite(&month_, sizeof(month_), 1, file);
	fwrite(&year_, sizeof(year_), 1, file);
	fwrite(&active_, sizeof(active_), 1, file);
	SaveID_END(file);
}

void Date::Print()
{
	std::println("Date: %d:%d:%d, %d/%d/%d", hour_, minute_, second_, day_, month_, year_);

	if (active_)
		std::println("Syncronised with real world time");
	else
		std::println("Not Syncronised with real world time");
}

void Date::Update() { TODO_ABORT; }

const char* Date::GetID()
{
	return "DATE";
}

const char* Date::GetMonthName(int month) { TODO_ABORT; }
const char* Date::GetShortString() { TODO_ABORT; }
const char* Date::GetLongString() { TODO_ABORT; }

void Date::SetDate(const Date& other)
{
	const auto second = other.GetSecond();
	const auto minute = other.GetMinute();
	const auto hour = other.GetHour();
	const auto day = other.GetDay();
	const auto month = other.GetMonth();
	const auto year = other.GetYear();
	return SetDate(second, minute, hour, day, month, year);
}

void Date::SetDate(const int second, const int minute, const int hour, const int day, const int month, const int year)
{
	second_ = second;
	minute_ = minute;
	hour_ = hour;
	day_ = day;
	month_ = month;
	year_ = year;
	AdvanceSecond(0);
}

void Date::Activate() { TODO_ABORT; }
void Date::DeActivate() { TODO_ABORT; }
void Date::AdvanceSecond(int count) { TODO_ABORT; }
void Date::AdvanceMinute(int count) { TODO_ABORT; }
void Date::AdvanceHour(int count) { TODO_ABORT; }
void Date::AdvanceDay(int count) { TODO_ABORT; }
void Date::AdvanceMonth(int count) { TODO_ABORT; }
void Date::AdvanceYear(int count) { TODO_ABORT; }
bool Date::After(Date& other) { TODO_ABORT; }
bool Date::Before(Date& other) { TODO_ABORT; }

bool Date::Equal(const Date& other) const
{
	return year_ == other.year_ &&
	       month_ == other.month_ &&
	       day_ == other.day_ &&
	       hour_ == other.hour_ &&
	       minute_ == other.minute_ &&
	       second_ == other.second_;
}
