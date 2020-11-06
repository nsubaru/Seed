#pragma once
#include "Seed.Core.IO.FS.File.h"

#include "Seed.Core.Threading.h"

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

			/// <summary>
			/// Об'єкт асинхроного файла
			/// </summary>
			/// <typeparam name="TDataType">Тип пакета даних</typeparam>
			template<typename TDataType>
			class BaseAsyncFile final : public IO::IBasePositionAsyncIO<TDataType> {
			private:
				Details::BaseAsyncFile* This;
			private:
				BaseAsyncFile(Details::BaseAsyncFile* This, const Threading::UnifiedThreadPool& ownerPool) :
					This(This),
					IO::IBasePositionAsyncIO<TDataType>(ownerPool) {};
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				/// <param name="ownerPool">Пул який буде обробляти I/O запити</param>
				/// <param name="path">Шлях до файла</param>
				/// <param name="mode">Режим відкриття файла</param>
				/// <param name="policy">Політика відриття файла</param>
				/// <param name="ShareWithOs">Якщо true то файл буде відрити в режимі спільного доступу</param>
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
				virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const {
					return *This->poolTask;
				}
			public:
				/// <summary>
				/// Вертає true якщо об'єкт валідний
				/// </summary>
				/// <returns>true якщо об'єкт валідний</returns>
				virtual bool IsValid() { return This->hFile != nullptr; }
			public:
				/// <summary>
				/// Ініціалізація асинхроної операції читання
				/// </summary>
				/// <param name="data">Буфер даних з його розміром</param>
				/// <param name="position">Позиція з якої почати читати</param>
				/// <param name="onFinishReadAction">Колбек який викличеться після завершеня читання</param>
				/// <returns>Об'єкт I/O операції</returns>
				virtual IOTask AsyncRead(TDataType&& data, SizeT position, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) override {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));

					if (!(This->mode.Read))
						Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->ntFileName));

					IRPBase* irp = nullptr;
					try {
						//Get buffer and its size
						void* buffer = (void*)(const Byte*)data;
						SizeT size = data.Count;
						//create irp packet
						irp = new ReadIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishReadAction, MoveRValue(data));
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

				/// <summary>
				/// Ініціалізація асинхроної операції запису
				/// </summary>
				/// <param name="data">Буфер даних з його розміром</param>
				/// <param name="position">Позиція з якої почати запис</param>
				/// <param name="onFinishReadAction">Колбек який викличеться після завершеня запису</param>
				/// <returns>Об'єкт I/O операції</returns>
				/// <returns></returns>
				virtual IOTask AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) override {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));

					if (!(This->mode.Write || This->mode.WriteThrought))
						Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->ntFileName));

					IRPBase* irp = nullptr;
					try {
						//Get buffer and its size
						void* buffer = (void*)(const Byte*)data;
						SizeT size = data.Count;
						//create irp packet
						irp = new WriteIRP<TDataType>(SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncFile(*this)), onFinishWriteAction, MoveRValue(data));
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
				/// <summary>
				/// Видаляє об'єкт файла
				/// </summary>
				void DeleteFile()&& {
					if (!IsValid())
						Throw(InvalidHandleUsage(ExceptionPosition));

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