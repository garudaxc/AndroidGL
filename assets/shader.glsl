#ifdef VERTEX_SHADER

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vUV;
varying vec3 v_diffuse;
varying vec2 v_texcoord;
void main() {

	vec4 worldPos = mWorld * vPosition;
	vec4 viewPos = mView * worldPos;
	gl_Position = mProj * viewPos;

	v_diffuse = mat3(mWorld) * vNormal; 
	v_texcoord = vUV;
}

#endif


#ifdef FRAGMENT_SHADER

precision mediump float;
varying vec3 v_diffuse;
varying vec2 v_texcoord;
void main() {
  gl_FragColor = vec4(v_diffuse, 1.0);
}

#endif