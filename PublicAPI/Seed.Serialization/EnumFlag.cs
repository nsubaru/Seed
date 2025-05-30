using LanguageExt.ClassInstances.Pred;
using System.Numerics;
using System.Runtime.CompilerServices;
using WinRT;

namespace Seed.Core;

/// <summary>
/// Store enum values as bit flags according to enum value
/// </summary>
/// <typeparam name="T">Enum type</typeparam>
/// <typeparam name="U">Integer type</typeparam>
public struct EnumFlag<T, U>
    where T: 
        unmanaged, 
        System.Enum
    where U: 
        unmanaged,
        IBinaryInteger<U>, 
        IBitwiseOperators<U, U, U>,
        IEqualityOperators<U, U, bool>
{
    private T value;

    /// <summary>
    /// Enum value
    /// </summary>
    public T[] Values => GetValuesAsEnumArray();

    /// <summary>
    /// Empty ctor
    /// </summary>
    public EnumFlag() { }

    /// <summary>
    /// Ctor from enum value
    /// </summary>
    /// <param name="value">Enum value</param>
    public EnumFlag(T value) {
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        if (values == null || underlyingValues == null)
        { 
            throw new InvalidCastException(); 
        }

        var res = 0;

        for (int i = 0; i < underlyingValues.Length; i++)
        {
            int f = 0;
            for (U j = U.Zero; j < underlyingValues[i]; j++, f++);

            if (values[i].Equals(value))
            {
                res |= (1 << f);
            }
        }

        this.value = Unsafe.As<int, T>(ref res);
    }

    /// <summary>
    /// Ctor from integer value, perform converting to enum
    /// </summary>
    /// <param name="value">Intgere value</param>
    /// <exception cref="InvalidCastException">If enum empty or T type is not enum</exception>
    public EnumFlag(U value)
    {
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        if (values == null || underlyingValues == null)
        { 
            throw new InvalidCastException(); 
        }

        var res = 0;

        for (int i = 0; i < underlyingValues.Length; i++)
        {
            int f = 0;
            for (U j = U.Zero; j < underlyingValues[i]; j++, f++);

            if ((value & (U.One << f)) != U.Zero)
            {
                res |= (1 << f);
            }
        }

        this.value = Unsafe.As<int, T>(ref res);
    }

    /// <summary>
    /// Convert enum value to integer value
    /// </summary>
    /// <returns>Integer value</returns>
    /// <exception cref="InvalidCastException">If enum empty or T type is not enum</exception>
    public U AsInteger()
    {
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        var v = this.value;
        var value = Unsafe.As<T, U>(ref v);

        if (values == null || underlyingValues == null)
        {
            throw new InvalidCastException();
        }

        U result = default;

        for (int i = 0; i < values.Length; i++)
        {
            int f = 0;
            for (U j = U.Zero; j < underlyingValues[i]; j++, f++);

            if ((value & (U.One << f)) != U.Zero)
            {
                result += (U.One << f);
            }
        }

        return result;
    }

    /// <summary>
    /// Format enum values as string where each value separated by comma 
    /// </summary>
    /// <returns>Formated string</returns>
    /// <exception cref="InvalidCastException">If enum empty or T type is not enum</exception>
    public override string ToString()
    {
        return string.Join(", ", Values);
    }

    private T[] GetValuesAsEnumArray()
    {
        var result = new List<T>();
        var v = this.value;
        var value = Unsafe.As<T, U>(ref v);
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        if (values == null || underlyingValues == null)
        {
            throw new InvalidCastException();
        }

        for (int i = 0; i < values.Length; i++)
        {
            int f = 0;
            for (U j = U.Zero; j < underlyingValues[i]; j++, f++) ;

            if ((value & (U.One << f)) != U.Zero)
            {
                result.Add(values[i]);
            }
        }

        return result.ToArray();
    }

    /// <summary>
    /// Combine flags from two enum flags values
    /// </summary>
    /// <param name="a">Left operand</param>
    /// <param name="b">Right operand</param>
    /// <returns>Combined flags from two enum flags values</returns>
    public static EnumFlag<T, U> operator|(EnumFlag<T, U> a, EnumFlag<T, U> b)
    {
        return new EnumFlag<T, U>(a.AsInteger() | b.AsInteger());
    }

    /// <summary>
    /// Combine flags from two enum flags values
    /// </summary>
    /// <param name="a">Left operand</param>
    /// <param name="b">Right operand</param>
    /// <returns>Combined flags from two enum flags values</returns>
    public static EnumFlag<T, U> operator |(EnumFlag<T, U> a, T b)
    {
        return new EnumFlag<T, U>(a.AsInteger() | new EnumFlag<T, U>(b).AsInteger());
    }
}