


#ifdef VERTEX_SHADER

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;
uniform mat4 mViewProj;

in vec4 vPosition;

void main() {

	vec4 pos = vec4(vPosition.xyz * 0.01, 1.0);
	vec4 worldPos = mWorld * vPosition;
	gl_Position = mViewProj * worldPos;
	gl_Position = pos;
}


#endif






#ifdef FRAGMENT_SHADER




layout (location = 0) out vec4 color;
// This is derived from the age of the particle read
// by the vertex shader
// in float intensity;

void main(void)
{
	// Blend between red-hot and cool-blue based on the
	// age of the particle.
	color = mix(vec4(0.0f, 0.2f, 1.0f, 1.0f),
				vec4(0.2f, 0.05f, 0.0f, 1.0f),
				0.2f);
}



#endif