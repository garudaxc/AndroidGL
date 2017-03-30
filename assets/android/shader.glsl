#ifdef VERTEX_SHADER

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vUV;
varying vec3 v_normal;
varying vec2 v_texcoord;
void main() {
	
	vec4 worldPos = mWorld * vPosition;
	vec4 viewPos = mView * worldPos;
	gl_Position = mProj * viewPos;
	
	v_normal = mat3(mWorld) * vNormal; 
	v_texcoord = vUV;
}

#endif


#ifdef FRAGMENT_SHADER

precision mediump float;

vec3 vLightDir = vec3(1, -1, 1);
vec3 vLightColor = vec3(1, 1, 1);

uniform sampler2D texDiffuse;

varying vec3 v_normal;
varying vec2 v_texcoord;
void main() {

	vec3 l = normalize(vLightDir);
	float dnl = max(dot(l, v_normal), 0.5);

	vec3 tex0 = texture2D(texDiffuse, v_texcoord).rgb;

	gl_FragColor = vec4(tex0 * vLightColor * dnl, 1.0);
	//gl_FragColor = vec4(tex0, 1.0);
}

#endif
