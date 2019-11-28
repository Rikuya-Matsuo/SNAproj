#pragma once
#include "SDL/SDL.h"
#include <vector>

// 前方宣言
class Actor;
class Camera;

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

protected:
	typedef Uint8 FlagType;

	static const FlagType mSceneChangeFlagMask;

	FlagType mFlags;

	std::vector<Actor *> mActors;

	std::vector<Camera *> mCameras;

	SceneBase * mNextScene;
};
