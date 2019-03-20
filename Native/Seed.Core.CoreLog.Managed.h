#pragma once
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Containers.List.h"
#include "Seed.Core.Containers.List.h"
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.String.h"

namespace Seed {
	inline namespace Core {
		//Вивід в логи складеного повідомлення в логи, кожен елемент масиву окремий рядок
		bool SeedKernelCall toCoreLog(const Array<String> & lines);
		//Вивід в логи рядка з повідомленням
		template<typename Base>
		__forceinline bool SeedKernelCall toCoreLog(const BaseString<Base> & msg)
		{
#pragma warning(push)
#pragma warning(disable: 4996)
			return toCoreLog((Char*)msg, msg.Size);
#pragma warning(pop)
		}

		template<typename Heap>
		__forceinline bool SeedKernelCall toCoreLog(const BaseString<Heap> & Msg, const Seed::Exception & ex)
		{
			auto location = ex.Location;
			return Seed::Core::toCoreLog({
				Seed::Core::String(Msg),
				Seed::Core::String(L"Cathed exeption in thread with TID	: ") + Seed::Core::String::toString(location.ThreadID),
				Seed::Core::String(L"        Exeption name				: ") + ex.Name,
				Seed::Core::String(L"        Exeption location			: "),
				Seed::Core::String(L"                 File				: ") + (Seed::ACharStr)location.File, //-V2005
				Seed::Core::String(L"                 In method			: ") + (Seed::ACharStr)location.FunctName, //-V2005
				Seed::Core::String(L"                 On exec line		: ") + Seed::Core::String::toString(location.Line) });
		}

		template<SizeT Lenght>
		__forceinline bool SeedKernelCall toCoreLog(const wchar_t(&Msg)[Lenght], const Seed::Exception & ex)
		{
			auto location = ex.Location;
			return Seed::Core::toCoreLog({
				Seed::Core::String(Msg),
				Seed::Core::String(L"Cathed exeption in thread with TID	: ") + Seed::Core::String::toString(location.ThreadID),
				Seed::Core::String(L"        Exeption name				: ") + ex.Name,
				Seed::Core::String(L"        Exeption location			: "),
				Seed::Core::String(L"                 File				: ") + (Seed::ACharStr)location.File, //-V2005
				Seed::Core::String(L"                 In method			: ") + (Seed::ACharStr)location.FunctName, //-V2005
				Seed::Core::String(L"                 On exec line		: ") + Seed::Core::String::toString(location.Line) });
		}
	}
}