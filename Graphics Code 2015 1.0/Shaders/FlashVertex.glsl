#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec2 texCoord;
in vec3 position;
in vec4 colour;

out Vertex {
	vec2 texCoord;
	vec4 colour;
}OUT;

void main(void) {
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colour = colour;
	gl_Position = vec4(position, 1.0);
}
