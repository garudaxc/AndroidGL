#ifdef VERTEX_SHADER

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec4 vColor;
layout (location = 3) in vec2 vUV;
out vec3 v_diffuse;
out vec2 v_texcoord;
void main() {

	vec4 worldPos = mWorld * vPosition;
	vec4 viewPos = mView * worldPos;
	gl_Position = mProj * viewPos;

	v_diffuse = vColor.rgb; 
	v_texcoord = vUV;
}




#endif


#ifdef FRAGMENT_SHADER

in vec3 v_diffuse;
in vec2 v_texcoord;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;

layout (location = 0) out vec4 color;

void main() {
	color = texture(texDiffuse, v_texcoord) * texture(texNormal, v_texcoord);
}


#endif