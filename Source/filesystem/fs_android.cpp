#ifdef PLATFORM_ANDROID 

#include "TheEngine/filesystem/stream.h"
#include "TheEngine/logger.h"
#include "TheEngine/timer.h"


the::filesystem::stream::stream(strref filename) {open(filename);}
the::filesystem::stream::~stream() {if(_file) close();}

void the::filesystem::stream::open(strref filename)
{
    _file = AAssetManager_open(aux::assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if(!_file)
        FATAL_ERROR("[File   ] can't load '%s'",filename.c_str());
    _size = AAsset_getLength(_file);   
}

void the::filesystem::stream::close()
{
  AAsset_close(_file);
} 

void the::filesystem::stream::read(char *buf, size_t size)
{
  AAsset_read (_file,buf,size);
}

void the::filesystem::stream::seek(long pos)
{
  AAsset_seek(_file, pos,SEEK_SET);
}

std::string the::filesystem::load_as_string(strref filename)
{
       
    the::filesystem::stream stream(filename);    
    std::string contents;
    contents.resize(stream.size());
    stream.read(&contents[0],stream.size());
    return contents;
}
void the::filesystem::load_content(strref filename, std::vector<char> &content)
{
    the::filesystem::stream stream(filename);    
    content.resize(stream.size());
    stream.read(&content[0],stream.size());
}


bool the::filesystem::is_exist(strref s) 
{

    return is_file(s) || is_dir(s); 
}

bool the::filesystem::is_dir(strref s) 
{
    bool res = false;
    auto t = the::timer::trace([&]()
    {
        auto dir  = AAssetManager_openDir(aux::assetManager, s.c_str());
        auto file = AAssetManager_open(aux::assetManager, s.c_str(), AASSET_MODE_UNKNOWN);
        if(dir && !file)
        {    
            AAssetDir_close(dir);
            res = true;
        }
        else if(file)
            AAsset_close(file);
    });        
    logger::error("[File   ] BAD PERFOMANCE! '%s' is_dir() = %s in %f seconds", s.c_str(), res ? "true" : "false", t);
    return res;

}

bool the::filesystem::is_file(strref filename) 
{
    auto file = AAssetManager_open(aux::assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if(file)
    {    
        AAsset_close(file);
        return true;
    }
    else
    {    
        logger::error("[File   ] '%s' not exist!", filename.c_str());
        return false;
    }
}  

bool the::filesystem::is_link(strref s) {return false;}
std::string the::filesystem::extension(strref s) 
{
    return s.substr(s.find_last_of(".") + 1);
}

bool the::filesystem::is_pack(strref s)
{
  return true;
} 

#endif