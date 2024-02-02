#pragma once
#include "Seed.Core.Threading.Extensions.Base.h"

namespace Seed::Core::RTTI::MetaData
{
    template<typename TMethodRecord>
    template<SizeT ... TIdx>
    Threading::BaseTask<Dynamic, void> AsyncMethodCallerGeneratorT<TMethodRecord>::AsyncMethodCallerGeneratorImpl<TypeTraits::Indexator<TIdx...>>::DynamicCallImpl(Dynamic& target, Record::RttiMethodRecord targetDef, Containers::Array<Dynamic>& args)
	{
		using Target = typename TMethodRecord::OriginalClass::Type;

		if constexpr (TypeTraits::IsSameV<typename TMethodRecord::OriginalRet::Type, Threading::BaseTask<void, void>>)
		{
			co_await TMethodRecord::DecayCall(
				target.GetValue<Target>(),
				MoveRValue(
					GetTypeFromDynamicArrayByIndex<(TMethodRecord::OriginalArgsTuple::Count - 1) - TIdx>(targetDef, args))...);

			co_return Dynamic();
		}
		else
		{
			co_return Dynamic(co_await TMethodRecord::DecayCall(
				target.GetValue<Target>(),
				MoveRValue(
					GetTypeFromDynamicArrayByIndex<(TMethodRecord::OriginalArgsTuple::Count - 1) - TIdx>(targetDef, args))...));
		}
	}

	template<typename TMethodRecord>
	Threading::BaseTask<Dynamic, void> AsyncMethodCallerGeneratorT<TMethodRecord>::DynamicCall(Dynamic& target, Record::RttiMethodRecord targetDef, Containers::Array<Dynamic>& args)
	{
		using Target = typename TMethodRecord::OriginalClass::Type;

		if (Record::GetRttiTypeRecord<Target>() != target.Type)
		{
			Throw(Exceptions::DynamicCallInvalidTargetException(SourcePosition, targetDef, Record::GetRttiTypeRecord<Target>(), target.Type));
		}

		if (TMethodRecord::OriginalArgsTuple::Count != args.Count)
		{
			Throw(Exceptions::DynamicCallInvalidArgumentsCountException(SourcePosition, targetDef, args.Count));
		}

		return AsyncMethodCallerGeneratorImpl<typename TypeTraits::Indexer<TMethodRecord::OriginalArgsTuple::Count>::Type>::DynamicCallImpl(target, targetDef, args);
	}
}