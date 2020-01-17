#pragma once
#include "Sound.h"
#include "Music.h"
#include <map>

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	int GetSoundIndex(const std::string & soundPath);

	int GetMusicIndex(const std::string & musicPath);

	void PlaySound(int soundIndex);
	void PlayMusic(int musicIndex);

private:
	std::map<int, Sound*> mSounds;
	std::map<int, Music*> mMusics;
};
