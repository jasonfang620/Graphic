
#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,&previous_memory);  //get previous memory function
	quad = Mesh::GenerateQuad();
	rotation = 0.0f;
	isDayTime = true;
	isFlash = true;
	ImageFlash = 0.0;
	camera = new Camera(0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 700, RAW_WIDTH * HEIGHTMAP_X));
	emitter = new ParticleEmitter();
	fog = new ParticleEmittersmoke();
	expolsion = new ParticleEmitterExpolsion();
	isExplosionTriggered = FALSE;
	explosionDendity = 100.0f;

	light = new	Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X / 1.0f), 700.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 1.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1), 4800.f);
	
	

	pointlightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment1.glsl");

	if (!pointlightShader->LinkProgram()) {
		return;
	}

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"ReflectFragment.glsl");

	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");

	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");

	FlashShader = new Shader(SHADERDIR"FlashVertex.glsl", SHADERDIR"FlashFragment.glsl");

	if (!FlashShader->LinkProgram())
	{
		return;
	}

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram()) {
		return;
	}
	sceneShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"bufferFragment.glsl");
	if (!sceneShader->LinkProgram()) {
		return;
	}
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textShader->LinkProgram()) {
		return;
	}
	combineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	if (!combineShader->LinkProgram()) {
		return;
	}
	particleShader = new Shader("vertex.glsl", "fragment.glsl", "geometry.glsl");		//particle shader reader
	if (!particleShader->LinkProgram()) {
		return;
	}
	shadowShader = new Shader(SHADERDIR"lightshaver.glsl", SHADERDIR"lightshafrag.glsl");//shadow
	if (!shadowShader->LinkProgram()) {
		return;
	}
	processShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"blurfrag.glsl");
	if (!processShader->LinkProgram()) {
		return;
	}




	addlight(
		Vector3(450.0f, 200.0f, 280.0f), Vector4(0.5f, 0.5f, 0.5f, 1),200.0f);

	for (unsigned i = 0; i < lightVector.size(); i++)
	{
		lightColour[i] = lightVector[i]->GetColour();
		lightPos[i] = lightVector[i]->GetPosition();
		lightRadius[i] = lightVector[i]->GetRadius();
	}


	
	heightMap = new HeightMap(TEXTUREDIR"Icelandkatla-2.raw");

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water1.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	quad->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"waterbup.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"BarrenReds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"BarrenRedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"SKrightImage.png", TEXTUREDIR"SKleftImage.png",
		TEXTUREDIR"SKupImage.png", TEXTUREDIR"SKdownImage.png",
		TEXTUREDIR"SKbackImage.png", TEXTUREDIR"SKfrontImage.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	cubeMap1 = SOIL_load_OGL_cubemap(TEXTUREDIR"SK1rightImage.png", TEXTUREDIR"SK1leftImage.png",
		TEXTUREDIR"SK1upImage.png", TEXTUREDIR"SK1downImage.png",
		TEXTUREDIR"SK1backImage.png", TEXTUREDIR"SK1frontImage.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	if (!cubeMap||!cubeMap1 || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}
	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(quad->GetBumpMap(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);







	

	
	
	
	glGenTextures(1, &shadowTex);	//shadowTex

	glBindTexture(GL_TEXTURE_2D, shadowTex);	//shadowTex
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);		//end

	glGenFramebuffers(1, &shadowFBO);		// bind shadow

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);	//attachment
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	



	init = true;
	waterRotate = 0.0f;
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	/*glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);*/
}

