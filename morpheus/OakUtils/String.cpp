#include "oakUtilsPCH.hpp"

// Class constructors

/*
==================
String::String
==================
*/
String::String()
{
	Construct();
}

/*
==================
String::String
==================
*/
String::String(const String& text)
{
	Construct();

	int l;
	
	l = text.Length();
	EnsureAlloced(l + 1);

	strcpy(data, text.data);
	length = l;
}

/*
==================
String::String
==================
*/
String::String(const String& text, int start, int end)
{
	Construct();

	int i;
	int l;
	
	if (end > text.Length())
	{
		end = text.Length();
	}

	if (start > text.Length())
	{
		start = text.Length();
	}

	else if (start < 0)
	{
		start = 0;
	}
	
	l = end - start;

	if (l < 0)
	{
		l = 0;
	}
	
	EnsureAlloced(l + 1);
	
	for (i = 0; i < l; i++)
	{
		data[i] = text[start + i];
	}
	
	data[l] = '\0';
	length = l;
}

/*
==================
String::String
==================
*/
String::String(const char* text)
{
	Construct();

	int l;
	
	if (text)
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		l = (int) strlen(text);
		// RB end
		EnsureAlloced(l + 1);
		strcpy(data, text);
		length = l;
	}	
}

/*
==================
String::String
==================
*/
String::String(const char* text, int start, int end)
{
	Construct();

	int i;
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	int l = (int) strlen(text);
	// RB end
	
	if (end > l)
	{
		end = l;
	}

	if (start > l)
	{
		start = l;
	}

	else if (start < 0)
	{
		start = 0;
	}
	
	l = end - start;
	if (l < 0)
	{
		l = 0;
	}
	
	EnsureAlloced(l + 1);
	
	for (i = 0; i < l; i++)
	{
		data[i] = text[start + i];
	}
	
	data[l] = '\0';
	length = l;
}

/*
==================
String::String
==================
*/
String::String(const bool b)
{
	Construct();
	EnsureAlloced( 2 );
	data[0] = b ? '1' : '0';
	data[1] = '\0';
	length = 1;
}

/*
==================
String::String
==================
*/
String::String(const char c)
{
	Construct();
	EnsureAlloced( 2 );
	data[0] = c;
	data[1] = '\0';
	length = 1;
}

/*
==================
String::String
==================
*/
String::String(const int i)
{
	Construct();
	char text[64];
	int l;
	
	l = sprintf(text, "%d", i);
	EnsureAlloced(l + 1);
	strcpy(data, text);
	length = l;
}

/*
==================
String::String
==================
*/
String::String(const unsigned u)
{
	Construct();
	char text[64];
	int l;
	
	l = sprintf(text, "%u", u);
	EnsureAlloced(l + 1);
	strcpy(data, text);
	length = l;
}

/*
==================
String::String
==================
*/
String::String(const float f)
{
	Construct();
	char text[64];
	int l;
	
	l = CharMethods.snPrintf( text, sizeof( text ), "%f", f );
	while(l > 0 && text[l - 1] == '0') 
	{
		text[--l] = '\0';
	}

	while(l > 0 && text[l - 1] == '.')
	{
		text[--l] = '\0';
	}
	EnsureAlloced(l + 1);
	strcpy(data, text);
	length = l;
}

/*
==================
String::String
==================
*/
String::~String()
{
	FreeData();
}

// String Operators

/*
==================
String::operator
==================
*/
String::operator const char* () const
{
	return c_str();
}

/*
==================
String::operator
==================
*/
String::operator const char* ()
{
	return c_str();
}

char String::operator[](int index) const
{
	assert((index >= 0) && (index <= length));
	return data[index];
}

char& String::operator[](int index)
{
	assert((index >= 0) && (index <= length));
	return data[index];
}

void String::operator=(const String& text)
{
	int l;
	
	l = text.Length();
	EnsureAlloced( l + 1, false );
	memcpy( data, text.data, l );
	data[l] = '\0';
	length = l;
}

String operator+(const String& a, const String& b)
{
	String result( a );
	result.Append( b );
	return result;
}

String operator+(const String& a, const char* b)
{
	String result( a );
	result.Append( b );
	return result;
}

String operator+(const char* a, const String& b)
{
	String result( a );
	result.Append( b );
	return result;
}

String operator+(const String& a, const bool b)
{
	String result( a );
	result.Append( b ? "true" : "false" );
	return result;
}

String operator+(const String& a, const char b)
{
	String result( a );
	result.Append( b );
	return result;
}

String operator+(const String& a, const float b)
{
	char	text[ 64 ];
	String	result( a );
	
	sprintf( text, "%f", b );
	result.Append( text );
	
	return result;
}

String operator+(const String& a, const int b)
{
	char	text[ 64 ];
	String	result( a );
	
	sprintf( text, "%d", b );
	result.Append( text );
	
	return result;
}

String operator+(const String& a, const unsigned b)
{
	char	text[64];
	String	result( a );
	
	sprintf(text, "%u", b);
	result.Append(text);
	
	return result;
}

String& String::operator+=(const float a)
{
	char text[64];
	
	sprintf(text, "%f", a);
	Append(text);
	
	return *this;
}

String& String::operator+=(const int a)
{
	char text[64];
	
	sprintf(text, "%d", a);
	Append(text);
	
	return *this;
}

String& String::operator+=(const unsigned a)
{
	char text[64];
	
	sprintf(text, "%u", a);
	Append(text);
	
	return *this;
}

String& String::operator+=(const String& a)
{
	Append(a);
	return *this;
}

