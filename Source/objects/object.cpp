#include "TheEngine/openGL.h"
#include "TheEngine/objects/object.h"
#include "TheEngine/logger.h"
#include "TheEngine/engine.h"
#include "TheEngine/filesystem/file.h"
#include <limits>

the::object::object() : 
    the::object("Object","--")  
{}

the::object::object(strref tag) : 
    the::object("Object",tag)
{}

the::object::object(strref type, strref tag) : 
    the::node(type, tag)
{}

the::object::~object()
{
    unbind();
    if(mousePressHandler!= 0) aux::engine->getInput()->delPressHandler(mousePressHandler);
    if(mouseTapHandler  != 0) aux::engine->getInput()->delTapHandler(mouseTapHandler);
    actions.clear();
}

void the::object::bind()
{
    uint sz = vertexes.size();
    if(sz == 0) return;
    glGenBuffers(1, &_bindId);
    glBindBuffer(GL_ARRAY_BUFFER, _bindId);
    glBufferData(GL_ARRAY_BUFFER, sz * sizeof(Vertex) , &vertexes[0], GL_STATIC_DRAW);
    for(auto & s : surfaces) s.bind();
    recalculateBoundingBox();
    triangles = 0;
    for(auto const &s : surfaces)
        triangles+=s.getNumTriangles();
    logger::debug("[Object ] create '%s' (%s) surfaces: %d; triangles: %d",getTag().c_str(), getType().c_str(),getNumSurfaces(),triangles);
    GL_CHECK("the::object::bind");
}

void the::object::unbind()
{
    if (!glIsBuffer(_bindId)) return;
    glDeleteBuffers(1, &_bindId);
    for (auto & s : surfaces) s.unbind();
    resetID();
    triangles = 0; 
    logger::debug("[Object ] remove '%s' (%s) surfaces: %d; triangles: %d",getTag().c_str(), getType().c_str(),getNumSurfaces(),triangles);
  
    GL_CHECK("the::object::unbind");
}

void the::object::onRender(render::state &state)
{
    the::node::onRender(state);
    glBindBuffer(GL_ARRAY_BUFFER, _bindId);
    GL_CHECK("the::object::onRender");
}

void   the::object::remap() {for(size_t i = 0; i < surfaces.size(); i++) tags[surfaces[i].getTag()] = i;}
size_t the::object::getNumTriangles() {return triangles;}
std::vector<Vertex> & the::object::getVertices() {return vertexes;}

void the::object::clear()                                     
{
    vertexes.clear(); 
    surfaces.clear(); 
    tags.clear();
}

void the::object::addVertex(const Vertex  &v)                        {vertexes.push_back(v);}
void the::object::addVertex(const std::initializer_list<Vertex> & v) {vertexes.insert(vertexes.end(), v.begin(), v.end());}
void the::object::addVertex(const std::vector<Vertex>  & v)          {vertexes.insert(vertexes.end(), v.begin(), v.end());}

void the::object::addSurface(the::surface &&s)                            {surfaces.push_back(s); remap();}
void the::object::addSurface(const std::initializer_list<the::surface> &s){surfaces.insert(surfaces.end(), s.begin(), s.end()); remap();}
void the::object::addSurface(const std::vector<the::surface> &s)          {surfaces.insert(surfaces.end(), s.begin(), s.end()); remap();}

void the::object::setTexture(int s, int n, the::texture::cref t) {surfaces[s].setTexture(n, t);}
void the::object::setTexture(int n, the::texture::cref t)        {for(auto & s : surfaces) s.setTexture(n, t);}
void the::object::setTexture(int n, strref t)               {for(auto & s : surfaces) s.setTexture(n, t);}

void the::object::setMaterial(int s, the::material::cref m )     {surfaces[s].setMaterial(m);}
void the::object::setMaterial(int s, strref m )             {surfaces[s].setMaterial(the::material::get(m));}
void the::object::setMaterial(the::material::cref m )            {for(auto & s : surfaces) s.setMaterial(m);}
void the::object::setMaterial(strref m )                    {for(auto & s : surfaces) s.setMaterial(the::material::get(m));}

