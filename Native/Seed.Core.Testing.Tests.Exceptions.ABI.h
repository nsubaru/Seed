#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Exceptions {
					namespace Abi {
						class SimpleThrow final : public TestClass<SimpleThrow> {
							virtual bool Excecute() override;
						};

						class SimpleThrowWithTwoCatches final : public TestClass<SimpleThrowWithTwoCatches> {
							virtual bool Excecute() override;
						};

						class SimpleThrowCatchAsParrentType final : public TestClass<SimpleThrowCatchAsParrentType> {
							virtual bool Excecute() override;
						};

						class SimpleThrowElipseCatch final : public TestClass<SimpleThrowElipseCatch> {
							virtual bool Excecute() override;
						};

						class SimpleThrowInnerCatch final : public TestClass<SimpleThrowInnerCatch> {
							virtual bool Excecute() override;
						};

						class ThrowSubMethod final : public TestClass<ThrowSubMethod> {
							virtual bool Excecute() override;
						};

						class ThrowSubMethodElipse final : public TestClass<ThrowSubMethodElipse> {
							virtual bool Excecute() override;
						};

						class ThrowCatchSubSubMethod final : public TestClass<ThrowCatchSubSubMethod> {
							virtual bool Excecute() override;
						};

						class ThrowDtorGenInnerException final : public TestClass<ThrowDtorGenInnerException> {
							virtual bool Excecute() override;
						};
					}

					class StoringException final : public TestClass<StoringException> {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}