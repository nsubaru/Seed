#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			class IExceptionIOCallback : protected IDisposable {
			public:
				virtual void OnError() = 0;
			public:
				virtual ~IExceptionIOCallback() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IFinishIOReadCallback : public IExceptionIOCallback {
			public:
				virtual void OnReadFinish(TDataType&& data) volatile = 0;
			private:
				virtual ~IFinishIOReadCallback() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IFinishWriteCallback : public IExceptionIOCallback {
			public:
				virtual void OnWriteFinish(const TDataType&& data) volatile = 0;
			private:
				virtual ~IFinishWriteCallback() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IAsyncIO : protected IDisposable {
			public:
				virtual void AsyncRead(TDataType&& data, SmartPtr<IFinishIOReadCallback<TDataType>> onFinishReadAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SmartPtr<IExceptionIOCallback<TDataType>> onErrorAction) = 0;
			public:
				virtual void IsValid() = 0;
			public:
				virtual ~IAsyncIO() = 0 {};
			};

			template<typename TDataType = Containers::Blob>
			class IPositionAsyncIO : protected IDisposable {
			public:
				virtual void AsyncRead(TDataType&& data, SizeT position, SmartPtr<IFinishIOReadCallback<TDataType>> onFinishReadAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IExceptionIOCallback<TDataType>> onErrorAction) = 0;
			public:
				virtual void IsValid() = 0;
			public:
				virtual ~IAsyncIO() = 0 {};
			};
		}
	}
}