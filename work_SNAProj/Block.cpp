#include "Block.h"
#include "System.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"

const float Block::mModelSize = 100.0f;

Block::Block(const std::string & texturePath, bool isGroundBlock):
	Actor()
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
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Block);
	// 一番下のコリジョンは、念のため下方向に深く大きくする
	if (isGroundBlock)
	{
		Vector3D boxSize = box.mMax - box.mMin;
		box.mMin.z -= boxSize.z * 3.0f;
	}
	bcc->SetObjectBox(box);

	// 重力落下を行わない。ヒット時に押し返されない。
	mFlags &= ~(mAffectGravityFlagMask_Base | mMovalFlagMask_Base);
}

Block::~Block()
{
}
