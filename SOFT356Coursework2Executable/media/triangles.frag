#version 400 core

out vec4 fColor;
in vec2 TexCoord;
in vec3 objectPosition;
in vec3 dirctionOfNormal;
in vec3 directionImLooking;
in vec3 directionOfLight;
in vec3 lightPosition;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform float brightness;


//defuse means colour, its directly affected by light
//ambeunce is the much weaker version of defuse
//specular is the reflection of light
//alpha means transparancy


void main()
{

    vec3 defuse = texture(ourTexture, TexCoord).rgb;
	float alpha = texture(ourTexture, TexCoord).a; 
	vec3 ambeunce = vec3(0.2,0.2,0.2)*defuse;
	vec3 specular = vec3(0.1,0.1,0.1);

	float distence =  length(lightPosition - objectPosition);

	vec3 normalNormals = normalize(dirctionOfNormal);
	vec3 normallightDirection = normalize(directionOfLight);
	vec3 normallookingDirection = normalize(directionImLooking);

	vec3 reflectionDirection = reflect(-normallightDirection,normalNormals);
	float cosOfNormalAndLight = clamp(dot(normalNormals,normallightDirection), 0.05f,1); //angles the light is hitting the vertex from
	float cosOfReflectionAndView = clamp(dot(normallookingDirection,reflectionDirection),0,1); //angle between reflected light and camra direction

	vec3 defuseColor = defuse*lightColor*brightness*(cosOfNormalAndLight/pow(distence,2)); //light gets weaker the thurther away it is
	vec3 specularColor = specular*lightColor*brightness*pow(cosOfReflectionAndView,5)/pow(distence,2); //increaded power of the reflection makes the shine sharper and less visable from extram angles

	fColor = vec4(
	defuseColor +
	ambeunce +
	specularColor,
	alpha);

}
