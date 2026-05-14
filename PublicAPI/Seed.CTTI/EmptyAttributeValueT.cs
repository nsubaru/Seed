using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.CTTI;

public struct EmptyAttributeValueT {
    public static EmptyAttributeValueT Empty => new EmptyAttributeValueT();

    public override bool Equals(object? obj)
    {
        return obj is EmptyAttributeValueT;
    }

    public override int GetHashCode() {
        return 0;
    }

    public override string ToString()
    {
        return "EmptyAttributeValueT";
    }
}
