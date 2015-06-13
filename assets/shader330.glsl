#ifdef VERTEX_SHADER

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

out vec3 v_normal;
out vec2 v_texcoord;
void main() {

	vec4 worldPos = mWorld * vPosition;
	vec4 viewPos = mView * worldPos;
	gl_Position = mProj * viewPos;

	v_normal = mat3(mWorld) * vNormal;
	v_texcoord = vUV;
}


#endif


#ifdef FRAGMENT_SHADER

uniform vec3 vLightDir = vec3(1, -1, 1);
uniform vec3 vLightColor = vec3(1, 1, 1);

in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;

layout (location = 0) out vec4 color;

void main() {
	
	vec3 l = normalize(vLightDir);

	float diffuse = max(dot(l, v_normal), 0.0);
	vec3 tex0 = texture2D(texDiffuse, v_texcoord).rgb;
	vec3 tex1 = texture2D(texNormal, v_texcoord).rgb;

	vec3 tex = tex0 * tex1;
	
	//color = vec4(tex0, 1);
	color = vec4(tex * vLightColor * diffuse, 1);
}


#endif