#pragma once
#include <vector>
#include <list>

namespace Common
{
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
}
