#pragma once
#include "Seed.Core.CTTI.TypeTraits.StructRemoteDefinition.h"
#include "Seed.Core.Time.Base.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeTraits {
				template<>
				struct StructCttiShema<Seed::Core::Time::Date> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Time::Date, 
						DefStructFieldData(Seed::Core::Time::Date, Year),
						DefStructFieldData(Seed::Core::Time::Date, Month),
						DefStructFieldData(Seed::Core::Time::Date, WeekDay),
						DefStructFieldData(Seed::Core::Time::Date, Day),
						DefStructFieldData(Seed::Core::Time::Date, Hour),
						DefStructFieldData(Seed::Core::Time::Date, Minutes),
						DefStructFieldData(Seed::Core::Time::Date, Second),
						DefStructFieldData(Seed::Core::Time::Date, MiliSecond)
					);
				};

				template<>
				struct StructCttiShema<Seed::Core::Time::DateSpan> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Time::DateSpan,
						DefStructFieldData(Seed::Core::Time::DateSpan, Year),
						DefStructFieldData(Seed::Core::Time::DateSpan, Month),
						DefStructFieldData(Seed::Core::Time::DateSpan, Weeks),
						DefStructFieldData(Seed::Core::Time::DateSpan, Day),
						DefStructFieldData(Seed::Core::Time::DateSpan, Hour),
						DefStructFieldData(Seed::Core::Time::DateSpan, Minutes),
						DefStructFieldData(Seed::Core::Time::DateSpan, Second),
						DefStructFieldData(Seed::Core::Time::DateSpan, MiliSecond),
						DefStructFieldData(Seed::Core::Time::DateSpan, Ticks)
					);
				};

				template<>
				struct StructCttiShema<Seed::Core::Time::TimeSpan> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Time::TimeSpan,
						DefStructFieldData(Seed::Core::Time::TimeSpan, Ticks)
					);
				};

				template<>
				struct StructCttiShema<Seed::Core::Time::DateTime> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Time::DateTime,
						DefStructFieldData(Seed::Core::Time::DateTime, Ticks)
					);
				};

				template<>
				struct StructCttiShema<Seed::Core::Time::TimeRange> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Time::DateTime,
						DefStructFieldData(Seed::Core::Time::TimeRange, StartTime),
						DefStructFieldData(Seed::Core::Time::TimeRange, Durration)
					);
				};
			}
		}
	}
}