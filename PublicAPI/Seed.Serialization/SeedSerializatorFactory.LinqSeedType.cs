using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? TryGetAsLinqSeedType(RTTI.Record.RttiTypeRecord record)
    {
        switch (record.TemplateTypeName)
        {
            case Constants.Types.Blob:
            case Constants.Types.SensitiveBlob:
                return typeof(byte[]);

            case Constants.Types.List:
            case Constants.Types.SensitiveList:
            case Constants.Types.ConcurencyList:
            case Constants.Types.ConcurencySensitiveList:
                {
                    var innerType = GetTypeWithOffset(0, record);

                    if (innerType == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ list type {1}", record.Fields.First().TypeName, record.TypeName);
                        return null;
                    }

                    return typeof(LinkedList<>).MakeGenericType(innerType);
                }

            case Constants.Types.Dictionary:
            case Constants.Types.SensitiveDictionary:
            case Constants.Types.ConcurencyDictionary:
            case Constants.Types.ConcurencySensitiveDictionary:

            case Constants.Types.Map:
            case Constants.Types.SensitiveMap:
            case Constants.Types.ConcurencyMap:
            case Constants.Types.ConcurencySensitiveMap:
                {
                    var innerTypeFirst = GetTypeWithOffset(1, record);
                    var innerTypeSecond = GetTypeWithOffset(2, record);

                    if (innerTypeFirst == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ associative type {1}", record.Fields.First().TypeName, record.TypeName);
                        return null;
                    }

                    if (innerTypeSecond == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ associative type {1}", record.Fields.Skip(1).First().TypeName, record.TypeName);
                        return null;
                    }

                    Type[] innerTypes = { innerTypeFirst, innerTypeSecond };

                    return typeof(Dictionary<,>).MakeGenericType(innerTypes);
                }
            case Constants.Types.Array:
            case Constants.Types.SensitiveArray:
            case Constants.Types.ConcurencyArray:
            case Constants.Types.ConcurencySensitiveArray:

            case Constants.Types.FixedArray:
            case Constants.Types.SensitiveFixedArray:
            case Constants.Types.ConcurencyFixedArray:
            case Constants.Types.ConcurencySensitiveFixedArray:

            case Constants.Types.DynamicStack:
            case Constants.Types.SensitiveDynamicStack:
            case Constants.Types.ConcurencyDynamicStack:
            case Constants.Types.ConcurencySensitiveDynamicStack:

            case Constants.Types.FixedStack:
            case Constants.Types.SensitiveFixedStack:
            case Constants.Types.ConcurencyFixedStack:
            case Constants.Types.ConcurencySensitiveFixedStack:

            case Constants.Types.FixedQueue:
            case Constants.Types.SensitiveFixedQueue:
            case Constants.Types.ConcurencyFixedQueue:
            case Constants.Types.ConcurencySensitiveFixedQueue:

            case Constants.Types.DynamicQueue:
            case Constants.Types.SensitiveDynamicQueue:
            case Constants.Types.ConcurencyDynamicQueue:
            case Constants.Types.ConcurencySensitiveDynamicQueue:
                {
                    var innerType = GetTypeWithOffset(0, record);

                    if (innerType == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ array type {1}", record.Fields.First().TypeName, record.TypeName);
                        return null; 
                    }

                    return typeof(List<>).MakeGenericType(innerType);
                }

            case Constants.Types.Pair:
            case Constants.Types.SensitivePair:
                {
                    var innerTypeFirst = GetTypeWithOffset(0, record);
                    var innerTypeSecond = GetTypeWithOffset(1, record);

                    if (innerTypeFirst == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ pair type {1}", record.Fields.First().TypeName, record.TypeName);
                        return null;
                    }

                    if (innerTypeSecond == null)
                    {
                        logger?.LogWarning("Can not found correspondent Seed type {0} for LINQ pair type {1}", record.Fields.Skip(1).First().TypeName, record.TypeName);
                        return null;
                    }

                    Type[] innerTypes = { innerTypeFirst, innerTypeSecond };

                    return typeof(KeyValuePair<,>).MakeGenericType(innerTypes);
                }

            default: return null;
        }
    }
}
