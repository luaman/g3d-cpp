/**
  @file ShadowMappedLightPass.glsl.frg
  @author Morgan McGuire matrix@graphics3d.com

  For use with G3D::SuperShader.

 */

/** World space, normalized (TODO: non-directional light) */
uniform vec4        lightPosition;
uniform vec3        lightColor;

uniform sampler2D   specularMap;
uniform vec3        specularConstant;

uniform vec3        specularExponentConstant;
#ifdef SPECULAREXPONENTMAP
    uniform sampler2D   specularExponentMap;
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

uniform sampler2DShadow shadowMap;

// World parameters
varying vec3        wsEyePos;
varying vec3        wsPosition;
varying vec2        texCoord;
varying vec3        _tsE;
varying vec4        tan_X, tan_Y, tan_Z, tan_W;

/** Coordinate to use in the shadow map */
varying vec4        shadowCoord;

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

    const vec3 wsEyePos = g3d_CameraToWorldMatrix[3].xyz;

    // World space normal
    const vec3 wsN = tan_Z.xyz;
    const vec3 wsL = lightPosition.xyz;
    vec3 wsE = wsEyePos - wsPosition;
    const vec3 wsR = normalize((wsN * 2.0 * dot(wsN, wsE)) - wsE);

    // Compute projected shadow coord.
    shadowCoord = shadowCoord / shadowCoord.w;

    const float s = 0.71 / 512.0;
    vec3 shadow = 
         shadow2D(shadowMap, shadowCoord.xyz).xyz / 3.0 +
         (shadow2D(shadowMap, shadowCoord.xyz + vec3( s,  s, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3( s, -s, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3(-s,  s, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3(-s, -s, 0)).xyz) / 6.0;

    gl_FragColor.rgb =
        lightColor * shadow *

          // Diffuse
        ( max(dot(wsL, wsN), 0) * diffuseColor +

          // Specular
          pow(max(dot(wsL, wsR), 0), specularExponentConstant) * specularConstant);
}

