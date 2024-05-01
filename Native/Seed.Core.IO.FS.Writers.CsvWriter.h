#pragma once
#include "Seed.Core.IO.FS.Writers.Base.h"

namespace Seed::Core::IO::FS::Writers
{
    /// <summary>
    /// Check is all fields of struct convertable to string
    /// </summary>
    template<typename T>
    concept CswWriterSupportableType = CTTI::Concepts::IsStructCttiType<T> && CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.Count > 0;

    template<typename T>
    concept IsAllFieldSupportable = CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.FirstType([] <typename U> () constexpr
    {
        return
            Concepts::IsSame<typename U::Type, StringView> ||
            Concepts::IsSame<typename U::Type, String> ||
            Concepts::IsSame<typename U::Type, SensitiveString> ||
            Concepts::IsSame<typename U::Type, SensitiveStringView> ||
            Concepts::IsConvertibleToString<typename U::Type>;
    }) != Math::Max<SizeT>();

    constexpr Char CsvDelimiter = L',';
    constexpr Char CsvDelimiterWithSpace[] = { CsvDelimiter, L' ', L'\0'};

    /// <summary>
    /// Async CSV writer, write each line async
    /// </summary>
    /// <typeparam name="T">Row type</typeparam>
    template<CswWriterSupportableType T>
    class AsyncCsvWriter final: public BaseAsyncWriter<T, AsyncCsvWriter<T>> {
    public:
        static_assert(IsAllFieldSupportable<T>);

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
        AsyncCsvWriter(AsyncFile&& file) : BaseAsyncWriter<T, AsyncCsvWriter<T>>(MoveRValue(file)), This(new Data(String(1Mb))) {}

        virtual Threading::Task<void> Init()
        {
            This->buffer = co_await this->AsyncWriteToFile([&] () -> String {
                CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.ForEachType([&] <typename TField, SizeT N> ()
                {
                    This->buffer.CombineTo(TField::NameGetter());

                    if (CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.Count - 1 > N)
                    {
                        This->buffer.CombineTo(CsvDelimiterWithSpace);
                    }
                });

                This->buffer += NextLine();

                return MoveRValue(This->buffer);
            });
        }

    public:
        /// <summary>
        /// Write row
        /// </summary>
        /// <param name="record">Row data</param>
        virtual Threading::Task<void> AsyncWrite(const T& record)
        {
            This->buffer = co_await this->AsyncWriteToFile([&] () -> String {
                This->buffer.Resize(0);

                CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.ForEachType([&] <typename TField, SizeT N> ()
                {
                    if constexpr (Concepts::IsConvertibleToStringFromString<typename TField::Type>)
                    {
                        This->buffer.CombineTo(TField::Selector(record));
                    }
                    else if constexpr (Concepts::IsSame<typename TField::Type, StringView>)
                    {
                        This->buffer.CombineTo(TField::Selector(record));
                    }
                    else
                    {
                        This->buffer.CombineTo(toSting(TField::Selector(record)));
                    }

                    if (CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.Count - 1 > N)
                    {
                        This->buffer.CombineTo(CsvDelimiterWithSpace);
                    }
                });

                This->buffer += NextLine();

                return MoveRValue(This->buffer);
            });
        }

    public:
        AsyncCsvWriter(const AsyncCsvWriter& writer) : BaseAsyncWriter<T, AsyncCsvWriter<T>>(writer), This(writer.This) { writer.This->RefCount++; }
        AsyncCsvWriter(AsyncCsvWriter&& writer) : BaseAsyncWriter<T, AsyncCsvWriter<T>>(writer), This(writer.This) { writer.This = nullptr; }

    public:
        RefCountInheritedClass(AsyncCsvWriter);

    public:
        template<typename T, typename TWriter>
        friend class BaseAsyncWriter;
    };

    /// <summary>
    /// CSW writer
    /// </summary>
    /// <typeparam name="T">Row type</typeparam>
    template<CswWriterSupportableType T> 
    class CsvWriter: public BaseWriter<T, CsvWriter<T>> {
    public:
        static_assert(IsAllFieldSupportable<T>);

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
        CsvWriter(File&& file): BaseWriter<T, CsvWriter<T>>(MoveRValue(file)), This(new Data(String(1Mb)))
        {
            CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.ForEachType([&] <typename TField, SizeT N> ()
            {
                This->buffer.CombineTo(TField::NameGetter());

                if (CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.Count - 1 > N)
                {
                    This->buffer.CombineTo(CsvDelimiterWithSpace);
                }
            });

            This->buffer += NextLine();

            BaseWriter<T, CsvWriter<T>>::file.Write(This->buffer);
        }

    public:
        /// <summary>
        /// Write row
        /// </summary>
        /// <param name="record">Row data</param>
        virtual void Write(const T& record)
        {
            This->buffer.Resize(0);

            CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.ForEachType([&] <typename TField, SizeT N> ()
            {
                if constexpr (Concepts::IsConvertibleToStringFromString<typename TField::Type>)
                {
                    This->buffer.CombineTo(TField::Selector(record));
                }
                else if constexpr (Concepts::IsSame<typename TField::Type, StringView>)
                {
                    This->buffer.CombineTo(TField::Selector(record));
                }
                else
                {
                    This->buffer.CombineTo(toString(TField::Selector(record)));
                }

                if (CTTI::TypeTraits::CTTITypeMetaData<T>::Fields.Count - 1 > N)
                {
                    This->buffer.CombineTo(CsvDelimiterWithSpace);
                }
            });

            This->buffer += NextLine();

            BaseWriter<T, CsvWriter<T>>::file.Write(This->buffer);
        }

    public:
        RefCountClass(CsvWriter);

    public:
        template<typename T, typename TWriter>
        friend class BaseWriter;
    };
}