#pragma once
#include "Seed.Core.CTTI.h"
#include "Seed.Core.Stage1.h"

namespace Seed {
	inline namespace Core {
		namespace Serlization {

#define StructFieldData(Struct, Name) \
	Seed::Core::TypeTraits::Types::Field<decltype(Struct::Name), GenTypeString(#Name) , StructOffsetOfMember(Struct, Name)>


			class SerilizableClass : Seed::IDisposable, TypeTraits::Types::ISerilizable {
			private:
				struct Data {
					Seed::RefCountT RefCount;
					String Name;
					Containers::Map<String, String> Storage;
					Data() : RefCount(1) {}
				};
			private:
				Data* This;
			public:
				using InternalMetaData = TypeTraits::Types::CustomStruct<Data, true, true, false,
					StructFieldData(Data, Name),
					StructFieldData(Data, Storage)>;
			private:
				virtual Byte* GetDataPtr() const override { return reinterpret_cast<Byte*>(This); }
			protected:
				virtual void Dispose() override {}
			public:
				String Name()& { return This->Name; }
				Containers::Map<String, String> Storage()& { return This->Storage; }
			public:
				SerilizableClass() : This(new Data) {}
			public:
				RefCountClass(SerilizableClass, This);
			};
		}
	}
}