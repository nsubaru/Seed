using System;
using System.Collections.Generic;
using System.Text;

namespace Seed
{
    public class DynamicObject
    {
        public object Value { get ;set; }

        public DynamicObject(object value)
        {
            Value = value ?? throw new ArgumentNullException(nameof(value));
        }
    }
}
