using System.Numerics;
using System.Runtime.CompilerServices;

namespace Seed.Core;

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
    public T Value { get; set; }

    public EnumFlag() { }

    public EnumFlag(T value) { Value = value; }

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
                res = res | (1 << f);
            }
        }

        Value = Unsafe.As<int, T>(ref res);
    }

    public U AsInteger()
    {
        var values = (typeof(T).GetEnumValues() as T[]);
        var underlyingValues = (typeof(T).GetEnumValuesAsUnderlyingType() as U[]);

        if (values == null || underlyingValues == null)
            throw new InvalidCastException();

        U result = default(U);

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
}

