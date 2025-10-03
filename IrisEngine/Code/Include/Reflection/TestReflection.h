#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <string_view>

//#include "Core/Tools/Macros.h"
#include "Core/Tools/Reflection.h"


enum class REFLECT MoveType : int
{
    TYPE0 = 0,
    TYPE1 = 1,
    TYPE2 = 2,
    NONE = 3
};

class REFLECT IObj
{
protected:
    IPROPERTY int lol = 0;
//automatically generated code
private:
    inline static std::hash<std::string_view> s_Hash{};

public:
    inline static meta::factory<IObj> Factory = REFLECT_CLASS(IObj)
        .REFLECT_DATA(IObj, lol);
//automatically generated code
};

class REFLECT Entity : public IObj
{
public:
    IFUNCTION Entity()
    {
        ++nbEntity;
    }

    IFUNCTION Entity(MoveType _type, std::string _name, int _health = 10)
        : type(_type), name(_name), health(_health)
    {
        ++nbEntity;
        std::cout << "Entity " << name << " created!\n" << nbEntity << " currently exist.\n\n";
    }

    ~Entity() = default;

    void Destroy()
    {
        --nbEntity;
        std::cout << "Entity " << name << " deleted!\n" << nbEntity << " curently exist.\n\n";
        this->~Entity();
    }

    IFUNCTION static bool TryChangeNbEntity(int _newNb)
    {
        int oldNb = nbEntity;
        nbEntity = _newNb;
        std::cout << "Try to change nbEntity with " << nbEntity << ".\n Changes revert! You can't change this value.\n\n";
        nbEntity = oldNb;

        return nbEntity;
    }

    IFUNCTION void Print() const
    {
        std::cout << "Entity: " << name << " | Health: " << health << "\n\n";
    }

    std::string GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }


public:
    IPROPERTY MoveType type = MoveType::NONE;
    IPROPERTY inline static int nbEntity = 0;

protected:
    IPROPERTY std::string name = "";

private:
    int health = 0;
//automatically generated code
private:
    inline static std::hash<std::string_view> s_Hash{};

public:
    inline static meta::factory<Entity> Factory = REFLECT_CLASS(Entity)
        .REFLECT_BASE(IObj)
        .REFLECT_CTOR()
        .REFLECT_CTOR(MoveType, std::string, int)
        .REFLECT_CTOR(MoveType, std::string)
        .REFLECT_FUNC(TryChangeNbEntity)
        .REFLECT_FUNC(Print)
        .REFLECT_DATA(Entity, type)
        .REFLECT_DATA(Entity, nbEntity)
        .REFLECT_DATA(Entity, name);
//automatically generated code
};


int Test()
{
    std::hash<std::string_view> hash{};

    /*
    meta::factory<MoveType> factory = REFLECT_ENUM(MoveType)
        .REFLECT_ELEM(MoveType::NONE, "NONE")
        .REFLECT_ELEM(MoveType::TYPE0, "TYPE0")
        .REFLECT_ELEM(MoveType::TYPE1, "TYPE1")
        .REFLECT_ELEM(MoveType::TYPE2, "TYPE2");
    */


    meta::type eReflect = meta::resolve(hash("Entity"));

    meta::func printFunc = meta::resolve(hash("Entity")).func(hash("Print"));
    meta::func staticFunc = meta::resolve(hash("Entity")).func(hash("TryChangeNbEntity"));
    meta::data namedata = meta::resolve(hash("Entity")).data(hash("name"));
    meta::data typedata = meta::resolve(hash("Entity")).data(hash("type"));


    meta::any e0any = eReflect.construct();
    Entity e0 = e0any.cast<Entity>();
    printFunc.invoke(e0);
    staticFunc.invoke(e0, 3);
    meta::any nameany = namedata.get(e0);
    std::string name = nameany.cast<std::string>();
    meta::any typeany = typedata.get(e0);
    MoveType type = typeany.cast<MoveType>();
    type;
    meta::any e1any = eReflect.construct(MoveType::TYPE2, std::string("lol"));
    Entity e1 = e1any.cast<Entity>();
    meta::handle e1Hany{ e1any };
    meta::handle e1H{ e1 };
    Entity e2 = eReflect.construct(MoveType::TYPE0, std::string("xd"), 5).cast<Entity>();
    bool result = eReflect.destroy(e1H);
    result;

    meta::any a = eReflect.data(hash("type")).get(e2);

    meta::ctor eConstruct = eReflect.ctor<MoveType, std::string, int>();
    meta::any e3any = eConstruct.invoke(MoveType::TYPE1, std::string("ptdr"), 3);
    Entity e3 = e3any.cast<Entity>();

    return 0;
}