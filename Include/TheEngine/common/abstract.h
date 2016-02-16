#ifndef _THE_OBJECT_INTERFACE
#define _THE_OBJECT_INTERFACE

#include "TheEngine/comtypes.h"

namespace abstract {

class object
{
public:

    object();
    object(strref type, strref tag = "--");
    object(const object &);
    object(object &&);
   
    virtual ~object(); 
     
    object & operator = (const object &);
    object & operator = (object &&);
   
    void setType(strref t) {_type = t;}
    void setTag(strref t)  {_tag = t;}; 
   
    strref  getType() const {return _type;}
    uint    getUID()  const {return _uid;};
    strref  getTag()  const {return _tag;}; 
    
    virtual std::string tostring() const {return getType() + " '" + getTag() + "'";};
    virtual void log() {}
 
private:
    static size_t _uniqueID;                   
    static size_t _objectCount;

    size_t      _uid;       // unique global-wide ID
    std::string _type;      // type name
    std::string _tag;       // object tag
};  


class gapi
{
public:
    virtual void bind()     = 0;
    virtual void unbind()   = 0;

    uint getID() const     {return _bindId;}
    void setID(uint id)    {_bindId = id;}
    void resetID()         {_bindId = 0;}
protected:
    GL::hdl _bindId = 0;     
};





}
#endif