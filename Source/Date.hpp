#pragma once
#include "UplinkObject.hpp"

class Date : public UplinkObject
{
	int _second = 1;
	int _minute = 1;
	int _hour = 1;
	int _day = 1;
	int _month = 1;
	int _year = 1000;
	int _lastTime = 0;
	bool _active = false;

public:
	~Date() override {} // NOLINT(*-use-equals-default)
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;

	[[nodiscard]] int GetSecond() const { return _second; }
	[[nodiscard]] int GetMinute() const { return _minute; }
	[[nodiscard]] int GetHour() const { return _hour; }
	[[nodiscard]] int GetDay() const { return _day; }
	[[nodiscard]] int GetMonth() const { return _month; }
	[[nodiscard]] int GetYear() const { return _year; }

	[[nodiscard]] const char* GetMonthName(int month);
	[[nodiscard]] const char* GetShortString();
	[[nodiscard]] const char* GetLongString();
	void SetDate(const Date& other);
	void SetDate(int second, int minute, int hour, int day, int month, int year);
	void Activate();
	void DeActivate();
	void AdvanceSecond(int count);
	void AdvanceMinute(int count);
	void AdvanceHour(int count);
	void AdvanceDay(int count);
	void AdvanceMonth(int count);
	void AdvanceYear(int count);
	bool After(Date& other);
	bool Before(Date& other);
	bool Equal(const Date& other) const;
};
