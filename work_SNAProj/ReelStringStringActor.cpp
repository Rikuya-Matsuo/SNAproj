#include "ReelStringStringActor.h"
#include "NAReelString.h"
#include "ReelStringEdgeActor.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "System.h"
#include "Player.h"
#include <cmath>

ReelStringStringActor::ReelStringStringActor(NAReelString * ninjaArts, ReelStringEdgeActor * edge):
	mNinjaArts(ninjaArts),
	mEdge(edge)
{
	SetPriority(mEdge->GetPriority() + 50);

	Mesh * msh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	msh->LoadTexture("Assets/ReelString.png", System::GetInstance().GetRenderer(), this);

	MeshComponent* meshCmp = new MeshComponent(this, 100, false);
	meshCmp->SetMesh(msh);

	SetFixScaleFlag(false, false, true);

	SetFixedScale(Vector3D(0.0f, 0.0f, 10.0f));

	SetAffectGravityFlag(false);

	SetActive(false);
}

void ReelStringStringActor::UpdateActor0()
{
	if (!mEdge->GetActiveFlag())
	{
		SetActive(false);
	}

	Player * player = mNinjaArts->GetUserPlayer();

	// プレイヤーから見たEdgeのx座標の差
	float distance = mEdge->GetPosition().x - player->GetPosition().x;

	SetPosition(player->GetPosition());
	mPosition.x += distance / 2.0f;

	SetScale(fabsf(distance));
}
