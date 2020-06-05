////////////////////////////////////////
// TextUI.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform int isBackground;
uniform mat4 projection;
uniform int renderInWorld;
uniform mat4 model;
uniform mat4 projectView;

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
    float z_value = 0.0;
    if (isBackground == 1) { z_value = -0.005; }

    if (renderInWorld == 0) {
        gl_Position = projection * vec4(vertex.xy, z_value, 1.0);
    }
    else {
        gl_Position = projectView * model * vec4(vertex.xy, z_value, 1.0);
    }

    TexCoords = vertex.zw;
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER

in vec2 TexCoords;
out vec4 color;

uniform int isBackground;
uniform sampler2D text;
uniform sampler2D background;
uniform vec3 textColor;
uniform float alpha;


////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    if (isBackground == 0) {
        color = vec4(textColor, alpha) * sampled;
    } else {
        color = vec4(1, 1, 1, alpha) * texture(background, TexCoords);
    }
}

////////////////////////////////////////////////////////////////////////////////

#endif