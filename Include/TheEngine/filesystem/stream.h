#ifndef _THE_CORE_FILESYSTEM_
#define _THE_CORE_FILESYSTEM_

#include "TheEngine/comtypes.h"
#include <fstream>

#ifdef PLATFORM_ANDROID	
	#include <android/asset_manager.h>
#endif

namespace the { namespace filesystem { 

class stream
{
private:
#ifdef PLATFORM_ANDROID	
    AAsset 		  * _file;
#endif
	std::ifstream 	_stream;
	long 		    _size = 0;
public:
	stream(strref);
	~stream();

	void open(strref);
	void close();
	void read(char *, size_t);
	void seek(long pos);

	inline long size()  {return _size;};

};

std::string load_as_string(strref);
void load_content(strref, std::vector<char> &);



bool is_exist(strref); 
bool is_dir(strref); 
bool is_file(strref); 
bool is_link(strref); 
bool is_pack(strref);
std::string extension(strref); 

}};




#endif

