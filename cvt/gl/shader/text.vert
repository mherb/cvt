#version 150 core

uniform mat4 MVP;

in vec3 in_Position;
in vec4 in_Color;
out vec4 vtx_Color;
out vec2 vtx_Index;

void main()
{
	vec4 vtx = vec4( in_Position.xy, 0.0, 1.0 );
	int idx = int( in_Position.z );

	vtx_Index = vec2( float( idx & 0xf ), float( idx >> 4 ) );
	vtx_Color = in_Color;
	gl_Position = MVP * vtx;
}
