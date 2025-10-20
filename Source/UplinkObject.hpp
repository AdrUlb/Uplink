#pragma once

#include <cstdio>

enum class UplinkObjectId
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
	BankRobberyEvent
};

class UplinkObject
{
public:
	virtual ~UplinkObject() {} // NOLINT(*-use-equals-default)
	virtual bool Load(FILE* file);
	virtual void Save(FILE* file);
	virtual void Print();
	virtual void Update();
	virtual const char* GetID();
	virtual UplinkObjectId GetOBJECTID();
	static void LoadID(FILE* file);
	static void SaveID(FILE* file);
	static void LoadID_END(FILE* file);
	static void SaveID_END(FILE* file);
	char* GetID_END();
};

UplinkObject* CreateUplinkObject(UplinkObjectId objectId);
