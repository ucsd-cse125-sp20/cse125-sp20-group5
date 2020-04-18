////////////////////////////////////////
// AssimpModel.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normals;

uniform mat4 model;
uniform mat4 projectView;

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
    TexCoords = aTexCoords;   
    Normals = aNormal;
    gl_Position = projectView * model * vec4(aPos, 1.0);
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
//#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform vec3 color;

////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords) + vec4(color, 0.0f);
}

#endif

////////////////////////////////////////////////////////////////////////////////