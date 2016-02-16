#include "TheEngine/filesystem/splitfile.h"
#include "TheEngine/filesystem/stream.h"

std::string err = "";
the::filesystem::splitfile::splitfile(strref filename)
{
	if(!the::filesystem::is_file(filename))
		FATAL_ERROR("[File   ] can't open file '%s'",filename.c_str());	
	wholeFile = the::filesystem::load_as_string(filename);
	_parse();
}
the::filesystem::splitfile::~splitfile()
{}
	
bool the::filesystem::splitfile::check(strref key) const
{
	return (sections.find(key) != sections.end());
}
strref the::filesystem::splitfile::get(strref key) 
{
	if(check(key)) return sections[key];
	logger::error("[File   ] can't find section '%s' in file ",key.c_str());
	return wholeFile;
} 

size_t the::filesystem::splitfile::size() const {return sections.size();}

struct _s
{
	size_t begin;
	size_t end;
	std::string name;
	_s(size_t b, size_t e, strref n) : begin(b), end(e), name(n)
	{}
};

void the::filesystem::splitfile::_parse()
{
	std::vector<_s> ndx;
	auto curpos = 0;

	while(true)
	{
		auto prefix = wholeFile.find(PREFIX,curpos);
		auto suffix = wholeFile.find(SUFFIX,curpos);
		if(prefix==std::string::npos) break;
		if(suffix==std::string::npos) FATAL_ERROR("File   ] brace error in section");
		auto name = wholeFile.substr(prefix+prefix_len,suffix-prefix-prefix_len);
		ndx.push_back(_s(suffix+suffix_len,wholeFile.length(),name));
		if(!ndx.empty())
			ndx[ndx.size()-2].end = prefix -1;	   	
	   	curpos = suffix+suffix_len;
	}
	for(auto &x : ndx)
	{	
		sections[x.name] = wholeFile.substr(x.begin,x.end-x.begin);
		logger::warning("[File   ] parse section '%s'", x.name.c_str());
	}	
	
}