#include "BlockHitChecker.h"
#include "SceneBase.h"
#include "Stage.h"
#include "Block.h"
#include "BoxColliderComponent.h"
#include "PhysicManager.h"

namespace Mask = BlockHitDirectionFlagMask;

BlockHitChecker::BlockHitChecker(Actor * owner, BoxColliderComponent * box):
	ComponentBase(owner, 1000, UpdateTiming::UpdateTiming_AfterAddMoveVector),
	mHitDirectionFlags(0),
	mBox(box)
{
}

void BlockHitChecker::Update()
{
	// ステージ情報を取得
	const Stage * stage = mOwner->GetBelongScene()->GetStage();

	if (stage == nullptr || mBox == nullptr)
	{
		return;
	}

	// ブロック情報取得
	Stage::BlockKindIDType **const blockArray = stage->GetBlocks();
	const float blockSize = stage->GetBlockScale() * Block::mModelSize;
	const float highest = stage->GetFloorHeight() + (stage->GetBlockMassY() * blockSize);

	// ボックスの取得
	AABB box = *mBox->GetWorldBox();

	// ボックスの各辺が何番目のブロックかを計算
	int headHeightBlock = static_cast<int>((highest - box.mMax.z) / blockSize);

	int footHeightBlock = static_cast<int>((highest - box.mMin.z) / blockSize);

	int midYBlock = static_cast<int>((highest - (box.mMin.z + (box.mMax.z - box.mMin.z) / 2)) / blockSize);

	int leftXBlock = static_cast<int>(box.mMin.x / blockSize);
	if (box.mMin.x < 0.0f)
	{
		// 負の領域だと-1に矯正
		leftXBlock = -1;
	}

	int rightXBlock = static_cast<int>(box.mMax.x / blockSize);
	if (box.mMax.x < 0.0f)
	{
		// 負の領域だと-1に矯正
		rightXBlock = -1;
	}

	float boxXMidPos = (box.mMin.x + (box.mMax.x - box.mMin.x) / 2);
	int midXBlock = static_cast<int>(boxXMidPos / blockSize);
	if (boxXMidPos < 0.0f)
	{
		// 負の領域だと-1に矯正
		midXBlock = -1;
	}

	///////////////////////////////////////
	// 判定プロセス
	///////////////////////////////////////
	// 指定したブロックインデックスの位置にブロックがあるか否かのフラグを返す。
	auto checkBlock = [stage, blockArray](int x, int y)
	{
		bool xRangeOut = (x >= stage->GetBlockMassX() || x < 0);
		bool yRangeOut = (y >= stage->GetBlockMassY() || y < 0);

		if (xRangeOut || yRangeOut)
		{
			return false;
		}

		bool ret = blockArray[y][x];
		return ret;
	};

	// 各位置のブロックの有無をビットフラグに記録
	mHitDirectionFlags = 0;
	mHitDirectionFlags |=	checkBlock(rightXBlock, headHeightBlock) ? Mask::mRUVerMask : 0;
	mHitDirectionFlags |=	checkBlock(rightXBlock, footHeightBlock) ? Mask::mRDVerMask : 0;
	mHitDirectionFlags |=	checkBlock(leftXBlock, headHeightBlock)  ? Mask::mLUVerMask : 0;
	mHitDirectionFlags |=	checkBlock(leftXBlock, footHeightBlock)  ? Mask::mLDVerMask : 0;

	// 上方向の判定
	bool upHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mRUVerMask | Mask::mLUVerMask)))
	{
		// 右上と左上にブロックがあるならば、真上にもあると判断してよい
		if (m == (Mask::mRUVerMask | Mask::mLUVerMask))
		{
			upHit1 = true;
		}
		// どちらか片方だけ接触している場合
		else if (m)
		{
			// (x, y) : (中央, 上)にブロックがある場合、真上にブロックはある
			if (checkBlock(midXBlock, headHeightBlock))
			{
				upHit1 = true;
			}
			else
			{
				// 縦横のめり込み発生部分の長さを比較し、横が長ければ上にブロックがある判定
				float blockFoot = highest - (blockSize * (headHeightBlock + 1));
				float overlapY = box.mMax.z - blockFoot;

				float overlapX;
				if (m == Mask::mRUVerMask)
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
					upHit1 = true;
				}
			}
		}
	}

	// 下方向の判定
	bool downHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mRDVerMask | Mask::mLDVerMask)))
	{
		// 右下と左下にブロックがあれば下にブロックはあると判断する
		if (m == (Mask::mRDVerMask | Mask::mLDVerMask))
		{
			downHit1 = true;
		}
		// どちらか片方だけ接触している場合
		else if (m)
		{
			// (x, y) : (中央, 下)にブロックがある場合、真下にブロックはある
			if (checkBlock(midXBlock, footHeightBlock))
			{
				downHit1 = true;
			}
			else
			{
				// 縦横のめり込み発生部分の長さを比較し、横が長ければ下にブロックがある判定
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

	// 右の判定
	bool rightHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mRUVerMask | Mask::mRDVerMask)))
	{
		// 右上、右下にブロックがあれば右にブロックはあると判断する
		if (m == (Mask::mRDVerMask | Mask::mRUVerMask))
		{
			rightHit1 = true;
		}
		// どちらか片方だけ接触している場合
		else if (m)
		{
			// (x, y) : (右, 中央)にブロックがあれば真右にブロックはある
			if (checkBlock(rightXBlock, midYBlock))
			{
				rightHit1 = true;
			}
			else
			{
				// 縦横のめり込み発生部分の長さを比較し、縦が長ければ右にブロックがある判定
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

	// 左の判定
	bool leftHit1 = false;
	if (Uint8 m = (mHitDirectionFlags & (Mask::mLUVerMask | Mask::mLDVerMask)))
	{
		// 左上、左下にブロックがあれば真左にブロックはあると判断する
		if (m == (Mask::mLDVerMask | Mask::mLUVerMask))
		{
			leftHit1 = true;
		}
		// どちらか片方だけ接触している場合
		else if (m)
		{
			// (x, y) : (左, 中央)にブロックがあれば、真左にブロックがある
			if (checkBlock(leftXBlock, midYBlock))
			{
				leftHit1 = true;
			}
			else
			{
				// 縦横のめり込み発生部分の長さを比較し、縦が長ければ左にブロックがある判定
				float blockEdgeR = blockSize * (leftXBlock + 1);
				float overlapX = blockEdgeR - box.mMin.x;

				float overlapY;
				if (m == Mask::mLDVerMask)
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

	mHitDirectionFlags |= upHit1 ? Mask::mUpMask : 0;
	mHitDirectionFlags |= downHit1 ? Mask::mDownMask : 0;
	mHitDirectionFlags |= rightHit1 ? Mask::mRightMask : 0;
	mHitDirectionFlags |= leftHit1 ? Mask::mLeftMask : 0;

	///////////////////////////////////////
	// 押し返しプロセス
	///////////////////////////////////////
	bool xBuryDeeplyFlag = false;
	bool zBuryDeeplyFlag = false;
	const Vector3D boxSize = box.mMax - box.mMin;
	const float buryRate = PhysicManager::mBuryRate;
	// 上方向
	if (upHit1)
	{
		float blockFoot = highest - (blockSize * (headHeightBlock + 1));
		float overlapY = box.mMax.z - blockFoot;

		// 押し返し
		mOwner->SetFixVector(Vector3D(0, 0, -overlapY));

		// 埋まりすぎていないか
		zBuryDeeplyFlag = overlapY >= boxSize.z * buryRate;

		// 移動ベクトル制限
		Vector3D vel = mOwner->GetMoveVector();
		if (vel.z > 0.0f)
		{
			vel.z = 0.0f;
			mOwner->SetMoveVector(vel);
		}
	}
	
	// 下方向
	if (downHit1)
	{
		float blockHead = highest - (blockSize * footHeightBlock);
		float overlapY = blockHead - box.mMin.z;

		// 押し返し
		mOwner->SetFixVector(Vector3D(0, 0, overlapY - 0.001f));

		// 埋まりすぎていないか
		zBuryDeeplyFlag = overlapY >= boxSize.z * buryRate;

		// 移動ベクトル制限
		Vector3D vel = mOwner->GetMoveVector();
		if (vel.z < 0.0f)
		{
			vel.z = 0.0f;
			mOwner->SetMoveVector(vel);
		}
	}

	// 右方向
	if (rightHit1)
	{
		float rightBlockEdgeL = blockSize * rightXBlock;
		float overlapX = box.mMax.x - rightBlockEdgeL;

		// 押し返し
		mOwner->SetFixVector(Vector3D(-overlapX - 0.001f, 0, 0));

		// 埋まりすぎていないか
		xBuryDeeplyFlag = overlapX >= boxSize.x * buryRate;

		// 移動ベクトル制限
		Vector3D vel = mOwner->GetMoveVector();
		if (vel.x > 0.0f)
		{
			vel.x = 0.0f;
			mOwner->SetMoveVector(vel);
		}
	}

	// 左方向
	if (leftHit1)
	{
		float leftBlockEdgeR = blockSize * (leftXBlock + 1);
		float overlapX = leftBlockEdgeR - box.mMin.x;

		// 押し返し
		mOwner->SetFixVector(Vector3D(overlapX, 0, 0));

		// 埋まりすぎていないか
		xBuryDeeplyFlag = overlapX >= boxSize.x * buryRate;

		// 移動ベクトル制限
		Vector3D vel = mOwner->GetMoveVector();
		if (vel.x < 0.0f)
		{
			vel.x = 0.0f;
			mOwner->SetMoveVector(vel);
		}
	}

	// 過剰に埋まっているなら元の位置に戻す処理を行う
	if (xBuryDeeplyFlag && zBuryDeeplyFlag)
	{
		mOwner->OnBuryDeeply();
	}
}

void BlockHitChecker::CheckProcess()
{
}

void BlockHitChecker::PushProcess()
{
}
