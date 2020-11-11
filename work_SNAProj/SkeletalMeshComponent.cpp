#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "System.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Animation.h"
#include "Skeleton.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner, int drawOrder)
	:MeshComponent(owner, drawOrder, true)
	, mSkeleton(nullptr)
{
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
	if (mSkeleton != nullptr)
	{
		delete mSkeleton;
		mSkeleton = nullptr;
	}

	if (mAnimation != nullptr)
	{
		delete mAnimation;
		mAnimation = nullptr;
	}
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	// メッシュがnullでない場合
	if (mMesh)
	{
		// ワールド変換をセット
		shader->SetMatrixUniform("uWorldTransform",
			mOwner->GetWorldTransform());
		// 行列パレットをセット    
		shader->SetMatrixUniforms("uMatrixPalette", &mPalette.mEntry[0],
			MAX_SKELETON_BONES);
		// スペキュラー強度をセット
		shader->SetFloatUniform("uSpecPower", 100);
		// テクスチャをセット 
		Texture* t = mMesh->GetTexture(mOwner);
		if (t)
		{
			t->SetActive();
		}
		// メッシュの頂点配列をアクティブに
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// 描画
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void SkeletalMeshComponent::Update()
{
	if (mAnimation && mSkeleton)
	{
		float deltaTime = System::GetInstance().GetDeltaTime();
		mAnimTime += deltaTime * mAnimPlayRate;
		// アニメを巻き戻して再生
		while (mAnimTime > mAnimation->GetDuration())
		{
			mAnimTime -= mAnimation->GetDuration();
		}

		// 行列パレットの再計算
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate)
{
	// アニメーション設定
	mAnimation = anim;

	// タイマー初期化
	mAnimTime = 0.0f;

	// 再生速度設定
	mAnimPlayRate = playRate;

	if (!mAnimation)
	{
		return 0.0f;
	}

	// 行列計算
	ComputeMatrixPalette();

	// 再生時間を返却
	return mAnimation->GetDuration();
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();   // グローバル逆バインド行列配列の取得
	std::vector<Matrix4> currentPoses;                                         // 現在のポーズ行列
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);       // アニメ時刻時点のグローバルポーズの取得

	// それぞれのボーンの行列パレットのセット
	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		// 行列パレット[i] = グローバル逆バインド行列[i]　×　現在のポーズ行列[i]  (iはボーンID)         
		mPalette.mEntry[i] = globalInvBindPoses[i] * currentPoses[i];
	}
}
