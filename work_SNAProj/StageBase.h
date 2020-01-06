#pragma once
#include "SDL/SDL.h"
#include <string>

class StageBase
{
public:
	StageBase();
	~StageBase();

	// ������
	virtual void Load(const std::string & fileName);

	static void SetBlockScale(float scale) { mBlockScale = scale; }

protected:
	Uint8 ** mBlocks;

	int mBlockMassX;
	int mBlockMassY;

	static float mBlockScale;

	std::string mBlockTexturePath;

	std::string mFloorTexturePath;

	float mFloorScale;

	// �u���b�N�z�u�z��Ɋ�Â��ău���b�N�̃C���X�^���X�𐶐�����B
	void Construct();
};
