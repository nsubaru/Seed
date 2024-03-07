#pragma once

#ifdef _CPPRTTI

struct __std_type_info_data
{
	const char* _UndecoratedName;
	const char   _DecoratedName[1];
	__std_type_info_data() = delete;
	__std_type_info_data(const __std_type_info_data&) = delete;
	__std_type_info_data(__std_type_info_data&&) = delete;

	__std_type_info_data& operator=(const __std_type_info_data&) = delete;
	__std_type_info_data& operator=(__std_type_info_data&&) = delete;
};

#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
class type_info
{
public:

	type_info(const type_info&) = delete;
	type_info& operator=(const type_info&) = delete;

	size_t GetHashCode() const;

	size_t __declspec(property(get = GetHashCode)) HashCode;

	bool operator==(const type_info& _Other) const;

	bool operator!=(const type_info& _Other) const;

	constexpr const char* GetCxxName() const
	{
		return _Data._DecoratedName;
	}

	virtual ~type_info() {}

private:

	mutable __std_type_info_data _Data;
};
#pragma warning(pop)

#endif