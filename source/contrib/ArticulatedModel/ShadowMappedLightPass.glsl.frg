/**
  @file ShadowMappedLightPass.glsl.frg
  @author Morgan McGuire matrix@graphics3d.com

  For use with G3D::SuperShader.

 */

/** World space, normalized (TODO: non-directional light, multiple lights) */
uniform vec4        lightPosition;
uniform vec3        lightColor;

uniform sampler2D   specularMap;
uniform vec3        specularConstant;

uniform sampler2D   specularExponentMap;
uniform vec3        specularExponentConstant;

uniform sampler2D   diffuseMap;
uniform vec3        diffuseConstant;

/** Multiplier for bump map.  Typically on the range [0, 0.05]
  This increases with texture scale and bump height. */
uniform float       bumpMapScale;

/** xyz = normal, w = bump height */
uniform sampler2D   normalBumpMap;

uniform sampler2DShadow shadowMap;

// World parameters
varying vec3        wsEyePos;
varying vec3        wsPosition;
varying vec3        _tsE;
varying vec4        tan_X, tan_Y, tan_Z, tan_W;

/** Coordinate to use in the shadow map */
varying vec4        shadowCoord;

void main(void) {
    //	texCoord	= gl_MultiTexCoord0.st;

    // World space normal
    const vec3 wsN = tan_Z.xyz;
    const vec3 wsL = lightPosition.xyz;
    vec3 wsE = normalize(wsEyePos - wsPosition);
    const vec3 wsR = (wsN * 2.0 * dot(wsN, wsE)) - wsE;

//    vec3 shadow = shadow2DProj(shadowMap, shadowCoord).xyz;

    // Compute projected shadow coord.
    shadowCoord = shadowCoord / shadowCoord.w;

    const float s = 1.0 / 512.0;
    vec3 shadow = 
         shadow2D(shadowMap, shadowCoord.xyz).xyz / 3.0 +
         (shadow2D(shadowMap, shadowCoord.xyz + vec3( 0,  s, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3( 0, -s, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3( s,  0, 0)).xyz +
          shadow2D(shadowMap, shadowCoord.xyz + vec3(-s,  0, 0)).xyz) / 6.0;

    gl_FragColor.rgb =
        lightColor * shadow *

          // Diffuse
        ( max(dot(wsL, wsN), 0) * diffuseConstant +

          // Specular
          pow(max(dot(wsL, wsR), 0), specularExponentConstant) * specularConstant);
}

