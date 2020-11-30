#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Testing
		{
			namespace Test
			{
				namespace API
				{
					namespace Containers
					{
						namespace V2Tuple
						{
							class CreateV2Leafs : public TestClass<CreateV2Leafs> {
								virtual bool Excecute() override;
							};

							class ConstexprCopyV2Leafs : public TestClass<ConstexprCopyV2Leafs> {
								virtual bool Excecute() override;
							};

							class AccessToValuesV2Leafs : public TestClass<AccessToValuesV2Leafs> {
								virtual bool Excecute() override;
							};

							class AcessToValueRefV2Leafs : public TestClass<AcessToValueRefV2Leafs> {
								virtual bool Excecute() override;
							};

							class AcessToValueConstRefV2Leafs : public TestClass<AcessToValueConstRefV2Leafs> {
								virtual bool Excecute() override;
							};

							class CreateV2Tuple : public TestClass<CreateV2Tuple> {
								virtual bool Excecute() override;
							};

							class ConstexprCopyV2Tuple : public TestClass<ConstexprCopyV2Tuple> {
								virtual bool Excecute() override;
							};

							class AccessToValuesV2Tuple : public TestClass<AccessToValuesV2Tuple> {
								virtual bool Excecute() override;
							};

							class AcessToValueRefV2Tuple : public TestClass<AcessToValueRefV2Tuple> {
								virtual bool Excecute() override;
							};

							class AcessToValueConstRefV2Tuple : public TestClass<AcessToValueConstRefV2Tuple> {
								virtual bool Excecute() override;
							};

							class ConstexprCallWithV2Tuple : public TestClass<ConstexprCallWithV2Tuple> {
								virtual bool Excecute() override;
							};

							class ConstextprV2TupleExpand : public TestClass<ConstextprV2TupleExpand> {
								virtual bool Excecute() override;
							};
						}
					}
				}
			}
		}
	}
}
