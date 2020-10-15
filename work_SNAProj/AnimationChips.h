#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <vector>
#include <string>
#include <unordered_map>

class Renderer;
class Texture;

class AnimationChips
{
public:
	AnimationChips();

	~AnimationChips();

	void Update();

	void Reset();

	size_t Load(Renderer * renderer, const std::string & fileName, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame);

	void SetSecondPerFrame(float second) { mSecondPerFrame = second; }

	void SetTextureIndex(size_t num);

	size_t GetCurrentTextureIndex() const { return mCurrentTextureIndex; }

	Texture * GetCurrentTexture() const { return mChipTextures[mCurrentTextureIndex]; }

	bool GetLoopEndFlag() const { return mFlags & mLoopEndFlagMask; }

	void StopPlaying() { mFlags |= mStopFlagMask; }

	void StartPlaying() { mFlags &= ~mStopFlagMask; }

	void SetRoutine(const int * frameNumberArray, size_t arraySize);
	void SetRoutine(const int * frameNumberArray);		// 末尾を-1にすること。

	size_t GetFrameMass() const { return mChipTextures.size(); }

	size_t GetRoutineMass() const { return mRoutine.size(); }

private:
	typedef Uint8 FlagType;
	static const FlagType mLoopEndFlagMask;
	static const FlagType mStopFlagMask;


	static std::unordered_map<std::string, std::vector<Texture*>> mFrameTextureList;

	FlagType mFlags;

	std::vector<Texture *> mChipTextures;

	size_t mCurrentTextureIndex;

	std::vector<size_t> mRoutine;
	size_t mCurrentRoutineIndex;

	float mSecondPerFrame;

	float mTimer;
};

