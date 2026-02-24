#include "Date.hpp"

#include "Util.hpp"
#include "_.hpp"

static char tempdate[0x40];

bool Date::Load(FILE* file)
{
	static_assert(sizeof(_second) == 4);
	static_assert(sizeof(_minute) == 4);
	static_assert(sizeof(_hour) == 4);
	static_assert(sizeof(_day) == 4);
	static_assert(sizeof(_month) == 4);
	static_assert(sizeof(_year) == 4);
	static_assert(sizeof(_active) == 1);

	if (!FileReadData(&_second, sizeof(_second), 1, file))
		return false;

	if (!FileReadData(&_minute, sizeof(_minute), 1, file))
		return false;

	if (!FileReadData(&_hour, sizeof(_hour), 1, file))
		return false;

	if (!FileReadData(&_day, sizeof(_day), 1, file))
		return false;

	if (!FileReadData(&_month, sizeof(_month), 1, file))
		return false;

	if (!FileReadData(&_year, sizeof(_year), 1, file))
		return false;

	if (!FileReadData(&_active, sizeof(_active), 1, file))
		return false;

	return true;
}

void Date::Save(FILE* file)
{
	fwrite(&_second, sizeof(_second), 1, file);
	fwrite(&_minute, sizeof(_minute), 1, file);
	fwrite(&_hour, sizeof(_hour), 1, file);
	fwrite(&_day, sizeof(_day), 1, file);
	fwrite(&_month, sizeof(_month), 1, file);
	fwrite(&_year, sizeof(_year), 1, file);
	fwrite(&_active, sizeof(_active), 1, file);
	SaveID_END(file);
}

void Date::Print()
{
	std::println("Date: %d:%d:%d, %d/%d/%d", _hour, _minute, _second, _day, _month, _year);

	if (_active)
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
	_second = second;
	_minute = minute;
	_hour = hour;
	_day = day;
	_month = month;
	_year = year;
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
	return _year == other._year &&
	       _month == other._month &&
	       _day == other._day &&
	       _hour == other._hour &&
	       _minute == other._minute &&
	       _second == other._second;
}
