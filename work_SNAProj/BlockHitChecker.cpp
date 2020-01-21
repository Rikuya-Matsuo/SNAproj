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

	auto checkBlock = [blockArray](int x, int y)
	{
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
	mHitDirectionFlags |= upHit ? Mask::mUpMask : 0;
	mHitDirectionFlags |= downHit ? Mask::mDownMask : 0;
	mHitDirectionFlags |= rightHit ? Mask::mRightMask : 0;
	mHitDirectionFlags |= leftHit ? Mask::mLeftMask : 0;
}
