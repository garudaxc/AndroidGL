#ifdef VERTEX_SHADER

uniform mat4 mViewProj;

attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec2 vUV;

varying vec4 v_color;
varying vec2 v_texcoord;
void main() {

	gl_Position = mViewProj * vPosition;
	v_color = vColor;
	v_texcoord = vUV;
}

#endif


#ifdef FRAGMENT_SHADER

precision mediump float;

varying vec4 v_color;
varying vec2 v_texcoord;

uniform sampler2D texDiffuse;

void main() {
	vec4 tex0 = texture2D(texDiffuse, v_texcoord).rgba;
	gl_FragColor = tex0 * v_color;
}

#endif
