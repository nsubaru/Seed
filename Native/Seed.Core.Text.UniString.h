#pragma once
#include "Seed.Core.String.h"
#include "Seed.Core.ConstString.h"

namespace Seed::Core
{
	/// <summary>
	/// Unified string object
	/// </summary>
	/// <typeparam name="THeap"></typeparam>
	template <typename THeap>
	class BaseUniString final : public Memory::Allocator<THeap> {
	private:
		/// <summary>
		/// Proxy character object for UniString object
		/// </summary>
		class CharProxy final {
		private:
			BaseUniString& ref;
			SizeT index;

		protected:
			ForceInline CharProxy(BaseUniString& ref, SizeT index) : ref(ref), index(index) {}

		public:
			CharProxy() = delete;
			CharProxy(const CharProxy&) = delete;
			CharProxy(CharProxy&&) = delete;

		public:
			/// <summary>
			/// Equality with another character from UniString
			/// </summary>
			/// <param name="left">Second operand</param>
			/// <returns>This object</returns>
			CharProxy& operator=(const CharProxy& ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] = (Char) ch;

				return *this;
			}

			/// <summary>
			/// Equality with character
			/// </summary>
			/// <param name="left">Second operand</param>
			/// <returns>This object</returns>
			CharProxy& operator=(CharProxy&& ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] = (Char) ch;

				return *this;
			}

			/// <summary>
			/// Equality with character
			/// </summary>
			/// <param name="left">Second operand</param>
			/// <returns>This object</returns>
			CharProxy& operator=(Char ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] = ch;

