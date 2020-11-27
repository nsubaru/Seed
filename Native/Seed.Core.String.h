#pragma once
#include "Seed.Core.String.Manipulation.h"
#include "Seed.Core.String.LinqApi.h"
#include "Seed.Core.String.Convertors.h"
#include "Seed.Core.String.HexPart.h"

namespace Seed {
    inline namespace Core {
        inline namespace Exceptions {
            class IntegerOverflowException;
        }
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
        template<class THeap>
        class BaseString final :
            public Memory::Allocator<THeap>, 
            public IClonable<BaseString<THeap>>,
            public Details::TManipulationStringPart<THeap, BaseString>,
            public Details::TLinqApi<THeap, BaseString>,
            public Details::TConvertorsStringPart<THeap, BaseString>,
            public Details::THexStringPart<THeap, BaseString>
            {
        private:
            ///<summary>�������� �� �������� �����</summary>
            CharStr str;
            ///<summary>������� ������� �������</summary>
            SizeT CurrentSize;
            ///<summary>����� ������</summary>
            SizeT ReservedSize;
            
            ///<summary>����������� ���������� ��� ��������� � ������� ������ � ��������� ������� ������ �������</summary>
            ///<params name = "rawPtr">�������� ���� ���� ����������� �� ����� ������� � ������ �����</params>
            ///<params name = "CurrentSize">ʳ������ ������� � ����-�����������</params>
            ///<params name = "ReservedSize">��������� ����� ������ �������</params>
            ForceInline BaseString(CharStr&& rawPtr, SizeT CurrentSize, SizeT ReservedSize)
                :str(rawPtr), CurrentSize(CurrentSize), ReservedSize(ReservedSize) {
                rawPtr = nullptr;
            }
        public:
            ///<summary>������ �����������</summary>
            constexpr ForceInline BaseString() : str(nullptr), CurrentSize(0), ReservedSize(0) {};

            ///<summary>����������� � ������ ������� �����</summary>
            ///<param name = "Size">����� ����� ������� � ����-�����������</param>
            ///<param name = "Text">˳����� �����</param>
            template<SizeT Size>
            ForceInline BaseString(StaticTextRef<Size> Text) : BaseString() {
                if constexpr (!(Size < 2))
                {
                    this->CurrentSize = Size;
                    this->ReservedSize = this->CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](this->ReservedSize * sizeof(Char));
                    Memory::Copy(Text, this->str, this->CurrentSize * sizeof(Char));
                }
            }

            ///<summary>����������� � ������ ������� ASCII �����</summary>
            ///<param name = "Size">����� ����� ������� � ����-�����������</param>
            ///<param name = "Text">˳����� �����</param>
            template<SizeT Size>
            inline BaseString(StaticATextRef<Size> Text) : CurrentSize(0), ReservedSize(0), str(nullptr) {
                if constexpr (!(Size < 2))
                {
                    this->CurrentSize = Size;
                    this->ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](this->ReservedSize * sizeof(Char));
                    
                    CharStr temp_str = this->str;
                    ACharStr text = (ACharStr) Text;
                    {
                        while (*temp_str++ = *text++);
                    }
                    
                    this->str[this->CurrentSize] = L'\0';
                }
            }

            ///<summary>����������� � ������ ������������ �����</summary>
            ///<param name = "Text">˳����� �����</param>
            ForceInline BaseString(const ConstString& Text) {
                if (Text.Lenght < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = Text.Lenght + 1;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(Text.Str, this->str, Text.Lenght * sizeof(Char));
                }
            }

            ///<summary>����������� � ������ ������������ ASCII �����</summary>
            ///<param name = "str">����������� �����</param>
            BaseString(const ConstAString& cstr) {
                if (cstr.Lenght < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = cstr.Lenght + 1;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    CharStr temp_str = this->str;
                    ACharStr text = (ACharStr)cstr.Str;
                    {
                        while (*temp_str++ = *text++);
                    }
                    this->str[CurrentSize] = L'\0';
                }
            }

            ///<summary>����������� � �������� ������ ���� ���� ���������� ����-�����������
            ///<para>���� ��� ����������� ������������</para></summary>
            ///<param name = "Text">����� ����</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">������ ��� �������� ������� ������� � �����</exception>
            InternalApiCall BaseString(Char* text) {
                SizeT size = CStr::Size(text);
                if (size == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = size + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < size; i++) {
                    this->str[i] = text[i];
                }
                this->str[size] = L'\0';
            }

