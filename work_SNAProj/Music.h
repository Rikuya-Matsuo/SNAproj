#pragma once
#include <SDL/SDL_mixer.h>
#include <string>

class Music
{
public:
	Music();
	~Music();

	bool Load(const std::string & filePath);

	void Play() const;
	void FadeIn(int time);

	const std::string & GetFilePath() const { return mFilePath; }

	static void Stop() { Mix_HaltMusic(); }
	static void FadeOut(int time) { Mix_FadeOutMusic(time); }
	static bool IsPlaying() { return Mix_PlayingMusic(); }

private:
	Mix_Music * mMusic;

	std::string mFilePath;
};

