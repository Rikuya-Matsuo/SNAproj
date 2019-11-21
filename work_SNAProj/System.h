#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Renderer.h"
#include <vector>
#include <list>

class Actor;
class Camera;
class DrawComponentBase;
class SceneBase;
class Renderer;

class System final
{
public:
	~System();

	static System& GetInstance()
	{
		static System instance;
		return instance;
	}

	bool Init();

	void Run();

	void Finish();

	void SetScene(SceneBase * in_scene) { mCurrentScene = in_scene; }

	float GetDeltaTime() { return mDeltaTime; }

	void ResisterActor(const Actor * in_act);

	void DeresisterActor(const Actor * in_act);

	void ResisterDrawComponent(const DrawComponentBase * in_cmp);

	void DeresisterDrawComponent(const DrawComponentBase * in_cmp);

	Renderer * GetRenderer() const { return mRenderer; }

	SDL_Renderer * GetSDLRenderer() const { return mRenderer->GetSDLRenderer(); }

	void SetActiveCamera(const Camera * cam) { mActiveCamera = const_cast<Camera *>(cam); }

	void ReportCameraDelete(const Camera * cam);

private:
	System();

	//SDL_Window * mWindow;

	//SDL_Renderer * mSDLRenderer;

	Renderer * mRenderer;

	Uint32 mPrevTicksCount;

	SceneBase * mCurrentScene;

	Camera * mActiveCamera;

	float mDeltaTime;

	int mWindowWidth;

	int mWindowHeight;

	std::vector<Actor *> mActorCollection;

	std::list<DrawComponentBase *> mSpriteComponentList;

	void UpdateDeltaTime();
	
	void UpdateScene();

	void UpdateActor();

	void Draw();
};
