#pragma once
#include "Sound.h"
#include "Music.h"
#include <map>

class AudioManager final
{
public:
	~AudioManager();

	static AudioManager & GetInstance()
	{
		static AudioManager instance;
		return instance;
	}

	// すでにロードしたサウンドデータのインデックスを取得し、
	// 未ロードのサウンドデータは新たにロードしてインデックスを返す。
	int GetSoundIndex(const std::string & soundPath);

	// すでにロードしたミュージックデータのインデックスを取得し、
	// 未ロードのミュージックデータは新たにロードしてインデックスを返す。
	int GetMusicIndex(const std::string & musicPath);

	void PlaySound(int soundIndex);
	void StopSound(int soundIndex);
	bool IsPlayingSound(int soundIndex);
	void DeresisterSound(int soundIndex);

	void PlayMusic(int musicIndex);
	void PlayFadeInMusic(int musicIndex, int time) const;
	void FadeOutMusic(int time) const { Music::FadeOut(time); }
	void StopMusic() const { Music::Stop(); };
	bool IsPlayingMusic() const { Music::IsPlaying(); }
	void DeresisterMusic(int musicIndex);

private:
	AudioManager();

	std::map<int, Sound*> mSounds;
	std::map<int, Music*> mMusics;
};
