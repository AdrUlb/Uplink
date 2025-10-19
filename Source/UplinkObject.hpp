#pragma once

#include <cstdio>

enum class UplinkObjectID
{
	UplinkObject,
	Option = 8
};

class UplinkObject
{
public:
	virtual ~UplinkObject();
	virtual bool Load(FILE* file);
	virtual void Save(FILE* file);
	virtual void Print();
	virtual void Update();
	virtual const char* GetID();
	virtual UplinkObjectID GetOBJECTID();
	static void LoadID(FILE* file);
	static void SaveID(FILE* file);
	static void LoadID_END(FILE* file);
	static void SaveID_END(FILE* file);
	char* GetID_END();
};