            ///<summary>����������� � �������� ������ ���� ���� ���������� ����-�����������
            ///<para>���� ��� ����������� ������������</para></summary>
            ///<param name = "Text">����� ����</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">������ ��� �������� ������� ������� � �����</exception>
            InternalApiCall BaseString(AChar* text) {
                SizeT size = CStr::Size(text);
                if (size == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = size + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < size; i++) {
                    this->str[i] = text[i];
                }
                this->str[size] = L'\0';
            }

            ///<summary>����������� � �������� ������ ���� ���� ���������� ����-�����������
            ///<para>���� ��� ����������� ������������</para></summary>
            ///<param name = "Text">����� ����</param>
            ///<param name = "Size">ʳ������ ������� ����� � ����-�����������</summary>
            ///<exception cref = "Seed::Exceptions::StringToLong">������ ��� �������� ������� ������� � �����</exception>
            ForceInline InternalApiCall BaseString(const AChar* text, SizeT textSize) {
                if (textSize == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = textSize + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < textSize; i++) {
                    this->str[i] = text[i];
                }
                this->str[textSize] = L'\0';
            }

            ///<summary>����������� � ������ ������� � ���� ������. ����� ���� � ������� ����-���������</summary>
            ///<params name = "rawText">����� �������</params>
            ///<params name = "Size">ʳ������ �������</params>
            ForceInline InternalApiCall BaseString(const CharStr rawText, SizeT Size) :
                CurrentSize(Size + 1),
                ReservedSize(Size + 1 + CurrentSettings().MemoryManager.StringReserveSize),
                str((CharStr)BaseString::operator new[]((Size + 1 + CurrentSettings().MemoryManager.StringReserveSize) * sizeof(Char)))
            {
                Memory::Copy(rawText, this->str, Size * sizeof(Char));
                this->str[Size] = L'\0';
            }

            ///<summary>����������� � ��������� ������������ �������</summary>
            ///<param name = "StartSize">��������� ����� � ������� ������� ��� ����-����������</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">������ ��� �������� ������� ������� � �����</exception>
            ForceInline explicit BaseString(Memory::DataSizeT StartSize) {
                if (StartSize.Bytes == Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, StartSize.Bytes);
                if (StartSize.Bytes == 0 || StartSize.Bytes < CurrentSettings().MemoryManager.StringReserveSize)
                    StartSize = Memory::DataSizeT(CurrentSettings().MemoryManager.StringReserveSize);
                this->ReservedSize = (StartSize.Bytes / 2) + 1;
                this->CurrentSize = 1;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * 2);
                this->str[0] = L'\0';
            }

            /// <summary>
            /// ����������� � compile time �����
            /// </summary>
            /// <param name="item">Compile time �����</param>
            /// <typeparam name="N">ʳ������ �������</typeparam>
            template<SizeT N>
            ForceInline BaseString(const CTTI::Details::ConstexprArray<Char, N>& item) : CurrentSize(0), ReservedSize(0), str(nullptr) {
                if constexpr (N == 0)
                {
                    return;
                }
                else
                {
                    this->CurrentSize = N + 1;
                    for (SizeT i = 0; i < N; i++)
                    {
                        if (item[i] == '\0')
                        {
                            this->CurrentSize = i + 1; break;
                        }
                    }
                    
                    this->ReservedSize = this->CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr) BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(item.Raw(), this->str, this->CurrentSize * sizeof(Char));
                    //for (SizeT i = 0; i < this->CurrentSize; i++)
                    //{
                    //    str[i] = item[i];
                    //}

                    this->str[this->CurrentSize - 1] = L'\0';
                }
            }

            ///<summary>����������� �����������</summary>
            ///<remarks>������� ���'��� ���� ��� ������</remarks>
            ///<param name = "from">�������� �����</param>
            ForceInline BaseString(const BaseString& from) {
                if (from.str != nullptr) {
                    this->CurrentSize = from.CurrentSize;
                    this->ReservedSize = from.ReservedSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(from.str, this->str, from.CurrentSize * sizeof(Char));
                }
                else {
                    this->CurrentSize = this->ReservedSize = 0;
                    this->str = nullptr;
                }
            }
            ///<summary>������������ �����������</summary>
            ///<param name = "from">RValue ����� �� ��������</param>
            ForceInline BaseString(BaseString&& from) {
                this->str = from.str; from.str = nullptr;
                this->CurrentSize = from.CurrentSize; from.CurrentSize = 0;
                this->ReservedSize = from.ReservedSize; from.ReservedSize = 0;
            }
