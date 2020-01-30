// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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
const MeshComponent::FlagType MeshComponent::mUIFlagMask = 1 << 4;

// メッシュコンポーネント　ownerとスキンメッシュかの情報入れる
MeshComponent::MeshComponent(Actor* owner, int drawOrder, bool isSkeletal, bool uiFlag)
	:ComponentBase(owner)
	, mMesh(nullptr)
	, mDrawOrder(drawOrder)
	, mTextureIndex(0)
	, mMeshCompFlags(mVisibleFlagMask | (isSkeletal ? mIsSkeletalFlagMask : 0))
{
	if (uiFlag && !isSkeletal)
	{
		mMeshCompFlags |= mUIFlagMask;
		System::GetInstance().GetRenderer()->AddUI(this);
	}
	else
	{
		System::GetInstance().GetRenderer()->AddMeshComponent(this);
	}
	//printf("new MeshComponent : [%5d] owner->( 0x%p )\n", GetID(), owner);
}

MeshComponent::~MeshComponent()
{
	if (mMesh)
	{
		mMesh->DeleteActorInfo(mOwner);
	}

	if (mMeshCompFlags & mUIFlagMask)
	{
		System::GetInstance().GetRenderer()->RemoveUI(this);
	}
	else
	{
		//printf("remove MeshComponent : [%5d] owner->( 0x%p )\n", GetID(), mOwner);
		System::GetInstance().GetRenderer()->RemoveMeshComponent(this);
	}
}

void MeshComponent::Update()
{
	BitFlagFunc::SetFlagByBool(mOwner->GetVisibleFlag(), mMeshCompFlags, mVisibleFlagMask);

	if (mMeshCompFlags & mAnimationModeFlagMask)
	{
		mMesh->Update(mOwner);
	}
}

void MeshComponent::Draw(Shader* shader)
{
	if ((mOwner->GetActiveFlag()) && (mMeshCompFlags & mVisibleFlagMask) && mMesh)
	{
		static bool forUISetting = false;
		if (mMeshCompFlags & mUIFlagMask)
		{
			if (!forUISetting)
			{
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);

				//glOrtho(0, System::GetInstance().GetRenderer()->GetScreenWidth(), 0, System::GetInstance().GetRenderer()->GetScreenHeight(), -1000, 1000);
				forUISetting = true;
			}
		}
		else
		{
			static bool forBoardSetting = false;
			bool isBoard = mMesh->GetIsBoardFlag();
			// ボード描画用の設定ではなく、ボードのメッシュであるとき
			if (!forBoardSetting && isBoard)
			{
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_BLEND);

				//glMatrixMode(GL_PROJECTION);

				glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

				forBoardSetting = true;
			}
			// ボード描画用の設定で、ボードではないとき
			else if (forBoardSetting && !isBoard)
			{
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);

				//glMatrixMode(GL_PROJECTION);

				forBoardSetting = false;
			}
			forUISetting = false;
		}

		if (!(mMeshCompFlags & mUIFlagMask))
		{
			// Set the world transform　ワールド変換をセット
			shader->SetMatrixUniform("uWorldTransform",
				mOwner->GetWorldTransform());
			// Set specular power　スペキュラ強度セット
			shader->SetFloatUniform("uSpecPower", 100);
			// Set the active texture　アクティブテクスチャセット
			Texture* t = mMesh->GetTexture(mOwner);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active　頂点配列をアクティブに
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw　描画するー
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			Texture * t = mMesh->GetTexture(mOwner);
			Matrix4 scaleMat = Matrix4::CreateScale(
				mOwner->GetScale(),
				1.0f,
				mOwner->GetScale());

			Vector3D v = mOwner->GetPosition();
			v.y = 0.0f;
			Matrix4 transMat = Matrix4::CreateTranslation(v);

			Matrix4 world = scaleMat * transMat;

			// Set the world transform　ワールド変換をセット
			shader->SetMatrixUniform("uWorldTransform",
				world);

			t->SetActive();
			// Set the mesh's vertex array as active　頂点配列をアクティブに
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw　描画するー
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

void MeshComponent::SetMesh(Mesh * mesh)
{
	mMesh = mesh;

	BitFlagFunc::SetFlagByBool(mMesh->GetAnimModeFlag(), mMeshCompFlags, mAnimationModeFlagMask);
}
