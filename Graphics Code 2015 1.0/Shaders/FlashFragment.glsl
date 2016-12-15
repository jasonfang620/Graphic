#version 150 core

uniform sampler2D diffuseTex;
uniform float flashSpeed;

in Vertex{
	vec2 texCoord;
	vec4 colour;
}IN;

out vec4 gl_FragColor;

void main(void){
  float blendSpeed = clamp(flashSpeed, 0.0 ,0.5);

	gl_FragColor=texture(diffuseTex,IN.texCoord);
  gl_FragColor.a = blendSpeed;
//  gl_FragColor.r = blendSpeed;
}
