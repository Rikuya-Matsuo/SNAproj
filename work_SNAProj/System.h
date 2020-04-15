#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "Renderer.h"
#include <vector>
#include <list>

// リリース版ではコメントアウトする
//#define DEBUG_SNA

class Actor;
class Camera;
class DrawComponentBase;
class SceneBase;
class Renderer;
class Sprite;

class System final
{
public:
	~System();

#ifdef DEBUG_SNA
	static Uint64 mFrameCount;
#endif // DEBUG_SNA

	static const float mMaxDeltaTime;

	static System& GetInstance()
	{
		static System instance;
		return instance;
	}

	bool Init();

	void Run();

	void Finish();

	void SetScene(SceneBase * in_scene) { mCurrentScene = in_scene; }

	float GetDeltaTime() const { return mDeltaTime; }

	//float GetAverageDeltaTime() const { return mAverageDeltaTime; }

	void ResisterActor(const Actor * in_act);

	void DeresisterActor(const Actor * in_act);

	void RequestSortActor() { mSortActorFlag = true; }

	void ResisterSprite(const Sprite * in_spr);

	void DeresisterSprite(const Sprite * in_spr);

	Renderer * GetRenderer() const { return mRenderer; }

	SDL_Renderer * GetSDLRenderer() const { return mRenderer->GetSDLRenderer(); }

	void SetActiveCamera(const Camera * cam) { mActiveCamera = const_cast<Camera *>(cam); }

	void ReportCameraDelete(const Camera * cam);

private:
	System();

	Renderer * mRenderer;

	Uint32 mPrevTicksCount;

	SceneBase * mCurrentScene;

	Camera * mActiveCamera;

	float mDeltaTime;

	int mWindowWidth;

	int mWindowHeight;

	bool mSortActorFlag;

	std::list<Actor *> mActorList;

	std::list<Sprite *> mSpriteList;

	void UpdateDeltaTime();
	
	void SortActor();

	void FixActorPosition();

	void UpdateScene();

	void UpdateActor();

	void Draw();

	void DrawSprites();

	void ChangeScene(bool & quitFlag);

	void GravityFall();
};
