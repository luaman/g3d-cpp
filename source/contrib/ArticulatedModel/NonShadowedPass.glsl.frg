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
#ifdef REFLECTCONSTANT
    uniform vec3        reflectConstant;
#endif

#ifdef REFLECTMAP
    uniform sampler2D   reflectMap;
#endif

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

#if defined(DIFFUSECONSTANT)
    uniform vec3        diffuseConstant;
#endif

#ifdef DIFFUSEMAP
    uniform sampler2D   diffuseMap;
#endif

#ifdef NORMALBUMPMAP
    /** Multiplier for bump map.  Typically on the range [0, 0.05]
      This increases with texture scale and bump height. */
    uniform float       bumpMapScale;

    /** xyz = normal, w = bump height */
    uniform sampler2D   normalBumpMap;

    /** Un-normalized (interpolated) tangent space eye vector */
    varying vec3        _tsE;
#endif

uniform vec3        ambientTop;
uniform vec3        ambientBottom;

// World parameters
varying vec3        wsEyePos;
varying vec3        wsPosition;
varying vec2        texCoord;
varying vec4        tan_X, tan_Y, tan_Z, tan_W;


void main(void) {

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

        // World space normal
        vec3 wsN = tan_Z.xyz;

        vec2 offsetTexCoord = texCoord;
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

#   if defined(EMITCONSTANT) || defined(EMITMAP)     
        vec3 emitColor =
#       ifdef EMITCONSTANT
            emitConstant
#           ifdef EMITMAP
                * texture2D(emitMap, offsetTexCoord).rgb
#          endif
#       else
            texture2D(emitMap, offsetTexCoord).rgb
#       endif
        ;
#   endif

#   if defined(REFLECTCONSTANT) || defined(REFLECTMAP)     
        vec3 reflectColor =
#       ifdef REFLECTCONSTANT
            reflectConstant
#           ifdef REFLECTMAP
                 * texture2D(reflectMap, offsetTexCoord).rgb
#           endif
#       else
            texture2D(reflectMap, offsetTexCoord).rgb
#       endif
        ;
#   endif

    // tan_Z is world space normal
    vec3 ambient = ambientTop + (ambientTop - ambientBottom) * min(wsN.y, 0.0);

    gl_FragColor.rgb =
#       if defined(EMITCONSTANT) || defined(EMITMAP)
            // Emissive
            emitColor
#       endif

#       if defined(DIFFUSECONSTANT) || defined(DIFFUSEMAP)
            diffuseColor * 
            (ambient + max(dot(wsL, wsN), 0.0) * lightColor)

#       endif

        // Specular
        + pow(vec3(max(dot(wsL, wsR), 0.0)), specularExponentConstant) * lightColor * specularConstant
        
#       if defined(REFLECTCONSTANT) || defined(REFLECTMAP)     
            // Reflection
            + textureCube(environmentMap, wsR).rgb * environmentConstant 
            * reflectColor
#       endif

        ;

}
