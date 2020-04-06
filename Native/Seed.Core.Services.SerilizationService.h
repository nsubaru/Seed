#pragma once
#include "Seed.Core.Stage1.h"
#include "Seed.Core.DepencyInjection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			class SerilizationService final : public IDisposable {
			private:
				struct Data {
					DI::IDI idi;
					RefCountT RefCount;

					Data(const DI::IDI& idi) : idi(idi), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			public:
				SerilizationService(const DI::IDI& idi) : This(new Data(idi)) {}
			public:
				template<typename T>
				Containers::Blob Serelize(const T& data) {
					return Serilization::Serilize(data);
				}

				template<typename T>
				T Deserelize(const Containers::Blob& data) {
					T val = T();
					Serilization::Deserilize(data, val);
					return val;
				}

				template<typename T>
				String GetTypeMetaData() {
					return Serilization::MetaDataGen<T>();
				}

				Containers::Blob SerelizeDynObj(const Reflection::DynamicObject& data);

				Reflection::DynamicObject DeserelizeDynObj(const Containers::Blob& data);

				String GetTypeMetaDataDynObj(const Reflection::DynamicObject& data);

			public:
				RefCountClass(SerilizationService, This);
			};
		}
	}
}