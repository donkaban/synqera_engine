#ifndef _THE_RENDER_STATE_
#define _THE_RENDER_STATE_

#include "TheEngine/math/all.h"
#include "TheEngine/openGL.h"

namespace render {
struct state
{
    mat4  modelMatrix         = mat4_identity; 
    mat4  viewMatrix          = mat4_identity;
    mat4  inverseViewMatrix   = mat4_identity;
    mat4  projectionMatrix    = mat4_identity;
    mat4  prjViewMatrix       = mat4_identity;

    vec3  eyePosition         = vec3_zero;
    vec3  lightPosition       = vec3_zero;
    vec4  lightAmbientColor   = vec4_zero;
    vec4  lightDiffuseColor   = vec4_zero;
    vec4  lightSpecularColor  = vec4_zero;
          
    float currentTime         = 0.0;
};
}

#endif