				return *this;
			}

		public:
			/// <summary>
			/// Conversion operator
			/// </summary>
			operator Char() const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index];
				}
				else
				{
					return ref.dynamicText[index];
				}
			}

		public:
			/// <summary>
			/// The concatenation operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a sum of itself and a string representation of input argument</returns>
			CharProxy& operator+=(Char ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] += ch;

				return *this;
			}

			/// <summary>
			/// The concatenation operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a sum of itself and a string representation of input argument</returns>
			CharProxy& operator+=(char ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] += ch;

				return *this;
			}

			/// <summary>
			/// The denomination operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a difference of itself and a string representation of input argument</returns>
			CharProxy& operator-=(Char ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] -= ch;

				return *this;
			}

			/// <summary>
			/// The denomination operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a difference of itself and a string representation of input argument</returns>
			CharProxy& operator-=(char ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] -= ch;

				return *this;
			}

			/// <summary>
			/// The concatenation operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a sum of itself and a string representation of input argument</returns>
			CharProxy& operator+=(Int32 ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] += ch;

				return *this;
			}

			/// <summary>
			/// The denomination operator of character
			/// </summary>
			/// <param name="second">Right side character</param>
			/// <returns>Reference to a left argument which is a difference of itself and a string representation of input argument</returns>
			CharProxy& operator-=(Int32 ch)
			{
				ref.DyncamicModePrepare();

				ref.dynamicText[index] -= ch;

				return *this;
			}

		public:
			/// <summary>
			/// Equality operator
			/// </summary>
			/// <param name = "second">Right operator argument</param>
			/// <returns>"<c>True</c>" if strings are equal</returns>
			bool operator==(Char ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] == ch;
				}
				else
				{
					return ref.dynamicText[index] == ch;
				}
			}

			/// <summary>
			/// Equality operator
			/// </summary>
			/// <param name = "second">Right operator argument</param>
			/// <returns>"<c>True</c>" if strings are equal</returns>
			bool operator==(char ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] == ch;
				}
				else
				{
					return ref.dynamicText[index] == ch;
				}
			}

			/// <summary>
			/// Equality operator
			/// </summary>
			/// <param name = "second">Right operator argument</param>
			/// <returns>"<c>True</c>" if strings are equal</returns>
			bool operator==(const CharProxy& ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] == (Char) ch;
				}
				else
				{
					return ref.dynamicText[index] == (Char) ch;
				}
			}

			/// <summary>
			/// Equality operator
			/// </summary>
			/// <param name = "second">Right operator argument</param>
			/// <returns>"<c>True</c>" if strings are equal</returns>
			bool operator==(Int32 ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] == ch;
				}
				else
				{
					return ref.dynamicText[index] == ch;
				}
			}

		public:
			/// <summary>
			/// Universal comparison operator
			/// </summary>
			/// <param name="right">Second line</param>
			/// <returns>The order of the first line relative to the second</returns>
			auto operator<=>(Char ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] <=> ch;
				}
				else
				{
					return ref.dynamicText[index] <=> ch;
				}
			}

			/// <summary>
			/// Universal comparison operator
			/// </summary>
			/// <param name="right">Second line</param>
			/// <returns>The order of the first line relative to the second</returns>
			auto operator<=>(char ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] <=> ch;
				}
				else
				{
					return ref.dynamicText[index] <=> ch;
				}
			}

			/// <summary>
			/// Universal comparison operator
			/// </summary>
			/// <param name="right">Second line</param>
			/// <returns>The order of the first line relative to the second</returns>
			auto operator<=>(const CharProxy& ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] <=> (Char) ch;
				}
				else
				{
					return ref.dynamicText[index] <=> (Char) ch;
				}
			}

			/// <summary>
			/// Universal comparison operator
			/// </summary>
			/// <param name="right">Second line</param>
			/// <returns>The order of the first line relative to the second</returns>
			auto operator<=>(Int32 ch) const
			{
				if (ref.constMode.Read())
				{
					return ref.staticText[index] <=> ch;
				}
				else
				{
					return ref.dynamicText[index] <=> ch;
				}
			}

		public:
			friend class BaseUniString;
		};

	private:
		ConstString staticText;
		BaseString<THeap> dynamicText;

		Atomic<bool> constMode;

	protected:
		void DyncamicModePrepare()
		{
			if (CompareAndSwap(constMode, false, true))
			{
				dynamicText = staticText;
			}
		}

	public:
		/// <summary>
		/// Empty object constructor
		/// </summary>
		BaseUniString() : staticText(), constMode(true) {};

		/// <summary>
		/// Constructor with the finished constant line
		/// </summary>
		/// <param name = "Text">String literal</param>
		BaseUniString(ConstString text) : staticText(text), constMode(true) {};

		/// <summary>
		/// Constructor with the finished constant "ASCII" line
		/// </summary>
		/// <param name = "str">Constant line</param>
		BaseUniString(ConstAString text) : dynamicText(text), constMode(false) {};

		/// <summary>
		/// Constructor with "compile-time ASCII" string
		/// </summary>
		/// <typeparam name="TN">Number of symbols</typeparam>
		/// <param name="text">"Compile time ASCII" string</param>
		template<SizeT TN>
		BaseUniString(const TypeTraits::ConstexprArray<Char, TN>& text) : staticText(text), constMode(true) {};

		/// <summary>
		/// Constructor with "compile-time" string
		/// </summary>
		/// <typeparam name="TN">Number of symbols</typeparam>
		/// <param name="text">"Compile time" string</param>
		template<SizeT TN>
		BaseUniString(const TypeTraits::ConstexprArray<AChar, TN>& text) : dynamicText(text), constMode(false) {};

		/// <summary>
		/// Constructor with the finished line literal
		/// </summary>
		/// <param name = "Size">Line size including "zero terminator"</param>
		/// <param name = "Text">String literal</param>
		template<SizeT TN>
		BaseUniString(StaticTextRef<TN> text) : staticText(text), constMode(true) {};

		/// <summary>
		/// Constructor with the finished "ASCII" line literal
		/// </summary>
		/// <param name = "Size">Line size including "zero terminator"</param>
		/// <param name = "Text">String literal</param>
		template<SizeT TN>
		BaseUniString(StaticATextRef<TN> text): dynamicText(text), constMode(false) {};

		/// <summary>
		/// String copying constructor
		/// </summary>
		/// <param name = "from">Original string</param>
		BaseUniString(const BaseString<THeap>& text) : dynamicText(text), constMode(false) {};

		/// <summary>
		/// String moving constructor
		/// </summary>
		/// <param name = "from">Original string</param>
		BaseUniString(BaseString<THeap>&& text) : dynamicText(MoveRValue(text)), constMode(false) {};

		/// <summary>
		/// Constructor from other heap string
		/// </summary>
		/// <typeparam name="UHeap">Heap</typeparam>
		/// <param name="text">Original string</param>
		template <typename UHeap>
		BaseUniString(const BaseString<UHeap>& text) : dynamicText(text), constMode(false) {};
		/// <summary>
		/// Constructor from other heap string
		/// </summary>
		/// <typeparam name="UHeap">Heap</typeparam>
		/// <param name="text">Original string</param>
		template <typename UHeap>
		BaseUniString(BaseString<UHeap>&& text) : dynamicText(text), constMode(false) {};

	public:
		/// <summary>
		/// Copying constructor
		/// </summary>
		/// <param name = "from">Original string</param>
		BaseUniString(const BaseUniString& orig)
		{
			constMode = orig.constMode;
			if (constMode.Read())
			{
				staticText = orig.staticText;
			}
			else
			{
				dynamicText = orig.dynamicText;
			}
		}

		/// <summary>
		/// Moving constructor
		/// </summary>
		/// <param name = "from">Original string</param>
		BaseUniString(BaseUniString&& orig)
		{
			constMode = MoveRValue(orig.constMode);
			if (constMode.Read())
			{
				staticText = MoveRValue(orig.staticText);
			}
			else
			{
				dynamicText = MoveRValue(orig.dynamicText);
			}
		}

	public:
		BaseUniString& operator=(const BaseUniString& orig)
		{
			this->BaseUniString::~BaseUniString();
			this->BaseUniString::BaseUniString(orig);
			return *this;
		}

		BaseUniString& operator=(BaseUniString&& orig)
		{
			this->BaseUniString::~BaseUniString();
			this->BaseUniString::BaseUniString(orig);
			return *this;
		}

	public:
		/// <summary>
		/// String character access operator
		/// </summary>
		/// <param name = "Index">Index of element from zero to "zero terminator" excluded</param>
		/// <returns>A reference to string element</returns>
		CharProxy operator[](SizeT index)
		{
			return CharProxy(*this, index);
		};

		/// <summary>
		/// String character access operator
		/// </summary>
		/// <param name = "Index">Index of element from zero to "zero terminator" excluded</param>
		/// <returns>Character copy</returns>
		Char operator[](SizeT index) const
		{
			if (constMode.Read())
			{
				return staticText[index];
			}
			else
			{
				return dynamicText[index];
			}
		};

