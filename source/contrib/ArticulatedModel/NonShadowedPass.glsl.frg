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


#if 0

    // Convert bumps to a world space distance
    float  bump   = (texture2D(normalBumpMap, texCoord).w - 0.5) * bumpMapScale;

	vec3 tsE = normalize(_tsE);

    // Offset the texture coord.  Note that texture coordinates are inverted (in the y direction)
	// from TBN space, so we must flip the y-axis.

    // We should technically divide by the z-coordinate

    vec2 offsetCoord = texCoord.xy + vec2(tsE.x, -tsE.y) * bump;

    vec4 surfColor = texture2D(diffuseMap, offsetCoord);

	// note that the columns might be slightly not orthogonal due to interpolation
	mat4 tangentToWorld = mat4(tan_X, tan_Y, tan_Z, tan_W);
	
    vec3 wsE = normalize(wsEyePos - wsPosition);
	// or... (tangentToWorld * vec4(tsE, 0.0)).xyz;

	vec3 wsL = normalize(lightPosition.xyz - wsPosition.xyz * lightPosition.w);

    // Take the normal map values back to (-1, 1) range to compute a tangent space normal
    vec3 tsN = ((texture2D(normalBumpMap, offsetCoord).xyz - vec3(0.5, 0.5, 0.5)) * 2.0);

	// Take the normal to world space 
	vec3 wsN = (tangentToWorld * vec4(tsN, 0.0)).xyz;

	// Reflection vector
	vec3 wsR = (wsN * 2.0 * dot(wsN, wsE)) - wsE;
	vec3 reflected  = textureCube(environmentMap, wsR).rgb;

    vec3 lightColor = vec3(0.9, 0.9, 0.75);
    vec3 ambient    = vec3(0.1, 0.1, 0.25);

    vec3 diffuse  = lightColor * max(dot(wsN, wsL), 0.0);
	vec3 specular = lightColor * pow(max(dot(wsL, wsR), 0.0), 4.0);

    gl_FragColor.rgb =
	        (diffuse + ambient) * surfColor.rgb +
			specular * specularConstant +
			reflected * reflectConstant;

	gl_FragColor.a   = surfColor.a;
#endif

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

