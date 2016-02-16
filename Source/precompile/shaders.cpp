#include "TheEngine/pre.h"

std::string vertex_header = R"(
attribute vec3 position;
attribute vec2 texcoord;
attribute vec3 tangent;
attribute vec3 binormal;
attribute vec3 normal;
attribute vec4 bone_id;
attribute vec4 bone_weight;

uniform mat4  modelMatrix;
uniform mat4  prjMatrix;    
uniform mat4  viewMatrix; 
uniform mat4  inverseViewMatrix; 
uniform mat4  prjViewMatrix;   

uniform vec3  lightPosition;    
uniform vec3  eyePosition;    
uniform float time;

uniform vec3  place0;    
uniform vec3  place1;    
uniform vec3  place2;    
uniform vec3  place3;    

uniform mat4  bones[56];

)";
/*----------------------------------------------------------------------------------------------------*/

std::string fragment_header = R"(
uniform samplerCube cubemap;
uniform sampler2D   texture0;
uniform sampler2D   texture1;
uniform sampler2D   texture2;
uniform sampler2D   texture3;

uniform vec4 lightDiffuse;        
uniform vec4 lightSpecular;
uniform vec4 lightAmbient;



)";

namespace precompiled { namespace shader {

shader base = shader("def::base",
vertex_header +  R"(
    varying vec2 v_texcoord;
    varying vec3 v_tangent;
    varying vec3 v_binormal;
    varying vec3 v_norm;
    varying vec3 v_light;
    varying vec3 v_eye; 
    varying vec3 v_half; 
    
    varying vec3 v_texcoord3D;  

    void main() 
    {
        vec4 p4;
     
     #ifdef SKINNING
        mat4 M  = bones[int(bone_id.x)] * bone_weight.x;
             M += bones[int(bone_id.y)] * bone_weight.y;
             M += bones[int(bone_id.z)] * bone_weight.z;
             M += bones[int(bone_id.w)] * bone_weight.w;
        mat3 A = mat3(M[0].xyz,M[1].xyz,M[2].xyz);     
        p4 = M * vec4(position, 1.0) * modelMatrix;
    #else    
        p4 = vec4(position ,1.) * modelMatrix;
    #endif 
        vec3 p3 = vec3(p4);    

    #ifdef LIGHTING
        mat3 normalMatrix = mat3(modelMatrix[0].xyz,modelMatrix[1].xyz,modelMatrix[2].xyz);
        v_light      = normalize(lightPosition - p3);
        v_eye        = normalize(eyePosition   - p3);
        v_half       = normalize(v_light + v_eye);
   
        #ifdef SKINNING
            v_tangent    = normalize(A * tangent  * normalMatrix);
            v_binormal   = normalize(A * binormal * normalMatrix);
            v_norm       = normalize(A * normal   * normalMatrix);
        #else
            v_tangent    = normalize(tangent  * normalMatrix);
            v_binormal   = normalize(binormal * normalMatrix);
            v_norm       = normalize(normal   * normalMatrix);

        #endif
    #endif  
    #ifdef CUBEMAP
        mat3 rotationMatrixY =mat3(vec3(cos(place0.y),0.0,sin(place0.y)),
                            vec3(0.0,1.0,0.0),
                            vec3(-sin(place0.y),0.0,cos(place0.y)));
        mat3 rotationMatrixX =mat3(vec3(1.0,0.0,0.0),
                            vec3(0.0,cos(place0.x),-sin(place0.x)),
                            vec3(0.0,sin(place0.x),cos(place0.x)));
        v_texcoord3D = vec3(vec4(reflect(p3,normalize(v_norm*rotationMatrixX*rotationMatrixY)),.0) * inverseViewMatrix);
    #endif
        v_texcoord = texcoord;
        gl_Position = p4 * prjViewMatrix;
    })",

fragment_header + R"(
    varying vec2 v_texcoord;
    varying vec3 v_texcoord3D; 
    varying vec3 v_tangent;
    varying vec3 v_binormal;
    varying vec3 v_norm;
    varying vec3 v_light;
    varying vec3 v_eye; 
    varying vec3 v_half; 
      
