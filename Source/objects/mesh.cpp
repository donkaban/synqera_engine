#include "TheEngine/objects/mesh.h"
#include "TheEngine/engine.h"

the::mesh::mesh() :
   the::object("Mesh","--") 
{}
the::mesh::mesh(strref filename) :  
   the::object("Mesh",filename) 
{   
    load(filename);
}

the::mesh::~mesh() {}    

the::mesh::ptr the::mesh::createPlane(const vec2 &dim)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();
    g->setTag(aux::format("auto::plane::%.2f::%.2f", dim.x, dim.y));
    float w = dim.x * .5;
    float h = dim.y * .5;

    g->addVertex(
    {
       {{ w, h, 0}, {1, 1}, {0, 0, 1}},
       {{-w, h, 0}, {0, 1}, {0, 0, 1}},
       {{-w,-h, 0}, {0, 0}, {0, 0, 1}},
       {{ w,-h, 0}, {1, 0}, {0, 0, 1}},
    });
    g->addSurface(std::move(the::surface({0, 1, 2, 2, 3, 0}, "plane")));    
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));

    return g;
}

the::mesh::ptr the::mesh::createTriangle(const vec3 &v1,const vec3 &v2,const vec3 &v3)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();
    g->setTag("auto::triangle");

    vec3 a = v1 - v2;
    vec3 b = v3 - v2;
    vec3 n = a.cross(b);
    n = n.normalize();

    g->addVertex(
    {
       {v1, {1, 1}, n},
       {v2, {0, 1}, n},
       {v3, {0, 0}, n}
    });
    g->addSurface(std::move(the::surface({0, 2, 1}, "triangle")));    
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));

    return g;
}

the::mesh::ptr the::mesh::createSlicedPlane(const vec2& dim, const vec2& pow)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();
    g->setTag(aux::format("auto::plane::%.2f::%.2f", dim.x, dim.y));
    
    float wStep = dim.x / pow.x;
    float hStep = dim.y / pow.y;
    float sStep = 1.0  / pow.x;    
    float tStep = 1.0  / pow.y;    
    
    float width  = dim.x * .5;
    float height = dim.y * .5;
    
    float s = 0.0;
    float t = 1.0;    

    for (float h = height; h >= -height; h -= hStep, t -= tStep)
    {   
        s = 0.0;         
        for (float w = -width; w <= width; w += wStep, s += sStep)
        {   
            g->addVertex({{w, h, 0}, {s, t}, {0, 0, 1}});
        }
    }
    
    the::surface surface;
    uint16_t i = 0;
    uint16_t offset = pow.x + 1;
    for (uint16_t y = 0; y <= pow.y; ++y)
    {
        for (uint16_t x = 0; x <= pow.x; ++x)        
        {
            uint16_t two = i + 1;
            uint16_t offseted = i + offset;
            surface.add({two, i, offseted, offseted, static_cast<uint16_t>(two + offset), two}, "");
            ++i;
        }
    }
    g->addSurface(std::move(surface));    
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));
    return g;
}


the::mesh::ptr the::mesh::createCube(const vec3 &dim)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();

    g->setTag(aux::format("auto::cube::%.2f::%.2f::%.2f", dim.x, dim.y, dim.z));
  
    float w = dim.x * .5;
    float h = dim.y * .5;
    float z = dim.z * .5;

    g->addVertex(
    {
        {{ -w, -h, -z}, {0, 0}, { 0,-1, 0}}, // 0  bottom
        {{ -w, -h,  z}, {0, 1}, { 0,-1, 0}}, // 1
        {{  w, -h,  z}, {1, 1}, { 0,-1, 0}}, // 2
        {{  w, -h, -z}, {1, 0}, { 0,-1, 0}}, // 3

        {{ -w,  h, -z}, {1, 0}, { 0, 1, 0}}, // 4  top
        {{ -w,  h,  z}, {1, 1}, { 0, 1, 0}}, // 5
        {{  w,  h,  z}, {0, 1}, { 0, 1, 0}}, // 6
        {{  w,  h, -z}, {0, 0}, { 0, 1, 0}}, // 7

        {{ -w, -h, -z}, {0, 0}, { 0, 0,-1}}, // 8  front
        {{ -w,  h, -z}, {0, 1}, { 0, 0,-1}}, // 9
        {{  w,  h, -z}, {1, 1}, { 0, 0,-1}}, //10
        {{  w, -h, -z}, {1, 0}, { 0, 0,-1}}, //11

        {{ -w, -h,  z}, {0, 0}, { 0, 0, 1}}, // 12  back
        {{ -w,  h,  z}, {0, 1}, { 0, 0, 1}}, // 13
        {{  w,  h,  z}, {1, 1}, { 0, 0, 1}}, // 14
        {{  w, -h,  z}, {1, 0}, { 0, 0, 1}}, // 15

        {{ -w, -h, -z}, {0, 0}, {-1, 0, 0}}, // 16  left
        {{ -w, -h,  z}, {0, 1}, {-1, 0, 0}}, // 17
        {{ -w,  h,  z}, {1, 1}, {-1, 0, 0}}, // 18
        {{ -w,  h, -z}, {1, 0}, {-1, 0, 0}}, // 19

        {{  w, -h, -z}, {0, 0}, { 1, 0, 0}}, // 20  right
        {{  w, -h,  z}, {0, 1}, { 1, 0, 0}}, // 21
        {{  w,  h,  z}, {1, 1}, { 1, 0, 0}}, // 22
        {{  w,  h, -z}, {1, 0}, { 1, 0, 0}}, // 23
    });

    g->addSurface(
    {
        the::surface({0, 2, 1, 0, 3, 2},       "bottom"),
        the::surface({4, 5, 6, 4, 6, 7},       "top"),
        the::surface({8, 9, 10, 8, 10, 11},    "front"),
        the::surface({12, 15, 14, 12, 14, 13}, "back"),
        the::surface({16, 17, 18, 16, 18, 19}, "left"),
        the::surface({20, 23, 22, 20, 22, 21}, "right")
    });
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));
    return g;
}

