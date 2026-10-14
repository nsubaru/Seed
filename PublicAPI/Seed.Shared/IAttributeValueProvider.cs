using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.RTTI;

/// <summary>
/// Attribute value reader provider interface
/// </summary>
/// <typeparam name="T">Attribute value type</typeparam>
public interface IAttributeValueProvider<T>
{
    T GetAttributeValue(IntPtr rawData);
}
