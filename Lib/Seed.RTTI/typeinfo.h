#pragma once

#ifdef _CPPRTTI

/// <summary>
/// Compiler ABI storage for a decorated type name and optional undecorated-name cache.
/// </summary>
struct __std_type_info_data
{
	/// <summary>
	/// Cached undecorated type name, when one has been produced.
	/// </summary>
	const char* _UndecoratedName;

	/// <summary>
	/// First character of the trailing compiler-decorated type name.
	/// </summary>
	const char   _DecoratedName[1];

	/// <summary>
	/// Disallows default construction of compiler-generated type metadata.
	/// </summary>
	__std_type_info_data() = delete;

	/// <summary>
	/// Disallows construction from another instance of this identity-bound object.
	/// </summary>
	__std_type_info_data(const __std_type_info_data&) = delete;

	/// <summary>
	/// Disallows construction from another instance of this identity-bound object.
	/// </summary>
	__std_type_info_data(__std_type_info_data&&) = delete;

	/// <summary>
	/// Disallows assignment of this object because its identity or owned state must remain bound to its original instance.
	/// </summary>
	__std_type_info_data& operator=(const __std_type_info_data&) = delete;

	/// <summary>
	/// Disallows assignment of this object because its identity or owned state must remain bound to its original instance.
	/// </summary>
	__std_type_info_data& operator=(__std_type_info_data&&) = delete;
};

#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
/// <summary>
/// Compiler ABI type descriptor used by the shared RTTI support library.
/// </summary>
class type_info
{
public:

	/// <summary>
	/// Disallows construction from another instance of this identity-bound object.
	/// </summary>
	type_info(const type_info&) = delete;

	/// <summary>
	/// Disallows assignment of this object because its identity or owned state must remain bound to its original instance.
	/// </summary>
	type_info& operator=(const type_info&) = delete;

	/// <summary>
	/// Computes a hash of the compiler type identity.
	/// </summary>
	/// <returns>Hash code for the represented compiler type.</returns>
	size_t GetHashCode() const;

	/// <summary>
	/// Hash of the compiler type identity.
	/// </summary>
	size_t __declspec(property(get = GetHashCode)) HashCode;

	/// <summary>
	/// Compares the compiler type identities of two descriptors.
	/// </summary>
	/// <param name="_Other">Compiler type descriptor to compare with this descriptor.</param>
	/// <returns>True when the type identities are equal.</returns>
	bool operator==(const type_info& _Other) const;

	/// <summary>
	/// Compares the compiler type identities of two descriptors.
	/// </summary>
	/// <param name="_Other">Compiler type descriptor to compare with this descriptor.</param>
	/// <returns>True when the type identities are different.</returns>
	bool operator!=(const type_info& _Other) const;

	/// <summary>
	/// Retrieves the compiler-decorated type name stored in the descriptor.
	/// </summary>
	/// <returns>Pointer to the descriptor-owned decorated type name.</returns>
	constexpr const char* GetCxxName() const {
		return _Data._DecoratedName;
	}

	/// <summary>
	/// Destroys the compiler type descriptor.
	/// </summary>
	virtual ~type_info() {}

private:

	mutable __std_type_info_data _Data;
};
#pragma warning(pop)

#endif