////////////////////////////////////////
// TextUI.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform int renderInWorld;
uniform mat4 model;
uniform mat4 projectView;

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
    if (renderInWorld == 0) {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    }
    else {
        gl_Position = projectView * model * vec4(vertex.xy, 0.0, 1.0);
    }

    TexCoords = vertex.zw;
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float alpha;


////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, alpha) * sampled;
}

////////////////////////////////////////////////////////////////////////////////

#endif