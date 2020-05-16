////////////////////////////////////////
// AssimpModel.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;                                             
layout (location = 3) in ivec4 BoneID;
layout (location = 4) in vec4 Weight;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexture;

out vec3 eyedir;

uniform mat4 model;
uniform mat4 projectView;
uniform vec3 eyepos=vec3(0,0,10);

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main()
{
	mat4 boneTransform = gBones[BoneID[0]] * Weight[0];
    boneTransform     += gBones[BoneID[1]] * Weight[1];
    boneTransform     += gBones[BoneID[2]] * Weight[2];
    boneTransform     += gBones[BoneID[3]] * Weight[3];

    vec4 animatedPosition = boneTransform * vec4(Position, 1.0);
	vec4 animatedNormal = boneTransform * vec4(Normal, 0.0);

	gl_Position = projectView * model * animatedPosition;
	fragPosition = vec3(model* animatedPosition);
	fragNormal = vec3(model * animatedNormal);
	fragTexture = Texture;


	vec3 mypos = vec3(gl_Position) / gl_Position.w; // Dehomogenize current location 
    eyedir = normalize(eyepos - mypos);
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
//#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexture;
in vec3 eyedir;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_ambient1;

uniform vec3 diffuseColor=vec3(0.0);
uniform vec3 ambientColor=vec3(0.0);
uniform vec3 specularColor=vec3(0.0);
uniform float shininess=0.0;

uniform vec3 lightColor=vec3(1.0,1.0,1.0);
uniform vec3 lightDirection=vec3(0,0,1);

uniform int hasTexture;

out vec4 fragColor;


////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

vec3 ComputeLight (const in vec3 direction, const in vec3 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec3 mydiffuse, const in vec3 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction);       
        vec3 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec);
        vec3 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess);

        vec3 retval = lambert + phong;
        return retval;
} 

vec3 DirLight (const in vec3 direction, const in vec3 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec3 mydiffuse, const in vec3 myspecular, const in float myshininess, vec3 eyedir) {

        vec3 lightdir = normalize(-direction);
		float diff = max(dot(normal, lightdir), 0.0);

		vec3 reflectdir = reflect(-lightdir, normal);
		float spec = pow(max(dot(eyedir, reflectdir), 0.0), myshininess);
		vec3 diffuse = diff*lightcolor*mydiffuse;
		vec3 specular = lightdir * spec *myspecular;
		return diffuse+specular;
} 



void main() {
	// Compute irradiance (sum of ambient & direct lighting)

	//vec3 color = DirLight(-lightDirection, lightColor, fragNormal, normalize(lightDirection + eyedir), diffuseColor, specularColor, 0.5, eyedir);
	//color += DirLight(lightDirection, vec3(1.0,1.0,1.0), fragNormal, normalize(lightDirection + eyedir), diffuseColor, specularColor, 0.5, eyedir);

	vec3 norm = normalize(fragNormal);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = diff * diffuseColor * lightColor; //only calculates diffuse
	
	int levels = 3;
	diffuse = floor(diffuse * levels)/levels + vec3(0.05,0.05,0.05);

	vec3 reflectdir = reflect(-lightDirection, norm);
	float spec = pow(max(dot(eyedir, reflectdir), 0.0), shininess);
	vec3 specular = spec * specularColor * lightColor;

	vec3 ambient = diffuseColor; // diffuseColor is added as ambient

	vec4 finalColor = vec4(diffuse + specular + ambient,1.0);
	if (hasTexture == 1)
		finalColor = finalColor * texture(texture_diffuse1, fragTexture);
	gl_FragColor = finalColor;

	// Add outline 
	float edge = max(0.0, dot(norm, normalize(eyedir)));
    if (edge < 0.2) {
        gl_FragColor = vec4(0.0,0.0,0.0,0.0f);
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////