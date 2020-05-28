////////////////////////////////////////
// HealthBar.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core

layout (location = 0) in vec3 Position;
//layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;

//out vec3 fragPosition;
//out vec3 fragNormal;
out vec2 fragTexture;

uniform mat4 model;
uniform mat4 projectView;


////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
	gl_Position = projectView * model * vec4(Position, 1.0);

	//fragPosition=vec3(model * vec4(Position,1));
	fragTexture = Texture;
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
//#version 330 core

//in vec3 fragPosition;
//in vec3 fragNormal;
in vec2 fragTexture;

uniform sampler2D myTexture;
uniform vec3 color;
uniform float alpha;

out vec4 fragColor;


////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main() {
	fragColor = vec4(color, alpha) * texture(myTexture, fragTexture);
}

#endif

////////////////////////////////////////////////////////////////////////////////