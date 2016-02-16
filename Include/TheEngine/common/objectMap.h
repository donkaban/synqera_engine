#ifndef _THE_UID_OBJECT_MAP_
#define _THE_UID_OBJECT_MAP_

#include "TheEngine/comtypes.h"
#include "TheEngine/common/abstract.h"
#include "TheEngine/logger.h"

template<typename T>
class objectMap
{
private:
	std::unordered_map<uint,T *> _map;
    
public:	
	objectMap() 
    {
        static_assert(std::is_convertible<T*, abstract::object *>::value,"[ObjectMap] not inherited form abstract::object!");
    }
	
    virtual ~objectMap() {}

	bool check(uint key) {return _map.find(key) != _map.end();}
   
	uint push(T *val)
	{
		auto id = val->getUID(); 
		_map[id] = val;
		return id;
	}
	uint push(const T val) {push(new T(val));}
   
    T * get(uint key)
    {
    	if(check(key)) 
    		return _map[key];
    	else 
    		FATAL_ERROR("[ObjectMap] can't find object with key %d",key);
        return nullptr;
    }
    void erase(uint key)
    {
    	if(check(key))
    	{ 
    		delete _map[key];
    		_map.erase(key);
   		}
   		else 
    		FATAL_ERROR("[ObjectMap] can't delete object with key %d", key);
    }
    void foreach(std::function<bool(T *)> func)
    {
    	auto i = _map.begin();
    	while(i != _map.end())
    	{
    		if(func(i->second)) 
    		{	
    			delete i->second;
    			i = _map.erase(i);
    		}
    		else i++;
    	}	
    }
    T * operator[](uint key) {return get(key);}  
};





#endif