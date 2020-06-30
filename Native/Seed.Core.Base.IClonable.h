#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Concepts.h"

namespace Seed
{
    inline namespace Core
    {
        /// <summary>
        /// Інтерфейс клонування
        /// </summary>
        template<typename T>
        class IClonable  // NOLINT
        {
        public:
            /// <summary>
            /// Метод клонування
            /// </summary>
            /// <returns>Вертає повну копію об'єкта</returns>
            virtual T Clone() const = 0;
        };

        namespace Concepts
        {
            template<typename T>
            concept HasCloneMethod = requires(const T & t) {
                t.Clone();
            };

            template<typename T>
            concept InheritClone = IsBasedOn<IClonable<T>, T>;

            template<typename T>
            concept IsClonable = IsPod<T> || InheritClone<T> || HasCloneMethod<T>;
        }
	}
}