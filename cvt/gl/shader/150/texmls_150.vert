#version 150

uniform mat4 MVP;
uniform sampler2D DPTex;

in vec3 in_Vertex;
in vec4 in_Color;
out vec4 vtx_Color;
out vec2 vtx_TexCoord;

void main()
{
	vtx_Color = in_Color;
	gl_Position = MVP * vec4( in_Vertex, 1.0 );
	vtx_TexCoord = in_Vertex.xy; 
}
