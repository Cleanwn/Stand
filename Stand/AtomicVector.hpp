#pragma once

#include <memory>
#include <vector>

namespace Stand
{
	template <typename T>
	class AtomicVector
	{
	private:
		struct Data
		{
			T* arr;
			const size_t size;
			const unsigned mutation;

			explicit Data(size_t size, unsigned mutation)
				: arr((T*)malloc(sizeof(T) * size)), size(size), mutation(mutation)
			{
			}

			~Data() noexcept
			{
				for (size_t i = 0; i != size; ++i)
				{
					std::destroy_at(&arr[i]);
				}
				free(arr);
			}
		};

		std::atomic<unsigned> mutation;
		std::shared_ptr<Data> data;

	public:
		struct Iterator
		{
			std::shared_ptr<Data> data{};
			size_t i = 0;

			[[nodiscard]] bool isEndIterator() const noexcept
			{
				return !data;
			}

			[[nodiscard]] bool hasReachedEnd() const noexcept
			{
				return i == data->size;
			}

			[[nodiscard]] bool operator ==(const Iterator& b) const noexcept
			{
				if (b.isEndIterator())
				{
					return hasReachedEnd();
				}
				if (isEndIterator())
				{
					return b.hasReachedEnd();
				}
				return data == b.data
					&& i == b.i
					;
			}

			[[nodiscard]] bool operator !=(const Iterator& b) const noexcept
			{
				return !operator==(b);
			}

			[[nodiscard]] T& operator*() const noexcept
			{
				return data->arr[i];
			}

			[[nodiscard]] T* operator->() const noexcept
			{
				return &data->arr[i];
			}

			[[nodiscard]] T* get() const noexcept
			{
				return &data->arr[i];
			}

			Iterator& operator++() noexcept
			{
				++i;
				return *this;
			}

			void operator +=(size_t off) noexcept
			{
				i += off;
			}

			[[nodiscard]] Iterator operator+(size_t off) const noexcept
			{
				Iterator it(*this);
				it += off;
				return it;
			}

			[[nodiscard]] size_t operator-(const Iterator& b) const noexcept
			{
				if (isEndIterator())
				{
					return b.data->size;
				}
				return i - b.i;
			}
		};

		AtomicVector(std::initializer_list<T>&& il)
		{
			operator=(std::move(il));
		}

		void operator =(std::initializer_list<T>&& il)
		{
			if (il.size() == 0)
			{
				clear();
			}
			else
			{
				data = std::make_shared<Data>(il.size(), ++mutation);
				size_t i = 0;
				for (auto&& elm : il)
				{
					std::construct_at<T>(&data->arr[i++], std::move(elm));
				}
			}
		}

		[[nodiscard]] Iterator begin() const noexcept
		{
			return Iterator{ data };
		}

		[[nodiscard]] Iterator cbegin() const noexcept
		{
			return Iterator{ data };
		}

		[[nodiscard]] Iterator end() const noexcept
		{
			return Iterator{};
		}

		[[nodiscard]] Iterator cend() const noexcept
		{
			return Iterator{};
		}

		[[nodiscard]] bool empty() const noexcept
		{
			return !data;
		}

		[[nodiscard]] size_t size() const noexcept
		{
			if (auto d = data; d)
			{
				return d->size;
			}
			return 0;
		}

		void clear() noexcept
		{
			++mutation;
			data.reset();
		}

		T& emplace_back(T&& elm)
		{
			std::shared_ptr<Data> d;
			size_t size;
			unsigned mutation;
			std::shared_ptr<Data> r;
			T* emplaced;
			do
			{
				d = data;
				if (d)
				{
					size = d->size;
					mutation = d->mutation;
				}
				else
				{
					size = 0;
					mutation = this->mutation.load();
				}
				r = std::make_shared<Data>(size + 1, mutation + 1);
				for (size_t i = 0; i != size; ++i)
				{
					std::construct_at(&r->arr[i], d->arr[i]);
				}
				emplaced = std::construct_at(&r->arr[size], std::move(elm));
			} while (!this->mutation.compare_exchange_strong(mutation, r->mutation));
			this->data = std::move(r);
			return *emplaced;
		}

		Iterator erase(const Iterator& it)
		{
			erase(it.get());
			return begin() + it.i;
		}

		void erase(const T* target)
		{
			std::shared_ptr<Data> d;
			std::shared_ptr<Data> r;
			unsigned mutation;
			do
			{
				d = data;
				if (!d)
				{
					// No data, so no items, so nothing to erase
					return;
				}
				if (d->size == 1)
				{
					if (&d->arr[0] == target)
					{
						clear();
					}
					return;
				}
				bool had_target = false;
				r = std::make_shared<Data>(d->size - 1, d->mutation + 1); 
				size_t ri = 0;
				for (size_t di = 0; di != d->size; ++di)
				{
					if (&d->arr[di] == target)
					{
						had_target = true;
					}
					else
					{
						std::construct_at(&r->arr[ri++], d->arr[di]);
					}
				}
				if (!had_target)
				{
					// Data didn't have the erase target, so we can go home early
					return;
				}
			} while (mutation = d->mutation, !this->mutation.compare_exchange_strong(mutation, r->mutation));
			this->data = std::move(r);
		}
	};
}
