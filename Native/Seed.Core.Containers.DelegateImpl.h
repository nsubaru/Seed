#pragma once
#include "Seed.Core.Containers.Delegate.h"

namespace Seed::Core
{
	inline void Delegate::Dispose()
    {
        delete This->function; This->function = nullptr;
    }

    inline Delegate Delegate::Duplicate()
    {
        return Delegate(new Data(this->This->function->Duplicate()));
    }

    inline bool Delegate::operator==(const Delegate& other) const
    {
        return this->This == other.This;
    }

    inline auto Delegate::operator<=>(const Delegate& other) const
    {
        return this->This <=> other.This;
    }

    inline Boolean Delegate::GetIsStarted() const
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        return this->This->function->IsStarted();
    }

    inline Boolean Delegate::GetIsEnded() const
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        return this->This->function->IsEnded();
    }

    inline void Delegate::Init() volatile
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        if (this->This->function->IsEnded() || this->This->function->IsStarted())
        {
            Throw(TryReStartDelegate(SourcePosition));
        }

        this->This->function->Init();
    }

    template<typename T>
    inline T Delegate::GetResult() const&
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        if (!this->This->function->IsStarted())
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This->function), NullPointException::Types::Execute));
        }

        if (this->This->function->IsStarted() && !this->This->function->IsEnded())
        {
            Throw(AlredyExecute(SourcePosition));
        }

        return this->This->function->GetResult<T>();
    }

    template<typename T>
    inline T Delegate::GetResult() &&
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        if (!this->This->function->IsStarted())
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This->function), NullPointException::Types::Execute));
        }

        if (this->This->function->IsStarted() && !this->This->function->IsEnded())
        {
            Throw(AlredyExecute(SourcePosition));
        }

        return MoveRValue(*this->This->function).GetResult<T>();
    }

    template<typename T>
    inline T Delegate::GetResult() volatile&&
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        if (!this->This->function->IsStarted())
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This->function), NullPointException::Types::Execute));
        }

        if (this->This->function->IsStarted() && !this->This->function->IsEnded())
        {
            Throw(AlredyExecute(SourcePosition));
        }

        return MoveRValue(*this->This->function).GetResult<T>();
    }

    template<typename T>
    inline T Delegate::GetResult() const volatile&
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        if (!this->This->function->IsStarted())
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This->function), NullPointException::Types::Execute));
        }

        if (this->This->function->IsStarted() && !this->This->function->IsEnded())
        {
            Throw(AlredyExecute(SourcePosition));
        }

        return this->This->function->GetResult<T>();
    }

    inline volatile Delegate& Delegate::operator()() volatile
    {
        this->Init();
        return *this;
    }

    inline Delegate& Delegate::operator()()
    {
        this->Init();
        return *this;
    }
}