#pragma region Comparing
#pragma region With Strings
            ///<summary>�������� ������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            ForceInline Boolean operator==(const BaseString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>�������� ��������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            ForceInline Boolean operator!=(const BaseString& second) const {
                return !this->operator==(second);
            }
#pragma endregion
#pragma region With Char
            ///<summary>�������� ������ � �������� ��������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(StaticTextRef<Size> second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>�������� �������� � �������� ��������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            template<SizeT Size>
            ForceInline Boolean operator!=(StaticTextRef<Size> second) const {
                return !this->operator==<Size>(second);
            }

            ///<summary>�������� ������ � �������� ��������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ������� ������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator==(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>�������� �������� � �������� ��������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ������� ������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator!=(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ASCII char
            ///<summary>�������� ������ � �������� �������� ASCII �������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(StaticATextRef<Size> second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>�������� �������� � �������� �������� ASCII �������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            template<SizeT Size>
            ForceInline Boolean operator!=(StaticATextRef<Size> second) const {
                return !this->operator==<Size>(second);
            }

            ///<summary>�������� ������ � �������� �������� ASCII �������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ������� ������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator==(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>�������� �������� � �������� �������� ASCII �������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ������� ������</param>
            ///<param name = "Size">ʳ������ ������� � ������ ����� � ����-�����������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator!=(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ConstString
            ///<summary>�������� ������ � ����������� ������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            ForceInline Boolean operator==(const ConstString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>�������� �������� � ����������� ������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            ForceInline Boolean operator!=(const ConstString& second) const {
                return !this->operator==(second);
            }

            ///<summary>�������� ������ � ����������� ������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ���������� �����</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            ForceInline friend Boolean operator==(const ConstString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>�������� �������� � ����������� ������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ���������� �����</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            ForceInline friend Boolean operator!=(const ConstString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With ASCII ConstString
            ///<summary>�������� ������ � ����������� ������ ASCII �������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(const ConstAString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>�������� �������� � ����������� ������ ASCII �������</summary>
            ///<param name = "second">������ �������� ���������</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            ForceInline Boolean operator!=(const ConstAString& second) const {
                return !this->operator==(second);
            }

            ///<summary>�������� ������ � ����������� ������ ASCII �������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ����������� �����</param>
            ///<returns><c>True</c> ���� ����� ���</returns>
            ForceInline friend Boolean operator==(const ConstAString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>�������� �������� � ����������� ������ ASCII �������</summary>
            ///<param name = "first">������ �������� ���������</param>
            ///<param name = "second">˳��� �������� ���������. ����������� �����</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            ForceInline friend Boolean operator!=(const ConstAString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With String which have other allocator
            ///<summary>�������� ������ � ������ ���������� � ����� ����</summary>
            ///<param name = "OtherString">���������� ��������� �� ������� ����</param>
            ///<returns><c>True</c> ���� ����� �� ���</returns>
            template<typename OtherBase>
            ForceInline Boolean operator==(const BaseString<OtherBase>& OtherString) const {
                return (*this <=> OtherString) == 0;
            }

            template<typename OtherBase>
            ForceInline Boolean operator!=(const BaseString<OtherBase>& OtherString) const {
                return (*this <=> OtherString) != 0;
            }
#pragma endregion
#pragma endregion
#pragma region Copy operators
            ///<summary>�������� ���������. ����� ����� � ������� ������� ����</summary>
            ///<param name = "orig">�������� ������</param>
            ///<returns>��������� �� ������� ����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///����� ������ ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            BaseString& operator=(const BaseString& orig) {
                if (this->str == orig.str) return *this;
                if (this->ReservedSize >= orig.CurrentSize) {
                    Memory::Copy(orig.str, this->str, orig.CurrentSize * sizeof(Char));
                    this->CurrentSize = orig.CurrentSize;
                }
                else {
                    BaseString::operator delete[](this->str);
                    this->str = nullptr;
                    this->str = (CharStr)BaseString::operator new[](sizeof(Char)* orig.ReservedSize);
                    Memory::Copy(orig.str, this->str, orig.CurrentSize * sizeof(Char));
                    this->CurrentSize = orig.CurrentSize;
                    this->ReservedSize = orig.ReservedSize;
                }
                return *this;
            }

            ///<summary>�������� ������������� ���������</summary>
            ///<param name = "orig">RValue ����� �� ��������</param>
            ///<returns>��������� �� ������� ����</returns>
            ///<remarks>����������� ���������� ��� ��������� �����.
            ///�������� ���� � ���������. � ������� ���� � ��������� �����������</remarks>
            ForceInline BaseString& operator=(BaseString&& orig) {
                if (orig.str == this->str) return *this;
                this->~BaseString();
                this->str = orig.str; orig.str = nullptr;
                this->CurrentSize = orig.CurrentSize; orig.CurrentSize = 0;
                this->ReservedSize = orig.ReservedSize; orig.ReservedSize = 0;
                return *this;
            }
