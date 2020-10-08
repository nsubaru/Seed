#pragma once
#include "Seed.Core.Memory.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Memory {
			class DataSizeT final {
			public:
				using ByteSizeT = UInt64;
			public:
				static constexpr ByteSizeT Kilobyte = 1024;
				static constexpr ByteSizeT Megabyte = Kilobyte * 1024;
				static constexpr ByteSizeT Gigabyte = Megabyte * 1024;
				static constexpr ByteSizeT Terabyte = Gigabyte * 1024;
				static constexpr ByteSizeT Petabyte = Terabyte * 1024;
				static constexpr ByteSizeT Exabyte = Petabyte * 1024;

				static constexpr ByteSizeT Maxbytes = Math::Max<ByteSizeT>();
			private:
				ByteSizeT bytes;
			public:
				constexpr DataSizeT() : bytes(0) {}
				constexpr explicit DataSizeT(ByteSizeT bytes) : bytes(bytes) {}
			public:
				constexpr ByteSizeT GetBytes() const { return bytes; }
				constexpr void SetBytes(ByteSizeT bytes) { this->bytes = bytes; }
				constexpr void AddBytes(ByteSizeT bytes) { this->bytes += bytes; }
			public:
				ByteSizeT Property(GetBytes, SetBytes) Bytes;
			public:
				constexpr ByteSizeT GetKilobytes() const { return bytes / Kilobyte; }
				constexpr void SetKilobytes(ByteSizeT kilobytes) { this->bytes = kilobytes * Kilobyte; }
				constexpr DataSizeT& AddKilobytes(ByteSizeT kilobytes) { this->bytes += kilobytes * Kilobyte; return *this; }
			public:
				ByteSizeT Property(GetKilobytes, SetKilobytes) Kilobytes;
			public:
				constexpr ByteSizeT GetMegabytes() const { return bytes / Megabyte; }
				constexpr void SetMegabytes(ByteSizeT megabytes) { this->bytes = megabytes * Megabyte; }
				constexpr DataSizeT& AddMegabytes(ByteSizeT megabytes) { this->bytes += megabytes * Megabyte; return *this; }
			public:
				ByteSizeT Property(GetMegabytes, SetMegabytes) Megabytes;
			public:
				constexpr UInt64 GetGigabytes() const { return bytes / Gigabyte; }
				constexpr void SetGigabytes(ByteSizeT gigabytes) { this->bytes = gigabytes * Gigabyte; }
				constexpr DataSizeT& AddGigabytes(ByteSizeT gigabytes) { this->bytes += gigabytes * Gigabyte; return *this; }
			public:
				ByteSizeT Property(GetGigabytes, SetGigabytes) Gigabytes;
			public:
				constexpr ByteSizeT GetTerabytes() const { return bytes / Terabyte; }
				constexpr void SetTerabytes(ByteSizeT terabytes) { this->bytes = terabytes * Terabyte; }
				constexpr DataSizeT& AddTerabytes(ByteSizeT terabytes) { this->bytes += terabytes * Terabyte; return *this; }
			public:
				ByteSizeT Property(GetTerabytes, SetTerabytes) Terabytes;
			public:
				constexpr ByteSizeT GetPetabytes() const { return bytes / Petabyte; }
				constexpr void SetPetabytes(ByteSizeT petabytes) { this->bytes = petabytes * Petabyte; }
				constexpr DataSizeT& AddPetabytes(ByteSizeT petabytes) { this->bytes += petabytes * Petabyte; return *this; }
			public:
				ByteSizeT Property(GetPetabytes, SetPetabytes) Petabytes;
			public:
				constexpr ByteSizeT GetExabytes() const { return bytes / Exabyte; }
				constexpr void SetExabytes(ByteSizeT exabytes) { this->bytes = exabytes * Exabyte; }
				constexpr DataSizeT& AddExabytes(ByteSizeT exabytes) { this->bytes += exabytes * Exabyte; return *this; }
			public:
				ByteSizeT Property(GetExabytes, SetExabytes) Exabytes;
			public:
				constexpr DataSizeT operator+(DataSizeT other) const { return DataSizeT(this->bytes + other.bytes); }
				constexpr DataSizeT operator-(DataSizeT other) const { return DataSizeT(this->bytes - other.bytes); }

				constexpr DataSizeT operator+(ByteSizeT other) const { return DataSizeT(this->bytes + bytes); }
				constexpr DataSizeT operator-(ByteSizeT other) const { return DataSizeT(this->bytes - bytes); }
			public:
				constexpr DataSizeT& operator+=(DataSizeT other) { this->bytes += other.bytes; return *this; }
				constexpr DataSizeT& operator-=(DataSizeT other) { this->bytes -= other.bytes; return *this; }

				constexpr DataSizeT& operator+=(ByteSizeT other) { this->bytes += other; return *this; }
				constexpr DataSizeT& operator-=(ByteSizeT other) { this->bytes -= other; return *this; }
			public:
				constexpr auto operator<=>(const DataSizeT& other) const { return this->bytes <=> other.bytes; }
				constexpr auto operator==(const DataSizeT& other) const { return this->bytes == other.bytes; }
				constexpr auto operator!=(const DataSizeT& other) const { return this->bytes != other.bytes; }
			public:
				constexpr explicit operator ByteSizeT() { return this->GetBytes(); }
			};
		}
	}
}

#pragma warning(disable: 4455)
#pragma warning(disable: 4114)

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Kb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Kilobyte);
}

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Mb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Megabyte);
}

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Gb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Gigabyte);
}

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Tb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Terabyte);
}

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Pb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Petabyte);
}

__forceinline constexpr Seed::Core::Memory::DataSizeT operator "" Eb(const unsigned long long size) {
	using namespace Seed::Core::Memory;
	return DataSizeT(size * DataSizeT::Exabyte);
}

#pragma warning(pop)