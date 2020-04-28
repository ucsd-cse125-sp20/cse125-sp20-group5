////////////////////////////////////////
// Particle.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out float sampleExtraOutput;

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * view * model * vec4(position, 1.0);
    sampleExtraOutput = 1.0f;
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
#version 330 core

in float sampleExtraOutput;

uniform vec3 color;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main()
{
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
    fragColor = vec4(color, sampleExtraOutput);
    
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

#endif

////////////////////////////////////////////////////////////////////////////////