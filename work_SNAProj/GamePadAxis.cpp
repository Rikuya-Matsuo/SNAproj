#include "GamePadAxis.h"
#include "Common.h"
#include <climits>
#include <cmath>

GamePadAxis::GamePadAxis():
	x(0),
	y(0)
{
}

// �����t16�r�b�g�̒l��-1.0�`1.0�́A���ΓI�Ȓl�ŕ\�������_��
// cpp�ł̒�`�Ȃ̂ő��N���X�ł͎g���Ȃ�
auto GetRelativeValue = [](Sint16 val)
{
	float floatAbsVal = static_cast<float>(fabsf(val));

	// val�����Ȃ�short�^�ő�l���A���Ȃ�ŏ��l����
	Sint16 maxOrMin = ((val >= 0) ? SHRT_MAX : SHRT_MIN);

	return floatAbsVal / maxOrMin;
};

float GamePadAxis::GetFloatX() const
{
	return GetRelativeValue(x);
}

float GamePadAxis::GetFloatY() const
{
	return GetRelativeValue(y);
}
