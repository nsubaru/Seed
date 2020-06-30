#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Utils.h"
#include "Seed.Core.Base.TypeTraits.CVModificators.h"
#include "Seed.Core.Base.TypeTraits.Function.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
            /// <summary>
            /// Додаткова структура яка вказує чи даний тип є методом класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T>
            struct IsMemberFunctionPointerHelper : LogicFalse {};

            /// <summary>
            /// Додаткова структура яка вказує чи даний тип є методом класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T, class TU>
            struct IsMemberFunctionPointerHelper<T TU::*> : IsFunction<T> {};

            /// <summary>
            /// Вказує чи даний тип є покажчиком на метод класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T>
            struct IsMemberFunctionPointerT : IsMemberFunctionPointerHelper<RemoveCV<T>> {};

            /// <summary>
            /// Вказує чи даний тип є покажчиком на метод класу або чи існує метод з таким іменем в класу
            /// </summary>
            /// <typeparam name="T">Покажчик на метод</typeparam>
            template<typename T>
            constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointerT<T>::State;

			namespace Details
            {
            	/// <summary>
            	/// Виклик функції через покажчик на неї(об'єкт з operator()) і аргументами
            	/// </summary>
            	/// <typeparam name="TF">Тип функції або об'єкт з operator()</typeparam>
            	/// <typeparam name="TArgs">Типи аргументів</typeparam>
    			/// <param name="f">Функціїя</param>
    			/// <param name="args">Аргументи</param>
                template <class TF, class... TArgs>
                inline auto Invoke(TF&& f, TArgs&& ... args) ->
                    decltype(Seed::ForwardReference<TF>(f)(Seed::ForwardReference<TArgs>(args)...)) {
                    return Seed::ForwardReference<TF>(f)(Seed::ForwardReference<TArgs>(args)...);
                }

                template <class TBase, class T, class TDerived>
                inline auto Invoke(T TBase::* pmd, TDerived&& ref) ->
                    decltype(Seed::ForwardReference<TDerived>(ref).*pmd) {
                    return Seed::ForwardReference<TDerived>(ref).*pmd;
                }

                template <class PMD, class TPointer>
                inline auto Invoke(PMD&& pmd, TPointer&& ptr) ->
                    decltype((*Seed::ForwardReference<TPointer>(ptr)).*Seed::ForwardReference<PMD>(pmd)) {
                    return (*Seed::ForwardReference<TPointer>(ptr)).*Seed::ForwardReference<PMD>(pmd);
                }

                template <class TBase, class T, class TDerived, class... TArgs>
                inline auto Invoke(T TBase::* pmf, TDerived&& ref, TArgs&& ... args) ->
                    decltype((Seed::ForwardReference<TDerived>(ref).*pmf)(Seed::ForwardReference<TArgs>(args)...)) {
                    return (Seed::ForwardReference<TDerived>(ref).*pmf)(Seed::ForwardReference<TArgs>(args)...);
                }

                template <class PMF, class TPointer, class... TArgs>
                inline auto Invoke(PMF&& pmf, TPointer&& ptr, TArgs&& ... args) ->
                    decltype(((*Seed::ForwardReference<TPointer>(ptr)).*Seed::ForwardReference<PMF>(pmf))(Seed::ForwardReference<TArgs>(args)...)) {
                    return ((*Seed::ForwardReference<TPointer>(ptr)).*Seed::ForwardReference<PMF>(pmf))(Seed::ForwardReference<TArgs>(args)...);
                }
            }
        	
            template <typename, typename = void>
            struct ResultOfT {
                using Type = VoidT<>;
            };

            template <typename TF, class...TArgs>
            struct ResultOfT<TF(TArgs...),
                decltype(void(Details::Invoke(TypeTraits::DeclVal<TF>(), TypeTraits::DeclVal<TArgs>()...)))> {
                using Type = decltype(Details::Invoke(TypeTraits::DeclVal<TF>(), TypeTraits::DeclVal<TArgs>()...));
            };

            ///<summary>Тип без силки</summary>
            template<typename T>
            using ResultOfV = typename ResultOfT<T>::Type;

			namespace Details
			{
				template<typename Void, typename Callable, typename ... TTypes>
				struct InvokeTraits {
					using Invockable = LogicFalse;
                    using Result = VoidT<>;
				};

				template<typename Callable, typename ... TTypes>
				struct InvokeTraits<VoidT<decltype(Invoke(DeclVal<Callable>(), DeclVal<TTypes>()...))>, Callable, TTypes...> {
					using Invockable = LogicTrue;
					using Result = typename decltype(Invoke(DeclVal<Callable>(), DeclVal<TTypes>()...));
				};
			}

            /// <summary>
            /// Перевірка чи тип TCallable можна викликати з набіром аргументів TArgs
            /// </summary>
			template<typename TCallable, typename ... TArgs>
			constexpr Boolean IsInvockable = Details::InvokeTraits<void, TCallable, TArgs...>::Invockable::State;

            /// <summary>
            /// Тип повернення при виклику TCallable з набіром аргументів TArgs
            /// </summary>
			template<typename TCallable, typename ... TArgs>
			using InvockableResult = typename Details::InvokeTraits<void, TCallable, TArgs...>::Result;
		}
	}
}