#pragma once
#include "SDL/SDL.h"
#include <vector>
#include <thread>

// 前方宣言
class Actor;
class Camera;
class Stage;
class SDLScreen;

class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase();

	virtual void Update() = 0;

	void Draw();

	SceneBase * GetNextScene() const { return mNextScene; }

	void RequestSceneChange() { mFlags |= mSceneChangeFlagMask; }

	bool GetSceneChangeFlag() const { return mFlags & mSceneChangeFlagMask; }

	const Stage * GetStage() const { return mStage; }

	static const SceneBase * GetLatestScene() { return mLatestScene; }

	static bool GetNowLoadingFlag() { return mNowLoadingFlag; }

protected:
	typedef Uint8 FlagType;

	static const FlagType mSceneChangeFlagMask;

	static SceneBase * mLatestScene;

	static SDLScreen * mLoadingScreen;

	static bool mNowLoadingFlag;

	FlagType mFlags;

	// 次のシーンに引き渡すアクターは、これにポインタを格納する
	std::vector<Actor *> mActors;

	std::vector<Camera *> mCameras;

	SceneBase * mNextScene;

	Stage * mStage;

	static std::thread mLoadingScreenThread;

	static void PlayLoadingScreen(SDL_Renderer * renderer);
};
