#include "BGObject.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

BGObject::BGObject(const std::string & meshFileName):
	mSucessToLoadFlag(false)
{
	MeshComponent * mc = new MeshComponent(this, 0);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh(meshFileName, this);
	if (msh)
	{
		mc->SetMesh(msh);

		// オブジェクトのサイズを計算
		AABB box = msh->GetCollisionBox();
		mModelSize = box.mMax - box.mMin;
	}

	mFlags &= ~(mAffectGravityFlagMask | mMovalFlagMask);
}

BGObject::~BGObject()
{
}
