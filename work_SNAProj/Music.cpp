#include "Music.h"

Music::Music():
	mMusic(nullptr),
	mFilePath("")
{
}

Music::~Music()
{
	Mix_FreeMusic(mMusic);
}

bool Music::Load(const std::string & filePath)
{
	mMusic = Mix_LoadMUS(filePath.c_str());
	
	bool success = mMusic != nullptr;
	if (success)
	{
		mFilePath = filePath;
		mFilePath.shrink_to_fit();
	}

	return success;
}

void Music::Play() const
{
	if (mMusic)
	{
		Mix_PlayMusic(mMusic, -1); // -1ÇÕÉãÅ[ÉvÇ∑ÇÈê›íË
	}
}

void Music::FadeIn(int time)
{
	if (mMusic)
	{
		Mix_FadeInMusic(mMusic, -1, time);
	}
}
