#include "BGObject.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

BGObject::BGObject(const std::string & meshFileName):
	mSucessToLoadFlag(false)
{
	MeshComponent * mc = new MeshComponent(this);
	Mesh * msh = new Mesh;
	mSucessToLoadFlag = msh->Load(meshFileName, System::GetInstance().GetRenderer(), this);
	if (mSucessToLoadFlag)
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
