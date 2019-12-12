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

// メッシュコンポーネント　ownerとスキンメッシュかの情報入れる
MeshComponent::MeshComponent(Actor* owner, bool isSkeletal)
	:ComponentBase(owner)
	, mMesh(nullptr)
	, mTextureIndex(0)
	, mMeshCompFlags(mVisibleFlagMask | (isSkeletal ? mIsSkeletalFlagMask : 0))
{
	System::GetInstance().GetRenderer()->AddMeshComponent(this);
	//printf("new MeshComponent : [%5d] owner->( 0x%p )\n", GetID(), owner);
}

MeshComponent::~MeshComponent()
{
	//printf("remove MeshComponent : [%5d] owner->( 0x%p )\n", GetID(), mOwner);
	System::GetInstance().GetRenderer()->RemoveMeshComponent(this);
}

void MeshComponent::Update()
{
	if (mMeshCompFlags & mAnimationModeFlagMask)
	{
		mMesh->Update();
	}
}

void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform　ワールド変換をセット
		shader->SetMatrixUniform("uWorldTransform",
			mOwner->GetWorldTransform());
		// Set specular power　スペキュラ強度セット
		shader->SetFloatUniform("uSpecPower", 100);
		// Set the active texture　アクティブテクスチャセット
		Texture* t = mMesh->GetTexture();
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
}

void MeshComponent::SetMesh(Mesh * mesh)
{
	mMesh = mesh;

	BitFlagFunc::SetFlagByBool(mMesh->GetAnimModeFlag(), mMeshCompFlags, mAnimationModeFlagMask);
}
