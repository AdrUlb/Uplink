#pragma once

#include "Util.hpp"

enum class UplinkObjectID
{
	None,
	VLocation,
	Message = 3,
	Person,
	Agent,
	Player,
	Option = 8,
	VLocationSpecial,
	LanComputer = 15,
	LanComputerSystem,
	LanComputerLink,
	Computer = 20,
	Data,
	AccessLog = 23,
	Record = 25,
	BankComputer = 27,
	BankAccount,
	SecuritySystem,
	GenericScreen,
	MessageScreen,
	PasswordScreen,
	MenuScreen,
	MenuScreenOption,
	DialogScreen,
	DialogScreenWidget,
	UserIDScreen,
	LogScreen,
	BBSScreen,
	LinksScreen,
	SharesListScreen,
	CypherScreen,
	HighSecurityScreen,
	DisconnectedScreen,
	Company = 50,
	CompanyUplink = 52,
	Mission = 56,
	Sale,
	SaleVersion,
	News,
	NotificationEvent = 70,
	ArrestEvent,
	ShotByFedsEvent,
	SeizeGatewayEvent,
	InstallHardwareEvent,
	WarningEvent,
	AttemptMissionEvent,
	RunPlotSceneEvent,
	ChangeGatewayEvent,
	BankRobberyEvent,
};

class UplinkObject
{
public:
	virtual ~UplinkObject() = default;
	virtual bool Load(FILE* file) { return true; }
	virtual void Save(FILE* file) {}
	virtual void Print() {}
	virtual void Update() {}
	virtual const char* GetID() { return "UOBJ"; }
	virtual UplinkObjectID GetOBJECTID() { return UplinkObjectID::None; }

#pragma region NOTE: Can probably be removed because useless?
	char* GetID_END()
	{
		const auto str = new char[13];
		UplinkSnprintf(str, 0xDu, "%s_END", GetID());
		snprintf(str, 0xDu, "%s_END", GetID());
		return str;
	}

	// ReSharper disable CppMemberFunctionMayBeStatic
	void LoadID(const FILE* file) { (void)file; } // NOLINT(*-convert-member-functions-to-static)
	void LoadID_END(const FILE* file) { (void)file; } // NOLINT(*-convert-member-functions-to-static)
	void SaveID(const FILE* file) { (void)file; } // NOLINT(*-convert-member-functions-to-static)
	void SaveID_END(const FILE* file) { (void)file; } // NOLINT(*-convert-member-functions-to-static)
	// ReSharper restore CppMemberFunctionMayBeStatic
#pragma endregion
};

void DeleteBTreeData(BTree<UplinkObject*>* tree);

template<std::derived_from<UplinkObject> T> void DeleteBTreeData(BTree<T*>* tree)
{
	DeleteBTreeData(reinterpret_cast<BTree<UplinkObject*>*>(tree));
}
