
varying vec3 N;

uniform sampler2DRect depth;

uniform vec3 bright;
uniform vec3 dim;

void main(void) {
	vec3 L = gl_LightSource[0].position.xyz;

	float z1 = texture2DRect(depth, gl_FragCoord.xy + vec2(0,1)).g;
	float z2 = texture2DRect(depth, gl_FragCoord.xy + vec2(1,0)).g;
	float z3 = texture2DRect(depth, gl_FragCoord.xy + vec2(0,-1)).g;
	float z4 = texture2DRect(depth, gl_FragCoord.xy + vec2(-1,0)).g;

	N = normalize(N);

	float d = max(dot(N, L), 0);
	
	float s = max(-reflect(L, N).z, 0);
	s = (s > 0.95) ? 1.0 : 0.0;

	vec2 dz = vec2(z2 - z4, z1 - z3);

	if (length(dz) > .006) {
		// Depth discontinuity
		gl_FragColor.rgb = vec3(0,0,0);
	} else {
		gl_FragColor.rgb = 
			(d > 0.5) ? bright : dim
			+ Kd * s * 2;
	}
}
