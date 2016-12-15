#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/Window.h"
#include "Renderer.h"

int main() {
	
	Window w("Chengwei Fang Course Work", 960,540,false); //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);
	
	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float fps = w.GetTimer()->GetTimedMS();
		renderer.UpdateScene(fps);

		renderer.CalFPS(fps);	//calculate fps
		renderer.RenderScene();
	}

	return 0;
}