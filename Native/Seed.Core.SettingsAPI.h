#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Settings {
			struct HEAP_CONFIGS {
				UInt8 HEAP_FREE_CHECK : 1;
				UInt8 HEAP_TAIL_CHECK : 1;
				UInt8 HEAP_ARGS_CHECK : 1;
			};

			inline SizeT EXCEPTION_HEAP_START_SIZE();
			inline SizeT EXCEPTION_HEAP_MAX_SIZE();
			inline HEAP_CONFIGS EXCEPTION_HEAP_ADDITIONAL_CONFIGS();

			inline SizeT HEAP_START_SIZE();
			inline SizeT HEAP_MAX_SIZE();
			inline HEAP_CONFIGS HEAP_ADDITIONAL_CONFIGS();

			inline SizeT SECURE_HEAP_START_SIZE();
			inline SizeT SECURE_HEAP_MAX_SIZE();
			inline HEAP_CONFIGS SECURE_HEAP_ADDITIONAL_CONFIGS();
		}
	}
}