#include "FriableBlock.h"
#include "System.h"
#include "BoxColliderComponent.h"

FriableBlock::FriableBlock(const std::string & texturePath, bool isGroundFlag):
	Block(texturePath, isGroundFlag)
{
	mCollider->ChangeAttribute(ColliderAttribute::ColAtt_FriableBlock);
}

FriableBlock::~FriableBlock()
{
}