Renderer::~Renderer(void){
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete pointlightShader;
	delete FlashShader;
	delete light;

	delete emitter;
	delete fog;

	delete sceneShader;
	delete combineShader;
	delete particleShader;

	delete basicFont;
	
	glDeleteTextures(1, &shadowTex);//shadow
	glDeleteFramebuffers(1, &shadowFBO);//shadow


	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	rotation = msec * 0.001f;
	waterRotate += msec * 0.01f;
	emitter->Update(msec);
	fog->Update(msec);
	expolsion->Update(msec);
	if (mov.x < ((RAW_HEIGHT*HEIGHTMAP_X) / 0.5f)) {
		mov.x += msec*0.5f;
	}
	else {
		mov.x -= msec*0.5f;
	}
	light->SetPosition(mov+ Vector3((RAW_HEIGHT*HEIGHTMAP_X / 1.0f), 700.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 1.0f)));
	

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0))
	{
		isExplosionTriggered = true;
	}

	if (isExplosionTriggered)
	{
		explosionDendity += msec;
		bombtimer += msec*0.001f;
		isFlash = true;

		if (bombtimer>=3.0) {
			isExplosionTriggered = false;
			bombtimer = 0.0f;
		}
	}
	else
	{
		//explosionDendity -= msec;
		//isFlash = false;
		explosionDendity = 0;
	}

	if (isFlash)
	{
		ImageFlash += msec*0.0005f;
		if (ImageFlash > 0.5)
		{
			ImageFlash = 0.5;
			isFlash = false;
		}
	}
	else
	{
		ImageFlash -= msec*0.0001f;
		if (ImageFlash < 0)
		{
			ImageFlash = 0;
		}
	}
}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawPointLights();
	DrawHeightmap();
	
	DrawWater();
	

	DrawShadowScene();
	
	DrawParticles();
	
	
	

	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,&cur_avail_mem_kb);		// get current available memory
	Textwrite((previous_memory-cur_avail_mem_kb)/1024.0f);

	DrawFlash(TEXTUREDIR"BarrenRedsDOT3.jpg");

	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, shadowFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, 960, 540, 0, 0, 960, 540, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);*/

	///here
	//DrawParticles();


	//CombineBuffers();
	SwapBuffers();
}

void Renderer::DrawShadowScene() {  //shadow

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);
	modelMatrix.ToIdentity();
	//viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	shadowMatrix = biasMatrix *(projMatrix * viewMatrix);



	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMap->Draw();

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	glUseProgram(0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);

}


void Renderer::DrawSkybox() {
	glEnable(GL_BLEND);

	//glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_FALSE);

	SetCurrentShader(skyboxShader);

	//glUseProgram(currentShader->GetProgram());

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex1"), 1);



	if (isDayTime)
	{
		x += 0.001f;
		if (x >= 1.0f)
		{
			isDayTime = false;
		}
	}
	else
	{
		x -= 0.001f;
		if (x <= 0.0f)
		{
			isDayTime = true;
		}
	}//skybox up 

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "x"), x);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1);


	quad->Draw();

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();


	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDepthMask(GL_TRUE);


	//glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::DrawHeightmap() {

	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_BLEND);*/
	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE);*/
	/*glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);*/
	SetCurrentShader(sceneShader);
	
	/*glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);*/
	/*glClearColor(0.f, 0.f, 0.f, 0.f);
	glDisable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);*/
	
	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	Matrix4 tempMatrix = shadowMatrix*modelMatrix;


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1,false, *&modelMatrix.values);//shadow

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false ,*&tempMatrix.values);//shadow


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 6);
	glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, shadowTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetBumpMap());

	UpdateShaderMatrices();
	
	heightMap->Draw();
	//textureMatrix.ToIdentity();
	
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();

	glUseProgram(0);
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);


	//glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_DEPTH_TEST);
}

void Renderer::DrawWater() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	SetCurrentShader(reflectShader);
	SetShaderLight(*light);


	
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quad->GetTexture());

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, quad->GetBumpMap());*/


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 2.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	glUseProgram(0);


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void Renderer::CombineBuffers() {

	SetCurrentShader(combineShader);
	
	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);


	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 3);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 4);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();

	glUseProgram(0);

}

void Renderer::SetShaderParticleSize(float a) {
	glUniform1f(glGetUniformLocation(particleShader->GetProgram(), "particleSize"), a);
}

