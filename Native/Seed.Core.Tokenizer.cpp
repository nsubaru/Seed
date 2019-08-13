// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Tokenizer.h"

using namespace Seed;

Tuple<SizeT, SizeT, SizeT> GetNextToken(CharStr Text, SizeT Size, CharStr* Delims, SizeT* DelimSizes, SizeT DelimsCount, SizeT StartSize)
{
	for (SizeT i = StartSize + 1; i < Size; i++)
	{
		for (SizeT j = 0; j < DelimsCount; j++)
		{
			if (Text[i] == Delims[j][0] && Size - i > DelimSizes[j]) {
				SizeT Pos = i;
				Boolean Found = true;

				if (DelimSizes[j] > 1) {
					for (i; i < Pos + DelimSizes[j]; i++)
					{
						if (Text[i] != Delims[j][i - Pos]) {
							Found = false;
							break;
						}
					}
				}

				if (Found)
					return Tuple<SizeT, SizeT, SizeT>( StartSize, Pos - StartSize, j );
				else
					i = Pos;
			}
		}
	}
	return Tuple<SizeT, SizeT, SizeT>(StartSize, Size - StartSize, Seed::Math::Max<SizeT>() );
}
