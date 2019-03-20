#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Internals.h"
#include "typeinfo"

#define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING

namespace {
	////Структура для збереження результату і його типу
	//struct variadic_data{
	//	void * data;
	//	size_t type_hash;
	//	__forceinline variadic_data(void *&& data, size_t type_hash) noexcept :data(data), type_hash(type_hash) {}
	//};
}

namespace Seed {
	inline namespace Exeptions {
		//Спроба отримати результат не виконаної ще функції
		class SeedKernelCall CantGetResultNotInvokedFunction :public Exception
		{
		public:
			//Генерація помилки спроби отримати результат не виконаної ще функції
			__forceinline CantGetResultNotInvokedFunction(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Can get result not invoked function")
			{}
		};

		//Спроба повторно запусти вже виконаний делегат або який виконується зараз
		class SeedKernelCall TryReStartDelegate :public Exception
		{
		public:
			//Генерація помилки спроби повторно запусти вже виконаний делегат або який виконується зараз
			__forceinline TryReStartDelegate(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Can start alredy started or alredy executed delegate")
			{}
		};

		//Делегат в процесі виконання
		class SeedKernelCall AlredyExecute :public Exception
		{
		public:
			//Генерація помилки
			__forceinline AlredyExecute(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Can start alredy started or alredy executed delegate")
			{}
		};
	}
}

namespace Seed {
	inline namespace Core {
		//Батьківський клас для об'єкта функції з аргументами
		class FunctionObj
		{
		protected:
			//Вертає результат. Реалізація в TemplateFunctionT
			virtual void * __get_result(const TypeInfo & type) = 0;
		public:
			//Виконує функцію. Реалізація в TemplateFunctionT
			virtual void Init() = 0;
			//Вертає true якщо функція запущена. Реалізація в TemplateFunctionT
			virtual bool IsStarted() const noexcept = 0;
			//Вертає true якщо функція завершила свою роботу. 
			//Якщо при виконанні викликано було помилку то функція буде важатись завершеною
			//Але результату не буде і при спробі його отримати буде згенеровано помилку NullPointExeption
			//Реалізація в TemplateFunctionT
			virtual bool IsEnded() const noexcept = 0;
			//Вертає результат якщо він є і якщо його тип збігається
			//Генерить наступні ексепшини
			//- NullPointExeption якщо результату немає, або функція завершилась через генерування помилки
			//- InvalidType якщо типи результату не коректний
			template<typename T>
			__forceinline T GetResult()
			{
				return *(T*)__get_result(typeid(T));
			}
			//Деструктор, для коректного видалення об'єкта який наслідує цей класс
			virtual ~FunctionObj()
			{};
		};
	}
}

//Внутрішні структури для запуску функції і розкрутки аргументів. 
//Магія, не чіпати
template<typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
	auto __forceinline static call(F f, Tuple&& t)
	{
		return call_impl<F, Tuple, TotalArgs == 1 + sizeof...(N),
			TotalArgs, N..., sizeof...(N)
		>::call(f, Seed::ForwardReference<Tuple>(t));
	}
};
//Внутрішні структури для запуску функції і розкрутки аргументів. 
//Магія, не чіпати
template<typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
	auto __forceinline static call(F f, Tuple&& t)
	{
		return f(Seed::Get<N>(Seed::MoveRValue<Tuple>(t))...);
	}
};
//Внутрішні структури для запуску функції і розкрутки аргументів. 
//Магія, не чіпати
template<typename F, typename Tuple>
auto __forceinline call(F f, Tuple&& t)
{
	typedef typename std::decay<Tuple>::type type;
	return call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
		std::tuple_size<type>::value
	>::call(f, std::forward<Tuple>(t));
}

//Внутрішній клас реалізація делегата
//фактично функція з аргументами
//Так як батьківським класом є FunctionObj то можна тримати покажчик на FunctionObj
//незалежно від типу самої функції
template<typename F, typename ... Args>
class TemplateFunctionT :public Seed::Core::FunctionObj
{
	//Функція
	F f;
	//Контейнер для результату
	void * result;
	//Контейнер для аргументів
	Seed::Tuple<Args...> args;
	//Флаг завершення виконання функції
	volatile bool isEnded = false;
	//Флаг запуску функції
	volatile bool isStarted = false;
protected:
	//Вертає результат
	//Генерить наступні ексепшини
	//- NullPointExeption якщо результату немає, або функція завершилась через генерацію ексепшина
	//- InvalidType якщо типи результату не коректний
	__forceinline virtual void * __get_result(const Seed::TypeInfo & type) override final
	{
		if constexpr (Seed::Core::StaticAsserts::IsSame<decltype(call(f, args)), void>::State)
			Throw(Seed::InvalidTypeException(ExceptionPosition, typeid(decltype(call(f,args)), type)));
		else if (result == nullptr)
			Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->result), NullPointException::Types::Read));
		else
		{
			if (typeid(decltype(call(f, args))).hash_code() != type.hash_code())
				Throw(Seed::InvalidTypeException(ExceptionPosition, typeid(decltype(call(f, args)), type)));
			return result;
		}
	}
