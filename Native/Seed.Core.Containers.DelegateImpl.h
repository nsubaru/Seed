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

    ForceInline Boolean Delegate::GetIsStarted() const
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        return this->This->function->IsStarted();
    }

    ForceInline Boolean Delegate::GetIsEnded() const
    {
        if (this->This == nullptr)
        {
            Throw(NullPointException(SourcePosition, reinterpret_cast<SizeT>(this->This), NullPointException::Types::Read));
        }

        return this->This->function->IsEnded();
    }

    ForceInline void Delegate::Init() volatile
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
    ForceInline T Delegate::GetResult()
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
    ForceInline T Delegate::GetResult() volatile
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

    ForceInline volatile Delegate& Delegate::operator()() volatile
    {
        this->Init();
        return *this;
    }

    ForceInline Delegate& Delegate::operator()()
    {
        this->Init();
        return *this;
    }
}