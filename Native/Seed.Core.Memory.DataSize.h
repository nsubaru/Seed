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
			public:
				ByteSizeT Bytes;
			public:
				constexpr DataSizeT() : Bytes(0) {}
				constexpr explicit DataSizeT(ByteSizeT bytes) : Bytes(bytes) {}
			public:
				constexpr void AddBytes(ByteSizeT bytes) { this->Bytes += bytes; }
			public:
				constexpr ByteSizeT GetKilobytes() const { return Bytes / Kilobyte; }
				constexpr void SetKilobytes(ByteSizeT kilobytes) { this->Bytes = kilobytes * Kilobyte; }
				constexpr DataSizeT& AddKilobytes(ByteSizeT kilobytes) { this->Bytes += kilobytes * Kilobyte; return *this; }
			public:
				ByteSizeT Property(GetKilobytes, SetKilobytes) Kilobytes;
			public:
				constexpr ByteSizeT GetMegabytes() const { return Bytes / Megabyte; }
				constexpr void SetMegabytes(ByteSizeT megabytes) { this->Bytes = megabytes * Megabyte; }
				constexpr DataSizeT& AddMegabytes(ByteSizeT megabytes) { this->Bytes += megabytes * Megabyte; return *this; }
			public:
				ByteSizeT Property(GetMegabytes, SetMegabytes) Megabytes;
			public:
				constexpr UInt64 GetGigabytes() const { return Bytes / Gigabyte; }
				constexpr void SetGigabytes(ByteSizeT gigabytes) { this->Bytes = gigabytes * Gigabyte; }
				constexpr DataSizeT& AddGigabytes(ByteSizeT gigabytes) { this->Bytes += gigabytes * Gigabyte; return *this; }
			public:
				ByteSizeT Property(GetGigabytes, SetGigabytes) Gigabytes;
			public:
				constexpr ByteSizeT GetTerabytes() const { return Bytes / Terabyte; }
				constexpr void SetTerabytes(ByteSizeT terabytes) { this->Bytes = terabytes * Terabyte; }
				constexpr DataSizeT& AddTerabytes(ByteSizeT terabytes) { this->Bytes += terabytes * Terabyte; return *this; }
			public:
				ByteSizeT Property(GetTerabytes, SetTerabytes) Terabytes;
			public:
				constexpr ByteSizeT GetPetabytes() const { return Bytes / Petabyte; }
				constexpr void SetPetabytes(ByteSizeT petabytes) { this->Bytes = petabytes * Petabyte; }
				constexpr DataSizeT& AddPetabytes(ByteSizeT petabytes) { this->Bytes += petabytes * Petabyte; return *this; }
			public:
				ByteSizeT Property(GetPetabytes, SetPetabytes) Petabytes;
			public:
				constexpr ByteSizeT GetExabytes() const { return Bytes / Exabyte; }
				constexpr void SetExabytes(ByteSizeT exabytes) { this->Bytes = exabytes * Exabyte; }
				constexpr DataSizeT& AddExabytes(ByteSizeT exabytes) { this->Bytes += exabytes * Exabyte; return *this; }
			public:
				ByteSizeT Property(GetExabytes, SetExabytes) Exabytes;
			public:
				constexpr DataSizeT operator+(DataSizeT other) const { return DataSizeT(this->Bytes + other.Bytes); }
				constexpr DataSizeT operator-(DataSizeT other) const { return DataSizeT(this->Bytes - other.Bytes); }

				constexpr DataSizeT operator+(ByteSizeT other) const { return DataSizeT(this->Bytes + Bytes); }
				constexpr DataSizeT operator-(ByteSizeT other) const { return DataSizeT(this->Bytes - Bytes); }
			public:
				constexpr DataSizeT& operator+=(DataSizeT other) { this->Bytes += other.Bytes; return *this; }
				constexpr DataSizeT& operator-=(DataSizeT other) { this->Bytes -= other.Bytes; return *this; }

				constexpr DataSizeT& operator+=(ByteSizeT other) { this->Bytes += other; return *this; }
				constexpr DataSizeT& operator-=(ByteSizeT other) { this->Bytes -= other; return *this; }
			public:
				constexpr auto operator<=>(const DataSizeT& other) const { return this->Bytes <=> other.Bytes; }
				constexpr auto operator==(const DataSizeT& other) const { return this->Bytes == other.Bytes; }
				constexpr auto operator!=(const DataSizeT& other) const { return this->Bytes != other.Bytes; }
			public:
				constexpr explicit operator ByteSizeT() { return this->Bytes; }
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