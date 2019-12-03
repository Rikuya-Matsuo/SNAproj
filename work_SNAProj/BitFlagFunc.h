#pragma once

namespace BitFlagFunc
{
	template<typename BitFlag>
	void SetFlagByBool(bool value, BitFlag& flags, const BitFlag mask)
	{
		(value) ? flags |= mask : flags &= ~mask;
	}
}
