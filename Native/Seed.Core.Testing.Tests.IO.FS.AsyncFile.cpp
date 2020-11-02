#include "Seed.Core.IO.h"

#include "Seed.Core.Testing.Tests.IO.FS.AsyncFile.h"

using namespace Seed;

class FinishWriteCallback : public Core::IO::IFinishWriteCallback<Core::Containers::Blob> {
    struct Data {
        Core::Containers::Blob data;
        Core::Threading::SRWLock lock;
        UInt32 statusCode;
        RefCountT RefCount;

        Data() : data(), statusCode(0), RefCount(1) {}
    };
private:
    Data* This;
protected:
    virtual void Dispose() {}
public:
    FinishWriteCallback() : This(new Data()) {}
public:
    virtual void OnWriteFinish(SizeT byteTransfered, const Core::Containers::Blob&& data) volatile {
        This->lock.ExclusiveLock([&, data = MoveRValue(data)]() mutable {
            This->data = MoveRValue(const_cast<Core::Containers::Blob&&>(data));
        });
    }
    virtual void OnError(UInt32 errorCode) {
        This->lock.ExclusiveLock([&]() mutable {
            This->statusCode = errorCode;
            });
    }
public:
    bool IsDataArrived() const {
        return This->lock.ShareLock([&]() {
            return This->data.Count != 0;
            });
    }

    UInt32 StatusCode() const {
        return This->lock.ShareLock([&]() {
            return This->statusCode;
            });
    }

    const Core::Containers::Blob& GetData() const {
        return This->lock.ShareLock([&]() -> const Core::Containers::Blob& {
            return This->data;
            });
    }
public:
    RefCountClass(FinishWriteCallback, This);
};

class FinishReadCallback final : public Core::IO::IFinishReadCallback<Core::Containers::Blob> {
    struct Data {
        Core::Containers::Blob data;
        Core::Threading::SRWLock lock;
        UInt32 statusCode;
        RefCountT RefCount;

        Data() : data(), statusCode(0), RefCount(1) {}
    };
private:
    Data* This;
protected:
    virtual void Dispose() {}
public:
    FinishReadCallback() : This(new Data()) {}
public:
    virtual void OnFinishRead(SizeT byteTransfered, Core::Containers::Blob&& data) volatile {
        This->lock.ExclusiveLock([&, data = MoveRValue(data)]() mutable {
            This->data = MoveRValue(data);
        });
    }
    virtual void OnError(UInt32 errorCode) {
        This->lock.ExclusiveLock([&]() mutable {
            This->statusCode = errorCode;
            });
    }
public:
    bool IsDataArrived() const {
        return This->lock.ShareLock([&]() {
            return This->data.Count != 0 || This->statusCode != 0;
            });
    }

    UInt32 StatusCode() const {
        return This->lock.ShareLock([&]() {
            return This->statusCode;
            });
    }

    const Core::Containers::Blob& GetData() const {
        return This->lock.ShareLock([&]() -> const Core::Containers::Blob& {
            return This->data;
            });
    }
public:
    RefCountClass(FinishReadCallback, This);
};

bool Seed::Core::Testing::Test::IO::FS::AsyncFile::CreateAsyncFile::Excecute()
{
    {
        auto pool = Core::Threading::UnifiedThreadPoolBuilder()
            .SetName(L"Test thread pool")
            .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetMinThreadCount(1)
            .SetMaxThreadCount(8)
            .CreateThreadPool();
        {
            auto asyncFile = Core::IO::FS::AsyncFile(
                pool, 
                "Test.txt",
                Core::IO::FS::File::WriteMode | Core::IO::FS::File::DeleteMode,
                Core::IO::FS::File::OpenDisposition::OverwriteIf, 
                true);

            MoveRValue(asyncFile).DeleteFile();
        }
    }

    return true;
}