String& String::operator+=(const char* a)
{
	Append(a);
	return *this;
}

String& String::operator+=(const char a)
{
	Append(a);
	return *this;
}

String& String::operator+=(const bool a)
{
	Append(a ? "true" : "false");
	return *this;
}

bool operator==(const String& a, const String& b)
{
	return (!CharMethods.Cmp(a.data, b.data));
}

bool operator==(const String& a, const char* b)
{
	assert(b);
	return (!CharMethods.Cmp(a.data, b));
}

bool operator==(const char* a, const String& b)
{
	assert(a);
	return (!CharMethods.Cmp(a, b.data));
}

bool operator!=(const String& a, const String& b)
{
	return !(a == b);
}

bool operator!=(const String& a, const char* b)
{
	return !(a == b);
}

bool operator!=(const char* a, const String& b)
{
	return !(a == b);
}

// Class Functions

/*
========================
String::Construct
========================
*/
void String::Construct()
{
	SetStatic(false);
	SetAlloced(STR_ALLOC_BASE);
	data = baseBuffer;
	length = 0;
	data[0] = '\0';
}

/*
============
String::FreeData
============
*/
void String::FreeData()
{
	if (IsStatic())
	{
		return;
	}
	
	if (data && data != baseBuffer)
	{
		delete[] data;
		data = baseBuffer;
	}
}

/*
============
String::IsStatic
============
*/
bool String::IsStatic() const
{
	return (allocedAndFlag & STATIC_MASK) != 0;
}

/*
============
String::SetStatic
============
*/
void String::SetStatic(const bool isStatic)
{
	allocedAndFlag = (allocedAndFlag & ALLOCED_MASK) | (isStatic << STATIC_BIT);
}

/*
============
String::GetAlloced
============
*/
int String::GetAlloced() const
{
	return allocedAndFlag & ALLOCED_MASK;
}

/*
============
String::SetAlloced
============
*/
void String::SetAlloced(const int a)
{
	allocedAndFlag = (allocedAndFlag & STATIC_MASK) | (a & ALLOCED_MASK);
}
/*
============
String::Allocated
============
*/
int String::Allocated() const
{
	if (data != baseBuffer)
	{
		return GetAlloced();
	}
	else
	{
		return 0;
	}
}

/*
============
String::EnsureAlloced
============
*/
void String::EnsureAlloced(int amount, bool keepold)
{
	// static string's can't reallocate
	if (IsStatic())
	{
		return;
	}
	if (amount > GetAlloced())
	{
		ReAllocate(amount, keepold);
	}
}

/*
============
String::ReAllocate
============
*/
void String::ReAllocate(int amount, bool keepold)
{
	char*	newbuffer;
	int		newsize;
	int		mod;
	
	assert(amount > 0);
	
	mod = amount % STR_ALLOC_GRAN;

	if (!mod)
	{
		newsize = amount;
	}

	else
	{
		newsize = amount + STR_ALLOC_GRAN - mod;
	}

	SetAlloced(newsize);
	
	newbuffer = new char[GetAlloced()];

	if (keepold && data)
	{
		data[length] = '\0';
		strcpy(newbuffer, data);
	}
	
	if (data && data != baseBuffer)
	{
		delete[] data;
	}
	
	data = newbuffer;
}

/*
============
String::Size
============
*/
size_t String::Size() const
{
	return sizeof(*this) + Allocated();
}

/*
============
String::c_str
============
*/
const char* String::c_str() const
{
	return data;
}

/*
============
String::Length
============
*/
int String::Length() const
{
	return length;
}

/*
============
String::LengthWithoutColors
============
*/
int String::LengthWithoutColors() const
{
	return CharMethods.LengthWithoutColors(data);
}

/*
============
String::Empty
============
*/
void String::Empty()
{
	EnsureAlloced(1);
	data[0] = '\0';
	length = 0;
}

/*
============
String::Clear
============
*/
void String::Clear()
{
	if (IsStatic())
	{
		length = 0;
		data[0] = '\0';
		return;
	}

	FreeData();
	Construct();
}

/*
============
String::CapLength
============
*/
void String::CapLength(int newLength)
{
	if (length <= newLength)
	{
		return;
	}

	data[newLength] = 0;
	length = newLength;
}

void String::Fill(const char ch, int newLength)
{
	EnsureAlloced(newLength + 1);
	length = newLength;
	memset(data, ch, length);
	data[length] = 0;
}

/*
============
String::Append
============
*/
void String::Append(const char a)
{
	EnsureAlloced(length + 2);

	data[length] = a;
	length++;
	data[length] = '\0';
}

/*
============
String::Append
============
*/
void String::Append(const String& text)
{
	int newLength;
	int i;
	
	newLength = length + text.Length();
	EnsureAlloced(newLength + 1);

	for (i = 0; i < text.length; i++)
	{
		data[length + i] = text[i];
	}

	length = newLength;
	data[length] = '\0';
}

/*
============
String::Append
============
*/
void String::Append(const char* text)
{
	int newLength;
	int i;
	
	if (text)
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		newLength = length + (int)strlen(text);
		// RB end
		EnsureAlloced(newLength + 1);

		for (i = 0; text[i]; i++)
		{
			data[length + i] = text[i];
		}

		length = newLength;
		data[length] = '\0';
	}
}

/*
============
String::Append
============
*/
void String::Append(const char* text, int l)
{
	int newLength;
	int i;
	
	if (text && l)
	{
		newLength = length + l;
		EnsureAlloced(newLength + 1);

		for (i = 0; text[i] && i < l; i++)
		{
			data[length + i] = text[i];
		}

		length = newLength;
		data[length] = '\0';
	}
}

