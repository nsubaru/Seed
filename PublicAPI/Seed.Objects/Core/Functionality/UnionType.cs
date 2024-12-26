using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Functionality;

public class UnionType<T0>
{
    public SizeT TypeId { get; private set; } = 0;

    private object value;

    public UnionType() { 
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value!;
    }

    public object GetValue()
    {
        if (TypeId != 0)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        if (typeof(T) == typeof(T0) && TypeId == 0)
        {
            return (T)value!;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public void SetValue(T0 value)
    {
        TypeId = 0;
        this.value = value!;
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        if (TypeId == SizeT.MaxValue)
        {
            return false;
        }
        
        return typeof(T) == typeof(T0);
    }
}

public class UnionType<T0, T1>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value) 
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value) 
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T) value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T) value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T) value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2, T3>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public UnionType(T3 value)
    {
        this.value = value;
        TypeId = 3;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 3:
                if (typeof(T) == typeof(T3))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else if (typeof(T) == typeof(T3))
        {
            this.value = value;
            TypeId = 3;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case 3:
                return typeof(T) == typeof(T3);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2, T3, T4>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public UnionType(T3 value)
    {
        this.value = value;
        TypeId = 3;
    }

    public UnionType(T4 value)
    {
        this.value = value;
        TypeId = 4;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 3:
                if (typeof(T) == typeof(T3))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 4:
                if (typeof(T) == typeof(T4))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else if (typeof(T) == typeof(T3))
        {
            this.value = value;
            TypeId = 3;
        }
        else if (typeof(T) == typeof(T4))
        {
            this.value = value;
            TypeId = 4;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case 3:
                return typeof(T) == typeof(T3);

            case 4:
                return typeof(T) == typeof(T4);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2, T3, T4, T5>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public UnionType(T3 value)
    {
        this.value = value;
        TypeId = 3;
    }

    public UnionType(T4 value)
    {
        this.value = value;
        TypeId = 4;
    }

    public UnionType(T5 value)
    {
        this.value = value;
        TypeId = 5;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 3:
                if (typeof(T) == typeof(T3))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 4:
                if (typeof(T) == typeof(T4))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 5:
                if (typeof(T) == typeof(T5))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else if (typeof(T) == typeof(T3))
        {
            this.value = value;
            TypeId = 3;
        }
        else if (typeof(T) == typeof(T4))
        {
            this.value = value;
            TypeId = 4;
        }
        else if (typeof(T) == typeof(T5))
        {
            this.value = value;
            TypeId = 5;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case 3:
                return typeof(T) == typeof(T3);

            case 4:
                return typeof(T) == typeof(T4);

            case 5:
                return typeof(T) == typeof(T5);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2, T3, T4, T5, T6>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public UnionType(T3 value)
    {
        this.value = value;
        TypeId = 3;
    }

    public UnionType(T4 value)
    {
        this.value = value;
        TypeId = 4;
    }

    public UnionType(T5 value)
    {
        this.value = value;
        TypeId = 5;
    }

    public UnionType(T6 value)
    {
        this.value = value;
        TypeId = 6;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId)
        {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 3:
                if (typeof(T) == typeof(T3))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 4:
                if (typeof(T) == typeof(T4))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 5:
                if (typeof(T) == typeof(T5))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 6:
                if (typeof(T) == typeof(T6))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else if (typeof(T) == typeof(T3))
        {
            this.value = value;
            TypeId = 3;
        }
        else if (typeof(T) == typeof(T4))
        {
            this.value = value;
            TypeId = 4;
        }
        else if (typeof(T) == typeof(T5))
        {
            this.value = value;
            TypeId = 5;
        }
        else if (typeof(T) == typeof(T6))
        {
            this.value = value;
            TypeId = 6;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case 3:
                return typeof(T) == typeof(T3);

            case 4:
                return typeof(T) == typeof(T4);

            case 5:
                return typeof(T) == typeof(T5);

            case 6:
                return typeof(T) == typeof(T6);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}

public class UnionType<T0, T1, T2, T3, T4, T5, T6, T7>
{
    public SizeT TypeId { get; private set; }

    private object? value;

    public UnionType()
    {
        value = default!;
        TypeId = SizeT.MaxValue;
    }

    public UnionType(T0 value)
    {
        this.value = value;
        TypeId = 0;
    }

    public UnionType(T1 value)
    {
        this.value = value;
        TypeId = 1;
    }

    public UnionType(T2 value)
    {
        this.value = value;
        TypeId = 2;
    }

    public UnionType(T3 value)
    {
        this.value = value;
        TypeId = 3;
    }

    public UnionType(T4 value)
    {
        this.value = value;
        TypeId = 4;
    }

    public UnionType(T5 value)
    {
        this.value = value;
        TypeId = 5;
    }

    public UnionType(T6 value)
    {
        this.value = value;
        TypeId = 6;
    }

    public UnionType(T7 value)
    {
        this.value = value;
        TypeId = 7;
    }

    public object GetValue()
    {
        if (TypeId == SizeT.MaxValue)
        {
            throw new InvalidDataException();
        }

        return value!;
    }

    public T GetValue<T>()
    {
        switch (TypeId) {
            case 0:
                if (typeof(T) == typeof(T0))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 1:
                if (typeof(T) == typeof(T1))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 2:
                if (typeof(T) == typeof(T2))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 3:
                if (typeof(T) == typeof(T3))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 4:
                if (typeof(T) == typeof(T4))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 5:
                if (typeof(T) == typeof(T5))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 6:
                if (typeof(T) == typeof(T6))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            case 7:
                if (typeof(T) == typeof(T7))
                {
                    return (T)value!;
                }
                else
                {
                    throw new InvalidDataException();
                }
            default:
                throw new InvalidDataException();
        }
    }

    public void SetValue<T>(T value)
    {
        if (typeof(T) == typeof(T0))
        {
            this.value = value;
            TypeId = 0;
        }
        else if (typeof(T) == typeof(T1))
        {
            this.value = value;
            TypeId = 1;
        }
        else if (typeof(T) == typeof(T2))
        {
            this.value = value;
            TypeId = 2;
        }
        else if (typeof(T) == typeof(T3))
        {
            this.value = value;
            TypeId = 3;
        }
        else if (typeof(T) == typeof(T4))
        {
            this.value = value;
            TypeId = 4;
        }
        else if (typeof(T) == typeof(T5))
        {
            this.value = value;
            TypeId = 5;
        }
        else if (typeof(T) == typeof(T6))
        {
            this.value = value;
            TypeId = 6;
        }
        else if (typeof(T) == typeof(T7))
        {
            this.value = value;
            TypeId = 7;
        }
        else
        {
            throw new InvalidDataException();
        }
    }

    public object Value => GetValue();

    public bool Is<T>()
    {
        switch (TypeId)
        {
            case 0:
                return typeof(T) == typeof(T0);

            case 1:
                return typeof(T) == typeof(T1);

            case 2:
                return typeof(T) == typeof(T2);

            case 3:
                return typeof(T) == typeof(T3);

            case 4:
                return typeof(T) == typeof(T4);

            case 5:
                return typeof(T) == typeof(T5);

            case 6:
                return typeof(T) == typeof(T6);

            case 7:
                return typeof(T) == typeof(T7);

            case SizeT.MaxValue:
                return false;

            default:
                throw new InvalidDataException();
        }
    }
}