uint      the::object::getNumSurfaces () const {return surfaces.size();}
the::surface & the::object::getSurface(int i)       {return surfaces[i];}
the::surface & the::object::getSurface(strref s)    {return surfaces[tags[s]];}
int       the::object::getSurfaceId(strref s)
{
    if (tags.find(s)!=tags.end()) 
        return tags[s];
    else
        return -1;
}

void the::object::setMaterialParam(strref s, strref p, float v)       {getSurface(s).setUserParam(p, v);}
void the::object::setMaterialParam(strref s, strref p, const vec4 &v) {getSurface(s).setUserParam(p, v);}
void the::object::setMaterialParam(int s, strref p, float v)          {getSurface(s).setUserParam(p, v);}
void the::object::setMaterialParam(int s, strref p, const vec4 &v)    {getSurface(s).setUserParam(p, v);}

void the::object::setMaterial(strref s, the::material::cref m)             {getSurface(s).setMaterial(m);}
void the::object::setMaterial(strref s, strref m )                    {getSurface(s).setMaterial(the::material::get(m));}

void the::object::setTexture(strref s, int n, the::texture::cref t )       {getSurface(s).setTexture(n, t);}
void the::object::setTexture(strref s, int n, strref t )              {getSurface(s).setTexture(n, t);}
void the::object::setTexture(int s, int n, strref t)                  {getSurface(s).setTexture(n, t);}

void the::object::setCubemap(int s, int i,the::cubetex::cref c)       {getSurface(s).setCubemap(i,c);}
void the::object::setCubemap(strref s, int i,the::cubetex::cref c)    {getSurface(s).setCubemap(i,c);}

void the::object::setCubemap(int s, int i,strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    surfaces[s].setCubemap(i,t1,t2,t3,t4,t5,t6);
}
void the::object::setCubemap(int i,strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    for(auto &s :surfaces)
        s.setCubemap(i,t1,t2,t3,t4,t5,t6);
}
void the::object::setCubemap(strref s, int i,strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    getSurface(s).setCubemap(i,t1,t2,t3,t4,t5,t6);
}

void the::object::setBlending(strref s, bool mode) {getSurface(s).setBlending(mode);}
void the::object::setBlending(int s,    bool mode) {getSurface(s).setBlending(mode);}

void the::object::serialize(strref filename)
{
    std::ofstream stream;
    the::object::header hdr;
    hdr.id = 0;
    hdr.surfaceSize = surfaces.size();
    hdr.vertexSize  = vertexes.size();
    stream.open(filename, std::ios::out | std::ios::binary);
    stream.write(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    stream.write(reinterpret_cast<char *>(&vertexes[0]), sizeof(Vertex) * vertexes.size());
    for (auto & surface : surfaces)
    {
        int len = surface.getTag().length();
        stream.write(reinterpret_cast<char *>(&len), sizeof(int));
        stream.write(surface.getTag().c_str(), len);
        int idxSize = surface.getIndecies().size();
        stream.write(reinterpret_cast<char *>(&idxSize), sizeof(idxSize));
        stream.write(reinterpret_cast<char *>(& (surface.getIndecies()[0])), sizeof(uint16_t)*idxSize);
        bool hasBones = surface.hasBones();
        stream.write(reinterpret_cast<char *>(& hasBones), sizeof(bool));
    }
    stream.close();
}

void the::object::computeTBN()
{
    for (auto & surface: surfaces)
        computeTBN(surface);   
}
void the::object::computeTBN(the::surface &surface)
{
    auto &ndx = surface.getIndecies();
    for (uint i = 0; i < ndx.size(); i += 3)
    {   
        auto &v0 = vertexes[ndx[i    ]];
        auto &v1 = vertexes[ndx[i + 1]];
        auto &v2 = vertexes[ndx[i + 2]];
    
        auto Edge1 = v1.position - v0.position;
        auto Edge2 = v2.position - v0.position;
    
        vec2 dUV1(v1.texcoord.x - v0.texcoord.x, v1.texcoord.y - v0.texcoord.y);
        vec2 dUV2(v2.texcoord.x - v0.texcoord.x,v2.texcoord.y - v0.texcoord.y);
    
        double f = 1.0 / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);
        vec3 tangent = (Edge1 * dUV2.y - Edge2 * dUV1.y) * f;
        v0.tangent = tangent.normalize();
        v0.binormal = v0.normal.cross(v0.tangent);
        v1.tangent = v0.tangent;
        v2.tangent = v0.tangent;
        v1.binormal = v0.binormal;
        v2.binormal = v0.binormal;
    }  
}