/*
============
String::Insert
============
*/
void String::Insert(const char a, int index)
{
	int i, l;
	
	if (index < 0)
	{
		index = 0;
	}
	else if (index > length)
	{
		index = length;
	}
	
	l = 1;

	EnsureAlloced(length + l + 1);

	for (i = length; i >= index; i--)
	{
		data[i + l] = data[i];
	}

	data[index] = a;
	length++;
}

/*
============
String::Insert
============
*/
void String::Insert(const char* text, int index)
{
	int i, l;
	
	if (index < 0)
	{
		index = 0;
	}

	else if (index > length)
	{
		index = length;
	}
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(text);
	// RB end

	EnsureAlloced(length + l + 1);

	for (i = length; i >= index; i--)
	{
		data[i + l] = data[i];
	}

	for (i = 0; i < l; i++)
	{
		data[index + i] = text[i];
	}

	length += l;
}

/*
============
String::ToLower
============
*/
void String::ToLower()
{
	for (int i = 0; data[i]; i++)
	{
		if (CharMethods.CharIsUpper(data[i]))
		{
			data[i] += ( 'a' - 'A' );
		}
	}
}

/*
============
String::ToUpper
============
*/
void String::ToUpper()
{
	for (int i = 0; data[i]; i++)
	{
		if (CharMethods.CharIsLower(data[i]))
		{
			data[i] -= ( 'a' - 'A' );
		}
	}
}

/*
============
String::IsEmpty
============
*/
bool String::IsEmpty() const
{
	return (CharMethods.Cmp(data, "") == 0);
}

/*
============
String::IsNumeric
============
*/
bool String::IsNumeric() const
{
	return CharMethods.IsNumeric(data);
}

/*
============
String::IsColor
============
*/
bool String::IsColor() const
{
	return CharMethods.IsColor(data);
}

/*
============
String::HasLower
============
*/
bool String::HasLower() const
{
	return CharMethods.HasLower(data);
}

/*
============
String::HasUpper
============
*/
bool String::HasUpper() const
{
	return CharMethods.HasUpper(data);
}

/*
============
String::Cmp
============
*/
int String::Cmp(const char* text) const
{
	assert(text);

	return CharMethods.Cmp(data, text);
}

/*
============
String::Cmpn
============
*/
int String::Cmpn(const char* text, int n) const
{
	assert(text);

	return CharMethods.Cmpn(data, text, n);
}

/*
============
String::CmpPrefix
============
*/
int String::CmpPrefix(const char* text) const
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods.Cmpn(data, text, (int)strlen(text));
	// RB end
}

/*
============
String::Icmp
============
*/	
int String::Icmp(const char* text) const
{
	assert(text);

	return CharMethods.Icmp(data, text);
}

/*
============
String::Icmpn
============
*/
int String::Icmpn(const char* text, int n) const
{
	assert(text);

	return CharMethods.Icmpn(data, text, n);
}

/*
============
String::IcmpPrefix
============
*/
int String::IcmpPrefix(const char* text) const
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods.Icmpn(data, text, (int)strlen(text));
	// RB end
}

/*
============
String::IcmpPath
============
*/
int String::IcmpPath(const char* text) const
{
	assert(text);

	return CharMethods.IcmpPath(data, text);
}

/*
============
String::IcmpnPath
============
*/
int String::IcmpnPath(const char* text, int n) const
{
	assert(text);

	return CharMethods.IcmpnPath(data, text, n);
}

/*
============
String::IcmpPrefixPath
============
*/
int String::IcmpPrefixPath(const char* text) const
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods.IcmpnPath(data, text, (int)strlen(text));
	// RB end
}

/*
============
String::IcmpNoColor
============
*/	
int String::IcmpNoColor(const char* text) const
{
	assert(text);

	return CharMethods.IcmpNoColor(data, text);
}

/*
============
String::UTF8Length
============
*/
int String::UTF8Length()
{
	return UTF8Length((byte*)data);
}

/*
============
String::UTF8Length
============
*/
int String::UTF8Length(const byte* s)
{
	int mbLen = 0;
	int charLen = 0;

	while(s[mbLen] != '\0')
	{
		uint32 cindex;
		cindex = s[mbLen];

		if (cindex < 0x80)
		{
			mbLen++;
		}
		else
		{
			int trailing = 0;

			if (cindex >= 0xc0)
			{
				static const byte trailingBytes[64] =
				{
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
				};
				trailing = trailingBytes[cindex - 0xc0];
			}

			mbLen += trailing + 1;
		}

		charLen++;
	}
	return charLen;
}

/*
============
String::Find
============
*/
int String::Find(const char c, int start, int end) const
{
	if (end == -1)
	{
		end = length;
	}

	return CharMethods.FindChar(data, c, start, end);
}

int String::Find(const char* text, bool casesensitive, int start, int end) const
{
	if (end == -1)
	{
		end = length;
	}

	return CharMethods.FindText(data, text, casesensitive, start, end);
}

/*
============
String::UTF8Char
============
*/
uint32 String::UTF8Char(int& idx)
{
	return UTF8Char((byte*)data, idx);
}

/*
============
String::UTF8Char
============
*/
uint32 String::UTF8Char(const char* s, int& idx)
{
	return UTF8Char((byte*)s, idx);
}

