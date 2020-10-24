#pragma once
#include "Seed.Core.Threading.h"

#include "Seed.Core.IO.FS.File.h"

namespace Seed {
	inline namespace Core {
		namespace IO::FS {
			template<typename TDataType>
			class BaseAsyncFile;

			namespace Details {
				class BaseAsyncFile final : protected IDisposable {
				public:
					template<typename TDataType>
					friend class Seed::Core::IO::FS::BaseAsyncFile;
				protected:
					Handle hFile;
					File::OpeningMode mode;
					File::OpenDisposition policy;
					bool ShareWithOs;
					String ntFileName;
					Threading::UnifiedThreadPoolIoTask* poolTask;

					RefCountT RefCount;
				protected:
					virtual void Dispose();
				protected:
					BaseAsyncFile(String&& path, File::OpeningMode mode, File::OpenDisposition policy, bool ShareWithOs);
				protected:
					void InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT position, SizeT size, void* ioStatusBlk);
					void InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT position, SizeT size, void* ioStatusBlk);
				protected:
					void DeleteFile();
				protected:
					virtual ~BaseAsyncFile() {}
				};
			}

			template<typename TDataType>
			class BaseAsyncFile final : public IO::IPositionAsyncIO<TDataType> {
			private:
				Details::BaseAsyncFile* This;
			private:
				BaseAsyncFile(Details::BaseAsyncFile* This, const Threading::UnifiedThreadPool& ownerPool) :
					This(This),
					IO::IPositionAsyncIO<TDataType>(ownerPool) {};
			public:
				BaseAsyncFile(const Threading::UnifiedThreadPool& ownerPool, String&& path, File::OpeningMode mode, File::OpenDisposition policy, bool ShareWithOs) :
					This(new Details::BaseAsyncFile(MoveRValue(path), mode, policy, ShareWithOs)
				) {
					This->poolTask = new Threading::UnifiedThreadPoolIoTask(RegisterNtIoObjectInThreadPool(ownerPool, This->hFile));
				}
			protected:
				virtual void Dispose() {
					if (This->poolTask != nullptr) {
						UnRegisterNtIoObjectInThreadPool(*This->poolTask);
						delete This->poolTask;
						This->poolTask = nullptr;
					}
					This->Dispose();
				}
			protected:
				virtual Handle GetHandle() { return This->hFile; }
			public:
				virtual bool IsValid() { return This->hFile != nullptr; }
			public:
				virtual IOTask AsyncRead(TDataType&& data, SizeT position, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction, bool isWaitable = true) override {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));
					IRPBase* irp = nullptr;
					try {
						//Get buffer and its size
						void* buffer = (void*)(const Byte*)data;
						SizeT size = data.Count;
						//create irp packet
						if (isWaitable) {
							Threading::SynchronizeEvent event = Threading::SynchronizeEvent(true);
							irp = new ReadIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishReadAction, MoveRValue(data), event);
						}
						else {
							irp = new ReadIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishReadAction, MoveRValue(data));
						}
						//create task object
						auto task = GetIOTaskFromIRPBase(irp);

						try {
							//notify about arrived io operation
							NotifyIoOperation(*This->poolTask);
							//Perfome os specific read operations
							This->InitAsyncRead(MoveRValue(irp), buffer, position, size, GetReservedPlaceInIrp(irp));
						}
						catch (...) {
							//If read failed cancel notification
							CancelNotifyIoOperation(*This->poolTask);
							//and rethrow exception as we don't how handle it
							ReThrow;
						}

						//return task object
						return task;
					}
					catch (...) {
						if (irp != nullptr)
							delete irp;
						ReThrow;
					}
				}

				virtual IOTask AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction, bool isWaitable = true) override {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));
					IRPBase* irp = nullptr;
					try {
						//Get buffer and its size
						void* buffer = (void*)(const Byte*)data;
						SizeT size = data.Count;
						//create irp packet
						if (isWaitable) {
							Threading::SynchronizeEvent event = Threading::SynchronizeEvent(true);
							irp = new WriteIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishWriteAction, MoveRValue(data), event);
						}
						else {
							irp = new WriteIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishWriteAction, MoveRValue(data));
						}
						//create task object
						auto task = GetIOTaskFromIRPBase(irp);
						
						try {
							//notify about arrived io operation
							NotifyIoOperation(*This->poolTask);
							//Perfome os specific read operations
							This->InitAsyncWrite(MoveRValue(irp), buffer, position, size, GetReservedPlaceInIrp(irp));
						}
						catch (...) {
							//If read failed cancel notification
							CancelNotifyIoOperation(*This->poolTask);
							//and rethrow exception as we don't how handle it
							ReThrow;
						}

						//return task object
						return task;
					}
					catch (...) {
						if (irp != nullptr)
							delete irp;
						ReThrow;
					}
				}
			public:
				void DeleteFile()&& {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));

					//notify about arrived io operation
					NotifyIoOperation(*This->poolTask);

					This->DeleteFile();

					UnRegisterNtIoObjectInThreadPool(*This->poolTask);
					
					delete This->poolTask;
					This->poolTask = nullptr;

					This->Dispose();
				}
			public:
				RefCountClass(BaseAsyncFile, This);
			};

			namespace {
				using AsyncFile = BaseAsyncFile<Containers::Blob>;
			}
		}
	}
}