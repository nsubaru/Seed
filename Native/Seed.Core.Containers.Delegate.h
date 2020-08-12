#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Tuple.h"
#include "typeinfo"

namespace Seed {
#pragma warning(push)
#pragma warning(disable: 4996)
    inline namespace Core {
        inline namespace Exceptions {
            ///<summary>Спроба отримати результат не виконаної ще функції</summary>
            class CantGetResultNotInvokedFunction final : public Exception
            {
            public:
                ///<summary>Генерація помилки спроби отримати результат не виконаної ще функції</summary>
                ///<param name = 'File'>Файл в якому згенерована помилка</param>
                ///<param name = 'FunctName'>Ім'я функції</param>
                ///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
                ForceInline CantGetResultNotInvokedFunction(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
                    noexcept : Exception(File, FunctName, Line, L"Не можливо отримати результат не виконаної функції")
                {}

                ///<summary>Текстове представлення інформації про помилки помилки</summary>
                virtual ExceptionString toString() const override final {
                    return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), ExceptionString(L".")
                    });
                }
            };

            ///<summary>Спроба повторно запусти вже виконаний делегат або який виконується зараз</summary>
            class TryReStartDelegate final : public Exception
            {
            public:
                ///<summary>Генерація помилки спроби повторно запусти вже виконаний делегат або який виконується зараз</summary>
                ///<param name = 'File'>Файл в якому згенерована помилка</param>
                ///<param name = 'FunctName'>Ім'я функції</param>
                ///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
                ForceInline TryReStartDelegate(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
                    noexcept : Exception(File, FunctName, Line, L"Неможливо повторно запустити делегат на виконання")
                {}

                ///<summary>Текстове представлення інформації про помилки помилки</summary>
                virtual ExceptionString toString() const override final {
                    return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), ExceptionString(L".")
                    });
                }
            };

            ///<summary>Делегат в процесі виконання</summary>
            class AlredyExecute final : public Exception
            {
            public:
                ///<summary>Конструктор помилки</summary>
                ///<param name = 'File'>Файл в якому згенерована помилка</param>
                ///<param name = 'FunctName'>Ім'я функції</param>
                ///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
                ForceInline AlredyExecute(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
                    noexcept : Exception(File, FunctName, Line, L"Неможливо отримати результат делегат в процесі його виконання")
                {}

                ///<summary>Текстове представлення інформації про помилки помилки</summary>
                virtual ExceptionString toString() const override final {
                   return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), ExceptionString(L".")
                   });
                }
            };
        }
    }
#pragma warning(pop)
}

namespace Seed {
    inline namespace Core {
        /// <summary>
        /// Батьківський клас для об'єкта функції з аргументами
        /// </summary>
        class FunctionObj
        {
        protected:
            /// <summary>
            /// Вертає результат. Реалізація в TemplateFunctionT
            /// </summary>
            /// <param name="type">Запитуваний тип</param>
            /// <returns>Покажчик на дані</returns>
            virtual void* __get_result(const TypeInfo& type) = 0;
        public:
            /// <summary>
            /// Виконує функцію. Реалізація в TemplateFunctionT
            /// </summary>
            virtual void Init() = 0;

            /// <summary>
            /// Вертає true якщо функція запущена. Реалізація в TemplateFunctionT
            /// </summary>
            /// <returns>Вертає true якщо функція запущена</returns>
            virtual bool IsStarted() const noexcept = 0;

            /// <summary>
            /// Вертає true якщо функція завершила свою роботу.
            /// Якщо при виконанні викликано було помилку то функція буде важатись завершеною
            /// Але результату не буде і при спробі його отримати буде згенеровано помилку NullPointExeption
            /// Реалізація в TemplateFunctionT
            /// </summary>
            /// <returns>Вертає true якщо функція завершила роботу</returns>
            virtual bool IsEnded() const noexcept = 0;

            /// <summary>
            /// Вертає true якщо функція запущена
            /// </summary>
            /// <exception cref="Seed::Core::Exceptions::NullPointException">якщо результату немає, або функція завершилась через генерування помилки</exception>
            /// <exception cref="Seed::Core::Exceptions::InvalidType">якщо типи результату не коректний</exception>
            template<typename T>
            ForceInline T GetResult()
            {
                return MoveRValue(*(T*)__get_result(typeid(T)));
            }

            /// <summary>
            /// Деструктор, для коректного видалення об'єкта який наслідує цей класс
            /// </summary>
            virtual ~FunctionObj() = 0
            {};
        };
    }
}

//Внутрішній клас реалізація делегата
//фактично функція з аргументами
//Так як батьківським класом є FunctionObj то можна тримати покажчик на FunctionObj
//незалежно від типу самої функції
template<typename TF, typename ... TArgs>
class TemplateFunctionT final :public Seed::Core::FunctionObj
{
    //Функція
    TF f;
    //Контейнер для результату
    void* result;
    //Контейнер для аргументів
    Seed::Containers::Tuple<TArgs...> args;
    //Флаг завершення виконання функції
    volatile bool isEnded = false;
    //Флаг запуску функції
    volatile bool isStarted = false;

