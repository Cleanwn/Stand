#pragma once

#include "datBase.hpp"

namespace rage
{
	template <typename T, typename Base = datBase>
	class atDNode : public Base
	{
	public:
		T Data;
		atDNode<T, Base>* Next;
		atDNode<T, Base>* Prev;
	};

	template <typename Node>
	class atDList
	{
	public:
		atDNode<Node>* Head;
		atDNode<Node>* Tail;
	};
}
