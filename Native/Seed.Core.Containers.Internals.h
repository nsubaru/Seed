#pragma once
#include "Seed.Core.Base.h"

namespace std {
	///<summary>Список ініціалізації</summary>
	template<class _Elem>
	class initializer_list
	{	// list of pointers to elements
	public:
		typedef _Elem value_type;
		typedef const _Elem& reference;
		typedef const _Elem& const_reference;
		typedef size_t size_type;

		typedef const _Elem* iterator;
		typedef const _Elem* const_iterator;

		constexpr initializer_list() noexcept
			: _First(nullptr), _Last(nullptr)
		{	// empty list
		}

		constexpr initializer_list(const _Elem *_First_arg,
			const _Elem *_Last_arg) noexcept
			: _First(_First_arg), _Last(_Last_arg)
		{	// construct with pointers
		}

		[[nodiscard]] constexpr const _Elem * begin() const noexcept
		{	// get beginning of list
			return (_First);
		}

		[[nodiscard]] constexpr const _Elem * end() const noexcept
		{	// get end of list
			return (_Last);
		}

		[[nodiscard]] constexpr size_t size() const noexcept
		{	// get length of list
			return (static_cast<size_t>(_Last - _First));
		}

	private:
		const _Elem *_First;
		const _Elem *_Last;
	};

	// FUNCTION TEMPLATE begin
	template<class _Elem>
	[[nodiscard]] constexpr const _Elem * begin(initializer_list<_Elem> _Ilist) noexcept
	{	// get beginning of sequence
		return (_Ilist.begin());
	}

	// FUNCTION TEMPLATE end
	template<class _Elem>
	[[nodiscard]] constexpr const _Elem * end(initializer_list<_Elem> _Ilist) noexcept
	{	// get end of sequence
		return (_Ilist.end());
	}

	///<summary>Дефайн для імплементація конструктора\оператора копіювання\переміщення та деструктора</summary>
	///<param name = 'ClassName'>Ім'я классу</summary>
	///<param name = 'This'>Покажчик на екземпляр классу(this)</param>
	///<param name = '...'>Операції при знищенні</param>
#define CopyAndMoveSemantic(ClassName, This, ...) \
ClassName(const ClassName & Orig) :This(Orig.This) { ++This->RefCount; }; \
ClassName(ClassName && From) : This(From.This) { From.This = nullptr; }; \
ClassName & operator=(const ClassName & Orig){this->~ClassName(); this->ClassName::ClassName(Orig); return *this; }; \
ClassName & operator=(ClassName && From){if(this == &From) return From; this->~ClassName(); this->ClassName::ClassName(std::move(From)); return *this; }; \
~ClassName(){if (This != nullptr) {if (--This->RefCount == 0) {__VA_ARGS__ ;delete This;}This = nullptr;}}
}

namespace Seed {
	inline namespace Core {
		///<summary>Список ініціалізації</summary>
		namespace {
			template<typename T>
			using InitilizerList = std::initializer_list;
		}
	}
}