#include "TestStage.h"

TestStage::TestStage()
{
	// �R���X�g���N�^�������q���g���Č������Ƃ邩�A���[�J���ϐ��ɂ�������i�[���ăR�s�[���邱�Ƃŉǐ����Ƃ邩
	// �����I�񂾂̂͌�҂������B
	Uint8 testBlocks[mTestBlockMassY][mTestBlockMassX] =
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1}
	};

	// ���I�������m�ۂ��g�킸�ɐ������ꂽ�񎟌��z��́A��������ɘA�����ĕ���ł���͂��Ȃ̂ŁA��C�ɃR�s�[�ł���B
	SDL_memcpy(mTestBlocks, testBlocks, mTestBlockMassX * mTestBlockMassY);

	// �u���b�N�A���̃e�N�X�`���t�@�C������ݒ�
	mBlockTexturePath = "Assets/SM_Snow_Rock_Wall_A.png";
	mBlockTexturePath.shrink_to_fit();

	mFloorTexturePath = "Assets/SM_Snow_Rock_Wall_A.png";
	mFloorTexturePath.shrink_to_fit();

	// ���I�u�W�F�N�g�̃X�P�[���l��ݒ�
	mFloorScale = 1000.0f;
}

TestStage::~TestStage()
{
}

void TestStage::Load(const std::string & fileName)
{
	/////////////////////////
	// �u���b�N���т��R�s�[
	/////////////////////////
	// �������m��
	mBlocks = new Uint8* [mTestBlockMassY];
	for (Uint8 i = 0; i < mTestBlockMassY; ++i)
	{
		mBlocks[i] = new Uint8[mTestBlockMassX];
	}

	// �X�e�[�W�̏c���̃u���b�N����ݒ�
	mBlockMassX = mTestBlockMassX;
	mBlockMassY = mTestBlockMassY;

	// ���e�R�s�[
	for (Uint8 yLoop = 0; yLoop < mTestBlockMassY; ++yLoop)
	{
		SDL_memcpy(mBlocks[yLoop], mTestBlocks[yLoop], mTestBlockMassX);
	}

	/////////////////////////
	// �C���X�^���X����
	/////////////////////////
	Construct();
}
