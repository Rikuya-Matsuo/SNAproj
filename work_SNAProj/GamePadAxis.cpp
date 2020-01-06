#include "GamePadAxis.h"
#include "Common.h"
#include <climits>
#include <cmath>

GamePadAxis::GamePadAxis():
	x(0),
	y(0)
{
}

// 符号付16ビットの値を-1.0～1.0の、相対的な値で表すラムダ式
// cppでの定義なので他クラスでは使えない
auto GetRelativeValue = [](Sint16 val)
{
	float floatAbsVal = static_cast<float>(fabsf(val));

	// valが正ならshort型最大値を、負なら最小値を代入
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
