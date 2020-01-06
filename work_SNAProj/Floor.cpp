#include "Floor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

Floor::Floor(const std::string & texturePath)
{
	MeshComponent * mc = new MeshComponent(this, 0);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	bool texLoadSucess = msh->LoadTexture(texturePath, System::GetInstance().GetRenderer(), this);
	if (!texLoadSucess)
	{
		SDL_Log("�e�N�X�`���̃��[�h�Ɏ��s\n");
	}

	if (!mc)
	{
		SDL_Log("���b�V���̃��[�h�Ɏ��s\n");
	}
	else
	{
		mc->SetMesh(msh);
	}

	// ��]
	mRotation = Quaternion(Vector3D(1, 0, 0), static_cast<float>(M_PI / 2.0f));

	// �t���O����
	mFlags &= ~(mAffectGravityFlagMask | mMovalFlagMask);
}


Floor::~Floor()
{
}
