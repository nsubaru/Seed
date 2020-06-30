#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					namespace Array
					{
						namespace NonConcurency
						{
							class ForEachTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class ForEachFromTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class ForEachFromToTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class AgregateSimpleItemsTypeTest : public TestClass<AgregateSimpleItemsTypeTest> {
								virtual bool Excecute() override;
							};

							class AgregateConvertionsItemsTypeTest : public TestClass<AgregateSimpleItemsTypeTest> {
								virtual bool Excecute() override;
							};

							class AnyEmpty : public TestClass<AnyEmpty> {
								virtual bool Excecute() override;
							};

							class AnyWithItem : public TestClass<AnyWithItem> {
								virtual bool Excecute() override;
							};

							class AnyWithCondition : public TestClass<AnyWithCondition> {
								virtual bool Excecute() override;
							};

							class AllWithCondition : public TestClass<AllWithCondition> {
								virtual bool Excecute() override;
							};

							class CloneArrayWithPods : public TestClass<CloneArrayWithPods> {
								virtual bool Excecute() override;
							};

							class CloneArrayWithClonableItems : public TestClass<CloneArrayWithClonableItems> {
								virtual bool Excecute() override;
							};

							class ResizeArray : public TestClass<ResizeArray> {
								virtual bool Excecute() override;
							};

							class FirstCondition : public TestClass<FirstCondition> {
								virtual bool Excecute() override;
							};

							class LastCondition : public TestClass<LastCondition> {
								virtual bool Excecute() override;
							};
						}

						namespace Concurency
						{
							class ForEachTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class ForEachFromTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class ForEachFromToTest : public TestClass<ForEachTest> {
								virtual bool Excecute() override;
							};

							class AgregateSimpleItemsTypeTest : public TestClass<AgregateSimpleItemsTypeTest> {
								virtual bool Excecute() override;
							};

							class AgregateConvertionsItemsTypeTest : public TestClass<AgregateSimpleItemsTypeTest> {
								virtual bool Excecute() override;
							};

							class AnyEmpty : public TestClass<AnyEmpty> {
								virtual bool Excecute() override;
							};

							class AnyWithItem : public TestClass<AnyWithItem> {
								virtual bool Excecute() override;
							};

							class AnyWithCondition : public TestClass<AnyWithCondition> {
								virtual bool Excecute() override;
							};

							class AllWithCondition : public TestClass<AllWithCondition> {
								virtual bool Excecute() override;
							};

							class CloneArrayWithPods : public TestClass<CloneArrayWithPods> {
								virtual bool Excecute() override;
							};

							class CloneArrayWithClonableItems : public TestClass<CloneArrayWithClonableItems> {
								virtual bool Excecute() override;
							};

							class ResizeArray : public TestClass<ResizeArray> {
								virtual bool Excecute() override;
							};

							class FirstCondition : public TestClass<FirstCondition> {
								virtual bool Excecute() override;
							};

							class LastCondition : public TestClass<LastCondition> {
								virtual bool Excecute() override;
							};
						}
					}
				}
			}
		}
	}
}