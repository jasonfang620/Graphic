#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Light.h"
#include "TextMesh.h"
#include"ParticleEmitter.h"
#include"ParticleEmitterSmoke.h"
#include"ParticleEmitterExpolsion.h"
#include<string>
#include<sstream>

//#define LIGHTNUM 8 // We ’ll generate LIGHTNUM squared lights ...
#define SHADOWSIZE 2048 //set shadowsize 

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

class Renderer : public OGLRenderer {
public:

	Renderer(Window & parent);
	virtual ~Renderer(void);


	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	/*
	Draws the passed in line of text, at the passed in position. Can have an optional font size, and
	whether to draw it in orthographic or perspective mode.
	*/
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
	void	Textwrite(float memoryuseage);
	void	CalFPS(float a);
protected:
	void DrawShadowScene();
	//void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void DrawHeightmap();;
	void DrawWater();
	void DrawSkybox();
	//void GenerateScreenTexture(GLuint & into, bool depth = false);	// Make a new texture ...
	void DrawParticles();// draw particle
	void SetShaderParticleSize(float a);//for particle

	void addlight(Vector3 position, Vector4 colour, float radius);

	void PresentScene();
	void DrawPostProcess();


	Shader * sceneShader; // Shader to fill our GBuffers
	Shader * pointlightShader; // Shader to calculate lighting
	Shader * combineShader; // shader to stick it all together
	Shader * lightShader;
	Shader * reflectShader;
	Shader * skyboxShader;		//skybox shader
	Shader * shadowShader;		//shadow shader
	Shader* textShader;			//text shader
	Shader* particleShader;		//particle Shader
	Shader * processShader;
	Shader* FlashShader;


	Light * light;
	Light * pointLights; // Array of lighting data
	Mesh * heightMap; // Terrain !
	OBJMesh * sphere; // Light volume
	Mesh * quad; // To draw a full - screen quad
	Camera * camera; // Our usual camera

	float rotation; // How much to increase rotation by

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here


	
	GLuint cubeMap;
	GLuint cubeMap1;
	//GLuint cubeMap2;

	GLuint shadowTex;
	GLuint shadowFBO;

	//GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting	
	float waterRotate;

	Font*	basicFont;	 //A font! a basic one...
	
	ParticleEmitter* emitter; //for particle
	ParticleEmittersmoke* fog; //for fog particle
	ParticleEmitterExpolsion* expolsion;	//for expolsion particle
	
	
	float FPS; //for FPS
	
	bool isDayTime;// skybox change time

	float x = 0.0; //sky fade

	float bombtimer = 0.f;

	float ImageFlash;
	bool  isFlash;

	GLint previous_memory; // previous memory 
	GLint cur_avail_mem_kb = 0;


	

	static const int MAX_LIGHT_COUNT = 10;	//multlight max 10

	vector<Light*> lightVector;
	Vector4 lightColour[MAX_LIGHT_COUNT];
	Vector3 lightPos[MAX_LIGHT_COUNT];
	GLfloat lightRadius[MAX_LIGHT_COUNT]; 


	Vector3 mov;


	float explosionDendity;
	bool isExplosionTriggered;

	void DrawFlash(char* imagePath);
	

};