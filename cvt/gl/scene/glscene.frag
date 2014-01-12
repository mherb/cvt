#version 150

#define MAX_LIGHTS 8

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    sampler2D diffusemap;
    sampler2D specularmap;
    sampler2D normalmap;
};

uniform Material material;

struct Light {
    vec4 position;
    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float attenuation[ 3 ];
};

uniform int   numlights;
uniform Light light[ MAX_LIGHTS ];

in vec4 vtx_Color;
in vec3 vtx_Normal;
in vec4 vtx_Position;

out vec4 output;

vec4 PhongLightModel( int l, vec4 position, vec3 norm )
{
    vec3 s = normalize( vec3( light[ l ].position - position ) );
    vec3 v = normalize( -position.xyz );
    vec3 r = normalize( -reflect( s, norm ) );

    vec4 ambient = light[ l ].ambient * material.ambient;

    float sDotN = max( dot( s, norm ) , 0.0 );
    vec4 diffuse = clamp( light[ l ].diffuse * material.diffuse * sDotN, 0.0, 1.0 );

    vec4 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = clamp( light[ l ].specular * material.specular * pow( max( dot( r , v ), 0.0 ), material.shininess ), 0.0, 1.0 );

    return ambient + diffuse + spec;
}


void main( void )
{
    output = ( vec4 ) ( 0.0 );

    for( int i = 0; i < numlights; i++ ) {
       output += PhongLightModel( i, vtx_Position, vtx_Normal );
    }

    output /= ( float ) numlights;
} 