#pragma region Equality
	public:
		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		bool operator==(const ConstString& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		};

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		bool operator==(const ConstAString& second) const
		{
			if (constMode.Read())
			{
				if (staticText.Length != second.Length)
				{
					return false;
				}

				for (SizeT i = 0; i < staticText.Length; i++)
				{
					if (staticText[i] != second[i])
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return dynamicText == second;
			}
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		template<SizeT TN>
		bool operator==(TypeTraits::ConstexprArray<Char, TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		};

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		template<SizeT TN>
		bool operator==(TypeTraits::ConstexprArray<AChar, TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		template<SizeT TN>
		bool operator==(StaticTextRef<TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		};

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		template<SizeT TN>
		bool operator==(StaticATextRef<TN>& second) const
		{
			if constexpr (TN == 0)
			{
				return staticText.Length == 0;
			}

			if (constMode.Read())
			{
				if (staticText.Length != TN - 1)
				{
					return false;
				}

				for (SizeT i = 0; i < staticText.Length; i++)
				{
					if (staticText[i] != second[i])
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return dynamicText == second;
			}
		};

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		bool operator==(const BaseString<THeap>& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		/// <param name = "second">Right operator argument</param>
		/// <returns>"<c>True</c>" if strings are equal</returns>
		template<typename UHeap>
		bool operator==(const BaseString<UHeap>& second) const
		{
			if (constMode.Read())
			{
				return staticText == second;
			}
			else
			{
				return dynamicText == second;
			}
		}
#pragma endregion

#pragma region Spaceship
	public:
		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		auto operator<=>(const ConstString& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		};

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		auto operator<=>(const ConstAString& second) const
		{
			if (constMode.Read())
			{
				auto countedSize = Min(staticText.Length, second.Length);

				for (SizeT i = 0; i < countedSize; i++)
				{
					if (staticText[i] != second[i])
					{
						if (staticText[i] > second[i])
							return StrongOrdering::greater;
						else
							return StrongOrdering::less;
					}
				}

				return staticText.Length <=> second.Length;
			}
			else
			{
				return dynamicText <=> second;
			}
		}

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		template<SizeT TN>
		auto operator<=>(TypeTraits::ConstexprArray<Char, TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		};

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		template<SizeT TN>
		auto operator<=>(TypeTraits::ConstexprArray<AChar, TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		}

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		template<SizeT TN>
		auto operator<=>(StaticTextRef<TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		};

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		template<SizeT TN>
		auto operator<=>(StaticATextRef<TN>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		};

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		auto operator<=>(BaseString<THeap>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		}

		/// <summary>
		/// Universal comparison operator
		/// </summary>
		/// <param name="right">Second line</param>
		/// <returns>The order of the first line relative to the second</returns>
		template<typename UHeap>
		auto operator<=>(BaseString<UHeap>& second) const
		{
			if (constMode.Read())
			{
				return staticText <=> second;
			}
			else
			{
				return dynamicText <=> second;
			}
		}
#pragma endregion

#pragma region Contatenation
	public:
		BaseUniString operator+=(ConstString second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		BaseUniString operator+=(ConstAString second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		template<SizeT TN>
		BaseUniString operator+=(TypeTraits::ConstexprArray<Char, TN> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		template<SizeT TN>
		BaseUniString operator+=(TypeTraits::ConstexprArray<AChar, TN> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		template<SizeT TN>
		BaseUniString operator+=(StaticTextRef<TN> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		template<SizeT TN>
		BaseUniString operator+=(StaticATextRef<TN> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		BaseUniString operator+=(BaseString<THeap> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}

		template<typename UHeap>
		BaseUniString operator+=(BaseString<UHeap> second)
		{
			DyncamicModePrepare();

			dynamicText += second;

			return *this;
		}
#pragma endregion

	public:
		BaseUniString operator+(const ConstString& second) const 
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + second.Length);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + second.Length);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		BaseUniString operator+(const ConstAString& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + second.Length);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + second.Length);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		template<SizeT TN>
		BaseUniString operator+(const StaticTextRef<TN>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + TN);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + TN);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		template<SizeT TN>
		BaseUniString operator+(const StaticATextRef<TN>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + TN);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + TN);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		template<SizeT TN>
		BaseUniString operator+(const TypeTraits::ConstexprArray<Char, TN>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + TN);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + TN);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		template<SizeT TN>
		BaseUniString operator+(const TypeTraits::ConstexprArray<AChar, TN>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + TN);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + TN);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		BaseUniString operator+(const BaseString<THeap>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + second.Length);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + second.Length);

				text += dynamicText;
				text += second;
			}

			return text;
		}

		template<typename UHeap>
		BaseUniString operator+(const BaseString<UHeap>& second) const
		{
			String text = String();

			if (constMode.Read())
			{
				text.Reserve(staticText.Length + second.Length);

				text += staticText;
				text += second;
			}
			else
			{
				text.Reserve(dynamicText.Length + second.Length);

				text += dynamicText;
				text += second;
			}

			return text;
		}

	public:
		/// <summary>
		/// Explicit cast to String
		/// </summary>
		explicit operator BaseString<THeap>() const&
		{
			if (constMode.Read())
			{
				return staticText;
			}
			else
			{
				return dynamicText;
			}
		}

		/// <summary>
		/// Optimized explicit cast to String
		/// </summary>
		explicit operator BaseString<THeap>() &&
		{
			DyncamicModePrepare();

			return MoveRValue(dynamicText);
		}

	public:
		/// <summary>
		/// Returns string size
		/// </summary>
		SizeT GetLength()
		{
			if (constMode.Read())
			{
				return staticText.GetLength();
			}
			else
			{
				return dynamicText.GetLength();
			}
		}

		/// <summary>
		/// String size
		/// </summary>
		SizeT Property(GetLength) Length;

		/// <summary>
		/// Returns actual memory size which used by text
		/// </summary>
		Memory::DataSizeT GetSize()  
		{
			if (constMode.Read())
			{
				return staticText.GetSize();
			}
			else
			{
				return dynamicText.GetSize();
			}
		}

		/// <summary>
		/// Actual memory size which used by text
		/// </summary>
		Memory::DataSizeT Property(GetSize) Size;

		/// <summary>
		/// Returns "True" if object in "const" mode
		/// </summary>
		/// <returns>"True" if object in "const" mode</returns>
		bool GetIsConstMode()
		{
			return constMode.Read();
		}
		
		/// <summary>
		/// Returns "True" if object in "const" mode
		/// </summary>
		bool Property(GetIsConstMode) IsConstMode;

	public:
		~BaseUniString() {}
	};

	/// <summary>
	/// Standart heap UniString
	/// </summary>
	using UniString = BaseUniString<Memory::Heap>;
	
	/// <summary>
	/// Sensitive heap UniString
	/// </summary>
	using SenitiveUniString = BaseUniString<Memory::SensitiveDataHeap>;

	/// <summary>
	/// Exception heap UniString
	/// </summary>
	using ExceptionUniString = BaseUniString<Memory::ExceptionHeap>;
}