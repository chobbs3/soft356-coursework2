#version 400 core

//uses vertect and light position and view and modle matricies to work out the direction of the light, normals and camra
layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec2 vtexcoord;
layout (location = 2) in vec3 vnormal;
uniform mat4 mvp;
uniform vec3 lightPositionIn;
uniform mat4 moduleMatrix;
uniform mat4 viewMatrix;

out vec2 TexCoord; 
out vec3 objectPosition;
out vec3 dirctionOfNormal;
out vec3 directionImLooking;
out vec3 directionOfLight;
out vec3 lightPosition;

void main()
{
    gl_Position = mvp * vec4(vPosition,1);
	TexCoord = vtexcoord;
	objectPosition = (moduleMatrix * vec4(vPosition,1)).xyz;
	vec3 TempVertexInView = (viewMatrix * moduleMatrix * vec4(vPosition,1)).xyz;
	directionImLooking = vec3(0,0,0) - TempVertexInView; //vec3 0,0,0, is the origin of the camra so the shader is seeing things from the camras prespective (as the view matrix is used)
	vec3 directionOfLightToView = (viewMatrix * vec4(lightPosition, 1)).xyz;
	directionOfLight = lightPosition - vPosition;
	dirctionOfNormal = vnormal;
	lightPosition = lightPositionIn;
}
