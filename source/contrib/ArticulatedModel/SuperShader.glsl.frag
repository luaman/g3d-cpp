/**
  @file SuperShader.glsl.frag
  @author Morgan McGuire matrix@graphics3d.com

  For use with G3D::SuperShader.

 */

// Lighting environment
uniform vec3        ambientTop;
uniform vec3        ambientDown;

/** World space, normalized (TODO: non-directional light, multiple lights) */
uniform vec4        lightPosition;
uniform vec3        lightColor;

uniform vec3        environmentConstant;
uniform samplerCube environmentMap;

// Material
uniform sampler2D   reflectMap
uniform vec3        reflectConstant;

uniform sampler2D   specularMap
uniform vec3        specularConstant;

uniform sampler2D   specularExponentMap
uniform vec3        specularExponentConstant;

uniform sampler2D   emitMap
uniform vec3        emitConstant;

uniform sampler2D   diffuseMap
uniform vec3        diffuseConstant;

/** Multiplier for bump map.  Typically on the range [0, 0.05]
  This increases with texture scale and bump height. */
uniform float       bumpScale;

/** xyz = normal, w = bump height */
uniform sampler2D   normalBumpMap;

// World parameters
uniform vec3        wsEyePos;
varying vec3        wsPosition;
varying vec3        _tsE;
varying vec2        texCoord;
varying vec4        tan_X, tan_Y, tan_Z, tan_W;

void main(void) {

    // Convert bumps to a world space distance
    float  bump   = (texture2D(normalBumpMap, texCoord).w - 0.5) * bumpScale;

	vec3 tsE = normalize(_tsE);

    // Offset the texture coord.  Note that texture coordinates are inverted (in the y direction)
	// from TBN space, so we must flip the y-axis.

    vec2 offsetCoord = texCoord.xy + vec2(tsE.x, -tsE.y) * bump;

    vec4 surfColor = texture2D(texture, offsetCoord);

	// note that the columns might be slightly not orthogonal due to interpolation
	mat4 tangentToWorld = mat4(tan_X, tan_Y, tan_Z, tan_W);
	
    vec3 wsE = normalize(wsEyePos - wsPosition);
	// or... (tangentToWorld * vec4(tsE, 0.0)).xyz;

	vec3 wsL = normalize(lightPos.xyz - wsPosition.xyz * lightPos.w);

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
			specular * specularity +
			reflected * reflectivity;

	gl_FragColor.a   = surfColor.a;
}

