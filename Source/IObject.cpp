#include "TheEngine/common.h"
#include "TheEngine/logger.h"

size_t abstract::object::_uniqueID    = 1;
size_t abstract::object::_objectCount = 0;

abstract::object::object() : abstract::object("notype") 
{} 
abstract::object::object(strref type, strref tag): 
    _type(type),
    _tag(tag)
{
    _uid = _uniqueID ++;
    _objectCount++;
}    

abstract::object::object(const abstract::object &obj)
{
    _uid  = obj.getUID();
    _tag  = obj.getTag();
    _type = obj.getType();
    _objectCount++;
}

abstract::object::object(abstract::object &&obj)
{
    _uid  = obj.getUID();
    _tag  = obj.getTag();
    _type = obj.getType();
}

abstract::object & abstract::object::operator = (abstract::object &&obj)
{
    _uid  = obj.getUID();
    _tag  = obj.getTag();
    _type = obj.getType();
    _objectCount++;
    return *this; 
}

abstract::object & abstract::object::operator = (const abstract::object &obj)
{
    _uid  = obj.getUID();
    _tag  = obj.getTag();
    _type = obj.getType();
    _objectCount++;
    return *this; 
}

abstract::object::~object()
{
	_objectCount--;
}