    const float SPEC_POWER = 55.0;  
    void main() 
    {
        vec4 col = texture2D(texture0, v_texcoord);
        float a = col.a;            
        
        #ifdef LIGHTING
            vec3 normal;
            #ifdef NORMALMAP
                vec4 norm_value = texture2D (texture1,v_texcoord);
                normal = normalize(2.0 * vec3(norm_value) - 1.0);
                normal *= mat3(v_tangent,v_binormal,v_norm);
             #else
                normal = v_norm;
            #endif    
            float shadow = max(dot(v_norm,v_light),.2);
            vec3  phong  = reflect(-v_eye,v_norm);
            float spec   = pow(max(dot(v_light,phong),.0), SPEC_POWER);
            col *= (shadow * lightDiffuse);              
            col += (spec * lightSpecular);

        #endif
        #ifdef CUBEMAP
            col += (textureCube(cubemap,v_texcoord3D)* 0.25);
        #endif
        col.a = a;
        gl_FragColor = col;
    })"
);

/*----------------------------------------------------------------------------------------------------*/

shader alphatest = shader("def::alphatest",
vertex_header + R"(
    varying vec2 v_texcoord;
    void main() 
    {
        vec4 pos   = vec4(position,1.);
        v_texcoord = texcoord;
        gl_Position = pos * modelMatrix * prjViewMatrix;
    })",
fragment_header + R"(
    uniform float mixer;
    varying vec2 v_texcoord;
    void main() 
    {
        vec4 col1 = texture2D(texture0, v_texcoord);
        if (col1.a < 0.1)
            discard;
        gl_FragColor = col1;
    })"
);


shader text = shader("def::text",
vertex_header + R"(
    uniform   vec4  offset;
    varying vec2 v_texcoord;
    varying vec4 v_pos;
    void main()     
    {
        vec4 pos   = vec4(position,1) + offset;
        v_texcoord = texcoord;
        v_pos  = pos;
        gl_Position = pos * modelMatrix * prjViewMatrix;
    })",
fragment_header + R"(
    uniform vec4 color;
    uniform vec4 size;
    uniform float blend_size;
    varying vec2 v_texcoord;
    varying vec4 v_pos;
    void main() 
    {
        float alpha = v_pos.y - size.y > 0.0 && v_pos.y < 0.0 ? 1.0 : 0.0;
        float alpha2 = size.x - v_pos.x > 0.0 && v_pos.x > 0.0 ? 1.0 : 0.0;
        float fade1 = 1.0;//min(1.0,(v_pos.y - size.y) / blend_size);
        float fade2 = 1.0;//min(1.0,-v_pos.y / blend_size);
        float finalAlpha = alpha*alpha2*color.w*fade2*fade1;
        gl_FragColor = texture2D(texture0, v_texcoord) * vec4(color.x,color.y,color.z,finalAlpha);
    })"
);

shader simple_text = shader("def::simple_text",
vertex_header + R"(
    varying vec2 v_texcoord;
    void main()     
    {
        vec4 pos   = vec4(position,1);
        v_texcoord = texcoord;
        gl_Position = pos * modelMatrix * prjViewMatrix;
    })",
fragment_header + R"(
    uniform vec4 color;
    varying vec2 v_texcoord;
    void main() 
    {
        gl_FragColor = texture2D(texture0, v_texcoord) * color;
    })"
);


shader line = shader("def::line",
vertex_header + R"(
    uniform mat4 mpv;
    varying vec4 v_col;

    void main()     
    {
        v_col = bone_id;
        gl_Position = vec4(position,1) * mpv;
    })",
fragment_header + R"(
    varying vec4 v_col;
    void main() 
    {
        gl_FragColor = v_col;
    })"
);

shader mix = shader("def::mix",
base.vertex,
fragment_header + R"(
    uniform float mixer;
    varying vec2 v_texcoord;
    void main() 
    {
        vec4 col1 = texture2D(texture0, v_texcoord);
        vec4 col2 = texture2D(texture1, v_texcoord);
        gl_FragColor = mix(col1,col2,mixer);
    })"
);

/*----------------------------------------------------------------------------------------------------*/


}}