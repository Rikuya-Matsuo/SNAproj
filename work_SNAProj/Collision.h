#pragma once
#include "Vector.h"
#include "Common.h"

struct AABB
{
	AABB();
	AABB(const Vector3D & min, const Vector3D & max);

	Vector3D mMin;
	Vector3D mMax;

	void InitMinMax(const Vector3D & point) { mMin = mMax = point; }

	// �p�̍��W���X�V
	// ���͂��ꂽ�p�̍��W�ƍ��̍ő�ŏ��̊p�̍��W���r���āA�l���X�V����
	void RenewalMinMax(const Vector3D & point);

};
