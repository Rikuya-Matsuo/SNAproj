#include "BlockHitChecker.h"
#include "SceneBase.h"
#include "StageBase.h"
#include "Block.h"
#include "BoxColliderComponent.h"

namespace Mask = BlockHitDirectionFlagMask;

BlockHitChecker::BlockHitChecker(Actor * owner, BoxColliderComponent * box):
	ComponentBase(owner, 1000),
	mHitDirectionFlags(0),
	mBox(box),
	mStage(nullptr)
{
	mStage = const_cast<StageBase*>(mOwner->GetBelongScene()->GetStage());
}

void BlockHitChecker::Update()
{
	if (mStage == nullptr || mBox == nullptr)
	{
		return;
	}

	// �u���b�N���擾
	Uint8 **const blockArray = mStage->GetBlocks();
	const float blockSize = mStage->GetBlockScale() * Block::mModelSize;
	const float highest = (mStage->GetBlockMassY() - 1) * blockSize;

	// �{�b�N�X�̎擾
	AABB box = *mBox->GetWorldBox();
	//box.mMin += mOwner->GetMoveVector();
	//box.mMax += mOwner->GetMoveVector();

	// 0.0�̉���1�u���b�N����̂�-1
	int headHeightBlock = static_cast<int>(box.mMax.z / blockSize) - 1;

	int footHeightBlock = static_cast<int>(box.mMin.z / blockSize) - 1;

	int midYBlock = static_cast<int>((box.mMin.z + (box.mMax.z - box.mMin.z) / 2) / blockSize) - 1;

	int leftXBlock = static_cast<int>(box.mMin.x / blockSize);

	int rightXBlock = static_cast<int>(box.mMax.x / blockSize);

	int midXBlock = static_cast<int>((box.mMin.x + (box.mMax.x - box.mMin.x) / 2) / blockSize);

	auto checkBlock = [this, blockArray](int x, int y)
	{
		bool xRangeOut = (x >= mStage->GetBlockMassX() || x < 0);
		bool yRangeOut = (y >= mStage->GetBlockMassY() || y < 0);

		if (xRangeOut || yRangeOut)
		{
			return false;
		}

		bool ret = blockArray[y][x];
		return ret;
	};

	bool upHit = checkBlock(leftXBlock, headHeightBlock) ||
		checkBlock(rightXBlock, headHeightBlock);

	bool downHit = checkBlock(leftXBlock, footHeightBlock) ||
		checkBlock(rightXBlock, footHeightBlock);

	bool rightHit = checkBlock(rightXBlock, headHeightBlock) ||
		checkBlock(rightXBlock, footHeightBlock);

	bool leftHit = checkBlock(leftXBlock, headHeightBlock) ||
		checkBlock(leftXBlock, footHeightBlock);

	mHitDirectionFlags = 0;
	mHitDirectionFlags |=	checkBlock(rightXBlock, headHeightBlock) ? Mask::mRUVerMask : 0;
	mHitDirectionFlags |=	checkBlock(rightXBlock, footHeightBlock) ? Mask::mRDVerMask : 0;
	mHitDirectionFlags |=	checkBlock(leftXBlock, headHeightBlock)  ? Mask::mLUVerMask : 0;
	mHitDirectionFlags |=	checkBlock(leftXBlock, footHeightBlock)  ? Mask::mLDVerMask : 0;
	
	bool downHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mRDVerMask | Mask::mLDVerMask)))
	{
		if (m == (Mask::mRDVerMask | Mask::mLDVerMask))
		{
			downHit1 = true;
		}
		else if (m)
		{
			if (checkBlock(midXBlock, footHeightBlock))
			{
				downHit1 = true;
			}
			else
			{
				float blockHead = highest - (blockSize * footHeightBlock);
				float overlapY = blockHead - box.mMin.z;

				float overlapX;
				if (m == Mask::mRDVerMask)
				{
					float blockEdgeL = blockSize * rightXBlock;
					overlapX = box.mMax.x - blockEdgeL;
				}
				else
				{
					float blockEdgeR = blockSize * (leftXBlock + 1);
					overlapX = blockEdgeR - box.mMin.x;
				}

				if (overlapX >= overlapY)
				{
					downHit1 = true;
				}
			}
		}
	}

	bool rightHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mRUVerMask | Mask::mRDVerMask)))
	{
		if (m == (Mask::mRDVerMask | Mask::mRUVerMask))
		{
			rightHit1 = true;
		}
		else if (m)
		{
			if (checkBlock(rightXBlock, midYBlock))
			{
				rightHit1 = true;
			}
			else
			{
				float blockEdgeL = blockSize * rightXBlock;
				float overlapX = box.mMax.x - blockEdgeL;
				
				float overlapY;
				if (m == Mask::mRDVerMask)
				{
					float blockHead = highest - (footHeightBlock * blockSize);
					overlapY = box.mMin.z - blockHead;
				}
				else
				{
					float blockFoot = highest - ((headHeightBlock + 1) * blockSize);
					overlapY = blockFoot - box.mMax.z;
				}

				if (overlapX < overlapY)
				{
					rightHit1 = true;
				}
			}
		}
	}

	bool leftHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mLUVerMask | Mask::mLDVerMask)))
	{
		if (m == (Mask::mLDVerMask | Mask::mLUVerMask))
		{
			leftHit1 = true;
		}
		else if (m)
		{
			if (checkBlock(leftXBlock, midYBlock))
			{
				leftHit1 = true;
			}
			else
			{
				float blockEdgeR = blockSize * leftXBlock;
				float overlapX = blockEdgeR - box.mMin.x;

				float overlapY;
				if (m == Mask::mRDVerMask)
				{
					float blockHead = highest - (footHeightBlock * blockSize);
					overlapY = box.mMin.z - blockHead;
				}
				else
				{
					float blockFoot = highest - ((headHeightBlock + 1) * blockSize);
					overlapY = blockFoot - box.mMax.z;
				}

				if (overlapX < overlapY)
				{
					leftHit1 = true;
				}
			}
		}
	}

	/*
	if (upHit && (rightHit || leftHit))
	{
		float blockFootHeight = highest - (blockSize * (headHeightBlock + 1));
		float overlapY = box.mMax.z - blockFootHeight;
		float leftBlockEdgeR = blockSize * (leftXBlock + 1);
		float rightBlockEdgeL = blockSize * rightXBlock;
		float overlapX = box.mMax.x - rightBlockEdgeL;
		if (overlapX > leftBlockEdgeR - box.mMin.x)
		{
			overlapX = leftBlockEdgeR - box.mMin.x;
		}

		if (overlapX >= overlapY)
		{
			rightHit ? rightHit = false : leftHit = false;
		}
		else
		{
			upHit = false;
		}
	}

	if (downHit && (rightHit || leftHit))
	{
		float blockHead = highest - (blockSize * footHeightBlock);
		float overlapY = blockHead - box.mMin.z;
		float leftBlockEdgeR = blockSize * (leftXBlock + 1);
		float rightBlockEdgeL = blockSize * rightXBlock;
		float overlapX = box.mMax.x - rightBlockEdgeL;
		if (overlapX > leftBlockEdgeR - box.mMin.x)
		{
			overlapX = leftBlockEdgeR - box.mMin.x;
		}

		if (overlapX > overlapY)
		{
			rightHit ? rightHit = false : leftHit = false;
		}
		else
		{
			downHit = false;
		}
	}
	*/
	mHitDirectionFlags |= upHit ? Mask::mUpMask : 0;
	mHitDirectionFlags |= downHit1 ? Mask::mDownMask : 0;
	mHitDirectionFlags |= rightHit1 ? Mask::mRightMask : 0;
	mHitDirectionFlags |= leftHit1 ? Mask::mLeftMask : 0;

	// �����Ԃ��v���Z�X
	if (upHit)
	{
		float blockFootHeight = highest - (blockSize * (headHeightBlock + 1));
		float overlapY = box.mMax.z - blockFootHeight;

		Vector3D v = mOwner->GetPosition();
		v.z -= overlapY;
		mOwner->SetPosition(v);

		Vector3D vel = mOwner->GetMoveVector();
		vel.z = 0.0f;
		mOwner->SetMoveVector(vel);
	}
	
	if (downHit1)
	{
		float blockHead = highest - (blockSize * footHeightBlock);
		float overlapY = blockHead - box.mMin.z;

		mOwner->SetFixVector(Vector3D(0, 0, overlapY));

		Vector3D vel = mOwner->GetMoveVector();
		vel.z = 0.0f;
		mOwner->SetMoveVector(vel);
	}

	if (rightHit1)
	{
		float rightBlockEdgeL = blockSize * rightXBlock;
		float overlapX = box.mMax.x - rightBlockEdgeL;

		mOwner->SetFixVector(Vector3D(-overlapX, 0, 0));

		Vector3D vel = mOwner->GetMoveVector();
		vel.x = 0.0f;
		mOwner->SetMoveVector(vel);
	}

	if (leftHit1)
	{
		float leftBlockEdgeR = blockSize * (leftXBlock + 1);
		float overlapX = leftBlockEdgeR - box.mMin.x;

		mOwner->SetFixVector(Vector3D(overlapX, 0, 0));

		Vector3D vel = mOwner->GetMoveVector();
		vel.x = 0.0f;
		mOwner->SetMoveVector(vel);
	}
}

void BlockHitChecker::CheckProcess()
{
}

void BlockHitChecker::PushProcess()
{
}
