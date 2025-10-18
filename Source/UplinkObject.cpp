#include "UplinkObject.hpp"
#include "Util.hpp"
#include "_.hpp"

UplinkObject::~UplinkObject()
{
    TODO_ABORT;
}

bool UplinkObject::Load(FILE* file)
{
    return true;
}

void UplinkObject::Save(FILE* file) {}

void UplinkObject::Print() {}

void UplinkObject::Update() {}

const char* UplinkObject::GetID()
{
    return "UOBJ";
}

UplinkObjectID UplinkObject::GetOBJECTID()
{
    return UplinkObjectID::UplinkObject;
}

void UplinkObject::LoadID(FILE* file) {}

void UplinkObject::SaveID(FILE* file) {}

void UplinkObject::LoadID_END(FILE* file) {}

void UplinkObject::SaveID_END(FILE* file) {}

char* UplinkObject::GetID_END()
{
    static constexpr size_t length = 13; // 8 + strlen("_END") + 1
    auto* const str = new char[length]; 
    UplinkSnprintf(str, length, "%s_END", GetID());
    return str;
}