/*
============
String::UTF8Char
============
*/
uint32 String::UTF8Char(const byte* s, int& idx)
{
	if (idx >= 0)
	{
		while(s[idx] != '\0')
		{
			uint32 cindex = s[idx];

			if (cindex < 0x80)
			{
				idx++;
				return cindex;
			}
			int trailing = 0;
			if (cindex >= 0xc0)
			{
				static const byte trailingBytes[ 64 ] =
				{
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
				};
				trailing = trailingBytes[ cindex - 0xc0 ];
			}
			static const uint32 trailingMask[ 6 ] = {0x0000007f, 0x0000001f, 0x0000000f, 0x00000007, 0x00000003, 0x00000001};

			cindex &= trailingMask[trailing];

			while (trailing-- > 0)
			{
				cindex <<= 6;
				cindex += s[ ++idx ] & 0x0000003f;
			}
			idx++;
			return cindex;
		}
	}

	idx++;
	return 0;	// return a null terminator if out of range
}

/*
============
String::AppendUTF8Char
============
*/
void String::AppendUTF8Char(uint32 c)
{
	if (c < 0x80)
	{
		Append((char)c);
	}
	else if (c < 0x800)      // 11 bits
	{
		Append((char)(0xC0 | (c >> 6)));
		Append((char)(0x80 | (c & 0x3F)));
	}
	else if (c < 0x10000)      // 16 bits
	{
		Append((char)(0xE0 | (c >> 12)));
		Append((char)(0x80 | ((c >> 6) & 0x3F)));
		Append((char)(0x80 | (c & 0x3F)));
	}
	else if (c < 0x200000)  	// 21 bits
	{
		Append((char)(0xF0 | (c >> 18)));
		Append((char)(0x80 | (( c >> 12) & 0x3F)));
		Append((char)(0x80 | (( c >> 6) & 0x3F)));
		Append((char)(0x80 | (c & 0x3F)));
	}
	else
	{
		// UTF-8 can encode up to 6 bytes. Why don't we support that?
		// This is an invalid Unicode character
		Append('?');
	}
}

/*
============
String::ConvertToUTF8
============
*/
void String::ConvertToUTF8()
{
	String temp(*this);

	Clear();

	for(int index = 0; index < temp.Length(); ++index)
	{
		AppendUTF8Char(temp[index]);
	}
}

/*
============
String::IsValidUTF8
============
*/
bool String::IsValidUTF8(const uint8* s, const int maxLen, utf8Encoding_t& encoding)
{
	struct local_t
	{
		static int GetNumEncodedUTF8Bytes(const uint8 c)
		{
			if (c < 0x80)
			{
				return 1;
			}
			else if ((c >> 5) == 0x06)
			{
				// 2 byte encoding - the next byte must begin with
				return 2;
			}
			else if ((c >> 4) == 0x0E)
			{
				// 3 byte encoding
				return 3;
			}
			else if ((c >> 5) == 0x1E)
			{
				// 4 byte encoding
				return 4;
			}
			// this isnt' a valid UTF-8 precursor character
			return 0;
		}

		static bool RemainingCharsAreUTF8FollowingBytes(const uint8* s, const int curChar, const int maxLen, const int num)
		{
			if (maxLen - curChar < num)
			{
				return false;
			}
			for (int i = curChar + 1; i <= curChar + num; i++)
			{
				if (s[i] == '\0')
				{
					return false;
				}

				if((s[i] >> 6 ) != 0x02)
				{
					return false;
				}
			}
			return true;
		}
	};
	
	// check for byte-order-marker
	encoding = UTF8_PURE_ASCII;
	utf8Encoding_t utf8Type = UTF8_ENCODED_NO_BOM;

	if (maxLen > 3 && s[0] == 0xEF && s[1] == 0xBB && s[2] == 0xBF)
	{
		utf8Type = UTF8_ENCODED_BOM;
	}
	
	for (int i = 0; s[i] != '\0' && i < maxLen; i++)
	{
		int numBytes = local_t::GetNumEncodedUTF8Bytes(s[i]);
		if (numBytes == 1)
		{
			continue;	// just low ASCII
		}
		else if (numBytes == 2)
		{
			// 2 byte encoding - the next byte must begin with bit pattern 10
			if (!local_t::RemainingCharsAreUTF8FollowingBytes(s, i, maxLen, 1))
			{
				return false;
			}
			// skip over UTF-8 character
			i += 1;
			encoding = utf8Type;
		}
		else if (numBytes == 3)
		{
			// 3 byte encoding - the next 2 bytes must begin with bit pattern 10
			if (!local_t::RemainingCharsAreUTF8FollowingBytes(s, i, maxLen, 2))
			{
				return false;
			}
			// skip over UTF-8 character
			i += 2;
			encoding = utf8Type;
		}
		else if (numBytes == 4)
		{
			// 4 byte encoding - the next 3 bytes must begin with bit pattern 10
			if (!local_t::RemainingCharsAreUTF8FollowingBytes(s, i, maxLen, 3))
			{
				return false;
			}
			// skip over UTF-8 character
			i += 3;
			encoding = utf8Type;
		}
		else
		{
			// this isnt' a valid UTF-8 character
			if (utf8Type == UTF8_ENCODED_BOM)
			{
				encoding = UTF8_INVALID_BOM;
			}
			else
			{
				encoding = UTF8_INVALID;
			}

			return false;
		}
	}
	return true;
}

/*
============
String::IsValidUTF8
============
*/
bool String::IsValidUTF8(const char* s, const int maxLen, utf8Encoding_t& encoding)
{
	return IsValidUTF8((const uint8*)s, maxLen, encoding);
}

