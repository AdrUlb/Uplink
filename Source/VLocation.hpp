#pragma once

#include "UplinkObject.hpp"

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

	// TODO:
	// GetComputer
	// SetIP
	// SetComputer
	// SetListed
	// SetDisplayed
	// SetColorDisplayed
	// SetPLocation
	// VerifyIP
	// VerifyPLocation
};
