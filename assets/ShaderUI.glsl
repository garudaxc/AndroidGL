#ifdef VERTEX_SHADER

in vec4 vPosition;
in vec4 vColor;
in vec2 vUV;

out vec4 v_color;
out vec2 v_texcoord;
void main() {
	gl_Position = vPosition;
	v_color = vColor;
	v_texcoord = vUV;
}

#endif


#ifdef FRAGMENT_SHADER

in vec4 v_color;
in vec2 v_texcoord;

uniform sampler2D texDiffuse;

layout (location = 0) out vec4 color;

void main() {
	vec4 tex0 = texture2D(texDiffuse, v_texcoord).rgba;
	color = tex0 * v_color;
}


#endif