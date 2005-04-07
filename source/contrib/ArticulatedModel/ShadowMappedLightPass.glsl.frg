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

#if defined(DIFFUSECONSTANT)
    uniform vec3        diffuseConstant;
#endif

#ifdef DIFFUSEMAP
    uniform sampler2D   diffuseMap;
#endif


uniform sampler2DShadow shadowMap;

// World parameters
varying vec3        wsEyePos;
varying vec2        texCoord;
varying vec3        wsPosition;

#ifdef NORMALBUMPMAP
    /** Multiplier for bump map.  Typically on the range [0, 0.05]
      This increases with texture scale and bump height. */
    uniform float       bumpMapScale;

    /** xyz = normal, w = bump height */
    uniform sampler2D   normalBumpMap;

    /** Un-normalized (interpolated) tangent space eye vector */
    varying vec3        _tsE;
#endif

varying vec4        tan_X, tan_Y, tan_Z, tan_W;

/** Coordinate to use in the shadow map */
varying vec4        shadowCoord;

void main(void) {

    vec3 wsEyePos = g3d_CameraToWorldMatrix[3].xyz;

#   ifdef NORMALBUMPMAP
        // Convert bumps to a world space distance
        float  bump   = (texture2D(normalBumpMap, texCoord).w - 0.5) * bumpMapScale;

	    vec3 tsE = normalize(_tsE);

        // Offset the texture coord.  Note that texture coordinates are inverted (in the y direction)
	    // from TBN space, so we must flip the y-axis.

        vec2 offsetTexCoord = texCoord.xy + vec2(tsE.x, -tsE.y) * bump;

	    // note that the columns might be slightly not orthogonal due to interpolation
	    mat4 tangentToWorld = mat4(tan_X, tan_Y, tan_Z, tan_W);
	    
        // Take the normal map values back to (-1, 1) range to compute a tangent space normal
        vec3 tsN = ((texture2D(normalBumpMap, offsetTexCoord).xyz - vec3(0.5, 0.5, 0.5)) * 2.0);

	    // Take the normal to world space 
	    vec3 wsN = (tangentToWorld * vec4(tsN, 0.0)).xyz;

#   else
        vec2 offsetTexCoord = texCoord;

        // World space normal
        vec3 wsN = tan_Z.xyz;
#   endif

    // Light vector      
	vec3 wsL = normalize(lightPosition.xyz - wsPosition.xyz * lightPosition.w);

    // Eye vector
    vec3 wsE = normalize(wsEyePos - wsPosition);
	// or... (tangentToWorld * vec4(tsE, 0.0)).xyz;

    // Reflection vector
    vec3 wsR = normalize((wsN * 2.0 * dot(wsN, wsE)) - wsE);

#   if (defined(DIFFUSECONSTANT) || defined(DIFFUSEMAP))
        vec3 diffuseColor =
#       ifdef DIFFUSECONSTANT
            diffuseConstant
#           ifdef DIFFUSEMAP
                * texture2D(diffuseMap, offsetTexCoord).rgb
#           endif
#       else
            texture2D(diffuseMap, offsetTexCoord).rgb
#       endif
        ;
#   endif

    // Compute projected shadow coord.
    vec4 projShadowCoord = shadowCoord / shadowCoord.w;

    const float s = 0.5 / 512.0;
    vec3 shadow =
         (shadow2D(shadowMap, projShadowCoord.xyz).xyz +
          shadow2D(shadowMap, projShadowCoord.xyz + vec3( s,  s, 0.0)).xyz +
          shadow2D(shadowMap, projShadowCoord.xyz + vec3( s, -s, 0.0)).xyz +
          shadow2D(shadowMap, projShadowCoord.xyz + vec3(-s,  s, 0.0)).xyz +
          shadow2D(shadowMap, projShadowCoord.xyz + vec3(-s, -s, 0.0)).xyz) / 5.0;

    gl_FragColor.rgb =
            lightColor * shadow *
        ( 
#          if defined(DIFFUSECONSTANT) || defined(DIFFUSEMAP)
                // Diffuse
                max(dot(wsL, wsN), 0.0) 
                * diffuseColor
#          endif

           // Specular
           + pow(vec3(max(dot(wsL, wsR), 0.0)), specularExponentConstant) * specularConstant);
}

