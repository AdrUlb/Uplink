#pragma once

#include "Computer.hpp"
#include "UplinkObject.hpp"

static constexpr size_t VIRTUAL_WIDTH = 594;
static constexpr size_t VIRTUAL_HEIGHT = 315;

class VLocation : public UplinkObject
{
	char ip_[0x18] = "0.0.0.0";
	char computer_[0x40] = "";
	int x_ = 0;
	int y_ = 0;
	bool listed_ = true;
	bool displayed_ = true;
	bool colored_ = false;

public:
	~VLocation() override {} // NOLINT(*-use-equals-default)
	bool Load(FILE* file) override;
	void Save(FILE* file) override;
	void Print() override;
	void Update() override;
	const char* GetID() override;
	UplinkObjectId GetOBJECTID() override;
	[[nodiscard]] Computer* GetComputer();
	void SetIP(const char* ip);
	void SetComputer(const char* computer);
	void SetListed(bool listed);
	void SetDisplayed(bool displayed);
	void SetColorDisplayed(bool colorDisplayed);
	void SetPLocation(int x, int y);
	bool VerifyIP(const char* ip);
	bool VerifyPLocation(int x, int y);
};
