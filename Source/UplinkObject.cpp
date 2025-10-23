#include "UplinkObject.hpp"

#include "Option.hpp"
#include "Util.hpp"
#include "VLocation.hpp"

#include "_.hpp"

bool UplinkObject::Load(FILE*)
{
	return true;
}

void UplinkObject::Save(FILE*) {}

void UplinkObject::Print() {}

void UplinkObject::Update() {}

const char* UplinkObject::GetID()
{
	return "UOBJ";
}

UplinkObjectId UplinkObject::GetOBJECTID()
{
	return UplinkObjectId::None;
}

void UplinkObject::LoadID(FILE*) {}

void UplinkObject::SaveID(FILE*) {}

void UplinkObject::LoadID_END(FILE*) {}

void UplinkObject::SaveID_END(FILE*) {}

char* UplinkObject::GetID_END()
{
	static constexpr size_t length = 13; // 8 + strlen("_END") + 1
	auto* const str = new char[length];
	UplinkSnprintf(str, length, "%s_END", GetID());
	return str;
}

UplinkObject* CreateUplinkObject(const UplinkObjectId objectId)
{
	switch (objectId)
	{
		case UplinkObjectId::None:
		{
			std::println("WARNING: CreateUplinkObject: OBJECTID Not assigned");
			return nullptr;
		}
		case UplinkObjectId::VLocation: return new VLocation();
		/* TODO
		case UplinkObjectId::Message: return new Message();
		case UplinkObjectId::Person: return new Person();
		case UplinkObjectId::Agent: return new Agent();
		case UplinkObjectId::Player: return new Player();
		*/
		case UplinkObjectId::Option: return new Option();
		/* TODO
		case UplinkObjectId::VLocationSpecial: return new VLocationSpecial();
		case UplinkObjectId::LanComputer: return new LanComputer();
		case UplinkObjectId::LanComputerSystem: return new LanComputerSystem();
		case UplinkObjectId::LanComputerLink: return new LanComputerLink();
		case UplinkObjectId::Computer: return new Computer();
		case UplinkObjectId::Data: return new Data();
		case UplinkObjectId::AccessLog: return new AccessLog();
		case UplinkObjectId::Record: return new Record();
		case UplinkObjectId::BankComputer: return new BankComputer();
		case UplinkObjectId::BankAccount: return new BankAccount();
		case UplinkObjectId::SecuritySystem: return new SecuritySystem();
		case UplinkObjectId::GenericScreen: return new GenericScreen();
		case UplinkObjectId::MessageScreen: return new MessageScreen();
		case UplinkObjectId::PasswordScreen: return new PasswordScreen();
		case UplinkObjectId::MenuScreen: return new MenuScreen();
		case UplinkObjectId::MenuScreenOption: return new MenuScreenOption();
		case UplinkObjectId::DialogScreen: return new DialogScreen();
		case UplinkObjectId::DialogScreenWidget: return new DialogScreenWidget();
		case UplinkObjectId::UserIDScreen: return new UserIDScreen();
		case UplinkObjectId::LogScreen: return new LogScreen();
		case UplinkObjectId::BBSScreen: return new BBSScreen();
		case UplinkObjectId::LinksScreen: return new LinksScreen();
		case UplinkObjectId::SharesListScreen: return new SharesListScreen();
		case UplinkObjectId::CypherScreen: return new CypherScreen();
		case UplinkObjectId::HighSecurityScreen: return new HighSecurityScreen();
		case UplinkObjectId::DisconnectedScreen: return new DisconnectedScreen();
		case UplinkObjectId::Company: return new Company();
		case UplinkObjectId::CompanyUplink: return new CompanyUplink();
		case UplinkObjectId::Mission: return new Mission();
		case UplinkObjectId::Sale: return new Sale();
		case UplinkObjectId::SaleVersion: return new SaleVersion();
		case UplinkObjectId::News: return new News();
		case UplinkObjectId::NotificationEvent: return new NotificationEvent();
		case UplinkObjectId::ArrestEvent: return new ArrestEvent();
		case UplinkObjectId::ShotByFedsEvent: return new ShotByFedsEvent();
		case UplinkObjectId::SeizeGatewayEvent: return new SeizeGatewayEvent();
		case UplinkObjectId::InstallHardwareEvent: return new InstallHardwareEvent();
		case UplinkObjectId::WarningEvent: return new WarningEvent();
		case UplinkObjectId::AttemptMissionEvent: return new AttemptMissionEvent();
		case UplinkObjectId::RunPlotSceneEvent: return new RunPlotSceneEvent();
		case UplinkObjectId::ChangeGatewayEvent: return new ChangeGatewayEvent();
		case UplinkObjectId::BankRobberyEvent: return new BankRobberyEvent();
		*/
		default:
		{
			std::println("WARNING: CreateUplinkObject: Unrecognised OBJECTID={}", static_cast<int>(objectId));
			return nullptr;
		}
	}
}
