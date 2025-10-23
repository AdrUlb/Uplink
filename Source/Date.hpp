#pragma once
#include "UplinkObject.hpp"

class Date : public UplinkObject
{
	int second_ = 1;
	int minute_ = 1;
	int hour_ = 1;
	int day_ = 1;
	int month_ = 1;
	int year_ = 1000;
	int lastTime_ = 0;
	bool active_ = false;

public:
	~Date() override {} // NOLINT(*-use-equals-default)
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;

	[[nodiscard]] int GetSecond() const { return second_; }
	[[nodiscard]] int GetMinute() const { return minute_; }
	[[nodiscard]] int GetHour() const { return hour_; }
	[[nodiscard]] int GetDay() const { return day_; }
	[[nodiscard]] int GetMonth() const { return month_; }
	[[nodiscard]] int GetYear() const { return year_; }

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
