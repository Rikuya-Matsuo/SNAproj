﻿#include "Floor.h"
#include "System.h"
#include "MeshComponent.h"
#include "Mesh.h"

Floor::Floor(const std::string & texturePath)
{
	// メッシュのロード
	MeshComponent * mc = new MeshComponent(this, 0);
	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	bool texLoadSucess = msh->LoadTexture(texturePath, System::GetInstance().GetRenderer(), this);
	if (!texLoadSucess)
	{
		SDL_Log("テクスチャのロードに失敗\n");
	}

	if (!mc)
	{
		SDL_Log("メッシュのロードに失敗\n");
	}
	else
	{
		mc->SetMesh(msh);

		// 視野の範囲外にあっても描画するフラグを有効にする
		mc->SetRangeOutDrawFlag(true);
	}

	// 回転
	mRotation = Quaternion(Vector3D(1, 0, 0), static_cast<float>(M_PI / 2.0f));

	// フラグ調整
	mFlags &= ~(mAffectGravityFlagMask_Base | mMovalFlagMask_Base);
}


Floor::~Floor()
{
}
