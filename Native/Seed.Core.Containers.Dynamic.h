#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Containers.Tuple.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Containers
        {
            /// <summary>
            /// Динамічний об'єкт, тип даних відомий при виконанні, містить кортеж змінних
            /// </summary>
            class SeedKernelCall Dynamic final : protected IDisposable{
            private:
                /// <summary>
                /// Інтерфейс динамічного типу
                /// </summary>
                class IDynamicImpl {
                protected:
                    /// <summary>
                    /// RTTI інформація про тип кортежу
                    /// </summary>
                    /// <returns>RTTI TypeInfo</returns>
                    virtual const TypeInfo& GetVariablesPackTypeInfo() const = 0;
                    /// <summary>
                    /// Покажчик на пакет змінних
                    /// </summary>
                    /// <returns>Вертає сирий покажчик</returns>
                    virtual void* GetVariablePackRaw() = 0;
                public:
                    /// <summary>
                    /// Вертає пакет змінних якщо тип вірний, якщо ні генерує помилку InvalidTypeException
                    /// </summary>
                    /// <typeparam name="TTypes">Кортеж типів</typeparam>
                    template<typename ... TTypes>
                    Tuple<TTypes...>& GetVariables() {
                        using Pack = Tuple<TTypes...>;
                        if (typeid(Pack).hash_code() == GetVariablesPackTypeInfo().hash_code())
                        {
                            return *reinterpret_cast<Pack*>(GetVariablePackRaw());
                        }
                        Throw(InvalidTypeException(ExceptionPosition, typeid(Pack), GetVariablesPackTypeInfo()));
                    }

                    /// <summary>
                    /// Вертає тип кортежу який містить об'єкт
                    /// </summary>
                    /// <returns>RTTI TypeInfo</returns>
                    const TypeInfo& GetType() const {
                        return GetVariablesPackTypeInfo();
                    }

                    /// <summary>
                    /// Деструктор
                    /// </summary>
                    virtual ~IDynamicImpl() = 0 {};
                };

                /// <summary>
                /// Імплементація динамічного типу
                /// </summary>
                /// <typeparam name="TTypes">Набір типів кортеж</typeparam>
                template<typename ... TTypes>
                class DynamicImpl final : public IDynamicImpl {
                private:
                    Tuple<TTypes...> data;
                private:
                    /// <summary>
                    /// Інформацію типу який містить об'єкт
                    /// </summary>
                    /// <returns>RTTI TypeInfo</returns>
                    const TypeInfo& GetVariablesPackTypeInfo() const override
                    {
                        return typeid(decltype(data));
                    }

                    /// <summary>
                    /// Покажчик на пакет змінних
                    /// </summary>
                    /// <returns></returns>
                    void* GetVariablePackRaw() override
                    {
                        return reinterpret_cast<void*>(&data);
                    }
                public:
                    /// <summary>
                    /// Конструктор з пакету змінних
                    /// </summary>
                    /// <param name="types">Пакет змінних</param>
                    DynamicImpl(TTypes... types) : data(types...) {}

                    /// <summary>
                    /// Деструктор
                    /// </summary>
                    virtual ~DynamicImpl() override final {};
                };
            private:
                /// <summary>
                /// Контейнер даних даного об'єкту
                /// </summary>
                struct Data {
                    /// <summary>
                    /// Покажчик на інтерфейс контейнер пакету
                    /// </summary>
                    IDynamicImpl* pack;
                    RefCountT RefCount;

                    /// <summary>
                    /// Конструктор з інтерфейсу контейнера пакету даних
                    /// </summary>
                    /// <param name="pack">Покажчик на інтерфейс контейнера пакету</param>
                    /// <returns></returns>
                    forceinline Data(IDynamicImpl* pack) : pack(pack), RefCount(1) {}
                };
            private:
                /// <summary>
                /// Покажчик на дані об'єкта
                /// </summary>
                Data* This;
            private:
                /// <summary>
                /// Звільнення ресурсів
                /// </summary>
                virtual void Dispose() override final {
                    delete This->pack;
                }
            public:
                /// <summary>
                /// Конструктор з пакета змінних
                /// </summary>
                /// <typeparam name="TTypes">Пакет типів</typeparam>
                template<typename ... TTypes>
                Dynamic(TTypes ... types) : This(new Data(new DynamicImpl<TTypes...>(types...))) {}

                /// <summary>
                /// Вертає силку на пакет змінних які містить об'єкт,
                /// якщо типи вказані некоректно то генерується помилка
                /// </summary>
                /// <typeparam name="TTypes">Типи пакету</typeparam>
                template<typename ... TTypes>
                Tuple<TTypes...>& GetVariables() {
                    return This->pack->GetVariables<TTypes...>();
                }

                /// <summary>
                /// Вертає тип кортежу пакета які на даний момент містить об'єкт
                /// </summary>
                /// <returns></returns>
                const TypeInfo& GetVariableType() const
                {
                    return This->pack->GetType();
                }

                /// <summary>
                /// Тип кортежу пакета які на даний момент містить об'єкт
                /// </summary>
                TypeInfo Property(GetVariableType) Type;

                RefCountClass(Dynamic, This);
            };
        }
    }
}