/*
============
String::RemoveColors
============
*/
String& String::RemoveColors()
{
	CharMethods.RemoveColors(data);
	length = CharMethods.Length(data);
	return *this;
}

/*
============
String::StripTrailingOnce
============
*/
bool String::StripTrailingOnce(const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if ((l > 0) && (length >= l) && !CharMethods.Cmpn(string, data + length - l, l))
	{
		length -= l;
		data[length] = '\0';
		return true;
	}

	return false;
}

/*
============
String::StripLeadingOnce
============
*/
bool String::StripLeadingOnce(const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if ((l > 0) && !Cmpn(string, l))
	{
		memmove(data, data + l, length - l + 1);

		length -= l;
		return true;
	}

	return false;
}

/*
============
String::StripLeading
============
*/
void String::StripLeading(const char c)
{
	while (data[0] == c)
	{
		memmove(&data[0], &data[1], length);
		length--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripLeading(const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end
	if (l > 0)
	{
		while (!Cmpn(string, l))
		{
			memmove (data, data + l, length - l + 1);
			length -= l;
		}
	}
}

/*
============
String::StripTrailing
============
*/
void String::StripTrailing(const char c)
{
	int i;
	
	for (i = Length(); i > 0 && data[i - 1] == c; i--)
	{
		data[i - 1] = '\0';
		length--;
	}
}

/*
============
String::StripLeading
============
*/
void String::StripTrailing(const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if (l > 0)
	{
		while ((length >= l) && !CharMethods.Cmpn(string, data + length - l, l))
		{
			length -= l;
			data[length] = '\0';
		}
	}

}

/*
==================
CharMethods::Length
==================
*/
int CharMethods::Length(const char* s)
{
	int i;

	for(i = 0; s[i]; i++) {}
	return i;
}

/*
==================
CharMethods::LengthWithoutColors
==================
*/
int CharMethods::LengthWithoutColors(const char* s)
{
	int len;
	const char* p;
	
	if (!s)
	{
		return 0;
	}
	
	len = 0;
	p = s;

	while (*p)
	{
		if (IsColor(p))
		{
			p += 2;
			continue;
		}
		p++;
		len++;
	}
	
	return len;
}

/*
==================
CharMethods::Cmp
==================
*/
int CharMethods::Cmp(const char* s1, const char* s2)
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;

		if (d)
		{
			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}

	while (c1);
	
	return 0;		// strings are equal
}

/*
==================
CharMethods::Cmpn
==================
*/
int CharMethods::Cmpn(const char* s1, const char* s2, int n)
{
	int c1, c2, d;
	
	assert(n >= 0);
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if( !n-- )
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;
		if (d)
		{
			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}

	while (c1);
	
	return 0;		// strings are equal
}

/*
================
CharMethods::Icmp
================
*/
int CharMethods::Icmp(const char* s1, const char* s2)
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;

		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ('a' - 'A');
				if (!d)
				{
					break;
				}
			}
			
			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ('a' - 'A');
				if (!d)
				{
					break;
				}
			}

			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}

	}

	while (c1);
	
	return 0;		// strings are equal
}


/*
==================
CharMethods::Icmpn
==================
*/
int	CharMethods::Icmpn(const char* s1, const char* s2, int n)
{
	int c1, c2, d;
	
	assert(n >= 0);
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if (!n--)
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;

		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ( 'a' - 'A' );
				if (!d)
				{
					break;
				}
			}

			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ( 'a' - 'A' );
				if (!d)
				{
					break;
				}
			}

			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}

	while (c1);
	
	return 0;		// strings are equal
}

/*
==================
CharMethods::IcmpNoColor
==================
*/
int CharMethods::IcmpNoColor(const char* s1, const char* s2)
{
	int c1, c2, d;
	
	do
	{
		while (IsColor(s1))
		{
			s1 += 2;
		}

		while (IsColor( s2))
		{
			s2 += 2;
		}

		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;

		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ('a' - 'A');

				if (!d)
				{
					break;
				}
			}
			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ('a' - 'A');
				if (!d)
				{
					break;
				}
			}

			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}

	while (c1);
	
	return 0;		// strings are equal
}

/*
==================
CharMethods::IcmpPath
==================
*/
int CharMethods::IcmpPath(const char* s1, const char* s2)
{
	int c1, c2, d;
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		d = c1 - c2;

		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ('a' - 'A');

				if (!d)
				{
					break;
				}
			}
			if (c1 == '\\')
			{
				d += ( '/' - '\\' );

				if (!d)
				{
					break;
				}
			}
			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ( 'a' - 'A' );
				if (!d)
				{
					break;
				}
			}
			if (c2 == '\\')
			{
				d -= ( '/' - '\\' );
				if (!d)
				{
					break;
				}
			}
			// make sure folders come first
			while (c1)
			{
				if (c1 == '/' || c1 == '\\')
				{
					break;
				}

				c1 = *s1++;
			}

			while (c2)
			{
				if (c2 == '/' || c2 == '\\')
				{
					break;
				}
				c2 = *s2++;
			}
			
			if (c1 && !c2)
			{
				return -1;
			}
			else if (!c1 && c2)
			{
				return 1;
			}

			// same folder depth so use the regular compare
			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}
	while (c1);
	
	return 0;
}

