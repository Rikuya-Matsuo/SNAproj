#pragma once
#include <vector>
#include <list>

namespace Common
{
	// 汎用性に問題を発見。削除対象のデストラクタにコンテナからの削除機能がなければエラーとなる。
	template <class T>
	void DeleteContainerOfPointer(std::vector<T*> & in_vec)
	{
		while (!in_vec.empty())
		{
			if (in_vec.back() != nullptr)
			{
				delete in_vec.back();
			}
			else
			{
				in_vec.pop_back();
			}
		}
	}

	// 汎用性に問題を発見。削除対象のデストラクタにコンテナからの削除機能がなければエラーとなる。
	template <class T>
	void DeleteContainerOfPointer(std::list<T*> & in_lst)
	{
		while (!in_lst.empty())
		{
			if (in_lst.back() != nullptr)
			{
				delete in_lst.back();
			}
			else
			{
				in_lst.pop_back();
			}
		}
	}

	// 値が大きい方を返す
	static float Larger(float a, float b)
	{
		return (a > b) ? a : b;
	}

	// 値が小さい方を返す
	static float Smaller(float a, float b)
	{
		return (!(a > b)) ? a : b;
	}

	template <typename T>
	static T Clamp(T val, T min, T max)
	{
		if (val < min)
		{
			val = min;
		}
		else if (val > max)
		{
			val = max;
		}

		return val;
	}

	static float DegreesFix(float degrees)
	{
		if (degrees > 360.000f)
		{
			int num = static_cast<int>(degrees) / 360;
			degrees -= 360.000f * num;
		}
		else if (degrees < 0.00f)
		{
			degrees = 360.000f + degrees;
		}

		return degrees;
	}

	static float DegToRad(float degrees)
	{
		degrees = DegreesFix(degrees);

		const float pi = static_cast<float>(M_PI);
		return (pi / 180.000f) * degrees;
	}
}
