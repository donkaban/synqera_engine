#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

#include "TheEngine/filesystem/stream.h"
#include "TheEngine/common.h"
#include "TheEngine/logger.h"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

the::filesystem::stream::stream(strref filename) {open(filename);}
the::filesystem::stream::~stream() {close();}

void the::filesystem::stream::open(strref filename) 
{
    _stream.open(filename, std::ios::in | std::ios::binary);
    _stream.seekg(0, std::ios::end);
    _size = _stream.tellg();
    _stream.seekg(0, std::ios::beg);
}

void the::filesystem::stream::close()
{
    if(_stream.is_open())
        _stream.close();
}
void the::filesystem::stream::read(char *buf, size_t size)
{
  _stream.read(buf,size);
}

void the::filesystem::stream::seek(long pos)
{
  _stream.seekg(pos, std::ios::beg);
}

std::string the::filesystem::load_as_string(strref filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in.fail())
        FATAL_ERROR("[File   ] can't load '%s'",filename.c_str());
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return (contents);
}
void the::filesystem::load_content(strref filename, std::vector<char> &content)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in.fail())
        FATAL_ERROR("[File   ] can't load '%s'",filename.c_str());
    in.seekg(0, std::ios::end);
    content.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&content[0], content.size());
    in.close();
}

bool the::filesystem::is_exist(strref s) 
{
  struct stat buffer;   
  return (stat(s.c_str(), &buffer) == 0); 
}

bool the::filesystem::is_dir(strref s) 
{
  struct stat buffer;   
  return (stat(s.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode) ; 
}

bool the::filesystem::is_file(strref s) 
{
  struct stat buffer;   
  return (stat(s.c_str(), &buffer) == 0) && S_ISREG(buffer.st_mode) ; 
}

bool the::filesystem::is_link(strref s) 
{
  struct stat buffer;   
  return (stat(s.c_str(), &buffer) == 0) && S_ISLNK(buffer.st_mode) ; 
}

std::string the::filesystem::extension(strref s) 
{
    return s.substr(s.find_last_of(".") + 1);
}


bool the::filesystem::is_pack(strref s)
{
  return true;
} 



#endif