void Renderer::DrawParticles() {
	glEnable(GL_DEPTH_TEST);

	SetCurrentShader(particleShader);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);




	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(60.0f);
	emitter->SetParticleVariance(0.5f);
	emitter->SetLaunchParticles(160.0f);
	emitter->SetParticleLifetime(3000.0f);
	emitter->SetParticleSpeed(0.5f);

	

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	modelMatrix = Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f) + 1300.f, 400, (RAW_WIDTH*HEIGHTMAP_Z / 2.0f) + 1200.f));


	UpdateShaderMatrices();



	emitter->Draw();


	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();


	SetShaderParticleSize(fog->GetParticleSize());
	//fog->SetParticleRate(50.0f);
	fog->SetParticleSize(5.0f);
	fog->SetParticleVariance(0.9f);
	fog->SetLaunchParticles(8);
	fog->SetParticleLifetime(80000.0f);
	fog->SetParticleSpeed(0.08f);



	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	modelMatrix = Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f), 500, (RAW_WIDTH*HEIGHTMAP_Z / 2.0f)));


	UpdateShaderMatrices();



	fog->Draw();

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();

	SetShaderParticleSize(expolsion->GetParticleSize());
	//expolsion->SetParticleRate(50.0f);
	expolsion->SetParticleSize(25.f);
	expolsion->SetParticleVariance(1.f);
	expolsion->SetLaunchParticles(explosionDendity);
	expolsion->SetParticleLifetime(1000.0f);
	expolsion->SetParticleSpeed(2.0f);


	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	modelMatrix = Matrix4::Translation(Vector3((RAW_WIDTH*HEIGHTMAP_X / 2.0f) + 1200.f, 600, (RAW_WIDTH*HEIGHTMAP_Z / 2.0f) + 1200.f));


	UpdateShaderMatrices();

	expolsion->Draw();

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();


	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);



}

void Renderer::CalFPS(float a) {
	FPS = 1000.0f / a;
}

void Renderer::Textwrite(float memoryuseage) {


	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_FALSE);


	SetCurrentShader(textShader);

	glUseProgram(currentShader->GetProgram());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, basicFont->texture);

	stringstream ss;
	float fps = float(FPS);
	ss << fps;
	string s = ss.str();


	stringstream s2;
	s2 << memoryuseage;
	string memUse = s2.str();
	//Render function to encapsulate our font rendering!

	DrawText("FPS:" + s, Vector3(0, 0, 0), 20.0f);
	DrawText("Memory:" + memUse +"Mb", Vector3(0, 30, 0), 20.0f);
	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_DEPTH_TEST);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}


void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {


	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		textureMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		textureMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	SetCurrentShader(textShader);
	glUseProgram(currentShader->GetProgram());	//Enable the shader...
												//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!

}


void Renderer::addlight(Vector3 position, Vector4 colour, float radius) {
	

	if (lightVector.size() < Renderer::MAX_LIGHT_COUNT)
	{
		lightVector.push_back(new Light(position, colour, radius));
	}
		
}

void Renderer::DrawPointLights()
{

	//glEnable(GL_DEPTH_TEST);
	SetCurrentShader(pointlightShader);

	/*modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);*/
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "lightCount"), lightVector.size());

	for (unsigned i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		ostringstream lcolourss;
		lcolourss << "lightColour[" << i << "]";
		string lcstr = lcolourss.str();

		glUniform4f(glGetUniformLocation(currentShader->GetProgram(), lcstr.c_str()),
			lightColour[i].x, lightColour[i].y, lightColour[i].z, lightColour[i].w);


		ostringstream lpositionss;
		lpositionss << "lightPos[" << i << "]";
		string lpstr = lpositionss.str();

		glUniform3f(glGetUniformLocation(currentShader->GetProgram(), lpstr.c_str()),lightPos[i].x, lightPos[i].y, lightPos[i].z);

		ostringstream lradiuss;
		lradiuss << "lightRadius[" << i << "]";
		string lrstr = lradiuss.str();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), lrstr.c_str()), lightRadius[i]);
	}
	/*modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	glDisable(GL_DEPTH_TEST);*/
}

void Renderer::DrawFlash(char* imagePath)
{
	glEnable(GL_BLEND);
	SetCurrentShader(FlashShader);

	GLuint image = SOIL_load_OGL_texture(imagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "flashSpeed"), ImageFlash);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDisable(GL_BLEND);
}