#version 150 core

uniform samplerCube cubeTex;
uniform samplerCube cubeTex1;
uniform vec3        cameraPos;


uniform float x;


//uniform vec3 fogColour;  //new

//float lowerLimt=0.0;    //new
//float upperLimt=30.0;  //new


in Vertex{
	vec3 normal;
}IN;

out vec4 FragColor;

void main(void){

	vec4 sky = texture(cubeTex,normalize(IN.normal));
	vec4 sky1 = texture(cubeTex1,normalize(IN.normal));

	float blend = clamp(x, 0.0 ,1.0);
	vec4 colour = mix(sky, sky1, blend);

	FragColor = colour;

	//FragColor=()

}
