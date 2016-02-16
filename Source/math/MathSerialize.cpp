#include "TheEngine/math/all.h"

void quaternion::serialize(pugi::xml_node &node)
{
    node.append_attribute("r") = s;
    node.append_attribute("x") = v.x;
    node.append_attribute("y") = v.x;
    node.append_attribute("z") = v.x;

}
void quaternion::deserialize(const pugi::xml_node &node)
{
    s   = node.attribute("r").as_float();
    v.x = node.attribute("x").as_float();
    v.y = node.attribute("y").as_float();
    v.z = node.attribute("z").as_float();
}

void mat3::serialize(pugi::xml_node &node)
{
    pugi::xml_node n;
    n = node.append_child("m0") ; mat[0].serialize(n);
    n = node.append_child("m1") ; mat[1].serialize(n);
    n = node.append_child("m2") ; mat[2].serialize(n);
}

void mat3::deserialize(const pugi::xml_node &node)
{
    pugi::xml_node n;
    n = node.child("m0") ; mat[0].deserialize(n);
    n = node.child("m1") ; mat[1].deserialize(n);
    n = node.child("m2") ; mat[2].deserialize(n);
}

void mat4::serialize(pugi::xml_node &node)
{
    pugi::xml_node n;
    n = node.append_child("m0") ; mat[0].serialize(n);
    n = node.append_child("m1") ; mat[1].serialize(n);
    n = node.append_child("m2") ; mat[2].serialize(n);
    n = node.append_child("m3") ; mat[3].serialize(n);
}

void mat4::deserialize(const pugi::xml_node &node)
{   
    pugi::xml_node m0 = node.child("m0");
    pugi::xml_node m1 = node.child("m1");
    pugi::xml_node m2 = node.child("m2");
    pugi::xml_node m3 = node.child("m3");
    
    if(!m0.empty() && !m1.empty() && !m2.empty() && !m3.empty())
    {
        logger::debug("[Math   ] deserialize mat4");
 
        mat[0].deserialize(m0);   
        mat[1].deserialize(m1);
        mat[2].deserialize(m2);
        mat[3].deserialize(m3);
        return;
    }

    pugi::xml_node pos_n = node.child("position");
    pugi::xml_node rot_n = node.child("rotate");
    pugi::xml_node sca_n = node.child("scale");
    if(pos_n.empty() && rot_n.empty() && sca_n.empty())
        FATAL_ERROR("[Math   ] can't deserialize mat4. bad format!");
    vec3 p(0,0,0);
    vec3 r(0,0,0);
    vec3 s(1,1,1);

    if (!pos_n.empty())  p.deserialize(pos_n);
    if (!rot_n.empty())  r.deserialize(rot_n);
    if (!sca_n.empty())  s.deserialize(sca_n);  

    mat4 T = mat4::translate(p) * 
             mat4::rotateZ(r.z * 0.01745329251) *
             mat4::rotateY(r.y * 0.01745329251) *
             mat4::rotateX(r.x * 0.01745329251) *
             mat4::scale(s);
    std::memcpy(mat, &T, sizeof(T));   
}

