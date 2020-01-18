#include "AudioManager.h"
#include <climits>

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
	auto sItr = mSounds.begin();
	for (; !mSounds.empty(); ++sItr)
	{
		delete sItr->second;
		mSounds.erase(sItr--);
	}

	auto mItr = mMusics.begin();
	for (; !mMusics.empty(); ++mItr)
	{
		delete mItr->second;
		mMusics.erase(mItr--);
	}
}

int AudioManager::GetSoundIndex(const std::string & soundPath)
{
	// 同じパスのデータを探す
	for (auto itr : mSounds)
	{
		if (soundPath == itr.second->GetFilePath())
		{
			return itr.first;
		}
	}

	// 見つからなければロードする
	int ret = INT_MIN;
	Sound * snd = new Sound;
	if (snd->Load(soundPath))
	{
		// インデックスをランダムにすることでループ回数削減が見込める
		// 後でやりたいと思う
		int i = 0;
		bool findFlag = false;
		while (!findFlag)
		{
			auto itr = mSounds.find(i);
			(itr == mSounds.end()) ? ret = i : i++;
			findFlag = (ret == i);
		}

		mSounds[ret] = snd;
	}

	return ret;
}

int AudioManager::GetMusicIndex(const std::string & musicPath)
{
	// 同じパスのデータを探す
	for (auto itr : mMusics)
	{
		if (musicPath == itr.second->GetFilePath())
		{
			return itr.first;
		}
	}

	// 見つからなければロードする
	int ret = INT_MIN;
	Music * msc = new Music;
	if (msc->Load(musicPath))
	{
		// インデックスをランダムにすることでループ回数削減が見込める
		// 後でやりたいと思う
		int i = 0;
		bool findFlag = false;
		while (!findFlag)
		{
			auto itr = mMusics.find(i);
			(itr == mMusics.end()) ? ret = i : i++;
			findFlag = (ret == i);
		}

		mMusics[ret] = msc;
	}

	return ret;
}

void AudioManager::PlaySound(int soundIndex)
{
	auto itr = mSounds.find(soundIndex);
	if (itr != mSounds.end())
	{
		itr->second->Play();
	}
}

void AudioManager::StopSound(int soundIndex)
{
	auto itr = mSounds.find(soundIndex);
	if (itr != mSounds.end())
	{
		itr->second->Stop();
	}
}

bool AudioManager::IsPlayingSound(int soundIndex)
{
	auto itr = mSounds.find(soundIndex);
	if (itr != mSounds.end())
	{
		return itr->second->IsPlaying();
	}

	return false;
}

void AudioManager::DeresisterSound(int soundIndex)
{
	auto itr = mSounds.find(soundIndex);
	if (itr != mSounds.end())
	{
		delete itr->second;
		mSounds.erase(itr->first);
	}
}

void AudioManager::PlayMusic(int musicIndex)
{
	auto itr = mMusics.find(musicIndex);
	if (itr != mMusics.end())
	{
		itr->second->Play();
	}
}

void AudioManager::PlayFadeInMusic(int musicIndex, int time) const
{
	auto itr = mMusics.find(musicIndex);
	if (itr != mMusics.end())
	{
		itr->second->FadeIn(time);
	}
}

void AudioManager::DeresisterMusic(int musicIndex)
{
	auto itr = mMusics.find(musicIndex);
	if (itr != mMusics.end())
	{
		delete itr->second;
		mMusics.erase(itr);
	}
}
