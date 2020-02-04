#version 430 core

layout (local_size_x = 128) in;
// Uniform block containing positions and masses of the attractors
layout (std140, binding = 0) uniform attractor_block
{
vec4 attractor[32]; // xyz = position, w = mass
};


// Buffers containing the positions and velocities of the particles
layout (rgba32f, binding = 0) uniform imageBuffer position_buffer;
layout (rgba32f, binding = 1) uniform imageBuffer velocity_buffer;

uniform float dt;


/*
void main(void)
{
	float i = float(gl_GlobalInvocationID.x);
	i = attractor[0].w;
	i = dt;
	vec4 value = vec4(i, i, i, i);
	imageStore(position_buffer, int(gl_GlobalInvocationID.x), value);
// Do nothing.
}
*/

void main(void)
{
	// Read the current position and velocity from the buffers
	vec4 vel = imageLoad(velocity_buffer, int(gl_GlobalInvocationID.x));
	vec4 pos = imageLoad(position_buffer, int(gl_GlobalInvocationID.x));
	int i;
	// Update position using current velocity * time
	pos.xyz += vel.xyz * dt;
	// Update "life" of particle in w component
	pos.w -= 0.01 * dt;
	// For each attractor...
	for (i = 0; i < 32; i++)
	{
		// Calculate force and update velocity accordingly
		vec3 dist = (attractor[i].xyz - pos.xyz);
		vel.xyz += dt * dt * attractor[i].w * normalize(dist) / (dot(dist, dist) + 10.0);
	}
	// If the particle expires, reset it
	
	if (pos.w <= 0.0)
	{
		pos.xyz = -pos.xyz * 0.01;
		vel.xyz *= 0.01;
		pos.w += 1.0f;
	}

	//pos = attractor[0];
	// Store the new position and velocity back into the buffers
	imageStore(position_buffer, int(gl_GlobalInvocationID.x), pos);
	imageStore(velocity_buffer, int(gl_GlobalInvocationID.x), vel);
}