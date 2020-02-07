#pragma once
#include <SDL/SDL_mixer.h>
#include <string>

class Sound
{
public:
	Sound();
	~Sound();

	bool Load(const std::string & filePath);

	void Play();
	void Stop();

	const std::string & GetFilePath() const { return mFilePath; }

	bool IsPlaying() const { return mChannel >= 0 && Mix_Playing(mChannel); }

private:
	Mix_Chunk * mChunk;

	int mChannel;

	std::string mFilePath;
};
