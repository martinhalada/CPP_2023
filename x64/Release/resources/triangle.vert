#version 330 core
layout (location = 0) in vec3 aPos;		// Positions/Coordinates
layout (location = 1) in vec3 aColor;	// Color
layout (location = 2) in vec2 aTex;		// Texture Coordinates
layout (location = 3) in vec3 aNormal;	// Normals (not necessarily normalized)

uniform mat4 uProj_m = mat4(1.0f);	// Imports Projection matrix from the main function
uniform mat4 uV_m = mat4(1.0f);		// Imports View matrix from the main function
uniform mat4 uM_m = mat4(1.0f);		// Imports Model matrix from the main function

out vec3 color;		// Outputs the color for the Fragment Shader
out vec2 texCoord;	// Outputs the texture coordinates to the Fragment Shader
out vec3 Normal;	// Outputs the normal for the Fragment Shader
out vec3 crntPos;	// Outputs the current position for the Fragment Shader

void main() {
	// Calculates current position
	crntPos = vec3(uM_m * vec4(aPos, 1.0f));
	// Outputs coordinates of all vertices
	gl_Position = uProj_m * uV_m * uM_m * vec4(aPos,1.0f);
	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = mat3(uM_m) * aNormal;
}