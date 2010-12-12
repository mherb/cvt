#version 150

uniform mat4 MVP;
uniform mat3 NORMM;
uniform vec3 LIGHTPOS;

in vec3 in_Vertex;
in vec3 in_Normal;
in vec4 in_Color;
out vec4 vtx_Color;

out float vtx_LightIntensity;

const float specularContribution = 0.1;
const float diffuseContribution  = 1.0 - specularContribution;

void main()
{
	vtx_Color = in_Color;

    vec4 ecPosition = MVP * vec4( in_Vertex, 1.0 );
    vec3 tnorm      = normalize( NORMM * gl_Normal );
    vec3 lightVec   = normalize( LIGHTPOS - vec3( ecPosition ) );
    vec3 reflectVec = reflect( -lightVec, tnorm );
    vec3 viewVec    = normalize( -vec3( ecPosition ) );

    float spec      = clamp( dot( reflectVec, viewVec ), 0.0, 1.0);
    spec            = pow(spec, 16.0);

    vtx_LightIntensity  = diffuseContribution * max( dot( lightVec, tnorm ), 0.0 ) + specularContribution * spec;
	gl_Position = ecPosition;
}
