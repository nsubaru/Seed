#pragma once
#include "Seed.Core.Memory.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Memory {
			/// <summary>
			/// Binary data size type
			/// </summary>
			class DataSizeT final {
			public:
				using ByteSizeT = UInt64;
			public:
				/// <summary>
				/// Kilobyte in bytes
				/// </summary>
				static constexpr ByteSizeT Kilobyte = 1024;

				/// <summary>
				/// Megabyte in bytes
				/// </summary>
				static constexpr ByteSizeT Megabyte = Kilobyte * 1024;

				/// <summary>
				/// Gigabyte in bytes
				/// </summary>
				static constexpr ByteSizeT Gigabyte = Megabyte * 1024;

				/// <summary>
				/// Terabyte in bytes
				/// </summary>
				static constexpr ByteSizeT Terabyte = Gigabyte * 1024;

				/// <summary>
				/// Petabyte in bytes
				/// </summary>
				static constexpr ByteSizeT Petabyte = Terabyte * 1024;

				/// <summary>
				/// Exabyte in bytes
				/// </summary>
				static constexpr ByteSizeT Exabyte = Petabyte * 1024;


				/// <summary>
				/// Max allowed bytes
				/// </summary>
				static constexpr ByteSizeT Maxbytes = Math::Max<ByteSizeT>();
			public:
				/// <summary>
				/// Bytes
				/// </summary>
				ByteSizeT Bytes;
			public:
				/// <summary>
				/// Empty ctor
				/// </summary>
				constexpr DataSizeT() : Bytes(0) {}

				/// <summary>
				/// Ctor from bytes
				/// </summary>
				/// <param name="bytes">Bytes count</param>
				constexpr explicit DataSizeT(ByteSizeT bytes) : Bytes(bytes) {}
			public:
				/// <summary>
				/// Add bytes
				/// </summary>
				/// <param name="bytes">Bytes count></param>
				constexpr void AddBytes(ByteSizeT bytes) { this->Bytes += bytes; }
			public:
				/// <summary>
				/// Get as kilobytes
				/// </summary>
				/// <returns></returns>
				constexpr ByteSizeT GetKilobytes() const { return Bytes / Kilobyte; }

				/// <summary>
				/// Set as kilobytes
				/// </summary>
				/// <param name="kilobytes">Kiobytes count</param>
				constexpr void SetKilobytes(ByteSizeT kilobytes) { this->Bytes = kilobytes * Kilobyte; }

				/// <summary>
				/// Add kilobytes
				/// </summary>
				/// <param name="kilobytes">Kilobytes count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& AddKilobytes(ByteSizeT kilobytes) { this->Bytes += kilobytes * Kilobyte; return *this; }
			public:
				/// <valuw>
				/// Kilobytes count
				/// </valuw>
				ByteSizeT Property(GetKilobytes, SetKilobytes) Kilobytes;
			public:
				/// <summary>
				/// Get as megabytes
				/// </summary>
				/// <returns>Count of megabytes</returns>
				constexpr ByteSizeT GetMegabytes() const { return Bytes / Megabyte; }

				/// <summary>
				/// Set as megabytes
				/// </summary>
				/// <param name="megabytes">Megabytes count</param>
				constexpr void SetMegabytes(ByteSizeT megabytes) { this->Bytes = megabytes * Megabyte; }

				/// <summary>
				/// Add megabytes
				/// </summary>
				/// <param name="megabytes">Megabytes count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& AddMegabytes(ByteSizeT megabytes) { this->Bytes += megabytes * Megabyte; return *this; }
			public:
				/// <summary>
				/// Megabytes count
				/// </summary>
				ByteSizeT Property(GetMegabytes, SetMegabytes) Megabytes;
			public:
				/// <summary>
				/// Get as gigabytes
				/// </summary>
				/// <returns>Count of gigabytes</returns>
				constexpr ByteSizeT GetGigabytes() const { return Bytes / Gigabyte; }

				/// <summary>
				/// Set as gigabytes
				/// </summary>
				/// <param name="gigabytes">Gigabytes count</param>
				constexpr void SetGigabytes(ByteSizeT gigabytes) { this->Bytes = gigabytes * Gigabyte; }

				/// <summary>
				/// Add gigabytes
				/// </summary>
				/// <param name="gigabytes">Gigabytes count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& AddGigabytes(ByteSizeT gigabytes) { this->Bytes += gigabytes * Gigabyte; return *this; }
			public:
				/// <summary>
				/// Gigabytes count
				/// </summary>
				ByteSizeT Property(GetGigabytes, SetGigabytes) Gigabytes;
			public:
				/// <summary>
				/// Get as terabytes
				/// </summary>
				/// <returns>Count of terabytes</returns>
				constexpr ByteSizeT GetTerabytes() const { return Bytes / Terabyte; }

				/// <summary>
				/// Set as terabytes
				/// </summary>
				/// <param name="terabytes">Terabytes count</param>
				constexpr void SetTerabytes(ByteSizeT terabytes) { this->Bytes = terabytes * Terabyte; }

				/// <summary>
				/// Add terabytes
				/// </summary>
				/// <param name="terabytes">Terabytes count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& AddTerabytes(ByteSizeT terabytes) { this->Bytes += terabytes * Terabyte; return *this; }
			public:
				/// <summary>
				/// Terabytes count
				/// </summary>
				ByteSizeT Property(GetTerabytes, SetTerabytes) Terabytes;
			public:
				/// <summary>
				/// Get as petabytes
				/// </summary>
				/// <returns>Petabytes count</returns>
				constexpr ByteSizeT GetPetabytes() const { return Bytes / Petabyte; }

				/// <summary>
				/// Set as petabytes
				/// </summary>
				/// <param name="petabytes">Petabytes count</param>
				constexpr void SetPetabytes(ByteSizeT petabytes) { this->Bytes = petabytes * Petabyte; }

				/// <summary>
				/// Add petabytes
				/// </summary>
				/// <param name="petabytes">Petabytes count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& AddPetabytes(ByteSizeT petabytes) { this->Bytes += petabytes * Petabyte; return *this; }
			public:
				/// <summary>
				/// Petabytes count
				/// </summary>
				ByteSizeT Property(GetPetabytes, SetPetabytes) Petabytes;
			public:
				/// <summary>
				/// Get as exabytes
				/// </summary>
				/// <returns>Exabytes count</returns>
				constexpr ByteSizeT GetExabytes() const { return Bytes / Exabyte; }

				/// <summary>
				/// Set as exabytes
				/// </summary>
				/// <param name="exabytes">Exabytes count</param>
				constexpr void SetExabytes(ByteSizeT exabytes) { this->Bytes = exabytes * Exabyte; }

				/// <summary>
				/// Add exabytes
				/// </summary>
				/// <param name="exabytes">Exabytes count</param>
				/// <returns></returns>
				constexpr DataSizeT& AddExabytes(ByteSizeT exabytes) { this->Bytes += exabytes * Exabyte; return *this; }
			public:
				/// <summary>
				/// Exabytes
				/// </summary>
				ByteSizeT Property(GetExabytes, SetExabytes) Exabytes;
			public:
				/// <summary>
				/// Add data sizes
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Result value</returns>
				constexpr DataSizeT operator+(DataSizeT other) const { return DataSizeT(this->Bytes + other.Bytes); }

				/// <summary>
				/// Substract data sizes
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Result value</returns>
				constexpr DataSizeT operator-(DataSizeT other) const { return DataSizeT(this->Bytes - other.Bytes); }

				/// <summary>
				/// Add bytes to data size
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Result value></returns>
				constexpr DataSizeT operator+(ByteSizeT other) const { return DataSizeT(this->Bytes + Bytes); }

				/// <summary>
				/// Substract bytes to data size
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Result value></returns>
				constexpr DataSizeT operator-(ByteSizeT other) const { return DataSizeT(this->Bytes - Bytes); }

				/// <summary>
				/// Add bytes to data size
				/// </summary>
				/// <param name="first">First operand, bytes count</param>
				/// <param name="second">Second operand, data size</param>
				/// <returns>Result value</returns>
				constexpr friend DataSizeT operator+(ByteSizeT first, DataSizeT second) { return DataSizeT(first + second.Bytes); }

				/// <summary>
				/// Substract bytes to data size
				/// </summary>
				/// <param name="first">First operand, bytes count</param>
				/// <param name="second">Second operand, data size</param>
				/// <returns>Result value</returns>
				constexpr friend DataSizeT operator-(ByteSizeT first, DataSizeT second) { return DataSizeT(first - second.Bytes); }
			public:
				/// <summary>
				/// Add to this value data size
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& operator+=(DataSizeT other) { this->Bytes += other.Bytes; return *this; }

				/// <summary>
				/// Substract to this value data size
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& operator-=(DataSizeT other) { this->Bytes -= other.Bytes; return *this; }

				/// <summary>
				/// Add to this value bytes
				/// </summary>
				/// <param name="other">Second operand, byte count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& operator+=(ByteSizeT other) { this->Bytes += other; return *this; }

				/// <summary>
				/// Add to this value bytes
				/// </summary>
				/// <param name="other">Second operand, byte count</param>
				/// <returns>Ref to this object</returns>
				constexpr DataSizeT& operator-=(ByteSizeT other) { this->Bytes -= other; return *this; }
			public:
				/// <summary>
				/// Three way comparing
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Comparing result</returns>
				constexpr auto operator<=>(const DataSizeT& other) const { return this->Bytes <=> other.Bytes; }
				/// <summary>
				/// Equality operator
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Equality result</returns>
				constexpr auto operator==(const DataSizeT& other) const { return this->Bytes == other.Bytes; }
				/// <summary>
				/// Non equality operator
				/// </summary>
				/// <param name="other">Second operand</param>
				/// <returns>Non equality result</returns>
				constexpr auto operator!=(const DataSizeT& other) const { return this->Bytes != other.Bytes; }
			public:
				/// <summary>
				/// Explicit convertion to byte count operator
				/// </summary>
				constexpr explicit operator ByteSizeT() { return this->Bytes; }
			};
		}
	}
}
#pragma warning(push)
#pragma warning(disable: 4455)
#pragma warning(disable: 4114)
/// <summary>
/// Kilobytes literal
/// </summary>
/// <param name="size">Kilobytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Kb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Kilobyte);
}

/// <summary>
/// Megabytes literal
/// </summary>
/// <param name="size">Megabytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Mb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Megabyte);
}

/// <summary>
/// Gigabytes literal
/// </summary>
/// <param name="size">Gigabytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Gb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Gigabyte);
}

/// <summary>
/// Terabytes literal
/// </summary>
/// <param name="size">Terabytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Tb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Terabyte);
}

/// <summary>
/// Petabytes literal
/// </summary>
/// <param name="size">Petabytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Pb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Petabyte);
}

/// <summary>
/// Exabytes literal
/// </summary>
/// <param name="size">Exabytes count</param>
/// <returns>Data size value</returns>
__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Eb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Exabyte);
}

#pragma warning(pop)