#pragma once
#include "Seed.Core.Crypt.Algorithms.Base.h"
#include "Seed.Core.Crypt.Algorithms.HMAC.h"

namespace Seed::Core::Crypt::Algorithms
{
	/// <summary>
	/// Time-Based One-Time Password Algorithm implementation object
	/// </summary>
	/// <typeparam name="THashProvider">Hashing algorithm</typeparam>
	template<typename THashProvider> requires Concepts::IsBasedOn<Crypt::Hash::IHashProvider, THashProvider>
	class TOTP final : public IDisposable {
	private:
		struct Data
		{
			SizeT timeIntervalInSeconds;
			Time::UnixTime startTime;
			THashProvider::THash secretHash;
			UInt8 passwordSize;

			RefCountT RefCount;

			Data(
				SizeT timeIntervalInSeconds, 
				Time::UnixTime startTime, 
				THashProvider::THash&& secretHash, 
				UInt8 passwordSize
			) : timeIntervalInSeconds(timeIntervalInSeconds), 
				startTime(startTime), 
				secretHash(MoveRValue(secretHash)), 
				passwordSize(passwordSize), 
				RefCount(1) {}
		};

		Data* This;

	private:
		THashProvider::THash ResizeIfNeeded(Containers::SensitiveBlob&& secretKey)
		{
			if (THashProvider::HashSize < secretKey.Count)
			{
				return THashProvider(MoveRValue(secretKey)).GetFixedHash();
			}
			else
			{
				return secretKey.CopyToFixed<THashProvider::THash>(true);
			}
		}

	protected: 
		virtual void Dispose() override {}

	public:
		/// <summary>
		/// TOTP initializer from data blob
		/// </summary>
		/// <param name="secretHash">Secret key blob</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(typename THashProvider::THash&& secretHash,
			 SizeT timeIntervalInSeconds = 30, 
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		) : This(new Data(
				timeIntervalInSeconds, 
				startTime, 
				MoveRValue(secretHash),
				passwordSize)
			) 
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

		/// <summary>
		/// TOTP initializer from data blob
		/// </summary>
		/// <param name="secretHash">Secret key blob</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(const typename THashProvider::THash& secretHash,
			 SizeT timeIntervalInSeconds = 30,
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		): This(new Data(
			timeIntervalInSeconds,
			startTime,
			THashProvider::THash(secretHash),
			passwordSize)
		)
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

	public:
		/// <summary>
		/// TOTP initializer from Base32 text
		/// </summary>
		/// <param name="secretHash">Base32 text string</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(SensitiveString&& secretHash,
			 SizeT timeIntervalInSeconds = 30,
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		): This(new Data(
			timeIntervalInSeconds,
			startTime,
			ResizeIfNeeded(Text::Base32::Decode<Memory::SensitiveDataHeap>(secretHash)),
			passwordSize)
		)
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

		/// <summary>
		/// TOTP initializer from Base32 text
		/// </summary>
		/// <param name="secretHash">Base32 text string</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(const SensitiveString& secretHash,
			 SizeT timeIntervalInSeconds = 30,
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		): This(new Data(
			timeIntervalInSeconds,
			startTime,
			ResizeIfNeeded(Text::Base32::Decode<Memory::SensitiveDataHeap>(secretHash)),
			passwordSize)
		)
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

	public:
		/// <summary>
		/// TOTP initializer from data blob
		/// </summary>
		/// <param name="secretHash">Secret key blob</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(Containers::SensitiveBlob&& secretHash,
			 SizeT timeIntervalInSeconds = 30,
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		): This(new Data(
			timeIntervalInSeconds,
			startTime,
			ResizeIfNeeded(MoveRValue(secretHash)),
			passwordSize)
		)
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

		/// <summary>
		/// TOTP initializer from data blob
		/// </summary>
		/// <param name="secretHash">Secret key blob</param>
		/// <param name="timeIntervalInSeconds">Key time interval</param>
		/// <param name="startTime">TOTP algorithm sync time</param>
		/// <param name="passwordSize">TOTP auth key size</param>
		TOTP(const Containers::SensitiveBlob& secretHash,
			 SizeT timeIntervalInSeconds = 30,
			 Time::UnixTime startTime = {},
			 UInt8 passwordSize = 6
		): This(new Data(
			timeIntervalInSeconds,
			startTime,
			ResizeIfNeeded(Containers::SensitiveBlob(secretHash)),
			passwordSize)
		)
		{
			if (!(4 < passwordSize && passwordSize < Math::MaxNumberCount<UInt32>()))
			{
				Throw(InvalidArgumentException(SourcePosition, nameof(passwordSize), L"Invalid password size. Must be in range 4 < passwordSize < 10"));
			}
		}

	public:
		/// <summary>
		/// Creates current TOTP auth key
		/// </summary>
		/// <returns>Current TOTP auth key with length 6</returns>
		String CreateOneTimePasskey() {
			return CreateCustomTOTPKey(GetCurrentTimeIndex(), This->passwordSize);
		}

		/// <summary>
		/// Creates custom TOTP auth key with given timeIndex and length
		/// </summary>
		/// <param name="timeIndex">Time index</param>
		/// <param name="passwordSize">Auth key length</param>
		/// <returns>Custom TOTP auth key</returns>
		String CreateCustomTOTPKey(SizeT timeIndex, SizeT passwordSize)
		{
			auto msg = Containers::Blob(sizeof(SizeT));
			Memory::Copy(&timeIndex, (Byte*) msg.GetRaw(), sizeof(SizeT));
			auto hex = String::UInt64asHex(timeIndex).LeftResize(16);

			//Hash computing
			auto hashedData =
				Crypt::Algorithms::HMAC<THashProvider>(This->secretHash)
				.AddBlock(String::hexToBinary(hex))
				.Generate();

			//Hash truncating
			UInt8 truncStartPos = hashedData[hashedData.Count - 1] & 0x0f;

			UInt32 key = (hashedData[truncStartPos] & 0x7f) << 24
				| (hashedData[truncStartPos + 1] & 0xff) << 16
				| (hashedData[truncStartPos + 2] & 0xff) << 8
				| (hashedData[truncStartPos + 3] & 0xff);

			auto keyStr = toString(key % Math::Pow(10, static_cast<UInt8>(passwordSize)));

			if (keyStr.Length < passwordSize)
			{
				keyStr = String::CreateFilledString(passwordSize - keyStr.Length, L'0') + keyStr;
			}

			return keyStr;
		}

	public:
		/// <summary>
		/// Returns current TOTP time index
		/// </summary>
		/// <returns>Current TOTP time index</returns>
		SizeT GetCurrentTimeIndex()
		{
			return (Time::UnixTime::GetCurrentSystemTime() - This->startTime).Value / This->timeIntervalInSeconds;
		}

		/// <summary>
		/// Validates TOTP auth key as RFC 6238 declares
		/// </summary>
		/// <param name="key">TOTP auth key</param>
		/// <returns>Validation result</returns>
		Boolean ValidateTOTPKey(String key)
		{
			auto keyLength = key.Length;

			auto timeIndex = GetCurrentTimeIndex();

			for(SizeT i = timeIndex - 1; i < timeIndex + 1; i++)
			{
				if (CreateCustomTOTPKey(i, keyLength) == key)
				{
					return true;
				}
			}

			return false;
		}

	public:
		RefCountClass(TOTP, This);
	};
}