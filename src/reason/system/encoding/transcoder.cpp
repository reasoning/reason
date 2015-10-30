
/**
 * Reason
 * Copyright (C) 2009  Emerson Clarke
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "reason/system/encoding/transcoder.h"
#include "reason/system/string.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System::Encoding;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Transcoder::EncodeUrl()
{
	if (Reference.IsEmpty()) return;

	String encoded;
	encoded.Allocate(Reference.Size+20);

	char *c = Reference.Data;
	while(*c != 0)
	{
		if (Character::IsAlphanumeric(*c))
		{
			encoded.Append(*c);
		}
		else
		if (*c == ' ')
		{
			encoded.Append('+');
		}
		else
		{
			encoded.Append('%');

			int val = (int)(*c >> 4) & 0xF;
			if (val > 9)
			{

				val -=10;
				encoded.Append((char)('A' + val));
			}
			else
			{
				encoded.Append((char)('0' + val));
			}

			val = (int)(*c & 0xF);
			if (val > 9)
			{

				val -=10;
				encoded.Append((char)('A' + val));
			}
			else
			{
				encoded.Append((char)('0' + val));
			}
		}

		++c;
	}

	Reference.Construct(encoded);
}

void Transcoder::DecodeUrl()
{
	if (Reference.IsEmpty()) return;

	String decoded;
	if (decoded.Allocated < Reference.Size)
		decoded.Allocate(Reference.Size);

	char *c = Reference.Data;

	while (*c != 0)
	{
		if (*c == '+')
		{
			decoded << ' ';
			++c;
		}
		else
		if (*c == '%' && isxdigit(*(c+1)) != 0 && isxdigit(*(c+2)) != 0)
		{

			decoded << (char) Sequences::Hex((c+1),2);
			c += 3;
		}
		else
		{
			decoded << *c;
			++c;
		}
	}

	Reference.Construct(decoded);
}

void Transcoder::EncodeRunlength()
{
}

void Transcoder::DecodeRunlength()
{
}

void Transcoder::EncodeHex()
{

}

void Transcoder::DecodeHex()
{
}

void Transcoder::EncodeBase64()
{
	if (Reference.IsEmpty()) return;

    static const char encoderTable[] =
    {

		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',

		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',

		'0','1','2','3','4','5','6','7','8','9',

        '+','/'
    };

	String encoded;
	encoded.Allocate((Reference.Size*4)/3);

    int trailer = Reference.Size%3;
	int i=0;
    for (; i < (Reference.Size-trailer); i+=3)
    {
		encoded.Append(encoderTable[(Reference.Data[i]>>2)&0x3F]);
        encoded.Append(encoderTable[((Reference.Data[i]&0x03)<<4)|((Reference.Data[i+1]>>4)&0x0F)]);
        encoded.Append(encoderTable[((Reference.Data[i+1]&0x0F)<<2)|((Reference.Data[i+2]>>6)&0x03)]);
        encoded.Append(encoderTable[Reference.Data[i+2]&0x3F]);
    }

    if (trailer) 
    {
        if (trailer == 1)
        {

			Reference.Append((char)0);
			Reference.Append((char)0);
			encoded.Append(encoderTable[(Reference.Data[i]>>2)&0x3F]);
		    encoded.Append(encoderTable[((Reference.Data[i]&0x03)<<4)|((Reference.Data[i+1]>>4)&0x0F)]);
            encoded.Append("==");
        }
        else

        {

			Reference.Append((char)0);
			encoded.Append(encoderTable[(Reference.Data[i]>>2)&0x3F]);
		    encoded.Append(encoderTable[((Reference.Data[i]&0x03)<<4)|((Reference.Data[i+1]>>4)&0x0F)]);
	        encoded.Append(encoderTable[((Reference.Data[i+1]&0x0F)<<2)|((Reference.Data[i+2]>>6)&0x03)]);
			encoded.Append("=");
        }
     }

	Reference.Construct(encoded);
}

void Transcoder::DecodeBase64()
{
	if (Reference.IsEmpty()) return;

	static const int decoderTable[] =
	{
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	

		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,	
		52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,	

		 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,	
		15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,	

		 0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,	
		41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,	
	};

	char code;
	String decoded;
	decoded.Allocate(Reference.Size);

    for (int i=0; i<Reference.Size; i+=4)
    {

		code = ((decoderTable[Reference.Data[i]]<<2)|(decoderTable[Reference.Data[i+1]]>>4));
        decoded.Append(code);

		code = (((decoderTable[Reference.Data[i+1]]&0x0F)<<4)|(decoderTable[Reference.Data[i+2]]>>2));
		if (code != 0) decoded.Append(code);

		code = (((decoderTable[Reference.Data[i+2]]&0x03)<<6)|(decoderTable[Reference.Data[i+3]]));
		if (code != 0) decoded.Append(code);
    }

	Reference.Construct(decoded);
}

void Transcoder::EncodeUu()
{
}

void Transcoder::DecodeUu()
{
}

void Transcoder::EncodeMime()
{
}

void Transcoder::DecodeMime()
{
}

void Transcoder::EncodeUtf8()
{
}

void Transcoder::DecodeUtf8()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

