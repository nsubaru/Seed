#pragma once
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Tuple.h"

#pragma warning(push)
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		class SeedKernelCall Tokenizer final {
		private:
			struct Data;
		public:
			class Token;

			class TokenView {
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

				operator String() const {
					return String(This.Text->GetCharStr() + This.OffSet, This.Size);
				}

				const String& GetDelimiter() {
					return This.Text->data->Delims[This.DelimIndex];
				}
			};

			class SeedKernelCall Token final {
				Tokenizer* Text;
				SizeT OffSet;
				SizeT Size;
				SizeT DelimIndex;

			public:
				friend class Tokenizer;
				friend class TokenView;
				friend struct Tokenizer::Data;
			private:
				Token(Tokenizer* Text, SizeT OffSet, SizeT Size, SizeT DelimIndex)
					:Text(Text), OffSet(OffSet), Size(Size), DelimIndex(DelimIndex)
				{}
			public:
				Token(const Token& orig) : Text(new Tokenizer(*orig.Text)), OffSet(orig.OffSet), Size(orig.Size), DelimIndex(orig.DelimIndex) {};
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

					extern Containers::Tuple<SizeT, SizeT, SizeT> GetNextToken(CharStr Text, SizeT Size, CharStr * Delims, SizeT * DelimSizes, SizeT DelimsCount, SizeT StartSize);

					auto [NewOffSet, NewSize, NewDelimIndex] = //-V688
						GetNextToken(
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
			};

			friend class Token;
			friend class TokenView;
		private:
			struct Data {
				union {
					struct ParentTokenData {
						Token ParentToken;
						CharStr RawText;
						SizeT RawTextSize;

						ParentTokenData(const Token& ParentToken, CharStr RawText, SizeT RawTextSize)
							: ParentToken(ParentToken), RawText(RawText), RawTextSize(RawTextSize) {}
					};

					ParentTokenData ParentToken;
					String Text;
				};

				enum class DataTypes {
					ParentToken, String
				};

				DataTypes DataType;

				Containers::Array<String> Delims;
				Containers::Array<CharStr> DelimsRaw;
				Containers::Array<SizeT> DelimsSize;

				SizeT RefCount;

				Data(const String& Text, const Containers::Array<String>& Delims) :
					DataType(DataTypes::String),
					Text(Text), Delims(Delims),
					DelimsRaw(Delims.Count),
					DelimsSize(Delims.Count),
					RefCount(1)
				{
					LoadCachesData();
				}

				Data(const Token& ParentToken, const Containers::Array<String>& Delims) :
					DataType(DataTypes::ParentToken),
					ParentToken(ParentToken, ParentToken.Text->GetCharStr(), ParentToken.Text->GetCharStrSize()),
					Delims(Delims),
					DelimsRaw(Delims.Count),
					DelimsSize(Delims.Count),
					RefCount(1)
				{
					LoadCachesData();
				}

				forceinline void LoadCachesData() {
					for (SizeT i = 0; i < Delims.Count; i++)
					{
						DelimsRaw[i] = const_cast<CharStr>(static_cast<ConstCharStr>(Delims[i]));
						DelimsSize[i] = Delims[i].Size;
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
			//struct TokenPos { SizeT OffSet; SizeT Size; };

			//friend auto GetNextToken(CharStr Text, SizeT Size, CharStr* Delims, SizeT* DelimSizes, SizeT DelimsCount, SizeT StartSize = 0);

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
					return data->Text.Size;
				case Data::DataTypes::ParentToken:
					return data->ParentToken.RawTextSize;
				}
				__assume(false);
			}
		public:
			Tokenizer(const String& Text, const Containers::Array<String>& Delims) : data(new Data(Text, Delims)) {}
			Tokenizer(const Token& ParentToken, const Containers::Array<String>& Delims) : data(new Data(ParentToken, Delims)) {}

			Token begin() {
				extern Containers::Tuple<SizeT, SizeT, SizeT> GetNextToken(CharStr Text, SizeT Size, CharStr * Delims, SizeT * DelimSizes, SizeT DelimsCount, SizeT StartSize);

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
					return Token(new Tokenizer(*this), OffSet, Size, DelimIndex);
			}

			Token end() {
				return Token(nullptr, Math::Max<SizeT>(), Math::Max<SizeT>(), Math::Max<SizeT>());
			}
		};
	}
}

#pragma warning(pop)
