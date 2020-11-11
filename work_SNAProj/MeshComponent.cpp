#include "MeshComponent.h"
#include "System.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <typeinfo>

const MeshComponent::FlagType MeshComponent::mVisibleFlagMask = 1 << 0;
const MeshComponent::FlagType MeshComponent::mIsSkeletalFlagMask = 1 << 1;
const MeshComponent::FlagType MeshComponent::mAnimationModeFlagMask = 1 << 2;
const MeshComponent::FlagType MeshComponent::mRangeOutDrawFlagMask = 1 << 3;

Vector3D MeshComponent::mCamOffset = Vector3D::zero;

// メッシュコンポーネント : ownerとスキンメッシュかの情報入れる
MeshComponent::MeshComponent(Actor* owner, int drawOrder, bool isSkeletal)
	:ComponentBase(owner)
	, mMesh(nullptr)
	, mDrawOrder(drawOrder)
	, mTextureIndex(0)
	, mMeshCompFlags(mVisibleFlagMask | (isSkeletal ? mIsSkeletalFlagMask : 0))
{
	// レンダラーに自身を登録
	System::GetInstance().GetRenderer()->AddMeshComponent(this);
}

MeshComponent::~MeshComponent()
{
	// メッシュからアクター情報を削除
	if (mMesh)
	{
		mMesh->DeleteActorInfo(mOwner);
	}

	// レンダラーから自身を登録解除
	System::GetInstance().GetRenderer()->RemoveMeshComponent(this);
}

void MeshComponent::Update()
{
	// 描画フラグをアクターに合わせる
	BitFlagFunc::SetFlagByBool(mOwner->GetVisibleFlag(), mMeshCompFlags, mVisibleFlagMask);

	// アニメーションチップが設定されている場合、更新する
	if (mMeshCompFlags & mAnimationModeFlagMask)
	{
		mMesh->Update(mOwner);
	}
}

void MeshComponent::Draw(Shader* shader)
{
	// アクターの描画フラグと、自身の描画フラグが真でかつ、メッシュがnullでない場合
	if ((mOwner->GetActiveFlag()) && (mMeshCompFlags & mVisibleFlagMask) && mMesh)
	{
		{
			static bool forBoardSetting = false;
			bool isBoard = mMesh->GetIsBoardFlag();
			// ボード描画用の設定ではなく、ボードのメッシュであるとき
			if (!forBoardSetting && isBoard)
			{
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);

				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

				forBoardSetting = true;
			}
			// ボード描画用の設定で、ボードではないとき
			else if (forBoardSetting && !isBoard)
			{
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);

				forBoardSetting = false;
			}
		}

		{
			// ワールド変換をセット
			shader->SetMatrixUniform("uWorldTransform",
				mOwner->GetWorldTransform());
			// スペキュラ強度セット
			shader->SetFloatUniform("uSpecPower", 100);
			// アクティブテクスチャセット
			Texture* t = mMesh->GetTexture(mOwner);
			if (t)
			{
				t->SetActive();
			}
			// 頂点配列をアクティブに
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// 描画する
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
		
	}
}

void MeshComponent::SetMesh(Mesh * mesh)
{
	// 変数にセット
	mMesh = mesh;

	// メッシュにアニメーションチップが設定されているかをフラグに記録
	BitFlagFunc::SetFlagByBool(mMesh->GetAnimModeFlag(), mMeshCompFlags, mAnimationModeFlagMask);
}
