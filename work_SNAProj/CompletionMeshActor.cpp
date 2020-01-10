#include "CompletionMeshActor.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"

const Uint8 CompletionMeshActor::mFlipXFlagMask = 1 << 0;
const Uint8 CompletionMeshActor::mFlipYFlagMask = 1 << 1;
const Uint8 CompletionMeshActor::mNowFlippingFlagMask = 1 << 2;

CompletionMeshActor::CompletionMeshActor(const Actor * owner, int drawOrder):
	mOwner(owner),
	mCurrentIndex(-1),
	mPositionOffset(Vector3D::zero),
	mFlipFlag(0)
{
	mRotationAxis = mOwner->GetRotationAxis();

	mMeshComponent = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	// �e�X�g�p�ɓK���ȃA�j���[�V���������[�h
	bool success = mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.3f, 0);
	if (!success)
	{
		SDL_Log("Graph for test load failed\n");
	}
	mAnimIndexList.emplace_back(0);
	mMesh->SetAnimModeFlag(true);
	mMesh->SetAnimIndex(this, 0);
	mMeshComponent->SetMesh(mMesh);
	//mMeshComponent->SetActive(false);

	// �������̐ݒ�
	// ���L�҂�����łȂ���΂Ȃ�Ȃ�
	SetPriority(mOwner->GetPriority() + 50);

	AdaptPosition();
}

CompletionMeshActor::~CompletionMeshActor()
{
	std::list<int>().swap(mAnimIndexList);
}

void CompletionMeshActor::LoadAnimation(const std::string & filePath, Renderer * renderer, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex)
{
	auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), animIndex);
	if (itr != mAnimIndexList.end())
	{
		mAnimIndexList.emplace_back(animIndex);
	}
	else
	{
		SDL_Log("Fail to load Animation : Specified index has already been used.");
		return;
	}

	mMesh->LoadDivTexture(filePath, renderer, this, allNum, xNum, yNum, chipW, chipH, secondPerFrame, animIndex);
}

void CompletionMeshActor::SetPositionOffset(const Vector3D & offset)
{
	mPositionOffset = offset;

	// �t���O���u���]���Ă��Ȃ��v��Ԃɏ�����
	mFlipFlag &= ~mNowFlippingFlagMask;

	AdaptPosition();
}

void CompletionMeshActor::FlipPositionOffset()
{
	if (mFlipFlag & mFlipXFlagMask)
	{
		mPositionOffset.x *= -1;
	}
	
	if (mFlipFlag & mFlipYFlagMask)
	{
		mPositionOffset.z *= -1;
	}

	Uint8 flipping = ((mFlipFlag & mNowFlippingFlagMask) ^ mNowFlippingFlagMask);		// ���]��̃r�b�g�̒l
	Uint8 flipDir = (mFlipFlag & (mFlipXFlagMask | mFlipYFlagMask));					// ���]�����t���O�𒊏o
	mFlipFlag = flipping | flipDir;
}

void CompletionMeshActor::UpdateActor0()
{
	UpdateTransformData();

	// �C���f�b�N�X�������B�q�b�g����΃��b�V���R���|�[�l���g���A�N�e�B�u�ɁA���Ȃ���Δ�A�N�e�B�u�ɁB
	auto itr = std::find(mAnimIndexList.begin(), mAnimIndexList.end(), mCurrentIndex);
	bool activeFlag = (itr != mAnimIndexList.end());
	mMeshComponent->SetActive(activeFlag);
}

void CompletionMeshActor::UpdateActor1()
{
	UpdateTransformData();
}

void CompletionMeshActor::UpdateTransformData()
{
	bool scaleFlag = mScale != mOwner->GetScale();
	bool rotaFlag = mRotationAngle != mOwner->GetRotationAngle();
	bool posFlag = mPosition != (mOwner->GetPosition() + mPositionOffset);

	if (scaleFlag)
	{
		mScale = mOwner->GetScale();

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	if (rotaFlag)
	{
		mRotationAngle = mOwner->GetRotationAngle();

		mRotation = Quaternion(mOwner->GetRotationAxis(), mRotationAngle);

		mFlags |= mCalculateTransformFlagMask_Base;
	}

	if (posFlag)
	{
		AdaptPosition();

		mFlags |= mCalculateTransformFlagMask_Base;
	}
}
