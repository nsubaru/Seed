#pragma once
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Containers.List.h"
#include "Seed.Core.Containers.List.h"
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.String.h"

namespace Seed {
	inline namespace Core {
		///<summary>Вивід в логи складеного повідомлення в логи, кожен елемент масиву окремий рядок</summary>
		///<param name = "lines">Список рядків</param>
		///<returns><c>True</c> якщо запис в логи було проведено успішно</returns>
		bool SeedKernelCall toCoreLog(const Array<ExceptionString>& lines);

		///<summary>Вивід в логи рядка з повідомленням</summary>
		///<param name = "msg">Повідомлення</param>
		///<param name = "Heap">Куча для алокатора пам'яті</param>
		///<returns><c>True</c> якщо запис в логи було проведено успішно</returns>
		template<typename Heap>
		__forceinline bool toCoreLog(const BaseString<Heap>& msg)
		{
#pragma warning(push)
#pragma warning(disable: 4996)
			return toCoreLog((ConstCharStr)msg, msg.Size);
#pragma warning(pop)
		}

#pragma warning(push)
#pragma warning(disable: 4996)

		///<summary>Запис повідомлення про помилки</summary>
		///<param name = "Msg">Користувальницьке повідомлення</param>
		///<param name = "Base">Куча для алокатора пам'яті</param>
		///<param name = "ex">Об'єкт помилки</param>
		///<returns><c>True</c> якщо запис в логи було проведено успішно</returns>
		template<typename Heap>
		__forceinline bool toCoreLog(const BaseString<Heap>& Msg, const Seed::Exception& ex)
		{
			return Seed::Core::toCoreLog({ Msg, ex.toString() });
		}

		///<summary>Запис повідомлення про помилки</summary>
		///<param name = "Msg">Користувальницьке повідомлення</param>
		///<param name = "Lenght">Розмір повідомлення</param>
		///<param name = "ex">Об'єкт помилки</param>
		///<returns><c>True</c> якщо запис в логи було проведено успішно</returns>
		template<SizeT Lenght>
		__forceinline bool toCoreLog(const wchar_t(&Msg)[Lenght], const Seed::Exception& ex)
		{
			return Seed::Core::toCoreLog({ Msg, ex.toString() });
		}
#pragma warning(pop)
	}
}