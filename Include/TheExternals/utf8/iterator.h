#ifndef _THE_UTF8ITERATOR_
#define _THE_UTF8ITERATOR_

#include "TheEngine/comtypes.h"
#include "TheExternals/utf8/checked.h"
#include "TheExternals/utf8/unchecked.h"

typedef std::vector<uint32_t> UTF8String;

class Utf8StringIterator
{
private:
	std::string text;
	uint32_t currentChar;
	char* str;
	char* str_i;
	char* end;

public:
	Utf8StringIterator(strref _text)
	{
		text = _text;
		str = (char*)text.c_str();    // utf-8 string
	    str_i = str;                  // string iterator
	    end = str+strlen(str)+1;      // end iterator
	}

	bool next()
	{
		if(str_i < end)
		{	
			currentChar = utf8::next(str_i, end);
			return true;
		}

		return false;
	}

	bool prev()
	{
		if(end > str_i)
		{	
			currentChar = utf8::previous(end, str_i);
			return true;
		}

		return false;
	}

	uint32_t current()
	{
		return currentChar;
	}

	static UTF8String createUTF8(strref in)
	{
	    UTF8String result;
	    Utf8StringIterator iter(in);
	    while(iter.next())
	    {
	        uint32_t c = iter.current();
	        if (c == 0)
	        { 
	            continue;
	        }
	        result.push_back(c);
	    }   
	    return result;
	}

#define BUF_SIZE 10
	static std::string charToStr(uint32_t c)
	{
		char* buffer = new char[BUF_SIZE];
		std::memset(buffer,0,BUF_SIZE) ;
		utf8::append(c, buffer);
		std::string result(buffer);
		delete [] buffer;
		return result;
	}
};

#endif