bool Seed::Core::Testing::Test::IO::FS::AsyncFile::WriteAsyncFile::Excecute()
{
    {
        Core::Serilization::TypesRegistry registry;

        auto pool = Core::Threading::UnifiedThreadPoolBuilder()
            .SetName(L"Test thread pool")
            .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetMinThreadCount(1)
            .SetMaxThreadCount(8)
            .CreateThreadPool();
        {
            auto asyncFile = Core::IO::FS::AsyncFile(
                pool, 
                "Test.txt", 
                Core::IO::FS::File::WriteMode | Core::IO::FS::File::DeleteMode, 
                Core::IO::FS::File::OpenDisposition::OverwriteIf, 
                true);

            auto fileList = Core::IO::FS::QueryFilesInDirectory(L".\\");
            auto data = Core::Serilization::Serilize(fileList, registry);
            FinishWriteCallback callback;
            auto task = asyncFile.AsyncWrite(
                MoveRValue(data),
                0,
                SmartPtr<Core::IO::IFinishWriteCallback<Core::Containers::Blob>>((Core::IO::IFinishWriteCallback<Core::Containers::Blob>*&&)new FinishWriteCallback(callback))
                );

            task.WaitComplete();

            MoveRValue(asyncFile).DeleteFile();

            Asserts::AssertTrue(callback.IsDataArrived());
            Asserts::AssertEqual(callback.StatusCode(), 0);
        }
    }

    return true;
}

bool Seed::Core::Testing::Test::IO::FS::AsyncFile::ReadAsyncFile::Excecute()
{
    {
        Core::Serilization::TypesRegistry registry;

        auto pool = Core::Threading::UnifiedThreadPoolBuilder()
            .SetName(L"Test thread pool")
            .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetMinThreadCount(2)
            .SetMaxThreadCount(8)
            .CreateThreadPool();

        {
            
            auto fileList = Core::IO::FS::QueryFilesInDirectory(L".\\");
            auto data = Core::Serilization::Serilize(fileList, registry);

            {
                auto syncFile = Core::IO::FS::File(
                    "Test.txt", 
                    Core::IO::FS::File::WriteMode | Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode, 
                    Core::IO::FS::File::OpenDisposition::OverwriteIf, 
                    true);

                syncFile.Write(data);
                syncFile.Flush();
            }

            auto asyncFile = Core::IO::FS::AsyncFile(
                pool, 
                "Test.txt", 
                Core::IO::FS::File::WriteMode | Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode, 
                Core::IO::FS::File::OpenDisposition::Open, 
                true);

            auto readDataBuffer = Containers::Blob(data.Count);
            FinishReadCallback readCallback;
            auto readTask = asyncFile.AsyncRead(
                MoveRValue(readDataBuffer),
                0,
                SmartPtr<Core::IO::IFinishReadCallback<Core::Containers::Blob>>((Core::IO::IFinishReadCallback<Core::Containers::Blob>*&&)new FinishReadCallback(readCallback))
                );

            readTask.WaitComplete();

            decltype(fileList) readedFileList;

            Asserts::AssertTrue(readCallback.IsDataArrived());
            Asserts::AssertEqual(readCallback.StatusCode(), 0);

            Core::Serilization::Deserilize(readCallback.GetData(), readedFileList, registry);

            Asserts::AssertEqual(fileList.Count, readedFileList.Count);

            auto it1 = fileList.FirstIt;
            auto it2 = readedFileList.FirstIt;

            for (SizeT i = 0; i < fileList.Count; i++)
            {
                Asserts::AssertEqual(it1->Flags, it2->Flags);
                Asserts::AssertEqual(it1->Name, it2->Name);
                Asserts::AssertEqual(it1->Size, it2->Size);
                Asserts::AssertEqual(it1->Type, it2->Type);
            }

            MoveRValue(asyncFile).DeleteFile();
        }
    }

    return true;
}

