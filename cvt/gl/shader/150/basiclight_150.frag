#version 150
in float vtx_LightIntensity;
in vec4 vtx_Color;
out vec4 Output;

void main()
{
//    Output = vec4( vtx_Color.xyz * vtx_LightIntensity, vtx_Color.w );
	Output = vtx_Color;
}
