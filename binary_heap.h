#if !defined(BINARY_HEAP_H)
#define BINARY_HEAP_H

//
//  Copyright (C) 2020 Pharap (@Pharap)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

// std::forward, std::swap, std::move, std::declval
#include <utility>

// std::vector
#include <vector>

// std::less, std::greater
#include <functional>

template<typename Type, typename Container = std::vector<Type>, typename Compare = std::less<typename Container::value_type>>
class binary_heap;

template<typename Type, typename Container = std::vector<Type>>
using binary_min_heap = binary_heap<Type, Container, std::less<typename Container::value_type>>;

template<typename Type, typename Container = std::vector<Type>>
using binary_max_heap = binary_heap<Type, Container, std::greater<typename Container::value_type>>;

template<typename Type, typename Container, typename Compare>
class binary_heap
{
public:
	using container_type = Container;
	using value_compare = Compare;
	using value_type = typename Container::value_type;
	using size_type = typename Container::size_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;

private:
	container_type container;
	value_compare compare;

public:
	bool empty() const
	{
		return this->container.empty();
	}

	size_type size() const
	{
		return this->container.size();
	}

	reference top()
	{
		return this->container.front();
	}

	const_reference top() const
	{
		return this->container.front();
	}

	void push(const value_type & value)
	{
		this->container.push_back(value);
		this->enforce_heap_property_up(this->container.size() - 1);
	}

	// Strictly speaking the return type ought to be 'void'
	// for C++11 and C++14, and 'reference' for C++17,
	// but a trailing return type is less hassle than
	// any other options
	template<typename ... Args>
	auto emplace(Args && ... args)
		-> decltype(std::declval<container_type>().emplace_back(std::forward<Args>(args)...))
	{
		return this->container.emplace_back(std::forward<Args>(args)...);
	}

	void pop()
	{
		// Note: the fact 'pop_back' is used here is precisely why
		// 'std::vector' is a suitable default container type
		this->container.front() = std::move(this->container.back());
		this->container.pop_back();
		this->enforce_heap_property_down(0);
	}

	void swap(binary_heap & other)
	{
		using std::swap;

		swap(this->container, other.container);
		swap(this->compare, other.compare);
	}

private:
	void enforce_heap_property_up(size_type index)
	{
		using std::swap;

		size_type current_index = index;
		size_type parent_index = get_parent_index(current_index);

		// Note: this implementation is iterative rather than recursive
		while(compare(this->container[current_index], this->container[parent_index]))
		{
			swap(this->container[current_index], this->container[parent_index]);

			current_index = parent_index;
			parent_index = get_parent_index(current_index);
		}
	}

	void enforce_heap_property_down(size_type index)
	{
		using std::swap;

		size_type superlative_index = index;

		// Note: this implementation is iterative rather than recursive
		for(size_type new_superlative_index = superlative_index;;superlative_index = new_superlative_index)
		{
			const size_type left_index = get_left_index(superlative_index);

			if(left_index < this->size())
				if(compare(this->container[left_index], this->container[new_superlative_index]))
					new_superlative_index = left_index;

			const size_type right_index = get_right_index(superlative_index);

			if(right_index < this->size())
				if(compare(this->container[right_index], this->container[new_superlative_index]))
					new_superlative_index = right_index;

			if(new_superlative_index == superlative_index)
				break;

			swap(this->container[superlative_index], this->container[new_superlative_index]);
		}
	}

private:
	constexpr static size_type get_parent_index(size_type index)
	{
		return (index == 0) ? 0 : ((index - 1) / 2);
	}

	constexpr static size_type get_left_index(size_type index)
	{
		return ((index * 2) + 1);
	}

	constexpr static size_type get_right_index(size_type index)
	{
		return ((index * 2) + 2);
	}
};

#endif