bool Seed::Core::Testing::Test::IO::FS::AsyncFile::ReadWriteAsyncFile::Excecute()
{
    {
        Core::Serilization::TypesRegistry registry;

        auto pool = Core::Threading::UnifiedThreadPoolBuilder()
            .SetName(L"Test thread pool")
            .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetMinThreadCount(2)
            .SetMaxThreadCount(8)
            .CreateThreadPool();

        {

            auto fileList = Core::IO::FS::QueryFilesInDirectory(L".\\");
            auto data = Core::Serilization::Serilize(fileList, registry);

            auto asyncFile = Core::IO::FS::AsyncFile(
                pool, 
                "Test.txt", 
                Core::IO::FS::File::WriteMode | Core::IO::FS::File::ReadMode | Core::IO::FS::File::DeleteMode, 
                Core::IO::FS::File::OpenDisposition::OverwriteIf, 
                true);

            FinishWriteCallback writeCallback;
            auto writeTask = asyncFile.AsyncWrite(
                MoveRValue(data),
                0,
                SmartPtr<Core::IO::IFinishWriteCallback<Core::Containers::Blob>>((Core::IO::IFinishWriteCallback<Core::Containers::Blob>*&&)new FinishWriteCallback(writeCallback))
                );

            writeTask.WaitComplete();
            
            auto readDataBuffer = Containers::Blob(data.Count);
            FinishReadCallback readCallback;
            auto readTask = asyncFile.AsyncRead(
                MoveRValue(readDataBuffer),
                0,
                SmartPtr<Core::IO::IFinishReadCallback<Core::Containers::Blob>>((Core::IO::IFinishReadCallback<Core::Containers::Blob>*&&)new FinishReadCallback(readCallback))
                );

            readTask.WaitComplete();

            decltype(fileList) readedFileList;

            Asserts::AssertTrue(readCallback.IsDataArrived());
            Asserts::AssertEqual(readCallback.StatusCode(), 0);

            Core::Serilization::Deserilize(readCallback.GetData(), readedFileList, registry);

            Asserts::AssertEqual(fileList.Count, readedFileList.Count);

            auto it1 = fileList.FirstIt;
            auto it2 = readedFileList.FirstIt;

            for (SizeT i = 0; i < fileList.Count; i++)
            {
                Asserts::AssertEqual(it1->Flags, it2->Flags);
                Asserts::AssertEqual(it1->Name, it2->Name);
                Asserts::AssertEqual(it1->Size, it2->Size);
                Asserts::AssertEqual(it1->Type, it2->Type);
            }

            MoveRValue(asyncFile).DeleteFile();
        }
    }

    return true;
}

bool Seed::Core::Testing::Test::IO::FS::AsyncFile::TwoWritesAsyncFile::Excecute()
{
    {
        Core::Serilization::TypesRegistry registry;

        auto pool = Core::Threading::UnifiedThreadPoolBuilder()
            .SetName(L"Test thread pool")
            .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
            .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
            .SetMinThreadCount(1)
            .SetMaxThreadCount(8)
            .CreateThreadPool();
        {
            auto asyncFile = Core::IO::FS::AsyncFile(
                pool, 
                "Test.txt", 
                Core::IO::FS::File::WriteMode | Core::IO::FS::File::DeleteMode,
                Core::IO::FS::File::OpenDisposition::OverwriteIf, 
                true);

            auto fileList = Core::IO::FS::QueryFilesInDirectory(L".\\");
            auto data = Core::Serilization::Serilize(fileList, registry);
            FinishWriteCallback callback;
            auto task = asyncFile.AsyncWrite(
                MoveRValue(data),
                0,
                SmartPtr<Core::IO::IFinishWriteCallback<Core::Containers::Blob>>((Core::IO::IFinishWriteCallback<Core::Containers::Blob>*&&)new FinishWriteCallback(callback))
                );

            task.WaitComplete();

            auto data2 = Core::Serilization::Serilize(fileList, registry);
            auto task2 = asyncFile.AsyncWrite(
                MoveRValue(data2),
                data2.Count,
                SmartPtr<Core::IO::IFinishWriteCallback<Core::Containers::Blob>>((Core::IO::IFinishWriteCallback<Core::Containers::Blob>*&&)new FinishWriteCallback(callback))
                );

            task2.WaitComplete();

            MoveRValue(asyncFile).DeleteFile();

            Asserts::AssertTrue(callback.IsDataArrived());
            Asserts::AssertEqual(callback.StatusCode(), 0);
        }
    }

    return true;
}
