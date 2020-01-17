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
		int i = 0;
		bool findFlag = false;
		while (!findFlag)
		{
			auto itr = mSounds.find(i);
			(itr == mSounds.end()) ? ret = i : i++;
			findFlag = (ret == i);
		}
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
		int i = 0;
		bool findFlag = false;
		while (!findFlag)
		{
			auto itr = mMusics.find(i);
			(itr == mMusics.end()) ? ret = i : i++;
			findFlag = (ret == i);
		}
	}

	return ret;
}
