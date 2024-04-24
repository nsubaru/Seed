#pragma once
#include "Seed.Core.Threading.Funcionality.Base.h"

namespace Seed::Core::Functionality
{
	template<typename T> requires (!Concepts::IsSame<T, ExceptionHolderType>)
	template<typename TLambda> 
	inline NoDiscard Threading::BaseTask<Result<typename Result<T>::AsyncRetType<TLambda>>, void> Seed::Core::Functionality::Result<T>::OnSuccess(TLambda&& lambda)&& 
		requires (!Concepts::Void<T> && (Concepts::IsAsyncInvocable<TLambda, typename Result<T>::EitherRightType> || Concepts::IsAsyncInvocable<TLambda, typename Result<T>::EitherRightType&&>))
	{
		using U = AsyncRetType<TLambda>;

		using Type = Result<U>;

		try
		{
			if (data.HoldedTypeNumber == 0)
			{
				co_return Type(MoveRValue(data).template GetAs<ExceptionHolderType>());
			}
			else
			{
				if constexpr (Concepts::IsSame<TypeTraits::AsyncInvocableResult<TLambda, T>, void>)
				{
					co_await lambda(MoveRValue(data).template GetAs<T>());
					co_return Type();
				}
				else
				{
					co_return Type(co_await lambda(MoveRValue(data).template GetAs<T>()));
				}
			}
		}
		catch (Exception& ex)
		{
			co_return Type(ex);
		}
	}

	template<typename T> requires (!Concepts::IsSame<T, ExceptionHolderType>)
	template<typename TLambda>
	NoDiscard Threading::Task<Result<typename Result<T>::AsyncRetType<TLambda>>> Result<T>::OnSuccess(TLambda&& lambda) && requires (Concepts::Void<T>&& Concepts::IsAsyncInvocable<TLambda>)
	{
		using U = AsyncRetType<TLambda>;

		using Type = Result<U>;

		try
		{
			if (data.HoldedTypeNumber == 0)
			{
				co_return Type(MoveRValue(data).template GetAs<ExceptionHolderType>());
			}
			else
			{
				if constexpr (Concepts::IsSame<TypeTraits::AsyncInvocableResult<TLambda>, void>)
				{
					co_await lambda();
					co_return Type();
				}
				else
				{
					co_return Type(co_await lambda());
				}
			}
		}
		catch (Exception& ex)
		{
			co_return Type(ex);
		}
	}

	template<typename T> requires (!Concepts::IsSame<T, ExceptionHolderType>)
	template<typename TLambda>
	requires
		(Concepts::IsAsyncInvocable<TLambda, ExceptionHolderType> ||
		Concepts::IsAsyncInvocable<TLambda, ExceptionHolderType&> ||
		Concepts::IsAsyncInvocable<TLambda, const ExceptionHolderType&>)
	NoDiscard auto Result<T>::OnFail(TLambda&& lambda)&& -> Threading::Task<typename Result<T>::AsyncOnFailResult<TLambda>>
	{
		if constexpr (Concepts::IsSame<T, AsyncExceptionRetType<TLambda>>)
		{
			if (data.HoldedTypeNumber == 1)
			{
				if constexpr (Concepts::Void<T>)
				{
					co_return;
				}
				else
				{
					co_return MoveRValue(*this).GetSuccessResult();
				}
			}
			else
			{
				co_return co_await lambda(data.template GetAs<ExceptionHolderType>());

			}
		}
		else if constexpr (Concepts::NotVoid<AsyncExceptionRetType<TLambda>>)
		{
			if (data.HoldedTypeNumber == 1)
			{
				co_return Either<T, AsyncExceptionRetType>(MoveRValue(*this).GetSuccessResult());
			}
			else
			{
				co_return Either<T, AsyncExceptionRetType>(co_await lambda(data.template GetAs<ExceptionHolderType>()));
			}
		}
		else
		{
			if (data.HoldedTypeNumber == 1)
			{
				co_return MoveRValue(*this);
			}
			else
			{
				co_await lambda(data.template GetAs<ExceptionHolderType>());
				co_return MoveRValue(*this);
			}
		}
	}

	template<typename T> requires (!Concepts::IsSame<T, ExceptionHolderType>)
	template<typename TLambda>
		requires Concepts::IsAsyncInvocable<TLambda>
	NoDiscard Threading::Task<Result<T>> Result<T>::OnFail(TLambda&& lambda)&&
	{
		if (data.HoldedTypeNumber == 1)
		{
			co_return MoveRValue(*this);
		}
		else
		{
			co_await lambda();
			co_return MoveRValue(*this);
		}
	}
}