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
					return ExceptionString(exeptionName)
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
			template<typename Lambda>
			static void AssertTrue(Lambda&& lambda) {
				if (!lambda())
					Throw(AssertEqualException(ExceptionPosition));
			}

			template<typename Lambda>
			static void AssertNotEqual(Lambda&& lambda) {
				if (lambda())
					Throw(AssertNotEqualException(ExceptionPosition));
			}
		};
	}
}