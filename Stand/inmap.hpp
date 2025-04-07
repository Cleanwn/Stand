#pragma once

#include "struct_base.hpp"

namespace rage
{
	template< typename K, typename T >
	class inmap_node
	{
	public:
		T* m_right;
		T* m_left;
		T* m_parent;
		K m_key;
	};

	template< typename K, typename T, inmap_node< K, T > T::* NODE, typename P >
	class inmap_base
	{
	public:
		enum
		{
			ALLOW_DUP_KEYS = P::ALLOW_DUP_KEYS
		};

		typedef typename P::Predicate key_compare;

		T* m_root;
		size_t m_size;
		//key_compare m_pred;

		[[nodiscard]] static bool m_pred(const K& a, const K& b)
		{
			return a < b;
		}

		[[nodiscard]] static constexpr T* end()
		{
			return nullptr;
		}

		[[nodiscard]] static T* right(T* t)
		{
			return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>((t->*NODE).m_right) & 0xFFFFFFFFFFFFFFFEui64);
		}

		[[nodiscard]] static T*& left(T* t)
		{
			return (t->*NODE).m_left;
		}

		[[nodiscard]] static K key(T* t)
		{
			return (t->*NODE).m_key;
		}

		// Actually lower_bound, returns an iterator.
		[[nodiscard]] T* lowerBound(const K& k/*, size_t* cost = 0*/)
		{
			T* cur = m_root;
			T* n = nullptr;
			//size_t findCost = 0;

			while (cur)
			{
				if (m_pred(key(cur), k))
				{
					cur = right(cur);
				}
				else
				{
					n = cur;
					cur = left(cur);
				}

				//++findCost;
			}

			/*if (ALLOW_DUP_KEYS && n)
			{
				cur = predecessor(n);

				while (cur
					&& !m_pred(key(cur), k)
					&& !m_pred(k, key(cur)))
				{
					n = cur;
					cur = predecessor(cur);

					++findCost;
				}
			}*/

			//if (cost) *cost = findCost;

			return n;
		}

		[[nodiscard]] T* find(const K& k/*, size_t* cost = 0*/)
		{
			auto it = this->lowerBound(k/*, cost*/);
			return (this->end() == it || m_pred(k, key(it))) ? this->end() : it;
		}
	};

	template< typename PRED >
	struct inmap_policies
	{
		typedef PRED Predicate;
		enum
		{
			ALLOW_DUP_KEYS = false
		};
	};

	template< typename K, typename T, inmap_node< K, T > T::* NODE, typename P = std::less< K > >
	class inmap : public inmap_base< K, T, NODE, inmap_policies< P > >
	{
	};
}