the::mesh::ptr the::mesh::createMonoCube(const vec3 &dim)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();

    g->setTag(aux::format("auto::monocube::%.2f::%.2f::%.2f", dim.x, dim.y, dim.z));
 
    float w = dim.x * .5;
    float h = dim.y * .5;
    float z = dim.z * .5;

    g->addVertex(
    {
        {{ -w, -h, -z}, {0, 0}, { 0,-1, 0}}, // 0  bottom
        {{ -w, -h,  z}, {0, 1}, { 0,-1, 0}}, // 1
        {{  w, -h,  z}, {1, 1}, { 0,-1, 0}}, // 2
        {{  w, -h, -z}, {1, 0}, { 0,-1, 0}}, // 3

        {{ -w,  h, -z}, {1, 0}, { 0, 1, 0}}, // 4  top
        {{ -w,  h,  z}, {1, 1}, { 0, 1, 0}}, // 5
        {{  w,  h,  z}, {0, 1}, { 0, 1, 0}}, // 6
        {{  w,  h, -z}, {0, 0}, { 0, 1, 0}}, // 7

        {{ -w, -h, -z}, {0, 0}, { 0, 0,-1}}, // 8  front
        {{ -w,  h, -z}, {0, 1}, { 0, 0,-1}}, // 9
        {{  w,  h, -z}, {1, 1}, { 0, 0,-1}}, //10
        {{  w, -h, -z}, {1, 0}, { 0, 0,-1}}, //11

        {{ -w, -h,  z}, {0, 0}, { 0, 0, 1}}, // 12  back
        {{ -w,  h,  z}, {0, 1}, { 0, 0, 1}}, // 13
        {{  w,  h,  z}, {1, 1}, { 0, 0, 1}}, // 14
        {{  w, -h,  z}, {1, 0}, { 0, 0, 1}}, // 15

        {{ -w, -h, -z}, {0, 0}, {-1, 0, 0}}, // 16  left
        {{ -w, -h,  z}, {0, 1}, {-1, 0, 0}}, // 17
        {{ -w,  h,  z}, {1, 1}, {-1, 0, 0}}, // 18
        {{ -w,  h, -z}, {1, 0}, {-1, 0, 0}}, // 19

        {{  w, -h, -z}, {0, 0}, { 1, 0, 0}}, // 20  right
        {{  w, -h,  z}, {0, 1}, { 1, 0, 0}}, // 21
        {{  w,  h,  z}, {1, 1}, { 1, 0, 0}}, // 22
        {{  w,  h, -z}, {1, 0}, { 1, 0, 0}}, // 23
    });

    g->addSurface(std::move(the::surface(
    {
        0,  2,  1,  0, 
        3,  2,  4,  5, 
        6,  4,  6,  7,
        8,  9,  10, 8, 
        10, 11, 12, 15, 
        14, 12, 14, 13,
        16, 17, 18, 16, 
        18, 19, 20, 23, 
        22, 20, 22, 21
    },"cube")));
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));
    return g;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