#pragma endregion
#pragma region Comparing

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            template<typename TOtherHeap>
            ForceInline StrongOrdering operator<=>(const BaseString<TOtherHeap>& right) const {
                auto res = Internals::CompareString(this->str, right.str, this->Lenght, right.Lenght);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            template<> 
            ForceInline StrongOrdering operator<=>(const BaseString<THeap>& right) const {
                auto res = Internals::CompareString(this->str, right.str, this->Lenght, right.Lenght);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            template<SizeT TextLenght>
            ForceInline StrongOrdering operator<=>(StaticTextRef<TextLenght> right) const {
                auto res = Internals::CompareString(this->str, (Char*)right, this->Lenght, TextLenght - 1);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            ForceInline StrongOrdering operator<=>(const ConstString& right) const {
                auto res = Internals::CompareString(this->str, (Char*)right.Str, this->Lenght, right.Lenght);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            ForceInline StrongOrdering operator<=>(const ConstAString& right) const {
                auto countedSize = Min(this->Lenght, right.Lenght);

                for (SizeT i = 0; i < countedSize; i++)
                {
                    if (this->str[i] != right[i])
                    {
                        if (this->str[i] > right[i])
                            return StrongOrdering::greater;
                        else
                            return StrongOrdering::less;
                    }
                }

                return (this->Lenght <=> right.Lenght);
            }

            /// <summary>
            /// ������������ �������� ���������
            /// </summary>
            /// <param name="str1">������ �����</param>
            /// <returns>������� ������� ����� ������� �������</returns>
            template<SizeT TextLenght>
            ForceInline StrongOrdering operator<=>(StaticATextRef<TextLenght> right) const {
                auto countedSize = Min(this->Lenght, TextLenght - 1);

                for (SizeT i = 0; i < countedSize; i++)
                {
                    if (this->str[i] != right[i])
                    {
                        if (this->str[i] > right[i])
                            return StrongOrdering::greater;
                        else
                            return StrongOrdering::less;
                    }
                }

                return this->Lenght <=> TextLenght - 1;
            }
#pragma endregion
#pragma region Add operators
#pragma region Literals
            ///<summary>�������� ��������� ��������� �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<param name = "Size">ʳ������ ������� ������� � ����-�����������</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            template<SizeT Size>
            ForceInline BaseString operator+(StaticTextRef<Size> Text) const {
                if constexpr (Size < 2)
                    return *this;

                BaseString retData;
                retData.CurrentSize = this->Lenght + Size;
                retData.ReservedSize = retData.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* retData.ReservedSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(Text, retData.str + this->Lenght, Size * sizeof(Char));
                }
                else {
                    Memory::Copy(Text, retData.str, Size * sizeof(Char));
                }

                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>�������� ��������� ��������� �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<param name = "Size">ʳ������ ������� ������� � ����-�����������</param>
            ///<param name = "Right">������ �������, �� ���� ������� �� ���� �����</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            template<SizeT Size>
            ForceInline friend BaseString operator+(StaticTextRef<Size> Text, const BaseString& Right) {
                if constexpr (Size < 2)
                    return Right;

                BaseString retData(Text);
                retData += Right;

                return retData;
            }

            ///<summary>�������� ��������� ��������� ������� ASCII �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<param name = "Size">ʳ������ ������� ������� � ����-�����������</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            template<SizeT Size>
            BaseString operator+(StaticATextRef<Size> Text) const {
                if constexpr (Size < 2)
                    return *this;

                BaseString retData;
                retData.CurrentSize = this->Lenght + Size;
                retData.ReservedSize = retData.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char) * retData.ReservedSize); 

                if (this->str != nullptr)
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                CharStr tempStr = retData.str + this->CurrentSize;

                ACharStr second = (ACharStr)Text;
                {
                    while (*tempStr++ = *second++);
                }

                return retData;
            }

            ///<summary>�������� ��������� ��������� ������� ASCII �������</summary>
            ///<param name = "Text">�������� ������ ASCII �������</param>
            ///<param name = "Size">ʳ������ ������� ������� � ����-�����������</param>
            ///<param name = "Right">������ �������, �� ���� ������� �� ���� �����</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            template<SizeT Size>
            ForceInline friend BaseString operator+(StaticATextRef<Size> Text, const BaseString& Right) {
                if constexpr (Size < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }
#pragma endregion
#pragma region Const String
            ///<summary>�������� ��������� ��������� �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            BaseString operator+(const ConstString& Text) const {
                if (Text.Lenght < 1)
                    return *this;

                BaseString retData;
                SizeT NewSize = this->Lenght + Text.Lenght + 1;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(Text.Str, retData.str + this->Lenght, Text.Lenght * sizeof(Char));
                }
                else {
                    Memory::Copy(Text.Str, retData.str, Text.Lenght * sizeof(Char));
                }

                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>�������� ��������� ��������� �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<param name = "Right">������ �������, �� ���� ������� �� ���� �����</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            ForceInline friend BaseString operator+(const ConstString& Text, const BaseString& Right) {
                if (Text.Lenght < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }

            ///<summary>�������� ��������� ��������� ������� ASCII �������</summary>
            ///<param name = "Text">�������� ������</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            BaseString operator+(const ConstAString& Text) const {
                if (Text.Lenght < 1)
                    return *this;

                BaseString retData;
                retData.CurrentSize = this->Lenght + Text.Lenght + 1;
                retData.ReservedSize = retData.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* retData.ReservedSize);

                if (this->str != nullptr)
                    Memory::Copy(this->str, retData.str, (this->Lenght) * sizeof(Char));

                CharStr tempStr = retData.str + this->CurrentSize;

                ACharStr second = (ACharStr)Text.Str;
                {
                    while (*tempStr++ = *second++);
                }

                return retData;
            }

            ///<summary>�������� ��������� ��������� ������� ASCII �������</summary>
            ///<param name = "Text">�������� ������ ASCII �������</param>
            ///<param name = "Right">������ �������, �� ���� ������� �� ���� �����</param>
            ///<returns>����� ����� ���� � ����� ��������� � ��������� �������</returns>
            ForceInline friend BaseString operator+(const ConstAString& Text, const BaseString& Right) {
                if (Text.Lenght < 2)
                    return Right;

                BaseString retData(Text);
                retData += Right;

                return retData;
            }
#pragma endregion
            ///<summary>�������� ��������� ���� �����</summary>
            ///<param name = "second">������ �������� ��'���</param>
            ///<returns>���� �������� �����</returns>
            BaseString<THeap> operator+(const BaseString& second) const {
                if (this->CurrentSize == 0)
                    return second;

                SizeT NewSize = this->Lenght;
                if (second.CurrentSize != 0)
                    NewSize += second.Lenght + 1;
                else
                    return *this;

                BaseString retData;
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* retData.ReservedSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(second.str, retData.str + this->Lenght, second.CurrentSize * sizeof(Char));
                }
                else {
                    Memory::Copy(second.str, retData.str, second.CurrentSize * sizeof(Char));
                }

                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>�������� ��������� ���� ����� � ������ �����������</summary>
            ///<param name = "second">������ �������� ��'���</param>
            ///<returns>���� �������� ����� � ���������� ����� ��������</returns>
            template<typename OtherBase>
            BaseString<THeap> operator+(const BaseString<OtherBase>& second) const {
                if (this->CurrentSize == 0)
                    return second;
                SizeT NewSize = this->Lenght;
                if (second.Lenght != 0)
                    NewSize += second.Lenght + 1;
                else
                    return *this;

                BaseString retData;
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString<THeap>::operator new[](sizeof(Char)* retData.ReservedSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, (this->Lenght) * sizeof(Char));
                    Memory::Copy(second.operator const Seed::ConstCharStr(), retData.str + this->Lenght, second.Size.Bytes);
                }
                else {
                    Memory::Copy(second.operator const Seed::ConstCharStr(), retData.str, second.Lenght * sizeof(Char));
                }

                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>�������� ������������� ������ ���� �� ��������� �����</summary>
            ///<param name = "Text">�������� ������ ���� ���������� � ����� ��������� �����</param>
            ///<param name = "StrSize">ʳ������ ������� � ��������� ������</param>
            ///<returns>����� �� ���� �������� ���� � ����� ���� �����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///����� ������  ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            template<SizeT StrSize>
            BaseString<THeap>& operator+=(StaticTextRef<StrSize> Text) {
                SizeT NewSize = this->Lenght + StrSize;
                if (NewSize > ReservedSize) {
                    CharStr buf = nullptr;

                    try
                    {
                        SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                        buf = (CharStr) BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                        if (this->str != nullptr)
                        {
                            Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                            Memory::Copy(Text, buf + this->Lenght, StrSize * sizeof(Char));
                        }
                        else
                        {
                            Memory::Copy(Text, buf, StrSize * sizeof(Char));
                        }
                        BaseString::operator delete[](this->str);
                        this->str = buf;
                        CurrentSize = NewSize;
                        this->str[CurrentSize - 1] = L'\0';
                        ReservedSize = NewFullSize;
                    }
                    catch (...)
                    {
                        BaseString<THeap>::operator delete[] (buf);
                        Internals::Exceptions::__ReThrow();
                    }
                }
                else {
                    Memory::Copy(Text, this->str + this->Lenght, StrSize * sizeof(Char));
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>�������� ������������� ������� �� ��������� �����</summary>
            ///<param name = "Text">���������� ������ ���� ���������� � ����� ��������� �����</param>
            ///<returns>����� �� ���� �������� ���� � ����� ���� �����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///������ ������  ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            BaseString<THeap>& operator+=(Char Text) {
                SizeT NewSize = this->Lenght + 2;
                if (NewSize > ReservedSize)
                {
                    CharStr buf = nullptr;
                    try
                    {
                        SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                        buf = (CharStr) BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize);
                        if (this->str != nullptr)
                        {
                            Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                            buf[this->Lenght] = Text;
                        }
                        else
                        {
                            buf[0] = Text;
                        }
                        BaseString::operator delete[](this->str);
                        this->str = buf;
                        CurrentSize = NewSize;
                        this->str[CurrentSize - 1] = L'\0';
                        ReservedSize = NewFullSize;
                    }
                    catch (...)
                    {
                        BaseString<THeap>::operator delete[](buf);
                        Internals::Exceptions::__ReThrow();
                    }
                }
                else {
                    this->str[this->Lenght] = Text;
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>�������� ������������� ������ ���� �� ��������� �����</summary>
            ///<param name = "Text">�������� ������ ���� ���������� � ����� ��������� �����</param>
            ///<param name = "StrSize">ʳ������ ������� � ��������� ������</param>
            ///<returns>����� �� ���� �������� ���� � ����� ���� �����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///����� ������  ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            template<SizeT StrSize>
            BaseString<THeap>& operator+=(StaticATextRef<StrSize> Text) {
                SizeT NewSize = this->Lenght + StrSize;
                if (NewSize > ReservedSize)
                {
                    CharStr buf = nullptr;
                    try
                    {
                        SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                        buf = (CharStr) BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                        if (this->str != nullptr)
                        {
                            Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                        }
                        CharStr tempStr = this->str + this->CurrentSize;
                        ACharStr second = (ACharStr) Text;
                        {
                            while (*tempStr++ = *second++);
                        }
                        BaseString::operator delete[](this->str);
                        this->str = buf;
                        CurrentSize += StrSize;
                        ReservedSize = NewFullSize;
                    }
                    catch (...)
                    {
                        BaseString<THeap>::operator delete[](buf);
                        Internals::Exceptions::__ReThrow();
                    }
                }
                else {
                    CharStr tempStr = this->str + this->Lenght;
                    ACharStr second = (ACharStr)Text;
                    {
                        while (*tempStr++ = *second++);
                    }
                    CurrentSize = NewSize;
                }
                return *this;
            }

            ///<summary>�������� ������������� ����� �� ��������� �����</summary>
            ///<param name = "second">����� ���� ������ �� ���������</param>
            ///<returns>����� �� ���� �������� ���� � ����� ���� �����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///����� ������  ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            BaseString& operator+=(const BaseString& second) {
                if (second.Lenght == 0)
                    return *this;

                SizeT NewSize = this->Lenght + second.Lenght + 1;
                if (NewSize < this->ReservedSize) {
                    Memory::Copy(second.str, this->str + this->Lenght, second.Size.Bytes);
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                    return *this;
                }
                else {
                    ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)* ReservedSize);
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Size.Bytes);
                        Memory::Copy(second.str, buf + this->Lenght, second.Size.Bytes);
                    }
                    else {
                        Memory::Copy(second.str, buf, second.Size.Bytes);
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    this->CurrentSize = NewSize;
                }
                return *this;
            }
            ///<summary>�������� ������������� ����� �� ��������� �����</summary>
            ///<param name = "orig">����� � ����� ����������</summary>
            ///<param name = "OtherBase">�������� ������� �����</param>
            ///<returns>����� �� ���� �������� ���� � ����� ���� �����</returns>
            ///<remarks>���� ����� �������� � ����� � ������� ����� �� �������� ���'�� �� ����������.
            ///����� ������  ��������� �� �������. ���� �� �������� �� ���������� �������� ���'��.</remarks>
            template<typename OtherBase>
            BaseString& operator+=(const BaseString<OtherBase>& orig) {
                if (orig.Lenght == 0)
                    return *this;

                SizeT NewSize = this->Lenght + orig.Lenght + 1;
                if (NewSize < this->ReservedSize) {
                    Memory::Copy(orig.operator const Seed::ConstCharStr(), this->str + this->Lenght, orig.Lenght * sizeof(Char));
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                    return *this;
                }
                else {
                    ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;;
                    CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)* ReservedSize);
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Size.Bytes);
                        Memory::Copy(orig.operator const Seed::ConstCharStr(), buf + this->Lenght, orig.Size.Bytes);
                    }
                    else {
                        Memory::Copy(orig.operator const Seed::ConstCharStr(), buf, orig.Size.Bytes);
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>�������� ������� �� ������� �����</summary>
            ///<param name = "Index">������ �������� � ���� �� ����-���������� �� �������</param>
            ///<returns>����� �� ������� �����</returns>
            ForceInline Char& operator[](SizeT index) {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }
            ///<summary>���������� �������� ������� �� ������� �����
            ///<param name = "Index">������ �������� � ���� �� ����-���������� �� �������</param>
            ///<returns>���������� ����� �� ������� �����</returns>
            ForceInline const Char& operator[](SizeT index) const {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }

            ///<summary>����� �� ������ ���������� � ������ ExpectedString</summary>
            ///<param name = "ExpectedString">����� � ����� �� ���������� �������� �����</param>
            ///<results><c>True</c> ���� ����� �������� ����� ���������� ExpectedString</results>
            ForceInline bool IsStartWith(const BaseString& ExpectedString) const {
                if (this->CurrentSize < ExpectedString.CurrentSize) return false;
                for (SizeT i = 0; i < ExpectedString.CurrentSize; i++)
                    if (this->str[i] != ExpectedString.str[i]) return false;
                return true;
            }

            ///<summary>����� �� ������ ���������� � ������ ExpectedString</summary>
            ///<param name = "ExpectedString">����� � ����� �� ���������� �������� �����</param>
            ///<results><c>True</c> ���� ����� �������� ����� ���������� ExpectedString</results>
            template<SizeT ExpectedStringSize>
            ForceInline bool IsStartWith(StaticTextRef<ExpectedStringSize> ExpectedString) const {
                if (this->CurrentSize < ExpectedStringSize) return false;
                for (SizeT i = 0; i < ExpectedStringSize - 1; i++) {
                    if (this->str[i] != ExpectedString[i]) return false;
                }
                return true;
            }

            ///<summary>�������� ����������� �� ����������� ������</summary>
            ///<returns>����������� �����</returns>
            ForceInline explicit operator const ConstCharStr() const { return str; }

            ///<summary>����� ������� ������� �����</summary>
            ForceInline SizeT GetLenght() const { return (CurrentSize > 0) ? CurrentSize - 1 : 0; }
            ///<summary>������� ������� �����</summary>
            SizeT Property(GetLenght) Lenght;

            /// <summary>
            /// ����� ������� ������� ����� � ����-�����������
            /// </summary>
            ForceInline SizeT GetFullLenght() const {
                return CurrentSize;
            }

            /// <summary>
            /// ������� ������� ����� � ����-�����������
            /// </summary>
            SizeT Property(GetFullLenght) FullLenght;

            ///<summary>����� ������ ���� ����� ������</summary>
            ForceInline Boolean GetIsEmpty() const { return GetFullLenght() == 0; }
            ///<summary>����� ������ ���� ����� ������</summary>
            Boolean Property(GetIsEmpty) IsEmpty;

            /// <summary>
            /// ����� �����
            /// </summary>
            /// <returns>����� �����</returns>
            Memory::DataSizeT GetSize() const { return Memory::DataSizeT(CurrentSize * sizeof(Char)); }
            /// <summary>
            /// ����� ����� ������ �����
            /// </summary>
            Memory::DataSizeT GetBufferSize() const { return Memory::DataSizeT(ReservedSize * sizeof(Char)); }

            /// <summary>
            /// ����� ������ �����
            /// </summary>
            Memory::DataSizeT Property(GetSize) Size;

            /// <summary>
            /// ����� ������
            /// </summary>
            Memory::DataSizeT Property(GetBufferSize) BufferSize;

            ///<summary>����������� � ����� ����. ���� ����� ������ ����� ������ �����</summary>
            ///<returns>����� ���� ���� � ��ﳺ� ����� �����</returns>
            operator Containers::BaseArray<THeap, Byte>() const {
                if (this->str == nullptr || this->CurrentSize == 0)
                    return Containers::BaseArray<THeap, Byte>();
                CharStr data = (CharStr)BaseString::operator new[](this->CurrentSize * sizeof(Char));
                Memory::Copy(this->str, data, CurrentSize * sizeof(Char));
                return Containers::BaseArray<THeap, Byte>(MoveRValue(reinterpret_cast<Byte*>(data)), this->CurrentSize * sizeof(Char));
            }

            /// <summary>
            /// ������ �� ������� ������� ����� ������ ����� � ������
            /// </summary>
            /// <param name="item">������ �����</param>
            /// <returns>true ���� ����� � item �������� � ������� �����</returns>
            bool Containe(const BaseString& item) const {
                if (item.Lenght == this->Lenght && this->Lenght == 0)
                    return true;

                if (item.CurrentSize > this->CurrentSize)
                    return false;

                if (item.CurrentSize == this->CurrentSize)
                    return *this == item;

                auto searchSize = (this->CurrentSize - item.CurrentSize) + 1;
                for (SizeT i = 0; i < searchSize; i++)
                {
                    if (this->str[i] != item.str[0])
                    {
                        continue;
                    }

                    bool found = true;
                    for (SizeT j = 0; j < item.CurrentSize; j++)
                    {
                        if (this->str[i + j] != item.str[j])
                        {
                            found = false;
                            break;
                        }
                    }

                    if (found)
                        return true;
                }

                return false;
            }

            ///<summary>��������� ��� ����� � ����� ����������</summary>
            ///<param name = "NewBase">����� ��������</summary>
            ///<returns>���� ����� ��������� ���������� NewBase</returns>
            template<typename NewBase>
            ForceInline operator BaseString<NewBase>() const {
                return BaseString<NewBase>((const CharStr)(ConstCharStr)*this);
            }

            ForceInline virtual BaseString Clone() const override {
                return BaseString(*this);
            }

            ///<summary>������������ ���� ������� a � b</summary>
            ///<params name "a">������ �����</params>
            ///<params name "b">˳��� �����</params>
            friend void Swap(BaseString& a, BaseString& b) {
                Seed::Swap(a.str, b.str);
                Seed::Swap(a.CurrentSize, b.CurrentSize);
                Seed::Swap(a.ReservedSize, b.ReservedSize);
            }

            ///<summary>����������</summary>
            ForceInline virtual ~BaseString() override {
                if (str == nullptr) return;
                BaseString::operator delete[](str);
                str = nullptr;
                CurrentSize = ReservedSize = 0;
            }

            template<typename TOtherHeap>
            friend class BaseString;

            //String class parts

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TManipulationStringPart;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TLinqApi;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TConvertorsStringPart;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::THexStringPart;
        };

        ///<summary>��� ���� �����</summary>
        namespace { typedef const Char(&EndLineT)[3]; }
        ///<summary>�������� �������� �� ����� �����, ��������������� CRLF �����</summary>
        constexpr ForceInline EndLineT NextLine() {
            return L"\r\n";
        }

        ///<summary>������� �����</summary>
        namespace { using String = BaseString<Memory::Heap>; }
        ///<summary>����� ��� ���������� ������������ ����������</summary>
        namespace { using SensitiveString = BaseString<Memory::SensitiveDataHeap>; }

        namespace TypeTraits {
            namespace Collection {
                template<typename THeap>
                struct IsStringT<BaseString<THeap>> : LogicTrue {};
            }
        }
    }
}

#pragma warning(disable: 4455)

ForceInline Seed::Core::String operator "" S(const wchar_t* const text, const Seed::SizeT size)
{ //-V2537
	using namespace Seed;
	return String((Char*)text); //-V2533
}
    
#pragma warning(pop)