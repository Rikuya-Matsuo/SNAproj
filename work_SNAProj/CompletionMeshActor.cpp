#include "CompletionMeshActor.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "AnimationChips.h"
#include "System.h"

const Uint8 CompletionMeshActor::mFlipXFlagMask = 1 << 0;
const Uint8 CompletionMeshActor::mFlipYFlagMask = 1 << 1;
const Uint8 CompletionMeshActor::mNowFlippingFlagMask = 1 << 2;

const CompletionMeshActor::FlagType CompletionMeshActor::mSyncPositionFlagMask = 1 << 0;
const CompletionMeshActor::FlagType CompletionMeshActor::mSyncScaleFlagMask = 1 << 1;
const CompletionMeshActor::FlagType CompletionMeshActor::mSyncRotationFlagMask = 1 << 2;

CompletionMeshActor::CompletionMeshActor(const Actor * owner, int drawOrder):
	mOwner(owner),
	mCurrentIndex(-1),
	mPositionOffset(Vector3D::zero),
	mFlipFlag(0),
	mTransformUpdateFlags(mSyncPositionFlagMask | mSyncRotationFlagMask | mSyncRotationFlagMask)
{
	mRotationAxis = mOwner->GetRotationAxis();

	mMeshComponent = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mMeshComponent->SetMesh(mMesh);
	//mMeshComponent->SetActive(false);

	// 処理順の設定
	// 所有者よりも後でなければならない
	SetPriority(mOwner->GetPriority() + 50);

	mFlags &= ~mAffectGravityFlagMask_Base;

	AdaptPosition();
}

CompletionMeshActor::~CompletionMeshActor()
{
	std::list<int>().swap(mAnimIndexList);
}

void CompletionMeshActor::LoadAnimation(const std::string & filePath, Renderer * renderer, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex)
{
	auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), animIndex);
	if (itr != mAnimIndexList.end() || mAnimIndexList.empty())
	{
		mAnimIndexList.emplace_back(animIndex);
		mMesh->SetAnimModeFlag(true);
	}
	else
	{
		SDL_Log("Fail to load Animation : Specified index has already been used.");
		return;
	}

	mMesh->LoadDivTexture(filePath, renderer, this, allNum, xNum, yNum, chipW, chipH, secondPerFrame, animIndex);
}

void CompletionMeshActor::SetPositionOffset(const Vector3D & offset)
{
	mPositionOffset = offset;

	// フラグを「反転していない」状態に初期化
	mFlipFlag &= ~mNowFlippingFlagMask;

	AdaptPosition();
}

void CompletionMeshActor::FlipPositionOffset()
{
	if (mFlipFlag & mFlipXFlagMask)
	{
		mPositionOffset.x *= -1;
	}
	
	if (mFlipFlag & mFlipYFlagMask)
	{
		mPositionOffset.z *= -1;
	}

	Uint8 flipping = ((mFlipFlag & mNowFlippingFlagMask) ^ mNowFlippingFlagMask);		// 反転後のビットの値
	Uint8 flipDir = (mFlipFlag & (mFlipXFlagMask | mFlipYFlagMask));					// 反転方向フラグを抽出
	mFlipFlag = flipping | flipDir;
}

void CompletionMeshActor::ResetAnimation(int index)
{
	AnimationChips * anim = mMesh->GetAnimChips(this, index);

	if (!anim)
	{
		SDL_Log("anim is null\n");
		return;
	}

	anim->Reset();
}

void CompletionMeshActor::SetTexture(Texture * tex)
{
	mMesh->SetTexture(this, tex);
}

void CompletionMeshActor::UpdateActor0()
{
	UpdateTransformData();

	if (mMesh->GetUseAnimFlag(this))
	{
		// インデックスを検索。ヒットすれば描画するが、しなければ描画しない。
		auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), mCurrentIndex);
		bool drawFlag = (itr != mAnimIndexList.end());
		BitFlagFunc::SetFlagByBool(!drawFlag, mFlags, mStopDrawFlagMask_Base);

		if (drawFlag)
		{
			mMesh->SetAnimIndex(this, mCurrentIndex);
		}
	}
}

void CompletionMeshActor::UpdateActor1()
{
	UpdateTransformData();
}

void CompletionMeshActor::UpdateTransformData()
{
	bool scaleFlag = mScale != mOwner->GetScale() && mTransformUpdateFlags & mSyncScaleFlagMask;
	bool rotaFlag = mRotationAngle != mOwner->GetRotationAngle() && mTransformUpdateFlags & mSyncRotationFlagMask;
	bool posFlag = mPosition != (mOwner->GetPosition() + mPositionOffset) && mTransformUpdateFlags & mSyncPositionFlagMask;

	if (scaleFlag)
	{
		mScale = mOwner->GetScale();

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	if (rotaFlag)
	{
		mRotationAngle = mOwner->GetRotationAngle();

		mRotation = Quaternion(mOwner->GetRotationAxis(), mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	AdaptPosition();

	mFlags |= mCalculateTransformFlagMask_Base;
	if (posFlag)
	{
	}
}
