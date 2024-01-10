#pragma once
#include "Seed.Core.IO.FS.Writers.Base.h"

namespace Seed::Core::IO::FS::Writers
{
    /// <summary>
    /// Async text writer, write each line in async mode
    /// </summary>
    class AsyncTextWriter final: public BaseAsyncWriter<String, AsyncTextWriter> {
    public:
        static constexpr bool IsTextMode = true;

    private:
        struct Data {
            String buffer;
            RefCountT RefCount;

            Data(String&& buffer) : buffer(buffer), RefCount(1) {}
        };

    private:
        Data* This;

    private:
        virtual void Dispose() {}

    protected:
        AsyncTextWriter(AsyncFile&& file) : BaseAsyncWriter<String, AsyncTextWriter>(MoveRValue(file)), This(new Data(String(1Mb))) {}

    public:
        /// <summary>
        /// Write string plus next line marker
        /// </summary>
        /// <param name="record">String</param>
        virtual Threading::Task<void> AsyncWrite(const String& record)
        {
            This->buffer = co_await this->AsyncWriteToFile([&] () -> String {
                This->buffer.Resize(0);
                This->buffer = record;
                This->buffer += NextLine();
                return MoveRValue(This->buffer);
            });
        }

        /// <summary>
        /// Write args plus next line marker
        /// </summary>
        /// <typeparam name="TArgs">Args types</typeparam>
        /// <param name="args">Args data</param>
        template<typename ... TArgs>
        Threading::Task<void> AsyncWrite(TArgs&&... args)
        {
            This->buffer = co_await this->AsyncWriteToFile([this, &args...] () -> String {
                This->buffer.Resize(0);
                This->buffer.CombineTo(ForwardReference<TArgs>(args)...);
                This->buffer.CombineTo(NextLine());
                return MoveRValue(This->buffer);
            });
        }

    public:
        AsyncTextWriter(const AsyncTextWriter& writer) : BaseAsyncWriter<String, AsyncTextWriter>(writer), This(writer.This) { writer.This->RefCount++; }
        AsyncTextWriter(AsyncTextWriter&& writer) : BaseAsyncWriter<String, AsyncTextWriter>(writer), This(writer.This) { writer.This = nullptr; }

    public:
        RefCountInheritedClass(AsyncTextWriter); 
    
    public:
        template<typename T, typename TWriter>
        friend class BaseAsyncWriter;
    };

    /// <summary>
    /// UTF-16 text writer
    /// </summary>
    class TextWriter: public BaseWriter<String, TextWriter> {
    public:
        static constexpr bool IsTextMode = true;

    private:
        struct Data {
            String buffer;
            RefCountT RefCount;

            Data(String&& buffer): buffer(buffer), RefCount(1) {}
        };

    private:
        Data* This;

    protected:
        virtual void Dispose() {}

    protected:
        TextWriter(File&& file): BaseWriter<String, TextWriter>(MoveRValue(file)), This(new Data(String(1Mb))) {}

    public:
        /// <summary>
        /// Write record plus next line marker
        /// </summary>
        /// <param name="record">Line</param>
        virtual void Write(const String& record)
        {
            This->buffer.Resize(0);
            This->buffer = record; 
            This->buffer += NextLine();

            BaseWriter<String, TextWriter>::file.Write(This->buffer);
        }

        /// <summary>
        /// Write args plus next line marker
        /// </summary>
        /// <typeparam name="TArgs">Args types</typeparam>
        /// <param name="args">Args data</param>
        template<typename ... TArgs>
        void Write(TArgs&&... args)
        {
            This->buffer.Resize(0);
            This->buffer.CombineTo(ForwardReference<TArgs>(args)...);
            This->buffer.CombineTo(NextLine());

            BaseWriter<String, TextWriter>::file.Write(This->buffer);
        }

    public:
        TextWriter(const TextWriter& writer) : BaseWriter<String, TextWriter>(writer), This(writer.This) { writer.This->RefCount++; }
        TextWriter(TextWriter&& writer) : BaseWriter<String, TextWriter>(writer), This(writer.This) { writer.This = nullptr; }

    public:
        RefCountInheritedClass(TextWriter);

    public:
        template<typename T, typename TWriter>
        friend class BaseWriter;
    };
}