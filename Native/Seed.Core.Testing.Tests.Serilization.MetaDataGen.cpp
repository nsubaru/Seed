// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.MetaDataGen.h"
#include "Seed.Core.Serilization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Reflection.h"

bool Seed::Core::Testing::Test::Serilization::ComplexMetaDataGen::Excecute() {
	auto metaData = Core::Serilization::MetaDataGen<
		Core::Containers::Tuple<
			Core::Containers::Dictionary<
				Core::Containers::List<
					Core::Containers::FixedArray<10, Int32>
				>
			>, 
			Core::Containers::PriorityQueue<
				Int32, 
				String
			>, 
			Boolean
		>
	>();

	constexpr auto MetaDataConstExpr = Core::Serilization::CTTIMetaDataGen<
		Core::Containers::Tuple<
			Core::Containers::Dictionary<
				Core::Containers::List<
					Core::Containers::FixedArray<10, Int32>
				>
			>, 
			Core::Containers::PriorityQueue<
				Int32, 
				String
			>, 
			Boolean
		>
	>();

	return metaData.Lenght != 0;
}

bool Seed::Core::Testing::Test::Serilization::CustomStructMetaDataGen::Excecute() {
	auto metaData = Core::Serilization::MetaDataGen<Core::CTTI::TypeTraits::SerilizableClass>();

	return metaData.Lenght != 0;
}
