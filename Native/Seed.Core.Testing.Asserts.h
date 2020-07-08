#pragma once
#include "Seed.Core.Exceptions.h"

namespace Seed
{
	inline namespace Core
	{
		class Asserts final {
		public:
			class AssertException : public Exception {
			protected:
				ExceptionString assertMessage;
			public:
				AssertException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ExceptionString& AssertMessage) :
					Exception(File, FunctName, Line, L"Assert не пройшов"), assertMessage(AssertMessage) {}

				AssertException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& AssertMessage) :
					Exception(File, FunctName, Line, L"Assert не пройшов"), assertMessage(MoveRValue(AssertMessage)) {}
			public:
				const ExceptionString& GetAssertMessage() {
					return assertMessage;
				}
				ExceptionString Property(GetAssertMessage) AssertMessage;
			public:
				virtual ExceptionString toString() const override final {
					return ExceptionString(exceptionName)
						+ ExceptionString(L". ")
						+ assertMessage
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}
			};
		public:
			class AssertEqualException : public AssertException {
			public:
				AssertEqualException(const ConstAString& File, const ConstAString& FunctName, SizeT Line) :
					AssertException(File, FunctName, Line, L"Умова має бути істиною") {}
			};
			class AssertNotEqualException : public AssertException {
			public:
				AssertNotEqualException(const ConstAString& File, const ConstAString& FunctName, SizeT Line) :
					AssertException(File, FunctName, Line, L"Умова має бути хибою") {}
			};
		public:
			static void AssertTrue(bool value) {
				if (!value)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename TLambda>
			requires Concepts::IsInvockableWithResult<TLambda, Boolean>
			static void AssertTrue(TLambda&& lambda) {
				if (!lambda())
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(T&& a, U&& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(const T& a, U&& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(T&& a, const U& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveEqual<T, U>
			static void AssertEqual(const T& a, const U& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(T&& a, U&& b) {
				if (a == b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(const T& a, U&& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(T&& a, const U& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename T, typename U>
			requires Concepts::HaveNotEqual<T, U>
			static void AssertNotEqual(const T& a, const U& b) {
				if (a != b)
					Throw(AssertEqualException(ExceptionPosition));
			}

			static void AssertFalse(bool value) {
				if (value)
					Throw(AssertNotEqualException(ExceptionPosition));
			}

			template<typename TLambda>
			requires Concepts::IsInvockableWithResult<TLambda, Boolean>
			static void AssertFalse(TLambda&& lambda) {
				if (lambda())
					Throw(AssertNotEqualException(ExceptionPosition));
			}
		};
	}
}