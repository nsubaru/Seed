using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.RTTI.Utils
{
    internal unsafe static class Utils
    {
        public static SizeT BinnarySearchInBlob(Byte* blob, SizeT len, Byte[] what, SizeT offset)
        {
			for (SizeT i = offset; i < len - (SizeT) what.Length; i++)
			{
				if (blob[i] == what[0])
				{
					SizeT j = 1;

					for (; j < (SizeT) what.Length; j++)
					{
						if (blob[i + j] != what[j])
							break;
					}

					if (j != (SizeT) what.Length)
						continue;

					return i;
				}
			}

			return SizeT.MaxValue;
		}
    }
}
