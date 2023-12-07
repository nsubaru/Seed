#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Exceptions.h"

namespace Seed::Core::Threading
{
    /// <summary>
	/// An object interface on which can be waited asynchronously
	/// </summary>
	VirtualClass IAwaitable {
	protected:
		virtual Handle GetAwaitableHandle() const = 0;
		virtual Exception::RawException GetRawException() { return Exception::RawException(); }
	
	public:
		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~IAwaitable() = 0 {};
	
	private:
		friend Handle GetHandleFromIAwaitable(const IAwaitable& awaitable);

		template<typename T, typename TPayload>
		friend struct TaskAwaiter;
	};
}