/*
==================
CharMethods::IcmpnPath
==================
*/
int CharMethods::IcmpnPath(const char* s1, const char* s2, int n)
{
	int c1, c2, d;
	
	assert(n >= 0);
	
	do
	{
		c1 = *s1++;
		c2 = *s2++;
		
		if (!n--)
		{
			return 0;		// strings are equal until end point
		}
		
		d = c1 - c2;
		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ('a' - 'A');
				if (!d)
				{
					break;
				}
			}
			if (c1 == '\\')
			{
				d += ('/' - '\\');
				if (!d)
				{
					break;
				}
			}
			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ('a' - 'A');
				if (!d)
				{
					break;
				}
			}
			if (c2 == '\\')
			{
				d -= ( '/' - '\\' );
				if (!d)
				{
					break;
				}
			}
			// make sure folders come first
			while (c1)
			{
				if (c1 == '/' || c1 == '\\')
				{
					break;
				}
				c1 = *s1++;
			}
			while (c2)
			{
				if (c2 == '/' || c2 == '\\')
				{
					break;
				}
				c2 = *s2++;
			}
			if (c1 && !c2)
			{
				return -1;
			}
			else if (!c1 && c2)
			{
				return 1;
			}

			// same folder depth so use the regular compare
			return (INT32_SIGNBITNOTSET(d) << 1) - 1;
		}
	}

	while (c1);
	
	return 0;
}

/*
==================
CharMethods::snPrintf
==================
*/
int CharMethods::snPrintf(char* dest, int size, const char* fmt, ...)
{
	int len;
	va_list argptr;
	char buffer[32000];	// big, but small enough to fit in PPC stack
	
	va_start(argptr, fmt);
	len = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	if (len >= size)
	{
		len = size;
	}

	Copynz(dest, buffer, size);
	return len;
}

/*
============
CharMethods::vsnPrintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final string if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count, then count
characters are written and -1 is returned and no trailing '\0' is added.

CharMethods::vsnPrintf: always appends a trailing '\0', returns number of characters written (not including terminal \0)
or returns -1 on failure or if the buffer would be overflowed.
============
*/
int CharMethods::vsnPrintf(char* dest, int size, const char* fmt, va_list argptr)
{
	int ret;
	
	// RB begin
	#ifdef _WIN32
		#undef _vsnprintf
		ret = _vsnprintf(dest, size - 1, fmt, argptr);
		#define _vsnprintf	use_charMethods_vsnPrintf
	#else
		#undef vsnprintf
		ret = vsnprintf(dest, size, fmt, argptr);
		#define vsnprintf	use_charMethods_vsnPrintf
	#endif
	// RB end

	dest[size - 1] = '\0';

	if (ret < 0 || ret >= size)
	{
		return -1;
	}

	return ret;
}

/*
==================
CharMethods::FindChar
==================
*/
int CharMethods::FindChar(const char* str, const char c, int start, int end)
{
	int i;
	
	if (end == -1)
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		end = (int)strlen(str) - 1;
		// RB end
	}

	for (i = start; i <= end; i++)
	{
		if (str[i] == c)
		{
			return i;
		}
	}

	return -1;
}

/*
==================
CharMethods::FindText
==================
*/
int CharMethods::FindText(const char* str, const char* text, bool casesensitive, int start, int end)
{
	int l, i, j;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	if (end == -1)
	{
		end = (int)strlen(str);
	}

	l = end - (int)strlen(text);
	// RB end
	
	for (i = start; i <= l; i++)
	{
		if (casesensitive)
		{
			for (j = 0; text[j]; j++)
			{
				if (str[i + j] != text[j])
				{
					break;
				}
			}
		}
		else
		{
			for (j = 0; text[j]; j++)
			{
				if (::toupper(str[i + j]) != ::toupper(text[j]))
				{
					break;
				}
			}
		}
		if (!text[j])
		{
			return i;
		}
	}

	return -1;
}

/*
==================
CharMethods::Append

Never goes past bounds or leaves without a terminating 0
==================
*/
void CharMethods::Append(char* dest, int size, const char* src)
{
	int		l1;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l1 = (int)strlen(dest);
	// RB end
	if (l1 >= size)
	{
		// libBud::common->Error( "String::Append: already overflowed" );
		return;
	}
	Copynz(dest + l1, src, size - l1);
}

/*
==================
CharMethods::Copynz
==================
*/
void CharMethods::Copynz(char* dest, const char* src, int destsize)
{
	if (!src)
	{
		// libBud::common->Warning( "String::Copynz: NULL src" );
		return;
	}
	if (destsize < 1)
	{
		// libBud::common->Warning( "String::Copynz: destsize < 1" );
		return;
	}
	
	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = 0;
}

/*
==================
CharMethods::StripMediaName

Makes the string lower case, replaces backslashes with forward slashes, and removes extension
==================
*/
void CharMethods::StripMediaName(const char* name, String& mediaName)
{
	char c;
	
	mediaName.Empty();
	
	for (c = *name; c; c = *(++name))
	{
		// truncate at an extension
		if (c == '.')
		{
			break;
		}

		// convert backslashes to forward slashes
		if (c == '\\')
		{
			mediaName.Append('/');
		}

		else
		{
			mediaName.Append(ToLower(c));
		}
	}
}

bool CharMethods::CharIsPrintable(int c)
{
	// test for regular ascii and western European high-ascii chars
	return (c >= 0x20 && c <= 0x7E ) || ( c >= 0xA1 && c <= 0xFF);
}

bool CharMethods::CharIsLower(int c)
{
	// test for regular ascii and western European high-ascii chars
	return ( c >= 'a' && c <= 'z' ) || ( c >= 0xE0 && c <= 0xFF );
}

bool CharMethods::CharIsUpper(int c)
{
	// test for regular ascii and western European high-ascii chars
	return ( c <= 'Z' && c >= 'A' ) || ( c >= 0xC0 && c <= 0xDF );
}

