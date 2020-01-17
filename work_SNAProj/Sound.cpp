#include "Sound.h"

Sound::Sound():
	mChunk(nullptr),
	mChannel(-1),
	mFilePath("")
{
}

Sound::~Sound()
{
	Mix_FreeChunk(mChunk);
}

bool Sound::Load(const std::string & filePath)
{
	mChunk = Mix_LoadWAV(filePath.c_str());

	bool success = mChunk != nullptr;
	if (success)
	{
		mFilePath = filePath;
		mFilePath.shrink_to_fit();
	}

	return success;
}

void Sound::Play()
{
	if (mChunk)
	{
		mChannel = Mix_PlayChannel(-1, mChunk, 0);
	}
}

void Sound::Stop()
{
	if (mChannel < 0)
	{
		return;
	}

	if (Mix_Playing(mChannel))
	{
		Mix_HaltChannel(mChannel);
	}
	mChannel = -1;
}