the::mesh::ptr the::mesh::createSphere(float radius, int slices)
{

    the::mesh::ptr g = std::make_shared<the::mesh>();

    float step = (the::math::TWO_PI) / (static_cast<float>(slices));
    int parallels = slices / 2;
    float r = radius / 1.5;

    for (int i = 0; i < parallels + 1; i++)
    {
        for (int j = 0; j < slices + 1; j++)
        {
            vec3 pos(r * the::math::sin(step * static_cast<float>(i)) * the::math::sin(step * static_cast<float>(j)),
                     r * the::math::cos(step * static_cast<float>(i)),
                     r * the::math::sin(step * static_cast<float>(i))* the::math::cos(step * static_cast<float>(j)));

            g->addVertex(Vertex(pos, {static_cast<float>(j) / static_cast<float>(slices), static_cast<float>(i) / static_cast<float>(parallels)}, pos / r));
        }
    }
    g->surfaces.push_back({});
    for (uint8_t i = 0; i < slices / 2; i++)
    {

        for (uint8_t j = 0; j < slices; j++)
        {
            g-> surfaces[0].add((i * (slices + 1) + j));
            g-> surfaces[0].add(((i + 1) * (slices + 1) + j));
            g-> surfaces[0].add(((i + 1) * (slices + 1) + (j + 1)));
            g-> surfaces[0].add((i * (slices + 1) + j));
            g-> surfaces[0].add(((i + 1) * (slices + 1) + (j + 1)));
            g-> surfaces[0].add((i * (slices + 1) + (j + 1)));
        }
    }
    g->surfaces[0].setTag("auto::sphere");
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));
    return g;
}

the::mesh::ptr the::mesh::createTorus(float innerRadius, float outterRadius, uint sides, uint rings)
{
    the::mesh::ptr g = std::make_shared<the::mesh>();

    float phi, psi, dpsi, dphi, spsi, sphi, cpsi, cphi;
    if (sides < 3) sides = 3;
    if (rings < 3) rings = 3;

    uint16_t bigSides = sides + 1;
    uint16_t bigRings = rings + 1;

    dpsi =  2.0 * M_PI / static_cast<float>(rings);
    dphi = -2.0 * M_PI / static_cast<float>(sides);

    float tStep = 1.0 / static_cast<float>(bigSides);
    float sStep = 1.0 / static_cast<float>(bigRings);

    psi = 0.0;
    float s = 0;
    for (int j = 0; j < bigRings; ++j)
    {
        cpsi = the::math::cos(psi);
        spsi = the::math::sin(psi);
        phi = 0.0;                
        float t = 0;
        for (int i = 0; i < bigSides; ++i)
        {               
            cphi = the::math::cos(phi);
            sphi = the::math::sin(phi);
            g->addVertex({{cpsi * (outterRadius + cphi * innerRadius),
                           spsi * (outterRadius + cphi * innerRadius),
                           sphi * innerRadius
                           },
                           {s, t},
                           {cpsi * cphi,
                            spsi * cphi,
                            sphi
                           }});            
            phi += dphi;
            t += tStep; 
        }
        psi += dpsi;
        s += sStep;        
    }

    the::surface surface;
    
    uint16_t i = 0;    
    for (uint16_t x = 0; x <= rings; ++x)
    {                 
        for (uint16_t y = 0; y < sides; ++y)        
        {
            uint16_t tl = i;        
            uint16_t bl = i + 1;        
            uint16_t br = bl + bigSides;                    
            uint16_t tr = i + bigSides;              
            surface.add({tr, tl, bl, bl, br, tr});
            ++i;
        }           
    }
    g->addSurface(std::move(surface));    
    g->computeTBN();
    g->bind();
    g->setMaterial(the::material::get("def::base"));
    g->setTexture(0, the::texture::get("def::base"));
    return g;
}

void the::mesh::load(const std::string &filename)
{
    unbind();
    the::object::load(filename);
    setMaterial(the::material::get("def::base"));
    setTexture(0, the::texture::get("def::base"));
    bind();
}

void the::mesh::onRender(render::state &state)
{
    the::object::onRender(state);
    for (auto & s : surfaces)
    {
        s.setDefaultParameters(state);
        s.setUserParameters(state);
        s.draw();
    }
}

void the::mesh::serialize(pugi::xml_node &node)
{
    logger::error("call not implemented method the::mesh::serialize()");
}

void the::mesh::deserialize(const pugi::xml_node &node)
{
    the::object::deserialize(node);
    setMaterial(the::material::get("def::base"));
    setTexture(0, the::texture::get("def::base"));
    std::string defaultPath = getDefaultPath();
    std::string filename = node.attribute("file").value();
    setTag(filename);
    load(getDefaultPath() + filename);
    for (auto s = node.child("surface"); s; s = s.next_sibling("surface"))
        getSurface(s.attribute("name").value()).deserialize(s,defaultPath);
}

void the::mesh::log()
{
    logger::system("Mesh '%s' order: %d blend: %s", getTag().c_str(),getRenderOrder(),getBlend()? "true" : "false");
    the::object::log();
    ;  
    for(const auto &child : childs) child->log();
     
}

the::mesh::ptr the::mesh::load(strref path, strref xmlfile)
{
    the::mesh::ptr res = std::dynamic_pointer_cast<the::mesh>(the::node::load(aux::engine->getDataPath() + xmlfile, aux::engine->getDataPath()+path));
    if(!res)
        FATAL_ERROR("can't cast to Mesh : %s",(aux::engine->getDataPath() + xmlfile).c_str());
    aux::engine->addObject(res);
    return res;
}