bool CharMethods::CharIsAlpha(int c)
{
	// test for regular ascii and western European high-ascii chars
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
			 ( c >= 0xC0 && c <= 0xFF ) );
}

bool CharMethods::CharIsNumeric(int c)
{
	return ( c <= '9' && c >= '0' );
}

bool CharMethods::CharIsNewLine(char c)
{
	return ( c == '\n' || c == '\r' || c == '\v' );
}

bool CharMethods::CharIsTab(char c)
{
	return ( c == '\t' );
}

/*
==================
CharMethods::IsNumeric

Checks a string to see if it contains only numerical values
==================
*/
bool CharMethods::IsNumeric(const char* s)
{
	int		i;
	bool	dot;
	
	if (*s == '-')
	{
		s++;
	}
	
	dot = false;

	for (i = 0; s[i]; i++)
	{
		if (!isdigit( (const unsigned char)s[i]))
		{
			if ((s[i] == '.') && !dot)
			{
				dot = true;
				continue;
			}

			return false;
		}
	}
	
	return true;
}

/*
==================
CharMethods::IsColor
==================
*/
bool CharMethods::IsColor(const char* s)
{
	return (s[0] == C_COLOR_ESCAPE && s[1] != '\0' && s[1] != ' ');	
}

/*
==================
CharMethods::HasLower

Checks if a string has any lowercase chars
==================
*/
bool CharMethods::HasLower(const char* s)
{
	if (!s)
	{
		return false;
	}
	
	while (*s)
	{
		if (CharIsLower(*s))
		{
			return true;
		}

		s++;
	}
	
	return false;
}

/*
==================
CharMethods::HasUpper

Checks if a string has any uppercase chars
==================
*/
bool CharMethods::HasUpper(const char* s)
{
	if (!s)
	{
		return false;
	}
	
	while (*s)
	{
		if (CharIsUpper(*s))
		{
			return true;
		}
		s++;
	}
	
	return false;	
}

/*
==================
CharMethods::Filter

Returns true if the string conforms the given filter.
Several metacharacter may be used in the filter.

*          match any string of zero or more characters
?          match any single character
[abc...]   match any of the enclosed characters; a hyphen can
           be used to specify a range (e.g. a-z, A-Z, 0-9)
==================
*/
bool CharMethods::Filter(const char* filter, const char* name, bool casesensitive)
{
	String buf;
	int i, found, index;
	
	while (*filter)
	{
		if (*filter == '*')
		{
			filter++;
			buf.Empty();

			for (i = 0; *filter; i++)
			{
				if (*filter == '*' || *filter == '?' || (*filter == '[' && *(filter + 1) != '['))
				{
					break;
				}

				buf += *filter;

				if (*filter == '[')
				{
					filter++;
				}
				filter++;
			}
			if (buf.Length())
			{
				index = String(name).Find(buf.c_str(), casesensitive);
				if( index == -1 )
				{
					return false;
				}
				name += index + strlen(buf);
			}
		}
		else if (*filter == '?')
		{
			filter++;
			name++;
		}
		else if (*filter == '[')
		{
			if (*(filter + 1) == '[')
			{
				if( *name != '[' )
				{
					return false;
				}
				filter += 2;
				name++;
			}

			else
			{
				filter++;
				found = false;
				while (*filter && !found)
				{
					if (*filter == ']' && *(filter + 1) != ']')
					{
						break;
					}
					if (*(filter + 1) == '-' && *(filter + 2) && (*(filter + 2) != ']' || *(filter + 3) == ']'))
					{
						if (casesensitive)
						{
							if (*name >= *filter && *name <= *(filter + 2))
							{
								found = true;
							}
						}
						else
						{
							if (::toupper(*name) >= ::toupper(*filter) && ::toupper(*name) <= ::toupper(*(filter + 2)))
							{
								found = true;
							}
						}
						filter += 3;
					}
					else
					{
						if(casesensitive)
						{
							if (*filter == *name)
							{
								found = true;
							}
						}

						else
						{
							if (::toupper(*filter) == ::toupper(*name))
							{
								found = true;
							}
						}
						filter++;
					}
				}
				if( !found )
				{
					return false;
				}
				while (*filter)
				{
					if (*filter == ']' && *(filter + 1) != ']')
					{
						break;
					}
					filter++;
				}
				filter++;
				name++;
			}
		}

		else
		{
			if (casesensitive)
			{
				if (*filter != *name)
				{
					return false;
				}
			}

			else
			{
				if (::toupper(*filter) != ::toupper(*name))
				{
					return false;
				}
			}
			filter++;
			name++;
		}
	}

	return true;
}

/*
==================
CharMethods::CheckExtension
==================
*/
bool CharMethods::CheckExtension(const char* name, const char* ext)
{
	const char* s1 = name + Length(name) - 1;
	const char* s2 = ext + Length(ext) - 1;
	int c1, c2, d;
	
	do
	{
		c1 = *s1--;
		c2 = *s2--;
		
		d = c1 - c2;

		while (d)
		{
			if (c1 <= 'Z' && c1 >= 'A')
			{
				d += ('a' - 'A');

				if (!d)
				{
					break;
				}
			}

			if (c2 <= 'Z' && c2 >= 'A')
			{
				d -= ('a' - 'A');

				if (!d)
				{
					break;
				}

			}
			return false;
		}
	}
	while (s1 > name && s2 > ext);
	
	return (s1 >= name);
}

