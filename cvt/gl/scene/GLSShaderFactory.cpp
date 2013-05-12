#include <cvt/gl/scene/GLSShaderFactory.h>

static const char* _glsshader_material =
"struct Material {\n" \
"\tuniform vec3 diffuse;\n"\
"\tuniform vec3 specular;\n" \
"\tuniform float shininess;\n" \
"\tuniform sampler2D diffusemap;\n" \
"\tuniform sampler2D specularmap;\n" \
"\tuniform sampler2D normalmap;\n" \
"\t"
"}\n";

static const char* _glsshader_light =
"struct Light {\n" \
"\tuniform vec4 position;\n" \
"\tuniform vec3 color;\n" \
"\tuniform float attenuation[ 3 ];\n" \
"}\n";

static const char* _glsshader_ambientlight =
"struct AmbientLight {\n" \
"\tuniform vec3 color;\n" \
"}\n";