public:
	//Виконанує функцію, записує що функція запустилась і завершилась, 
	//і якщо результат функції на void то і результат запам'ятовує 
	__forceinline void Init() override final
	{
		isStarted = true;
		try
		{
			//Умова часу компіляції, яка провіряє чи функція вертає void
			if constexpr (!Seed::CoreAPI::StaticAsserrts::IsSame<decltype(call(f, args)), void>::State)
			{
				//Якщо ні то запамятовує результат в покажчику res
				//В який переміщає конструктором переміщення результат
				result = new decltype(call(f, args))(call(f, args));
			}
			//Якщо функція вертає void то просто викликаєм її
			else call(f, args);
		}
		//Якщо зловили якись ексепшн то важаєм що функція завершила свою роботу
		catch (Seed::Exeption * ex)
		{
			isEnded = true;
			throw ex;
		}
		isEnded = true;
	}
	//Конструктор, ініціалізує тупл з аргументами, обнулює покажчик на результат, і запам'ятовує функцію
	__forceinline TemplateFunctionT(F f, Args... args) :args(args...), result(nullptr), f(f)
	{}
	//Вертає true якщо функція запущена була
	__forceinline virtual bool IsStarted() const noexcept override final
	{
		return isStarted;
	}
	//Вертає true якщо функція завершилась
	__forceinline virtual bool IsEnded() const noexcept override final
	{
		return isEnded;
	}
	//Деструктор, видаляє результат якщо він є
	__forceinline ~TemplateFunctionT()
	{
		if constexpr (!std::is_same<decltype(call(f, args)), void>::value)
			if (result != nullptr)
			{
				delete (decltype(call(f, args))*)result;
				result = nullptr;
			}
	}
};

namespace Seed {
	inline namespace Core {
		//Делегат це об'єкт який інкапсулює, функцію з аргументами, 
		//тим самим дозволяючи її відкладено виконати, 
		//навіть в іншому потоці, apc якогось потоку чи пулі потоків
		class SeedKernelCall Delegate : IDisposable
		{
		private:
			//Внутрішня структура в які дані делегата
			struct delegate
			{
				FunctionObj * function;
				RefCountT RefCount;
				__forceinline delegate(FunctionObj *  function) : function(function), RefCount(1)
				{}
			};
			//Покажчик на дані даного делегата
			delegate * data;

			// Inherited via IDisposable
			virtual void Dispose() override final
			{
				delete data->function;
			}
		public:
			//Конструктор з функції F і набору аргументів Args(може бути і без аргументів)
			template<typename F, typename ... Args>
			__forceinline Delegate(F f, Args... args) : data(new delegate(new TemplateFunctionT<F, Args...>(f, args...)))
			{};

			RefCountClass(Delegate, data);
			

			//Вертає true якщо делегат виконується зараз
			//генерує NullPointExeption, якщо делегат не валідний
			__forceinline Boolean GetIsStarted()
			{
				if (this->data == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
				return this->data->function->IsStarted();
			}

			//Вертає true якщо виконання делегата завершене
			//генерує NullPointExeption, якщо делегат не валідний
			__forceinline Boolean GetIsEnded()
			{
				if (this->data == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
				return this->data->function->IsEnded();
			}

			//Запуск функції, можливий лиш раз, для отримання результату є функція GetResult<T>
			//генерує наступні помилки: 
			//- NullPointExeption якщо дані делегата силаються на nullptr
			//- AlredyExecuteOrExecuteEnded якщо делегат виконувався або виконується
			__forceinline void Init()
			{
				if (this->data == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
				if (this->data->function->IsEnded() || this->data->function->IsStarted())
					Throw(TryReStartDelegate(ExceptionPosition));
				this->data->function->Init();
			}

			//Вертає результат виконання делегата
			//Генерує наступні помилки
			//- NullPointExeption якщо делегат без функції(тобто не валідний) або функція ще не запушена
			//або функція не завершилась бо було згенеровано ексепшн
			//- AlredyExecute якщо функція в процесі виконання
			//- InvalidType якщо тип T не збігаєтья з типом результату функції
			template<typename T>
			__forceinline T GetResult()
			{
				if (this->data == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
				if (!this->data->function->IsStarted())
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data->function), NullPointException::Types::Execute));
				if (this->data->function->IsStarted() && !this->data->function->IsEnded())
					Throw(AlredyExecute(ExceptionPosition));
				return this->data->function->GetResult<T>();
			}
			//Вертає результат виконання делегата
			//Генерує наступні помилки
			//- NullPointExeption якщо делегат без функції(тобто не валідний) або функція ще не запушена
			//або функція не завершилась бо було згенеровано ексепшн
			//- AlredyExecute якщо функція в процесі виконання
			//- InvalidType якщо тип T не збігаєтья з типом результату функції
			template<typename T>
			__forceinline T GetResult() volatile
			{
				if (this->data == nullptr)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
				if (!this->data->function->IsStarted())
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data->function), NullPointException::Types::Execute));
				if (this->data->function->IsStarted() && !this->data->function->IsEnded())
					Throw(AlredyExecute(ExceptionPosition));
				return this->data->function->GetResult<T>();
			}
		};
		//Створює делегат з функцією F і аргументами Args(може бути і без аргументів)
		template<typename F, typename ... Args>
		__forceinline SeedKernelCall Delegate CreateAndPackFunctionObj(F f, Args... args)
		{
			return Delegate(f, args...);
		}
	}
};