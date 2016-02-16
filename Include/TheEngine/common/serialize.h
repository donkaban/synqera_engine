#ifndef _THE_ISERIALIZED_INTERFACE
#define _THE_ISERIALIZED_INTERFACE

#include "TheEngine/comtypes.h"
#include "TheEngine/filesystem/stream.h"

namespace the {class node;}

namespace serialize
{

	class binary
	{
	public:
	    virtual ~binary() {}
	    virtual void serialize(strref) 					     = 0;
	    virtual void deserialize(the::filesystem::stream &)  = 0;
	};

	class xml
	{
	public:
	    virtual ~xml() {}
	    virtual void serialize(pugi::xml_node &)  = 0;
	    virtual void deserialize(const pugi::xml_node &)  = 0;
	    virtual void deserializePass2(const pugi::xml_node &, std::unordered_map<uint, std::shared_ptr<the::node>> &) {}
	};
}

#endif