void the::object::deserialize(the::filesystem::stream &stream)
{
    TheFile the_file(stream);
    ChunkHeader vertexHdr = the_file.getHeader(ChunkType::VERTEX);
    vertexes.resize(vertexHdr.size/Vertex::OFFSET[NDX::TANGENT]);
    stream.seek(vertexHdr.offset);

    stream.read((char *)&vertexes[0], vertexHdr.size);

    std::vector<ChunkHeader> surfaceHdrs = the_file.getHeaders(ChunkType::SURFACE);
    surfaces.reserve(surfaceHdrs.size());
    for(uint i = 0; i < surfaceHdrs.size(); i++)
    {
        stream.seek(surfaceHdrs[i].offset);
        int len;
        stream.read(reinterpret_cast<char *>(&len), sizeof(int));
        char *str = new char[len + 1]();
        stream.read(str, len);
        str[len] = '\0';
        int idxSize;
        stream.read(reinterpret_cast<char *>(&idxSize), sizeof(idxSize));
        std::vector<uint16_t> indices(idxSize);
        stream.read((char *)&indices[0], idxSize * sizeof(uint16_t));
        bool hasBones;
        stream.read((char *)&hasBones, sizeof(bool));
        surfaces.emplace_back(std::move(the::surface(std::move(indices), std::string(str),hasBones)));
    }
    remap();
}

void the::object::load(strref filename)
{
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("[Object ] file %s not exist",filename.c_str());
    vertexes.clear();
    surfaces.clear();
    the::filesystem::stream stream(filename);
    deserialize(stream);
}

void the::object::serialize(pugi::xml_node &node)
{
    logger::error("call unimplemented method the::object::serialize()");
}

void the::object::deserialize(const pugi::xml_node & node)
{
    the::node::deserialize(node);
    remap();
}

void the::object::log()
{
    ;  
    for (auto & s : surfaces)
        logger::system("'%s'|%d|%s|%s|%s|%s",
                        s.getTag().c_str(),
                        s.getRenderOrder(),
                        s.getBlending() ? "true " : "false",
                        s.getMaterial()->getTag().c_str(),
                        aux::trim(s.getTexture(0)->getTag(),aux::dataPath).c_str(),
                        getEnabled() ? "enabled" : "disabled");
     

}

std::vector<Intersection> the::object::getIntersection(vec3 p0, vec3 p1)
{
    auto iTransform = getWorldTransform().inverse();
    p0 = p0 * iTransform;
    p1 = p1 * iTransform;
    vec3 rayPos = p0;
    vec3 rayDir = (p1 - p0).normalize();

    std::vector<Intersection> intersections;
    int id = 0;
    for (auto & s : surfaces)
    {
        if (s.getIntersection() && s.getEnabled())
        {
            std::vector<uint16_t> &indices = s.getIndecies();
            for (size_t i = 0; i < indices.size(); i += 3)
            {
                vec3 point;
                if (s.intersectTriangle(vertexes[indices[i]].position, vertexes[indices[i + 1]].position, vertexes[indices[i + 2]].position, rayPos, rayDir, point))
                {
                    intersections.push_back(
                        Intersection(id, point,
                                     vertexes[indices[i]].position,
                                     vertexes[indices[i + 1]].position,
                                     vertexes[indices[i + 2]].position)
                    );
                }
            }
        }
        id++;
    }

    return intersections;
}

BoundingBox the::object::getBoundingBox()
{
    BoundingBox b = boundingBox;
    b.transform = getWorldTransform();
    return b;
}

