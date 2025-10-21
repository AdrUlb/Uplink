#include "UplinkObject.hpp"
#include "Util.hpp"

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
	(void)objectId;
	TODO_ABORT;
}
