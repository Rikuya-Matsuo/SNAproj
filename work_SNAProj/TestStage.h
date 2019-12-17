#pragma once
#include "StageBase.h"

class TestStage : public StageBase
{
public:
	TestStage();
	~TestStage();

	// �t�@�C�����ɉ������Ă��e�X�g�p�̃X�e�[�W�𐶐�����
	void Load(const std::string & fileName) override;

private:
	// �N���X�̒�`���Ŏg���萔�ł���́A�N���X���ŏ���������
	static const Uint8 mTestBlockMassX = 32;
	static const Uint8 mTestBlockMassY = 4;

	Uint8 mTestBlocks[mTestBlockMassY][mTestBlockMassX];
};