    using retType = typename Seed::TypeTraits::RemoveCVRefV<decltype(args.CallWithArgsFromTuple(f))>;
protected:
    //Вертає результат
    //Генерить наступні ексепшини
    //- NullPointExeption якщо результату немає, або функція завершилась через генерацію ексепшина
    //- InvalidType якщо типи результату не коректний
    ForceInline virtual void* __get_result(const Seed::TypeInfo& type) override final
    {
        if constexpr (Seed::Core::TypeTraits::IsSameT<retType, void>::State)
        {
            Throw(Seed::InvalidTypeException(ExceptionPosition, typeid(void), type));
        }
        else
        {
            if (result == nullptr)
                Throw(Seed::NullPointException(ExceptionPosition, reinterpret_cast<Seed::SizeT>(this->result), Seed::NullPointException::Types::Read));
            else
            {
                if (typeid(retType) != type)
                    Throw(Seed::InvalidTypeException(ExceptionPosition, typeid(retType), type));
                return result;
            }
        }
    }
public:
    //Виконанує функцію, записує що функція запустилась і завершилась,
    //і якщо результат функції на void то і результат запам'ятовує
    ForceInline void Init() override final
    {
        isStarted = true;
        try
        {
            //Умова часу компіляції, яка провіряє чи функція вертає void
            if constexpr (!Seed::Core::TypeTraits::IsSameT<retType, void>::State)
            {
                //Якщо ні то запамятовує результат в покажчику res
                //В який переміщає конструктором переміщення результат
                result = new retType(args.CallWithArgsFromTuple(f));
            }
            //Якщо функція вертає void то просто викликаєм її
            else
            {
                args.CallWithArgsFromTuple(f);
            }
            //NOTE: Знищення аргументів(якщо цього не робити то модливі циклічні силки в деяких випадках)
            args.~Tuple<TArgs...>();
        }
        catch ([[maybe_unused]] Seed::InteruptException&) { //-V2010
            isEnded = true;
            ReThrow;
        }
        //Якщо зловили якись ексепшн то важаєм що функція завершила свою роботу
        catch ([[maybe_unused]] Seed::Exception&)
        {
            isEnded = true;
            ReThrow;
        }
        isEnded = true;
    }
    //Конструктор, ініціалізує тупл з аргументами, обнулює покажчик на результат, і запам'ятовує функцію
    template<typename TF, typename ... TArgs>
    ForceInline TemplateFunctionT(TF&& f, TArgs&&... args) : f(Seed::Core::ForwardReference<TF>(f)), result(nullptr), args(Seed::Core::ForwardReference<TArgs>(args)...)
    {}
    //Вертає true якщо функція запущена була
    ForceInline virtual bool IsStarted() const noexcept override final
    {
        return isStarted;
    }
    //Вертає true якщо функція завершилась
    ForceInline virtual bool IsEnded() const noexcept override final
    {
        return isEnded;
    }

    //Деструктор, видаляє результат якщо він є
    ForceInline virtual ~TemplateFunctionT() override final
    {
        if constexpr (!Seed::Core::TypeTraits::IsSameT<retType, void>::State)
            if (result != nullptr)
            {
                delete reinterpret_cast<Seed::TypeTraits::AddPointer<retType>>(result);
                result = nullptr;
            }
    }
};
//
//template<typename TF, typename ... TArgs>
//TemplateFunctionT(TF&& f, TArgs&&... args)->TemplateFunctionT<TF&&, Seed::TypeTraits::SpecialDecayT<TArgs>...>;

