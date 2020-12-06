#pragma once
#include "Seed.Core.Exceptions.h"

namespace Seed
{
	inline namespace Core
	{
		/// <summary>
		/// Набір логічних перевірок
		/// </summary>
		class Asserts final {
		public:
			/// <summary>
			/// Помилка логічної перевірик
			/// </summary>
			class AssertException : public Exception {
			protected:
				ExceptionString assertMessage;
			public:
				AssertException(const AssertException& orig) : Exception(orig), assertMessage(orig.assertMessage) {}
				AssertException(AssertException&& from) : Exception(MoveRValue(from)), assertMessage(MoveRValue(from.assertMessage)) {}
				AssertException& operator=(const AssertException& from) { this->~AssertException(); this->AssertException::AssertException(from); return *this; }
				AssertException& operator=(AssertException&& from) { this->~AssertException(); this->AssertException::AssertException(MoveRValue(from)); return *this; }
			public:
				/// <summary>
				/// Конструктор поимилки
				/// </summary>
				/// <param name="File">Файл</param>
				/// <param name="FunctName">Ім'я функції</param>
				/// <param name="Line">Рядок</param>
				/// <param name="AssertMessage">Повідомлення ассета</param>
				AssertException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ExceptionString& AssertMessage) :
					Exception(File, FunctName, Line, L"Assert не пройшов"), assertMessage(AssertMessage) {}

				/// <summary>
				/// Конструктор поимилки
				/// </summary>
				/// <param name="File">Файл</param>
				/// <param name="FunctName">Ім'я функції</param>
				/// <param name="Line">Рядок</param>
				/// <param name="AssertMessage">Повідомлення ассета</param>
				AssertException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& AssertMessage) :
					Exception(File, FunctName, Line, L"Assert не пройшов"), assertMessage(MoveRValue(AssertMessage)) {}
			public:
				/// <summary>
				/// Вертає повідомлення ассета
				/// </summary>
				/// <returns>Повідомлення ассета</returns>
				const ExceptionString& GetAssertMessage() {
					return assertMessage;
				}
				/// <summary>
				/// Повідомлення ассета
				/// </summary>
				ExceptionString Property(GetAssertMessage) AssertMessage;
			public:
				/// <summary>
				/// Генерує текстовий опис помилки
				/// </summary>
				/// <returns></returns>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". "),
						assertMessage,
						ExceptionString(L". "),
						location.toString(),
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new AssertException(*this));
				}

				virtual ~AssertException() {}
			};
		public:
			/// <summary>
			/// Не пройдена перевірка на рівність
			/// </summary>
			class AssertEqualException : public AssertException {
			public:
				AssertEqualException(const AssertEqualException& orig) : AssertException(orig) {}
				AssertEqualException(AssertEqualException&& from) : AssertException(MoveRValue(from)) {}
				AssertEqualException& operator=(const AssertEqualException& from) { this->~AssertEqualException(); this->AssertEqualException::AssertEqualException(from); return *this; }
				AssertEqualException& operator=(AssertEqualException&& from) { this->~AssertEqualException(); this->AssertEqualException::AssertEqualException(MoveRValue(from)); return *this; }
			public:
				AssertEqualException(const ConstAString& File, const ConstAString& FunctName, SizeT Line) :
					AssertException(File, FunctName, Line, L"Умова має бути істиною") {}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new AssertEqualException(*this));
				}

				virtual ~AssertEqualException() {}
			};
			class AssertNotEqualException : public AssertException {
			public:
				AssertNotEqualException(const AssertNotEqualException& orig) : AssertException(orig) {}
				AssertNotEqualException(AssertNotEqualException&& from) : AssertException(MoveRValue(from)) {}
				AssertNotEqualException& operator=(const AssertNotEqualException& from) { this->~AssertNotEqualException(); this->AssertNotEqualException::AssertNotEqualException(from); return *this; }
				AssertNotEqualException& operator=(AssertNotEqualException&& from) { this->~AssertNotEqualException(); this->AssertNotEqualException::AssertNotEqualException(MoveRValue(from)); return *this; }
			public:
				AssertNotEqualException(const ConstAString& File, const ConstAString& FunctName, SizeT Line) :
					AssertException(File, FunctName, Line, L"Умова має бути хибою") {}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new AssertNotEqualException(*this));
				}

				virtual ~AssertNotEqualException() {}
			};
		public:
			static void AssertTrue(bool value, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (!value)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename TLambda>
			requires Concepts::IsInvockableWithResult<TLambda, Boolean>
			static void AssertTrue(TLambda&& lambda, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (!lambda())
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(T&& a, U&& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(const T& a, U&& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(T&& a, const U& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(const T& a, const U& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(T&& a, U&& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a == b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(const T& a, U&& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(T&& a, const U& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(const T& a, const U& b, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (a != b)
					Throw(AssertEqualException(file, functName, line));
			}

			static void AssertFalse(bool value, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (value)
					Throw(AssertNotEqualException(file, functName, line));
			}

			template<typename TLambda>
			requires Concepts::IsInvockableWithResult<TLambda, Boolean>
			static void AssertFalse(TLambda&& lambda, const ConstAString& file, const ConstAString& functName, SizeT line) {
				if (lambda())
					Throw(AssertNotEqualException(file, functName, line));
			}

			template<typename TException, typename TLambda>
			requires Concepts::IsInvockable<TLambda> && Concepts::IsBasedOn<Exceptions::Exception, TException>//&& Concepts::IsException<TException>
			static void AssertThrowException(TLambda&& lambda, const ConstAString& file, const ConstAString& functName, SizeT line) {
				try
				{
					lambda();
				}
				catch ([[maybe_unused]] TException& ex)
				{
					return;
				}
				Throw(AssertNotEqualException(file, functName, line));
			}

			template<typename TException, typename TLambda>
			requires Concepts::IsInvockable<TLambda> && Concepts::IsBasedOn<Exceptions::Exception, TException>//&& Concepts::IsException<TException>
				static void AssertNoThrowException(TLambda&& lambda, const ConstAString& file, const ConstAString& functName, SizeT line) {
				try
				{
					lambda();
				}
				catch ([[maybe_unused]] TException& ex)
				{
					Throw(AssertNotEqualException(file, functName, line));
				}
			}
		};
	}
}