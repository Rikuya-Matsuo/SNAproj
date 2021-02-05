#include "Block.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"
#include "PhysicManager.h"
#include "Stage.h"

const float Block::mModelSize = 100.0f;

Block::Block(const std::string & texturePath, bool isGroundBlock):
	Actor(),
	mPrevInCameraFlag(true)
{
	// メッシュのロード
	mMeshComponent = new MeshComponent(this, 100);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Cube.gpmesh", this);
	msh->LoadTexture(texturePath, System::GetInstance().GetRenderer(), this);
	if (!msh)
	{
		SDL_Log("ブロック：メッシュの読み込みに失敗\n");
	}

	mMeshComponent->SetMesh(msh);

	// コリジョンの設定
	AABB box = msh->GetCollisionBox();
	mCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	// 一番下のコリジョンは、念のため下方向に深く大きくする
	if (isGroundBlock)
	{
		Vector3D boxSize = box.mMax - box.mMin;
		box.mMin.z -= boxSize.z * 3.0f;
	}
	mCollider->SetObjectBox(box);

	// 重力落下を行わない。ヒット時に押し返されない。
	mFlags &= ~(mAffectGravityFlagMask_Base | mMovalFlagMask_Base);
}

Block::~Block()
{
}

void Block::UpdateActor0()
{
	if (mPrevInCameraFlag == GetInCameraFlag())
	{
		return;
	}

	// カメラに映っていない場合、プレイヤー及び検知装置との接触を無視する
	if (!GetInCameraFlag())
	{
		PhysicManager::GetInstance().ResisterHitIgnoreAttribute(this, ColliderAttribute::ColAtt_Player);

		PhysicManager::GetInstance().ResisterHitIgnoreAttribute(this, ColliderAttribute::ColAtt_Detector);
	}
	// 映っている場合、当然無視を解除
	else
	{
		PhysicManager::GetInstance().DeresisterHitIgnoreAttribute(this, ColliderAttribute::ColAtt_Player);

		PhysicManager::GetInstance().DeresisterHitIgnoreAttribute(this, ColliderAttribute::ColAtt_Detector);
	}
}

void Block::UpdateActor1()
{
	mPrevInCameraFlag = GetInCameraFlag();
}

void Block::SetPositionOnStage(Uint16 x, Uint16 y)
{
	mPositionOnStage.mX = x;
	mPositionOnStage.mY = y;
}

void Block::VanishFromStage() const
{
	if (mBelongStage)
	{
		mBelongStage->ChangeToEmptyBlock(mPositionOnStage.mX, mPositionOnStage.mY);
	}
}