namespace Seed {
    inline namespace Core {
        /// <summary>
        /// Делегат це об'єкт який інкапсулює, функцію з аргументами,
        /// тим самим дозволяючи її відкладено виконати,
        /// навіть в іншому потоці, apc якогось потоку чи пулі потоків
        /// </summary>
        class Delegate final : protected IDisposable
        {
        private:
            //Внутрішня структура в які дані делегата
            struct delegate
            {
                FunctionObj* function;
                RefCountT RefCount;
                ForceInline delegate(FunctionObj* function) : function(function), RefCount(1)
                {}
            };
            //Покажчик на дані даного делегата
            delegate* data;

            // Inherited via IDisposable
            virtual void Dispose() override final
            {
                delete data->function;
            }
        public:
            /// <summary>
            /// Конструктор з функції F і набору аргументів Args(може бути і без аргументів)
            /// </summary>
            /// <param name="f">Функція(покажчик на неї)</param>
            /// <param name="args">Аргументи функції</param>
            template<typename TF, typename ... TArgs>
            requires Concepts::IsInvockable<TF, TArgs...>
            ForceInline Delegate(TF&& f, TArgs&&... args) : data(new delegate(new TemplateFunctionT<TF, TArgs...>(ForwardReference<TF>(f), ForwardReference<TArgs>(args)...)))
            {};

            /// <summary>
            /// Вертає true якщо делегат виконується зараз
            /// генерує NullPointExeption, якщо делегат не валідний
            /// </summary>
            /// <returns>Вертає true якщо делегат виконується зараз</returns>
            ForceInline Boolean GetIsStarted() const
            {
                if (this->data == nullptr)
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
                return this->data->function->IsStarted();
            }

            /// <summary>
            /// Вертає true якщо виконання делегата завершене
            /// генерує NullPointExeption, якщо делегат не валідний
            /// </summary>
            /// <returns>Вертає true якщо виконання делегата завершене</returns>
            ForceInline Boolean GetIsEnded() const
            {
                if (this->data == nullptr)
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
                return this->data->function->IsEnded();
            }

            /// <summary>
            /// Запуск функції, можливий лиш раз, для отримання результату є функція GetResult
            /// генерує наступні помилки:
            /// </summary>
            /// <exception cref="Seed::Core::Exceptions::NullPointException">якщо дані делегата силаються на nullptr</exception>
            /// <exception cref="Seed::Core::Exceptions::TryReStartDelegate">якщо делегат виконувався або виконується</exception>
            ForceInline void Init() volatile
            {
                if (this->data == nullptr)
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
                if (this->data->function->IsEnded() || this->data->function->IsStarted())
                    Throw(TryReStartDelegate(ExceptionPosition));
                this->data->function->Init();
            }

            /// <summary>
            /// Вертає результат виконання делегата
            /// </summary>
            /// <exception cref="Seed::Core::Exceptions::NullPointException">якщо делегат без функції(тобто не валідний) або функція ще не запушена
            /// або функція не завершилась бо було згенеровано ексепшн
            /// </exception>
            /// <exception cref=""Seed::Core::Exceptions::AlredyExecute>якщо функція в процесі виконання</exception>
            /// <exception cref="InvalidType">якщо тип T не збігаєтья з типом результату функції</exception>
            /// <returns>Значення результату функції</returns>
            template<typename T>
            ForceInline T GetResult()
            {
                if (this->data == nullptr)
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
                if (!this->data->function->IsStarted())
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data->function), NullPointException::Types::Execute));
                if (this->data->function->IsStarted() && !this->data->function->IsEnded())
                    Throw(AlredyExecute(ExceptionPosition));
                return this->data->function->GetResult<T>();
            }

            /// <summary>
            /// Вертає результат виконання делегата
            /// </summary>
            /// <exception cref="Seed::Core::Exceptions::NullPointException">якщо делегат без функції(тобто не валідний) або функція ще не запушена
            /// або функція не завершилась бо було згенеровано ексепшн
            /// </exception>
            /// <exception cref=""Seed::Core::Exceptions::AlredyExecute>якщо функція в процесі виконання</exception>
            /// <exception cref="InvalidType">якщо тип T не збігаєтья з типом результату функції</exception>
            /// <returns>Значення результату функції</returns>
            template<typename T>
            ForceInline T GetResult() volatile
            {
                if (this->data == nullptr)
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data), NullPointException::Types::Read));
                if (!this->data->function->IsStarted())
                    Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this->data->function), NullPointException::Types::Execute));
                if (this->data->function->IsStarted() && !this->data->function->IsEnded())
                    Throw(AlredyExecute(ExceptionPosition));
                return this->data->function->GetResult<T>();
            }

            /// <summary>
            /// Оператор функтор, виконує фукцію
            /// </summary>
            /// <returns>Вертає силку на даний об'єкт</returns>
            ForceInline volatile Delegate& operator()() volatile {
                this->Init();
                return *this;
            }

            /// <summary>
            /// Оператор функтор, виконує фукцію
            /// </summary>
            /// <returns>Вертає силку на даний об'єкт</returns>
            ForceInline Delegate& operator()() {
                this->Init();
                return *this;
            }

            RefCountClass(Delegate, data);
        };

        /// <summary>
        /// Створює делегат з функцією F і аргументами Args(може бути і без аргументів)
        /// </summary>
        /// <param name="f">Функція(покажчик на неї)</param>
        /// <param name="args">Аргументи функції</param>
        /// <returns>Об'єкт делегата</returns>
        template<typename TF, typename ... TArgs>
        requires Concepts::IsInvockable<TF, TArgs...>
        ForceInline Delegate CreateAndPackFunctionObj(TF&& f, TArgs&&... args)
        {
            return Delegate(Seed::Core::ForwardReference<TF>(f), Seed::Core::ForwardReference<TArgs>(args)...);
        }
    }
};