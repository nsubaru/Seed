using System.Numerics;
using System.Runtime.CompilerServices;

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
    /// <summary>
    /// Enum value
    /// </summary>
    public T Value { get; set; }

    /// <summary>
    /// Empty ctor
    /// </summary>
    public EnumFlag() { }

    /// <summary>
    /// Ctor from enum value
    /// </summary>
    /// <param name="value">Enum value</param>
    public EnumFlag(T value) { Value = value; }

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
            throw new InvalidCastException();

        var res = 0;
        for (int i = 0; i < underlyingValues.Length; i++)
        {
            int f = 0;
            for (U j = U.Zero; j < underlyingValues[i]; j++, f++) ;

            if ((value & (U.One << f)) == U.Zero)
            {
                res |= (1 << f);
            }
        }

        Value = Unsafe.As<int, T>(ref res);
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

        if (values == null || underlyingValues == null)
            throw new InvalidCastException();

        U result = default;

        for (int i = 0; i < values.Length; i++)
        {
            if (Value.HasFlag(values[i]))
            {
                int f = 0;
                for (U j = U.Zero; j < underlyingValues[i]; j++, f++) ;

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
        var result = new List<T>();
        var v = Value;
        var value = Unsafe.As<T, U>(ref v);
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        if (values == null || underlyingValues == null)
            throw new InvalidCastException();

        for (int i = 0; i < values.Length; i++)
        {
            if (Value.HasFlag(values[i]))
            {
                result.Add(values[i]);
            }
        }

        return string.Join(", ", result);
    }
}