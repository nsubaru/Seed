#pragma once
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Tuple.h"

#pragma warning(push)
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		template<typename THeap>
		class BaseTokenizer final : Memory::Allocator<THeap> {
		private:
			struct Data;
		public:
			class Token;

			class TokenView : Memory::Allocator<THeap> {
				const Token& This;
			public:
				TokenView(const Token& This) : This(This) {}
				TokenView(const TokenView&) = delete;
				TokenView(TokenView&& from) : This(from.This) {}

				TokenView& operator=(const TokenView&) = delete;
				TokenView& operator=(TokenView&& from) {
					this->~TokenView();
					this->TokenView::TokenView(MoveRValue(from));
				}

				const Char& operator[](SizeT Index) const {
					if (This.Size < Index)
						return *(This.Text->GetCharStr() + This.OffSet + Index);
					else
						Throw(Exceptions::IndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, This.Size));
				}

				operator BaseString<THeap>() const {
					return this->GetValue();
				}

				BaseString<THeap> GetValue() const {
					return String(This.Text->GetCharStr() + This.OffSet, This.Size);
				}

				const BaseString<THeap>& GetDelimiter() const  {
					return This.Text->data->Delims[This.DelimIndex];
				}

				SizeT GetValueSize() const {
					return This.Size;
				}
			public:
				BaseString<THeap> Property(GetValue) Value;
				BaseString<THeap> Property(GetDelimiter) Delimiter;
				SizeT Property(GetValueSize) ValueSize;
			};

			class Token final : Memory::Allocator<THeap> {
				BaseTokenizer* Text;
				SizeT OffSet;
				SizeT Size;
				SizeT DelimIndex;

			public:
				friend class BaseTokenizer;
				friend class TokenView;
				friend struct BaseTokenizer::Data;
			private:
				Token(BaseTokenizer* Text, SizeT OffSet, SizeT Size, SizeT DelimIndex)
					:Text(Text), OffSet(OffSet), Size(Size), DelimIndex(DelimIndex)
				{}
			public:
				Token(const Token& orig) : Text(new BaseTokenizer(*orig.Text)), OffSet(orig.OffSet), Size(orig.Size), DelimIndex(orig.DelimIndex) {};
				Token(Token&& from) : Text(from.Text), OffSet(from.OffSet), Size(from.Size), DelimIndex(from.DelimIndex) { from.Text = nullptr; from.OffSet = from.Size = from.DelimIndex = 0; };
				Token& operator=(const Token& orig) {
					this->~Token();
					this->Token::Token(orig);
					return *this;
				}

				Token& operator=(Token&& from) {
					this->~Token();
					this->Token::Token(MoveRValue(from));
					return *this;
				}

				bool operator!=(const Token& other) {
					return this->Text != other.Text &&
						this->OffSet != other.OffSet &&
						this->Size != other.Size;
				}

				TokenView operator*() {
					return GetView();
				}

				TokenView GetView() {
					if (Text != nullptr)
						return TokenView(*this);
					else
						Throw(Exceptions::NullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, NullPointException::Types::Read));
				}

				Token& operator++() {
					if (this->OffSet + this->Size == this->Text->GetCharStrSize()) {
						delete this->Text;
						this->Text = nullptr;
						this->DelimIndex = this->OffSet = this->Size = 0;
						return *this;
					}

					auto [NewOffSet, NewSize, NewDelimIndex] =
						BaseTokenizer::GetNextToken(
							this->Text->GetCharStr(),
							this->Text->GetCharStrSize(),
							(CharStr*)(const CharStr*)this->Text->data->DelimsRaw,
							(SizeT*)(const SizeT*)this->Text->data->DelimsSize,
							this->Text->data->Delims.Count,
							this->OffSet + this->Size + this->Text->data->DelimsSize[this->DelimIndex]
						);

					if (NewOffSet == 0 && NewSize == 0)
					{
						delete this->Text;
						this->Text = nullptr;
						this->DelimIndex = this->OffSet = this->Size = 0;
					}
					else {
						this->OffSet = NewOffSet;
						this->Size = NewSize;
						this->DelimIndex = NewDelimIndex;
					}

					return *this;
				};

				~Token()
				{
					delete Text;
					Text = nullptr;
					OffSet = Size = DelimIndex = 0;
				}
			public:
				TokenView Property(GetView) View;
			};

			friend class Token;
			friend class TokenView;
		private:
			struct Data : Memory::Allocator<THeap> {
				union {
					struct ParentTokenData {
						Token ParentToken;
						CharStr RawText;
						SizeT RawTextSize;

						ParentTokenData(const Token& ParentToken, CharStr RawText, SizeT RawTextSize)
							: ParentToken(ParentToken), RawText(RawText), RawTextSize(RawTextSize) {}
					};

					ParentTokenData ParentToken;
					BaseString<THeap> Text;
				};

				enum class DataTypes {
					ParentToken, String
				};

				DataTypes DataType;

				Containers::Array<BaseString<THeap>> Delims;
				Containers::Array<CharStr> DelimsRaw;
				Containers::Array<SizeT> DelimsSize;

				SizeT RefCount;

				Data(const String& Text, const Containers::Array<BaseString<THeap>>& Delims) :
					DataType(DataTypes::String),
					Text(Text), Delims(Delims),
					DelimsRaw(Delims.Count),
					DelimsSize(Delims.Count),
					RefCount(1)
				{
					LoadCachesData();
				}

				Data(const Token& ParentToken, const Containers::Array<BaseString<THeap>>& Delims) :
					DataType(DataTypes::ParentToken),
					ParentToken(ParentToken, ParentToken.Text->GetCharStr(), ParentToken.Text->GetCharStrSize()),
					Delims(Delims),
					DelimsRaw(Delims.Count),
					DelimsSize(Delims.Count),
					RefCount(1)
				{
					LoadCachesData();
				}

				ForceInline void LoadCachesData() {
					for (SizeT i = 0; i < Delims.Count; i++)
					{
						DelimsRaw[i] = const_cast<CharStr>(static_cast<ConstCharStr>(Delims[i]));
						DelimsSize[i] = Delims[i].Lenght;
					}
				}

				~Data()
				{
					switch (DataType)
					{
					case DataTypes::ParentToken:
						this->ParentToken.~ParentTokenData();
						break;
					case DataTypes::String:
						this->Text.~BaseString();
						break;
					}
				}
			};

			Data* data;
		private:
			static Containers::Tuple<SizeT, SizeT, SizeT> GetNextToken(const CharStr Text, SizeT Size, CharStr* Delims, SizeT* DelimSizes, SizeT DelimsCount, SizeT StartSize)
			{
				for (SizeT i = StartSize + 1; i < Size; i++)
				{
					for (SizeT j = 0; j < DelimsCount; j++)
					{
						if (Text[i] == Delims[j][0] && Size - i > DelimSizes[j]) {
							SizeT Pos = i;
							Boolean Found = true;

							if (DelimSizes[j] > 1) {
								for (i; i < Pos + DelimSizes[j]; i++)
								{
									if (Text[i] != Delims[j][i - Pos]) {
										Found = false;
										break;
									}
								}
							}

							if (Found)
								return Containers::Tuple<SizeT, SizeT, SizeT>(StartSize, i - StartSize, j);
							else
								i = Pos;
						}
					}
				}
				return Containers::Tuple<SizeT, SizeT, SizeT>(StartSize, Size - StartSize, Seed::Math::Max<SizeT>());
			}
		private:
			CharStr GetCharStr() {
				switch (data->DataType) {
				case Data::DataTypes::String:
					return const_cast<CharStr>(static_cast<ConstCharStr>(data->Text));
				case Data::DataTypes::ParentToken:
					return data->ParentToken.RawText;
				}
				__assume(false);
			}

			SizeT GetCharStrSize() {
				switch (data->DataType) {
				case Data::DataTypes::String:
					return data->Text.Size.Bytes;
				case Data::DataTypes::ParentToken:
					return data->ParentToken.RawTextSize;
				}
				__assume(false);
			}
		public:
			BaseTokenizer(const String& Text, const Containers::Array<BaseString<THeap>>& Delims) : data(new Data(Text, Delims)) {}
			BaseTokenizer(const Token& ParentToken, const Containers::Array<BaseString<THeap>>& Delims) : data(new Data(ParentToken, Delims)) {}

			Token begin() {
				auto [OffSet, Size, DelimIndex] =
					GetNextToken(
						GetCharStr(),
						GetCharStrSize(),
						(CharStr*)(const CharStr*)data->DelimsRaw,
						(SizeT*)(const SizeT*)data->DelimsSize,
						data->Delims.Count, 0
					);
				if (OffSet == 0 && Size == 0 && DelimIndex == 0)
					return Token(nullptr, -1, -1, -1);
				else
					return Token(new BaseTokenizer(*this), OffSet, Size, DelimIndex);
			}

			Token end() {
				return Token(nullptr, Math::Max<SizeT>(), Math::Max<SizeT>(), Math::Max<SizeT>());
			}
		public:
			friend class Token;
		};
	}
}

#pragma warning(pop)
