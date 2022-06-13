using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization
{
    public enum ETypes : byte
	{
		MetaData,
		Logic,
		Numeric,
		Enumeration,
		FloatNumeric,
		Character,
		Collection,
		String,
		FixedCollection,
		AssociableCollection,
		Box,
		Tuple,
		Pair,
		Pointer,
		Reference,
		FixedArray,
		Struct,
		Object,
		Dynamic,
		Abstract,
		Function,
		Union,
		Pod,
		Custom,
		Undefine = byte.MaxValue
    }
}
