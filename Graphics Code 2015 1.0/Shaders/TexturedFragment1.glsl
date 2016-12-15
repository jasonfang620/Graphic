#version 150 core

uniform sampler2D diffuseTex;

in Vertex{
	vec2 texCoord;
	vec3 position;

}IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor=texture(diffuseTex,IN.texCoord);
	//gl_FragColor.xy = IN.texCoord;
}
