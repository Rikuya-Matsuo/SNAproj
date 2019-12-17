#include "StageBase.h"
#include "Block.h"

float StageBase::mBlockScale = 0.1f;

StageBase::StageBase():
	mBlocks(nullptr),
	mBlockMassX(0),
	mBlockMassY(0)
{
}

StageBase::~StageBase()
{
	// �������폜
	if (mBlocks != nullptr)
	{
		for (Uint8 i = 0; i < mBlockMassY; ++i)
		{
			delete[] mBlocks[i];
		}
		delete[] mBlocks;
	}
}

void StageBase::Load(const std::string & fileName)
{
	return;
}

void StageBase::Construct()
{
	// �u���b�N��null�Ȃ�s��Ȃ�
	if (mBlocks == nullptr)
	{
		SDL_Log("Stage : Block is not set! Fail to construction.\n");
		return;
	}
	else if (mBlockMassX <= 0 || mBlockMassY <= 0)
	{
		SDL_Log("Stage : Block mass is invalid value. Fail to construction.\n");
	}

	// ����
	for (int yBlock = 0; yBlock < mBlockMassY; ++yBlock)
	{
		for (int xBlock = 0; xBlock < mBlockMassX; ++xBlock)
		{
			const Uint8 blockType = mBlocks[yBlock][xBlock];

			// �u���b�N�z�u�f�[�^���̒l��0�ł���ΐ������Ȃ�
			if (!blockType)
			{
				continue;
			}

			// ����
			Block * const bk = new Block;
			bk->SetScale(mBlockScale);

			// �u���b�N�̍����̔������v�Z
			const float bhh = (Block::mModelSize / 2) * mBlockScale;

			// ��ԉ��̒i�̃u���b�N�̏�ʂ�����0�ƂȂ�悤�A��������ʒu���v�Z
			Vector3D pos(Block::mModelSize * xBlock, 0.0f, Block::mModelSize * (mBlockMassY - (yBlock + 1)) - bhh);

			// �ʒu�����
			bk->SetPosition(pos);
		}
	}
}
