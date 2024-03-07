#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Exceptions.h"

namespace Seed::Core::RTTI::Record
{
	/// <summary>
	/// A base RTTI type record class
	/// </summary>
	/// <typeparam name="THeap">Heap type</typeparam>
	template<typename THeap>
	class BaseRttiTypeRecord;
}

namespace Seed::Core::Threading
{
    /// <summary>
	/// An object interface on which can be waited asynchronously
	/// </summary>
	VirtualClass IAwaitable {
	protected:
		virtual Handle GetAwaitableHandle() const = 0;
		virtual Exception::RawException GetRawException() { return Exception::RawException(); }

	protected:
		virtual RTTI::Record::BaseRttiTypeRecord<Memory::Heap> GetAwaitableType() const = 0;
	
	public:
		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~IAwaitable() = 0 {};
	
	private:
		friend Handle GetHandleFromIAwaitable(const IAwaitable& awaitable);

		template<typename T, typename TPayload>
		friend struct TaskAwaiter;

		friend class AwaitablePayload;
	};
}