#include "Bomb.h"
#include "BoxColliderComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
Bomb::Bomb():
	Block("Assets/Bomb.png", false)
{
	// �������A���g�̃R���W���������̔����͈͂������邩
	float explosionRangeRate = 4.0f;

	AABB box = mMeshComponent->GetMesh()->GetCollisionBox();
	box.mMin *= explosionRangeRate / 2.0f;
	box.mMax *= explosionRangeRate / 2.0f;

	mExplosionCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mExplosionCollider->SetActive(false);
}

Bomb::~Bomb()
{
}

void Bomb::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{

}
