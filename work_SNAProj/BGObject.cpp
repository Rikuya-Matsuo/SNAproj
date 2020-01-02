#include "BGObject.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

BGObject::BGObject(const std::string & meshFileName):
	mMeshComponent(nullptr)
{
	mMeshComponent = new MeshComponent(this);
	Mesh * msh = new Mesh;
	bool success = msh->Load(meshFileName, System::GetInstance().GetRenderer(), this);
	if (success)
	{
		mMeshComponent->SetMesh(msh);
	}

	mFlags &= ~(mAffectGravityFlagMask | mMovalFlagMask);
}

BGObject::~BGObject()
{
}
