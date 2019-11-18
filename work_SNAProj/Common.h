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
	float Larger(float a, float b)
	{
		return (a > b) ? a : b;
	}

	// 値が小さい方を返す
	float Smaller(float a, float b)
	{
		return (!(a > b)) ? a : b;
	}
}
