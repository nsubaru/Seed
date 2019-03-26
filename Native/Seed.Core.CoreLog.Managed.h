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
			return toCoreLog((ConstCharStr)msg, msg.Size);
#pragma warning(pop)
		}

#pragma warning(push)
#pragma warning(disable: 4996)

		template<typename Heap>
		__forceinline bool SeedKernelCall toCoreLog(const BaseString<Heap> & Msg, const Seed::Exception & ex)
		{
			auto location = ex.Location;
			return Seed::Core::toCoreLog({
				Msg,
				L"Перехоплено помилку з потоку з ID: " + Seed::Core::String::toString(location.ThreadID),
				L"        Ім'я помилки	           : " + String((CharStr)ex.Name),
				L"        Розташування помилки	   : ",
				L"                 Файл	           : " + String((Seed::ACharStr)location.File),
				L"                 Метод           : " + String((Seed::ACharStr)location.FunctName),
				L"                 Рядок           : " + Seed::Core::String::toString(location.Line) });
		}

		template<SizeT Lenght>
		__forceinline bool SeedKernelCall toCoreLog(const wchar_t(&Msg)[Lenght], const Seed::Exception & ex)
		{
			auto location = ex.Location;
			return Seed::Core::toCoreLog({
				Seed::Core::String(Msg),
				L"Перехоплено помилку з потоку з ID: " + Seed::Core::String::toString(location.ThreadID),
				L"        Ім'я помилки	           : " + String((CharStr)ex.Name),
				L"        Розташування помилки	   : ",
				L"                 Файл	           : " + String((Seed::ACharStr)location.File),
				L"                 Метод           : " + String((Seed::ACharStr)location.FunctName),
				L"                 Рядок           : " + Seed::Core::String::toString(location.Line) });
		}
#pragma warning(pop)
	}
}