void the::object::recalculateBoundingBox()
{
    float xMax = std::numeric_limits<float>::min();
    float yMax = std::numeric_limits<float>::min();
    float zMax = std::numeric_limits<float>::min();
    float xMin = std::numeric_limits<float>::max();
    float yMin = std::numeric_limits<float>::max();
    float zMin = std::numeric_limits<float>::max();

    for (auto & v : vertexes)
    {
        if (v.position.x > xMax) xMax = v.position.x;
        if (v.position.y > yMax) yMax = v.position.y;
        if (v.position.z > zMax) zMax = v.position.z;

        if (v.position.x < xMin) xMin = v.position.x;
        if (v.position.y < yMin) yMin = v.position.y;
        if (v.position.z < zMin) zMin = v.position.z;
    }

    boundingBox = BoundingBox(vec3(xMin, yMin, zMin), vec3(xMax, yMax, zMax));
}

void the::object::orderSort()
{
    bool blend = false;
    for (auto & s : surfaces) blend = blend || s.getBlending();
    setBlend(blend);    
    if(blend)
    {    
        for (auto & s : surfaces) s.unbind();
        std::sort(surfaces.begin(), surfaces.end(),[&](the::surface l, the::surface r) 
        {
            return (l.getRenderOrder() < r.getRenderOrder());
        });
        for (auto & s : surfaces) s.bind();
    } 
    for(auto &ch : childs) ch->orderSort(); 
    remap();  
}
void the::object::forEachSurface(std::function<void(the::surface &)> func) {for(auto &s : surfaces) func(s);}   


void the::object::addAction(strref name, Action && action)
{
    initHandlers();
    logger::debug("[Action ] add '%s'; object: '%s' surface : %d", name.c_str(), getTag().c_str(), action.getSurface());
    getSurface(action.getSurface()).setIntersection(true);
    actions[name] = action;
}

void the::object::addAction(strref name, int surface, Action::cbRef callback)
{
    addAction(name, std::move(Action(Action::type::PRESS, surface, callback)));
}
void the::object::addAction(strref name, strref surface, Action::cbRef callback)
{
    addAction(name, std::move(Action(Action::type::PRESS, getSurfaceId(surface), callback)));
}
void the::object::addTapAction(strref name, int surface, Action::cbRef callback)
{
    addAction(name, std::move(Action(Action::type::TAP, surface, callback)));
}
void the::object::addTapAction(strref name, strref surface, Action::cbRef callback)
{
    addAction(name, std::move(Action(Action::type::TAP, getSurfaceId(surface), callback)));
}

void the::object::removeAction(const std::string &name)
{
    auto it = actions.find(name);
    if(it != actions.end())
    {
        getSurface(it->second.getSurface()).setIntersection(false);
        it->second.destroy();
    }
}

void the::object::removeAllActions()
{
    for(auto & i : actions) 
    {
        getSurface(i.second.getSurface()).setIntersection(false);
        i.second.destroy();

    }
}

void the::object::checkAction(const Action::type &type, int x, int y)
{
    Intersection intersect;
    auto engine = aux::engine;
    auto trgObject = Intersection::get(x, y, intersect);

    if(trgObject && trgObject.get() == this)
    {
        for(auto i = actions.begin(); i != actions.end();)
        {
            auto action = i->second;
            if(action.destroed())
            {
                logger::extra("[Action ] del '%s'; object: '%s' surface : %d", i->first.c_str(), getTag().c_str(), action.getSurface());
                i = actions.erase(i);
            }    
            else
            {    
                if(action.getActionType() == type && action.getSurface() == intersect.surfaceId) 
                {    
                    action.invoke(intersect.point);
                }
                i++;
            }
        }
    }
}

void the::object::initHandlers()
{
    if(mousePressHandler== 0 && mouseTapHandler== 0 )
    {
        mousePressHandler = aux::engine->getInput()->addPressHandler([&](uint x, uint y) {checkAction(Action::type::PRESS,x, y);});
        mouseTapHandler   = aux::engine->getInput()->addTapHandler([&](uint x, uint y)   {checkAction(Action::type::TAP,x, y);});
    }        
}

std::vector<size_t> the::object::getAllHandlersId()
{
    return {mouseTapHandler,mousePressHandler};
}


void the::object::setSurfaceEnabled(strref s, bool mode)
{
    getSurface(s).setEnabled(mode);
}

void the::object::setSurfaceEnabled(int s, bool mode)
{
    getSurface(s).setEnabled(mode);
}

void the::object::setSurfaceEnabled(bool mode)
{
    forEachSurface([mode](the::surface &s)
    {
        s.setEnabled(mode);
    });
}