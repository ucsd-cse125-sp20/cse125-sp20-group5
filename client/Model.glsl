////////////////////////////////////////
// Model.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER
//#version 330 core


layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=1) in vec3 Color;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 eyedir;
out vec3 LightDirection;
out vec3 baseColor;

uniform mat4 ModelMtx=mat4(1.0);
uniform mat4 ModelViewProjMtx=mat4(1.0);
uniform vec3 eyepos=vec3(0);

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main() {
	gl_Position=ModelViewProjMtx * vec4(Position,1);

	fragPosition=vec3(ModelMtx * vec4(Position,1));

	mat3 inner_mat = mat3(ModelViewProjMtx[0][0], ModelViewProjMtx[0][1], ModelViewProjMtx[0][2], 
						ModelViewProjMtx[1][0], ModelViewProjMtx[1][1], ModelViewProjMtx[1][2], 
						ModelViewProjMtx[2][0], ModelViewProjMtx[2][1], ModelViewProjMtx[2][2]);
		// find the normal
	fragNormal = Normal;//normalize(transpose(inverse(inner_mat))*Normal) ; 

	vec3 mypos = vec3(gl_Position) / gl_Position.w; // Dehomogenize current location 
    vec3 eyedir = normalize(eyepos - mypos);
	LightDirection=normalize(vec3(vec4(1,5,2,0)));
	baseColor = Color;
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
//#version 330 core
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 eyedir;
in vec3 LightDirection;
in vec3 baseColor;

uniform vec3 AmbientColor=vec3(0.1);

uniform vec3 LightColor=vec3(1.0,1.0,1.0);
uniform vec3 DiffuseColor=vec3(0.5);

out vec3 finalColor;

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

	vec3 temp = baseColor;
	vec3 color = DirLight(-LightDirection, LightColor, fragNormal, normalize(LightDirection + eyedir),temp,temp, .5, eyedir);
	color += DirLight(LightDirection, vec3(1.0,1.0,1.0), fragNormal, normalize(LightDirection + eyedir),temp,temp, .5, eyedir);

	gl_FragColor = vec4(color,1.0)+vec4(AmbientColor,1.0);
}

#endif

////////////////////////////////////////////////////////////////////////////////