/*
==================
CharMethods::ToLower
==================
*/
char* CharMethods::ToLower(char* data)
{
	for (int i = 0; data[i]; i++)
	{
		if (CharIsUpper(data[i]))
		{
			data[i] += ( 'a' - 'A' );
		}
	}

	return data;
}

char CharMethods::ToLower(char c)
{
	if (c <= 'Z' && c >= 'A')
	{
		return (c + ('a' - 'A'));
	}

	return c;
}

/*
==================
CharMethods::ToUpper
==================
*/
char* CharMethods::ToUpper(char* data)
{
	for (int i = 0; data[i]; i++)
	{
		if (CharIsLower(data[i]))
		{
			data[i] -= ('a' - 'A');
		}
	}

	return data;
}

char CharMethods::ToUpper(char c)
{
	if (c >= 'a' && c <= 'z')
	{
		return (c - ('a' - 'A'));
	}

	return c;
}

/*
==================
CharMethods::RemoveColors
==================
*/
char* CharMethods::RemoveColors(char* string)
{
	char* d;
	char* s;
	int c;
	
	s = string;
	d = string;

	while ((c = *s) != 0)
	{
		if (IsColor(s))
		{
			s++;
		}

		else
		{
			*d++ = c;
		}

		s++;
	}

	*d = '\0';
	
	return string;
}

/*
==================
CharMethods::FloatArrayToString
==================
*/
const char* CharMethods::FloatArrayToString(const float* array, const int length, const int precision)
{
	static int index = 0;
	static char str[4][16384];	// in case called by nested functions
	int i, n;
	char format[16], *s;
	
	// use an array of string so that multiple calls won't collide
	s = str[index];
	index = (index + 1) & 3;
	
	snPrintf(format, sizeof(format), "%%.%df", precision);
	n = snPrintf(s, sizeof(str[0]), format, array[0]);
	if( precision > 0 )
	{
		while (n > 0 && s[n - 1] == '0') s[--n] = '\0';
		while (n > 0 && s[n - 1] == '.') s[--n] = '\0';
	}
	snPrintf(format, sizeof(format), " %%.%df", precision);
	for (i = 1; i < length; i++)
	{
		n += snPrintf(s + n, sizeof(str[0]) - n, format, array[i]);
		if (precision > 0)
		{
			while (n > 0 && s[n - 1] == '0') s[--n] = '\0';
			while (n > 0 && s[n - 1] == '.') s[--n] = '\0';
		}
	}
	return s;
}

/*
==================
CharMethods::CStyleQuote
==================
*/
const char* CharMethods::CStyleQuote(const char* str)
{
	static int index = 0;
	static char buffers[4][16384];	// in case called by nested functions
	unsigned int i;
	char* buf;
	
	buf = buffers[index];
	index = (index + 1) & 3;
	
	buf[0] = '\"';
	for (i = 1; i < sizeof(buffers[0]) - 2; i++)
	{
		int c = *str++;
		switch (c)
		{
			case '\0':
				buf[i++] = '\"';
				buf[i] = '\0';
				return buf;
			case '\\':
				buf[i++] = '\\';
				buf[i] = '\\';
				break;
			case '\n':
				buf[i++] = '\\';
				buf[i] = 'n';
				break;
			case '\r':
				buf[i++] = '\\';
				buf[i] = 'r';
				break;
			case '\t':
				buf[i++] = '\\';
				buf[i] = 't';
				break;
			case '\v':
				buf[i++] = '\\';
				buf[i] = 'v';
				break;
			case '\b':
				buf[i++] = '\\';
				buf[i] = 'b';
				break;
			case '\f':
				buf[i++] = '\\';
				buf[i] = 'f';
				break;
			case '\a':
				buf[i++] = '\\';
				buf[i] = 'a';
				break;
			case '\'':
				buf[i++] = '\\';
				buf[i] = '\'';
				break;
			case '\"':
				buf[i++] = '\\';
				buf[i] = '\"';
				break;
			case '\?':
				buf[i++] = '\\';
				buf[i] = '\?';
				break;
			default:
				buf[i] = c;
				break;
		}
	}
	buf[i++] = '\"';
	buf[i] = '\0';
	return buf;
}

/*
==================
CharMethods::CStyleUnQuote
==================
*/
const char* CharMethods::CStyleUnQuote(const char* str)
{
	if (str[0] != '\"')
	{
		return str;
	}
	
	static int index = 0;
	static char buffers[4][16384];	// in case called by nested functions
	unsigned int i;
	char* buf;
	
	buf = buffers[index];
	index = (index + 1) & 3;
	
	str++;
	for (i = 0; i < sizeof(buffers[0]) - 1; i++)
	{
		int c = *str++;
		if (c == '\0')
		{
			break;
		}
		else if (c == '\\')
		{
			c = *str++;
			switch( c )
			{
				case '\\':
					buf[i] = '\\';
					break;
				case 'n':
					buf[i] = '\n';
					break;
				case 'r':
					buf[i] = '\r';
					break;
				case 't':
					buf[i] = '\t';
					break;
				case 'v':
					buf[i] = '\v';
					break;
				case 'b':
					buf[i] = '\b';
					break;
				case 'f':
					buf[i] = '\f';
					break;
				case 'a':
					buf[i] = '\a';
					break;
				case '\'':
					buf[i] = '\'';
					break;
				case '\"':
					buf[i] = '\"';
					break;
				case '\?':
					buf[i] = '\?';
					break;
			}
		}
		else
		{
			buf[i] = c;
		}
	}
	assert(buf[i - 1] == '\"');

	buf[i - 1] = '\0';
	return buf;
}