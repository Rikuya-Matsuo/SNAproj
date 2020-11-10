#include "BGObject.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

BGObject::BGObject(const std::string & meshFileName):
	mSucessToLoadFlag(false)
{
	// メッシュコンポーネントのロード
	MeshComponent * mc = new MeshComponent(this, 0);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh(meshFileName, this);
	if (msh)
	{
		mc->SetMesh(msh);

		// オブジェクトのサイズを計算
		AABB box = msh->GetCollisionBox();
		mModelSize = box.mMax - box.mMin;
	}

	// 重力の不適用と、当たり判定時の押し返され無効を設定
	mFlags &= ~(mAffectGravityFlagMask_Base | mMovalFlagMask_Base);
}

BGObject::~BGObject()
{
}
