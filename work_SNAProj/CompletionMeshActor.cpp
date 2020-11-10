#include "CompletionMeshActor.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "AnimationChips.h"
#include "System.h"

const Uint8 CompletionMeshActor::mFlipXFlagMask = 1 << 0;
const Uint8 CompletionMeshActor::mFlipYFlagMask = 1 << 1;
const Uint8 CompletionMeshActor::mNowFlippingFlagMask = 1 << 2;

CompletionMeshActor::CompletionMeshActor(const Actor * owner, int drawOrder):
	mOwner(owner),
	mCurrentIndex(-1),
	mPositionOffset(Vector3D::zero),
	mFlipFlag(0)
{
	// 回転の軸を本体に合わせる
	mRotationAxis = mOwner->GetRotationAxis();

	// メッシュのロード
	mMeshComponent = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mMesh->SetAnimModeFlag(true);
	mMeshComponent->SetMesh(mMesh);

	// 処理順の設定
	// 所有者よりも後でなければならない
	SetPriority(mOwner->GetPriority() + 50);

	// 重力適用フラグを切る
	mFlags &= ~mAffectGravityFlagMask_Base;

	// 位置の適用
	AdaptPosition();
}

CompletionMeshActor::~CompletionMeshActor()
{
	// スワップ技法によるリストの掃除
	std::list<int>().swap(mAnimIndexList);
}

void CompletionMeshActor::LoadAnimation(const std::string & filePath, Renderer * renderer, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex)
{
	// インデックスの検索
	auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), animIndex);

	// 指定のインデックスが使われていない場合、登録
	if (itr != mAnimIndexList.end() || mAnimIndexList.empty())
	{
		mAnimIndexList.emplace_back(animIndex);
	}
	// 既に使われている場合は関数の終了
	else
	{
		SDL_Log("Fail to load Animation : Specified index has already been used.");
		return;
	}

	// ロード
	mMesh->LoadDivTexture(filePath, renderer, this, allNum, xNum, yNum, chipW, chipH, secondPerFrame, animIndex);
}

void CompletionMeshActor::SetPositionOffset(const Vector3D & offset)
{
	// アクターからずらす距離の設定
	mPositionOffset = offset;

	// フラグを「反転していない」状態に初期化
	mFlipFlag &= ~mNowFlippingFlagMask;

	// 位置の適用
	AdaptPosition();
}

void CompletionMeshActor::FlipPositionOffset()
{
	// 各方向の反転フラグを参照し、ずらす距離を計算
	if (mFlipFlag & mFlipXFlagMask)
	{
		mPositionOffset.x *= -1;
	}
	
	if (mFlipFlag & mFlipYFlagMask)
	{
		mPositionOffset.z *= -1;
	}

	// 反転後のビットの値
	Uint8 flipping = ((mFlipFlag & mNowFlippingFlagMask) ^ mNowFlippingFlagMask);
	// 反転方向フラグを抽出
	Uint8 flipDir = (mFlipFlag & (mFlipXFlagMask | mFlipYFlagMask));

	mFlipFlag = flipping | flipDir;
}

void CompletionMeshActor::ResetAnimation(int index)
{
	// アニメーションチップの取得
	AnimationChips * anim = mMesh->GetAnimChips(this, index);

	if (!anim)
	{
		SDL_Log("anim is null\n");
		return;
	}

	// 初期化
	anim->Reset();
}

void CompletionMeshActor::UpdateActor0()
{
	// 行列の更新
	UpdateTransformData();

	// インデックスを検索。ヒットすれば描画するが、しなければ描画しない。
	auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), mCurrentIndex);
	bool drawFlag = (itr != mAnimIndexList.end());
	BitFlagFunc::SetFlagByBool(!drawFlag, mFlags, mStopDrawFlagMask_Base);

	// 指示されたインデックスにコマを合わせる
	if (drawFlag)
	{
		mMesh->SetAnimIndex(this, mCurrentIndex);
	}
}

void CompletionMeshActor::UpdateActor1()
{
	// 行列の更新
	UpdateTransformData();
}

void CompletionMeshActor::UpdateTransformData()
{
	// 本体の位置情報に変わりがないかのチェック
	bool scaleFlag = mScale != mOwner->GetScale();
	bool rotaFlag = mRotationAngle != mOwner->GetRotationAngle();
	bool posFlag = mPosition != (mOwner->GetPosition() + mPositionOffset);

	// スケールの更新
	if (scaleFlag)
	{
		mScale = mOwner->GetScale();

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	// 回転の更新
	if (rotaFlag)
	{
		mRotationAngle = mOwner->GetRotationAngle();

		mRotation = Quaternion(mOwner->GetRotationAxis(), mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	// 位置の更新
	AdaptPosition();

	// 行列計算フラグを真にする
	mFlags |= mCalculateTransformFlagMask_Base;
}
