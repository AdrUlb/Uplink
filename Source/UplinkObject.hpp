#pragma once

#include <cstdio>

enum class UplinkObjectID
{
    UplinkObject = 0
};

class UplinkObject
{
    virtual ~UplinkObject();
    virtual bool Load(FILE* file);
    virtual void Save(FILE* file);
    virtual void Print();
    virtual void Update();
    virtual const char* GetID();
    virtual UplinkObjectID GetOBJECTID();
    void LoadID(FILE* file);
    void SaveID(FILE* file);
    void LoadID_END(FILE* file);
    void SaveID_END(FILE* file);
    char* GetID_END();
};
