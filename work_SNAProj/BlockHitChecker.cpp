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

	// ブロック情報取得
	Uint8 **const blockArray = mStage->GetBlocks();
	const float blockSize = mStage->GetBlockScale() * Block::mModelSize;
	const float highest = (mStage->GetBlockMassY() - 1) * blockSize;

	// ボックスの取得
	AABB box = *mBox->GetWorldBox();
	box.mMin += mOwner->GetMoveVector();
	box.mMax += mOwner->GetMoveVector();

	// 0.0の下に1ブロックあるので-1
	int headHeightBlock = static_cast<int>(box.mMax.z / blockSize) - 1;

	int footHeightBlock = static_cast<int>(box.mMin.z / blockSize) - 1;

	int leftXBlock = static_cast<int>(box.mMin.x / blockSize);

	int rightXBlock = static_cast<int>(box.mMax.x / blockSize);

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

		if (overlapX >= overlapY)
		{
			rightHit ? rightHit = false : leftHit = false;
		}
		else
		{
			downHit = false;
		}
	}

	mHitDirectionFlags |= upHit ? Mask::mUpMask : 0;
	mHitDirectionFlags |= downHit ? Mask::mDownMask : 0;
	mHitDirectionFlags |= rightHit ? Mask::mRightMask : 0;
	mHitDirectionFlags |= leftHit ? Mask::mLeftMask : 0;

	// 押し返しプロセス
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
	
	if (downHit)
	{
		float blockHead = highest - (blockSize * footHeightBlock);
		float overlapY = blockHead - box.mMin.z;

		Vector3D v = mOwner->GetPosition();
		v.z += overlapY;
		mOwner->SetPosition(v);

		Vector3D vel = mOwner->GetMoveVector();
		vel.z = 0.0f;
		mOwner->SetMoveVector(vel);
	}

	if (rightHit)
	{
		float rightBlockEdgeL = blockSize * rightXBlock;
		float overlapX = box.mMax.x - rightBlockEdgeL;

		Vector3D v = mOwner->GetPosition();
		v.x -= overlapX;
		mOwner->SetPosition(v);

		Vector3D vel = mOwner->GetMoveVector();
		vel.x = 0.0f;
		mOwner->SetMoveVector(vel);
	}

	if (leftHit)
	{
		float leftBlockEdgeR = blockSize * (leftXBlock + 1);
		float overlapX = leftBlockEdgeR - box.mMin.x;

		Vector3D v = mOwner->GetPosition();
		v.x += overlapX;
		mOwner->SetPosition(v);

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
