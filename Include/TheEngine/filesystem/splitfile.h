#ifndef _THE_SECTION_FILE_
#define _THE_SECTION_FILE_

#include "TheEngine/comtypes.h"
#include "TheEngine/logger.h"
namespace the { namespace filesystem { 

class splitfile
{
public:
	splitfile(strref);
	~splitfile();

	bool    check(strref) const ;
	strref  get(strref); 
    size_t  size() 	      const;
private:

	const char * PREFIX = "@{";
	const char * SUFFIX = "}";
	const size_t prefix_len = std::char_traits<char>::length(PREFIX);
	const size_t suffix_len = std::char_traits<char>::length(SUFFIX);
	
	std::string wholeFile;
	dictionary<std::string> sections;

	void _parse();


};
}}


#endif


