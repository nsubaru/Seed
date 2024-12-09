using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Functionality;

public class Optional<T>
{
    private readonly bool hasValue;

    private readonly dynamic? value;

    public Optional()
    {
        hasValue = false;
        value = null;
    }

    public Optional(T value)
    {
        hasValue = true;
        this.value = value;
    }

    public bool HasValue => hasValue;

    public T? Value => value;

    public static implicit operator Optional<T>(T value)
    {
        return new Optional<T>(value);
    }
}
