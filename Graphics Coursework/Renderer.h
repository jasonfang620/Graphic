#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"
#include "textmesh.h"

#define LIGHTNUM 8 // We ’ll generate LIGHTNUM squared lights ...

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	virtual void Pointlight();

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
protected:
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
							// Make a new texture ...
	void DrawSkybox();	//skybox draw
	void DrawWater();	//water draw

	void GenerateScreenTexture(GLuint & into, bool depth = false);
	Shader * sceneShader; // Shader to fill our GBuffers
	Shader * pointlightShader; // Shader to calculate lighting
	Shader * combineShader; // shader to stick it all together
	Shader* skyboxShader; //shader to skybox
	Shader* reflectShader; // reflecShader
	Shader* lightShader; 
	Shader*textShader;

	static const int MAX_LIGHT_COUNT = 10;
	vector<Light*> lightVector;
	Vector4 lightColour[MAX_LIGHT_COUNT];
	Vector3 lightPos[MAX_LIGHT_COUNT];
	GLfloat lightRadius[MAX_LIGHT_COUNT]; //point light




	Light*light;
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

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	GLuint cubeMap; //Store skybox texture

	Font*	basicFont;	//A font! a basic one...

	float waterRotate;

};
