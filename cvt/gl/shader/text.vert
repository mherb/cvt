#version 150

uniform mat4 MVP;

in vec3 Vertex;
in vec4 Color;
out vec4 vtx_Color;
out vec2 vtx_Index;

void main()
{
	vec4 vtx = vec4( Vertex.xy, 0.0, 1.0 );
	int idx = int( Vertex.z );

	vtx_Index = vec2( float( idx & 0xf ), float( idx >> 4 ) );
	vtx_Color = Color;
	gl_Position = MVP * vtx;
}
