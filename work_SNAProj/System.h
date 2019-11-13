#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <vector>
#include <list>

class Actor;
class DrawComponentBase;
class SceneBase;

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

	const SDL_Renderer * const GetRenderer() const { return mRenderer; }

private:
	System();

	SDL_Window * mWindow;

	SDL_Renderer * mRenderer;

	Uint32 mPrevTicksCount;

	SceneBase * mCurrentScene;

	float mDeltaTime;

	std::vector<Actor *> mActorCollection;

	std::list<DrawComponentBase *> mDrawComponentList;

	void UpdateDeltaTime();
	
	void UpdateScene();

	void UpdateActor();

	void Draw();
};
