// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.MetaDataGen.h"

bool Seed::Core::Testing::Test::Serilization::ComplexMetaDataGen::Excecute() {
	auto metaData = NewSerilization::MetaDataGen<
		Containers::Tuple<
			Containers::Dictionary<
				Containers::List<
					Containers::FixedArray<10, Int32>
				>
			>, 
			Containers::PriorityQueue<
				Int32, 
				String
			>, 
			Boolean
		>
	>();

	return metaData.Size != 0;
}

bool Seed::Core::Testing::Test::Serilization::CustomStructMetaDataGen::Excecute() {
	auto metaData = NewSerilization::MetaDataGen<NewCTTI::TypeTraits::SerilizableClass>();

	return metaData.Size != 0;
}
