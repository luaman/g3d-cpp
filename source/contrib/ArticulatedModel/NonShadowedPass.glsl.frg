/**
  @file SuperShader.glsl.frg
  @author Morgan McGuire matrix@graphics3d.com

  For use with G3D::SuperShader.

 */

/** World space, normalized (TODO: non-directional light, multiple lights) */
uniform vec4        lightPosition;
uniform vec3        lightColor;

uniform vec3        environmentConstant;
uniform samplerCube environmentMap;

// Material
uniform sampler2D   reflectMap;
uniform vec3        reflectConstant;

uniform sampler2D   specularMap;
uniform vec3        specularConstant;

uniform vec3        specularExponentConstant;
#ifdef SPECULAREXPONENTMAP
    uniform sampler2D   specularExponentMap;
#endif

#ifdef EMITCONSTANT
    uniform vec3        emitConstant;
#endif

#ifdef EMITMAP
    uniform sampler2D   emitMap;
#endif

#ifdef DIFFUSECONSTANT
    uniform vec3        diffuseConstant;
#endif

#ifdef DIFFUSEMAP
    uniform sampler2D   diffuseMap;
#endif

/** Multiplier for bump map.  Typically on the range [0, 0.05]
  This increases with texture scale and bump height. */
uniform float       bumpMapScale;

/** xyz = normal, w = bump height */
uniform sampler2D   normalBumpMap;

uniform vec3        ambientTop;
uniform vec3        ambientBottom;

// World parameters
varying vec3        wsEyePos;
varying vec3        wsPosition;
varying vec3        _tsE;
varying vec2        texCoord;
varying vec4        tan_X, tan_Y, tan_Z, tan_W;


void main(void) {

    const vec3 diffuseColor =
#   ifdef DIFFUSECONSTANT
        diffuseConstant
#       ifdef DIFFUSEMAP
             * tex2D(diffuseMap, texCoord).rgb
#       endif
#   else
#       ifdef DIFFUSEMAP
             tex2D(diffuseMap, texCoord).rgb
#       else
             vec3(0, 0, 0)
#       endif
#   endif
        ;

    const vec3 emitColor =
#   ifdef EMITCONSTANT
        emitConstant
#       ifdef EMITMAP
             * tex2D(emitMap, texCoord).rgb
#       endif
#   else
#       ifdef EMITMAP
             tex2D(emitMap, texCoord).rgb
#       else
             vec3(0, 0, 0)
#       endif
#   endif
        ;

    // World space normal
    const vec3 wsN = tan_Z.xyz;
    const vec3 wsL = lightPosition.xyz;
    vec3 wsE = wsEyePos - wsPosition;
    const vec3 wsR = normalize((wsN * 2.0 * dot(wsN, wsE)) - wsE);

    // tan_Z is world space normal
    vec3 ambient = ambientTop + (ambientTop - ambientBottom) * min(wsN.y, 0);

    gl_FragColor.rgb =
#       if defined(EMITCONSTANT) || defined(EMITMAP)
            // Emissive
            emitColor +
#       endif

#       if defined(DIFFUSECONSTANT) || defined(DIFFUSEMAP)
            // Ambient
            diffuseColor * ambient +

            // Diffuse
            max(dot(wsL, wsN), 0) * lightColor * diffuseColor +
#       endif

        // Specular
        pow(max(dot(wsL, wsR), 0), specularExponentConstant) * lightColor * specularConstant +
        
        // Reflection
        textureCube(environmentMap, wsR).rgb * environmentConstant * reflectConstant;

}

