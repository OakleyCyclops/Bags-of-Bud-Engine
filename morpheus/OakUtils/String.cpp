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
	
	l = this->Length;
	
	StringMethods::EnsureAlloced(this, l + 1);

	strcpy(Cstring, text.Cstring);
	this->Length = l;
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
	
	if (end > text.Length)
	{
		end = text.Length;
	}

	if (start > text.Length)
	{
		start = text.Length;
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
	
	StringMethods::EnsureAlloced(this, l + 1);
	
	for (i = 0; i < l; i++)
	{
		Cstring[i] = text[start + i];
	}
	
	Cstring[l] = '\0';
	Length = l;
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
		StringMethods::EnsureAlloced(this, l + 1);

		strcpy(Cstring, text);
		Length = l;
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
	
	StringMethods::EnsureAlloced(this, l + 1);
	
	for (i = 0; i < l; i++)
	{
		Cstring[i] = text[start + i];
	}
	
	Cstring[l] = '\0';
	Length = l;
}

/*
==================
String::String
==================
*/
String::String(const bool b)
{
	Construct();
	StringMethods::EnsureAlloced(this, 2);

	Cstring[0] = b ? '1' : '0';
	Cstring[1] = '\0';
	Length = 1;
}

/*
==================
String::String
==================
*/
String::String(const char c)
{
	Construct();
	StringMethods::EnsureAlloced(this, 2);

	Cstring[0] = c;
	Cstring[1] = '\0';
	Length = 1;
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
	StringMethods::EnsureAlloced(this, l + 1);

	strcpy(Cstring, text);
	Length = l;
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
	StringMethods::EnsureAlloced(this, l + 1);

	strcpy(Cstring, text);
	Length = l;
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
	
	l = CharMethods::snPrintf( text, sizeof( text ), "%f", f );
	while(l > 0 && text[l - 1] == '0') 
	{
		text[--l] = '\0';
	}

	while(l > 0 && text[l - 1] == '.')
	{
		text[--l] = '\0';
	}
	StringMethods::EnsureAlloced(this, l + 1);
	strcpy(Cstring, text);
	Length = l;
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
	return Cstring;
}

/*
==================
String::operator
==================
*/
String::operator const char* ()
{
	return Cstring;
}

char String::operator[](int index) const
{
	assert((index >= 0) && (index <= Length));
	return Cstring[index];
}

char& String::operator[](int index)
{
	assert((index >= 0) && (index <= Length));
	return Cstring[index];
}

void String::operator=(const String& text)
{
	int l;
	

	l = this->Length;
	StringMethods::EnsureAlloced(this, l + 1, false);

	memcpy(Cstring, text.Cstring, l);

	Cstring[l] = '\0';
	Length = l;
}

String operator+(const String& a, const String& b)
{
	String result(a);

	StringMethods::Append(&result, b);
	return result;
}

String operator+(const String& a, const char* b)
{
	String result(a);

	StringMethods::Append(&result, b);
	return result;
}

String operator+(const char* a, const String& b)
{
	String result(a);

	StringMethods::Append(&result, b);
	return result;
}

String operator+(const String& a, const bool b)
{
	String result(a);

	StringMethods::Append(&result, b ? "true" : "false");
	return result;
}

String operator+(const String& a, const char b)
{
	String result(a);

	StringMethods::Append(&result, b);
	return result;
}

String operator+(const String& a, const float b)
{
	char	text[64];
	String	result(a);
	
	sprintf(text, "%f", b);
	StringMethods::Append(&result, text);
	
	return result;
}

String operator+(const String& a, const int b)
{
	char	text[64];
	String	result(a);
	
	sprintf(text, "%d", b);
	StringMethods::Append(&result, text);
	
	return result;
}

String operator+(const String& a, const unsigned b)
{
	char	text[64];
	String	result(a);
	
	sprintf(text, "%u", b);
	StringMethods::Append(&result, text);
	
	return result;
}

String& String::operator+=(const float a)
{
	char text[64];
	
	sprintf(text, "%f", a);
	StringMethods::Append(this, text);
	
	return *this;
}

String& String::operator+=(const int a)
{
	char text[64];
	
	sprintf(text, "%d", a);
	StringMethods::Append(this, text);
	
	return *this;
}

String& String::operator+=(const unsigned a)
{
	char text[64];
	
	sprintf(text, "%u", a);
	StringMethods::Append(this, text);
	
	return *this;
}

String& String::operator+=(const String& a)
{
	StringMethods::Append(this, a);
	return *this;
}

String& String::operator+=(const char* a)
{
	StringMethods::Append(this, a);
	return *this;
}

String& String::operator+=(const char a)
{
	StringMethods::Append(this, a);
	return *this;
}

String& String::operator+=(const bool a)
{
	StringMethods::Append(this, a ? "true" : "false");
	return *this;
}

bool operator==(const String& a, const String& b)
{
	return (!CharMethods::Cmp(a.Cstring, b.Cstring));
}

bool operator==(const String& a, const char* b)
{
	assert(b);
	return (!CharMethods::Cmp(a.Cstring, b));
}

bool operator==(const char* a, const String& b)
{
	assert(a);
	return (!CharMethods::Cmp(a, b.Cstring));
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
	StringMethods::SetStatic(this, false);
	StringMethods::SetAlloced(this, STR_ALLOC_BASE);
	Cstring = BaseBuffer;
	Length = 0;
	Cstring[0] = '\0';
}

/*
============
String::FreeData
============
*/
void String::FreeData()
{
	if (StringMethods::IsStatic(this))
	{
		return;
	}
	
	if (Cstring && Cstring != BaseBuffer)
	{
		delete[] Cstring;
		Cstring = BaseBuffer;
	}
}

/*
============
StringMethods::IsStatic
============
*/
bool StringMethods::IsStatic(String* str)
{
	return (str->AllocedAndFlag & str->STATIC_MASK) != 0;
}

/*
============
StringMethods::SetStatic
============
*/
void StringMethods::SetStatic(String* str, const bool isStatic)
{
	str->AllocedAndFlag = (str->AllocedAndFlag & str->ALLOCED_MASK) | (isStatic << str->STATIC_BIT);
}

/*
============
StringMethods::GetAlloced
============
*/
int StringMethods::GetAlloced(String* str)
{
	return str->AllocedAndFlag & str->ALLOCED_MASK;
}

/*
============
StringMethods::SetAlloced
============
*/
void StringMethods::SetAlloced(String* str, const int a)
{
	str->AllocedAndFlag = (str->AllocedAndFlag & str->STATIC_MASK) | (a & str->ALLOCED_MASK);
}
/*
============
StringMethods::Allocated
============
*/
int StringMethods::Allocated(String* str)
{
	if (str->Cstring != str->BaseBuffer)
	{
		return StringMethods::GetAlloced(str);
	}
	else
	{
		return 0;
	}
}

/*
============
StringMethods::EnsureAlloced
============
*/
void StringMethods::EnsureAlloced(String* str, int amount, bool keepold)
{
	// static string's can't reallocate
	if (IsStatic(str))
	{
		return;
	}
	if (amount > GetAlloced(str))
	{
		ReAllocate(str, amount, keepold);
	}
}

/*
============
StringMethods::ReAllocate
============
*/
void StringMethods::ReAllocate(String* str, int amount, bool keepold)
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

	SetAlloced(str, newsize);
	
	newbuffer = new char[GetAlloced(str)];

	if (keepold && str->Cstring)
	{
		str->Cstring[str->Length] = '\0';
		strcpy(newbuffer, str->Cstring);
	}
	
	if (str->Cstring && str->Cstring != str->BaseBuffer)
	{
		delete[] str->Cstring;
	}
	
	str->Cstring = newbuffer;
}

/*
============
StringMethods::Size
============
*/
size_t StringMethods::Size(String* str)
{
	return sizeof(str) + Allocated(str);
}

/*
============
StringMethods::LengthWithoutColors
============
*/
int StringMethods::LengthWithoutColors(String* str)
{
	return CharMethods::LengthWithoutColors(str->Cstring);
}

/*
============
StringMethods::Empty
============
*/
void StringMethods::Empty(String* str)
{
	EnsureAlloced(str, 1);
	str->Cstring[0] = '\0';
	str->Length = 0;
}

/*
============
StringMethods::Clear
============
*/
void StringMethods::Clear(String* str)
{
	if (StringMethods::IsStatic(str))
	{
		str->Length = 0;
		str->Cstring[0] = '\0';
		return;
	}

	str->FreeData();
	str->Construct();
}

/*
============
StringMethods::CapLength
============
*/
void StringMethods::CapLength(String* str, int newLength)
{
	if (str->Length <= newLength)
	{
		return;
	}

	str->Cstring[newLength] = 0;
	str->Length = newLength;
}

void StringMethods::Fill(String* str, const char ch, int newLength)
{
	StringMethods::EnsureAlloced(str, newLength + 1);
	str->Length = newLength;

	memset(str->Cstring, ch, str->Length);
	str->Cstring[str->Length] = 0;
}

/*
============
StringMethods::Append
============
*/
void StringMethods::Append(String* str, const char a)
{
	StringMethods::EnsureAlloced(str, str->Length + 2);

	str->Cstring[str->Length] = a;
	str->Length++;
	str->Cstring[str->Length] = '\0';
}

/*
============
StringMethods::BackSlashesToForwardSlashes
============
*/
String& StringMethods::BackSlashesToForwardSlashes(String& str) 
{
	int i;

	for (i = 0; i < str.Length; i++) 
	{
		if (str.Cstring[i] == '\\' ) 
		{
			str.Cstring[i] = '/';
		}
	}

	return str;
}

/*
============
StringMethods::ForwardSlashesToBackSlashes
============
*/
String& StringMethods::ForwardSlashesToBackSlashes(String& str) 
{
	int i;

	for (i = 0; i < str.Length; i++) 
	{
		if (str.Cstring[i] == '/') 
		{
			str.Cstring[i] = '\\';
		}
	}

	return str;
}


/*
============
StringMethods::Append
============
*/
void StringMethods::Append(String* str, const String& text)
{
	int newLength;
	int i;
	
	newLength = str->Length + text.Length;
	StringMethods::EnsureAlloced(str, newLength + 1);

	for (i = 0; i < text.Length; i++)
	{
		str->Cstring[str->Length + i] = text[i];
	}

	str->Length = newLength;
	str->Cstring[str->Length] = '\0';
}

/*
============
StringMethods::Append
============
*/
void StringMethods::Append(String* str, const char* text)
{
	int newLength;
	int i;
	
	if (text)
	{
		// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
		newLength = str->Length + (int)strlen(text);
		// RB end
		EnsureAlloced(str, newLength + 1);

		for (i = 0; text[i]; i++)
		{
			str->Cstring[str->Length + i] = text[i];
		}

		str->Length = newLength;
		str->Cstring[str->Length] = '\0';
	}
}

/*
============
StringMethods::Append
============
*/
void StringMethods::Append(String* str, const char* text, int l)
{
	int newLength;
	int i;
	
	if (text && l)
	{
		newLength = str->Length + l;
		EnsureAlloced(str, newLength + 1);

		for (i = 0; text[i] && i < l; i++)
		{
			str->Cstring[str->Length + i] = text[i];
		}

		str->Length = newLength;
		str->Cstring[str->Length] = '\0';
	}
}

/*
============
StringMethods::Insert
============
*/
void StringMethods::Insert(String* str, const char a, int index)
{
	int i, l;
	
	if (index < 0)
	{
		index = 0;
	}
	else if (index > str->Length)
	{
		index = str->Length;
	}
	
	l = 1;

	EnsureAlloced(str, str->Length + l + 1);

	for (i = str->Length; i >= index; i--)
	{
		str->Cstring[i + l] = str->Cstring[i];
	}

	str->Cstring[index] = a;
	str->Length++;
}

/*
============
StringMethods::Insert
============
*/
void StringMethods::Insert(String* str, const char* text, int index)
{
	int i, l;
	
	if (index < 0)
	{
		index = 0;
	}

	else if (index > str->Length)
	{
		index = str->Length;
	}
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(text);
	// RB end

	EnsureAlloced(str, str->Length + l + 1);

	for (i = str->Length; i >= index; i--)
	{
		str->Cstring[i + l] = str->Cstring[i];
	}

	for (i = 0; i < l; i++)
	{
		str->Cstring[index + i] = text[i];
	}

	str->Length += l;
}

/*
============
StringMethods::ToLower
============
*/
void StringMethods::ToLower(String* str)
{
	for (int i = 0; str->Cstring[i]; i++)
	{
		if (CharMethods::CharIsUpper(str->Cstring[i]))
		{
			str->Cstring[i] += ( 'a' - 'A' );
		}
	}
}

/*
============
StringMethods::ToUpper
============
*/
void StringMethods::ToUpper(String* str)
{
	for (int i = 0; str->Cstring[i]; i++)
	{
		if (CharMethods::CharIsLower(str->Cstring[i]))
		{
			str->Cstring[i] -= ( 'a' - 'A' );
		}
	}
}

/*
============
StringMethods::IsEmpty
============
*/
bool StringMethods::IsEmpty(String* str)
{
	return (CharMethods::Cmp(str->Cstring, "") == 0);
}

/*
============
StringMethods::IsNumeric
============
*/
bool StringMethods::IsNumeric(String* str)
{
	return CharMethods::IsNumeric(str->Cstring);
}

/*
============
StringMethods::IsColor
============
*/
bool StringMethods::IsColor(String* str)
{
	return CharMethods::IsColor(str->Cstring);
}

/*
============
StringMethods::HasLower
============
*/
bool StringMethods::HasLower(String* str)
{
	return CharMethods::HasLower(str->Cstring);
}

/*
============
StringMethods::HasUpper
============
*/
bool StringMethods::HasUpper(String* str)
{
	return CharMethods::HasUpper(str->Cstring);
}

/*
============
StringMethods::Cmp
============
*/
int StringMethods::Cmp(String* str, const char* text)
{
	assert(text);

	return CharMethods::Cmp(str->Cstring, text);
}

/*
============
StringMethods::Cmpn
============
*/
int StringMethods::Cmpn(String* str, const char* text, int n)
{
	assert(text);

	return CharMethods::Cmpn(str->Cstring, text, n);
}

/*
============
StringMethods::CmpPrefix
============
*/
int StringMethods::CmpPrefix(String* str, const char* text)
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods::Cmpn(str->Cstring, text, (int)strlen(text));
	// RB end
}

/*
============
StringMethods::Icmp
============
*/	
int StringMethods::Icmp(String* str, const char* text)
{
	assert(text);

	return CharMethods::Icmp(str->Cstring, text);
}

/*
============
StringMethods::Icmpn
============
*/
int StringMethods::Icmpn(String* str, const char* text, int n)
{
	assert(text);

	return CharMethods::Icmpn(str->Cstring, text, n);
}

/*
============
StringMethods::IcmpPrefix
============
*/
int StringMethods::IcmpPrefix(String* str, const char* text)
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods::Icmpn(str->Cstring, text, (int)strlen(text));
	// RB end
}

/*
============
StringMethods::IcmpPath
============
*/
int StringMethods::IcmpPath(String* str, const char* text)
{
	assert(text);

	return CharMethods::IcmpPath(str->Cstring, text);
}

/*
============
StringMethods::IcmpnPath
============
*/
int StringMethods::IcmpnPath(String* str, const char* text, int n)
{
	assert(text);

	return CharMethods::IcmpnPath(str->Cstring, text, n);
}

/*
============
StringMethods::IcmpPrefixPath
============
*/
int StringMethods::IcmpPrefixPath(String* str, const char* text)
{
	assert(text);

	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	return CharMethods::IcmpnPath(str->Cstring, text, (int)strlen(text));
	// RB end
}

/*
============
StringMethods::IcmpNoColor
============
*/	
int StringMethods::IcmpNoColor(String* str, const char* text)
{
	assert(text);

	return CharMethods::IcmpNoColor(str->Cstring, text);
}

/*
============
StringMethods::UTF8Length
============
*/
int StringMethods::UTF8Length(String* str)
{
	return UTF8Length(str, (byte*)str->Cstring);
}

/*
============
StringMethods::UTF8Length
============
*/
int StringMethods::UTF8Length(String* str, const byte* s)
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
StringMethods::Find
============
*/
int StringMethods::Find(String* str, const char c, int start, int end)
{
	if (end == -1)
	{
		end = str->Length;
	}

	return CharMethods::FindChar(str->Cstring, c, start, end);
}

int StringMethods::Find(String* str, const char* text, bool casesensitive, int start, int end)
{
	if (end == -1)
	{
		end = str->Length;
	}

	return CharMethods::FindText(str->Cstring, text, casesensitive, start, end);
}

/*
============
StringMethods::UTF8Char
============
*/
uint32 StringMethods::UTF8Char(String* str, int& idx)
{
	return UTF8Char(str, (byte*)str->Cstring, idx);
}

/*
============
StringMethods::UTF8Char
============
*/
uint32 StringMethods::UTF8Char(String* str, const char* s, int& idx)
{
	return UTF8Char(str, (byte*)s, idx);
}

/*
============
StringMethods::UTF8Char
============
*/
uint32 StringMethods::UTF8Char(String* str, const byte* s, int& idx)
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
StringMethods::AppendUTF8Char
============
*/
void StringMethods::AppendUTF8Char(String* str, uint32 c)
{
	if (c < 0x80)
	{
		StringMethods::Append(str, (char)c);
	}
	else if (c < 0x800)      // 11 bits
	{
		StringMethods::Append(str, (char)(0xC0 | (c >> 6)));
		StringMethods::Append(str, (char)(0x80 | (c & 0x3F)));
	}
	else if (c < 0x10000)      // 16 bits
	{
		StringMethods::Append(str, (char)(0xE0 | (c >> 12)));
		StringMethods::Append(str, (char)(0x80 | ((c >> 6) & 0x3F)));
		StringMethods::Append(str, (char)(0x80 | (c & 0x3F)));
	}
	else if (c < 0x200000)  	// 21 bits
	{
		StringMethods::Append(str, (char)(0xF0 | (c >> 18)));
		StringMethods::Append(str, (char)(0x80 | (( c >> 12) & 0x3F)));
		StringMethods::Append(str, (char)(0x80 | (( c >> 6) & 0x3F)));
		StringMethods::Append(str, (char)(0x80 | (c & 0x3F)));
	}
	else
	{
		// UTF-8 can encode up to 6 bytes. Why don't we support that?
		// This is an invalid Unicode character
		StringMethods::Append(str, '?');
	}
}

/*
============
StringMethods::ConvertToUTF8
============
*/
void StringMethods::ConvertToUTF8(String* str)
{
	String temp(str);

	StringMethods::Clear(str);

	for(int index = 0; index < temp.Length; ++index)
	{
		AppendUTF8Char(str, temp[index]);
	}
}

/*
============
StringMethods::IsValidUTF8
============
*/
bool StringMethods::IsValidUTF8(String* str, const uint8* s, const int maxLen, utf8Encoding_t& encoding)
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
StringMethods::IsValidUTF8
============
*/
bool StringMethods::IsValidUTF8(String* str, const char* s, const int maxLen, utf8Encoding_t& encoding)
{
	return IsValidUTF8(str, (const uint8*)s, maxLen, encoding);
}

/*
============
StringMethods::RemoveColors
============
*/
String& StringMethods::RemoveColors(String* str)
{
	CharMethods::RemoveColors(str->Cstring);
	str->Length = CharMethods::Length(str->Cstring);
	return *str;
}

/*
============
StringMethods::StripTrailingOnce
============
*/
bool StringMethods::StripTrailingOnce(String* str, const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if ((l > 0) && (str->Length >= l) && !CharMethods::Cmpn(string, str->Cstring + str->Length - l, l))
	{
		str->Length -= l;
		str->Cstring[str->Length] = '\0';
		return true;
	}

	return false;
}

/*
============
StringMethods::StripLeadingOnce
============
*/
bool StringMethods::StripLeadingOnce(String* str, const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if ((l > 0) && !Cmpn(str, string, l))
	{
		memmove(str->Cstring, str->Cstring + l, str->Length - l + 1);

		str->Length -= l;
		return true;
	}

	return false;
}

/*
============
StringMethods::StripLeading
============
*/
void StringMethods::StripLeading(String* str, const char c)
{
	while (str->Cstring[0] == c)
	{
		memmove(&str->Cstring[0], &str->Cstring[1], str->Length);
		str->Length--;
	}
}

/*
============
StringMethods::StripLeading
============
*/
void StringMethods::StripLeading(String* str, const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end
	if (l > 0)
	{
		while (!Cmpn(str, string, l))
		{
			memmove(str->Cstring, str->Cstring + l, str->Length - l + 1);
			str->Length -= l;
		}
	}
}

/*
============
StringMethods::StripTrailing
============
*/
void StringMethods::StripTrailing(String* str, const char c)
{
	int i;
	
	for (i = str->Length; i > 0 && str->Cstring[i - 1] == c; i--)
	{
		str->Cstring[i - 1] = '\0';
		str->Length--;
	}
}

/*
============
StringMethods::StripLeading
============
*/
void StringMethods::StripTrailing(String* str, const char* string)
{
	int l;
	
	// RB: 64 bit fixes,  conversion from 'size_t' to 'int', possible loss of data
	l = (int)strlen(string);
	// RB end

	if (l > 0)
	{
		while ((str->Length >= l) && !CharMethods::Cmpn(string, str->Cstring + str->Length - l, l))
		{
			str->Length -= l;
			str->Cstring[str->Length] = '\0';
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
	}

	if (!text[j])
	{
		return i;
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
		return;
	}
	if (destsize < 1)
	{
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
	
	StringMethods::Empty(&mediaName);
	
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
			StringMethods::Append(&mediaName, '/');
		}

		else
		{
			StringMethods::Append(&mediaName, ToLower(c));
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
			StringMethods::Empty(&buf);

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
			if (buf.Length)
			{	
				String Name(name);

				index = StringMethods::Find(&Name, buf.Cstring, casesensitive);
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

char* CharMethods::FloatToString(float f, char* s, unsigned int afterPoint, unsigned int base)
{
	int intPart = (int)f;
	float floatPart = f - (float)intPart;

	char* integer = IntToString(intPart, integer, base);
	char* number = (char*)malloc(1);

	if (afterPoint != 0)
	{
		number[sizeof(integer)] = '.';

		floatPart = floatPart * pow(10, afterPoint);

		IntToString((int)floatPart, number, 10);
	}

	s = number;

	return s;

}

char* CharMethods::DoubleToString(double d, char* s, unsigned int afterPoint, unsigned int base)
{

}

char* CharMethods::LongToString(long l, char* s, unsigned int base)
{
	const char* table = "0123456789abcdef";	

	unsigned int length = 0;
	unsigned int a = 0;

	char prefix[3];
	char minus[2];

	char* number = (char*)malloc(0);

	bool negative = false;

	// If zero, just return null
	if (base == 0 || base > 16)
	{
		return nullptr;
	}

	if (l < 0 && base == 10)
	{
		l = -l;
		negative = true;
	}

	switch (base)
	{	
		// If binary, take our char and add the "0b" prefix
		case 2:
			{
				prefix[0] = '0';
				prefix[1] = 'b';
				prefix[2] = '\0';	
			}

		// If octal, take our char and add the "0c" prefix
		// I'm not fucking using "0" as the prefix fuck you
		case 8:
			{
				prefix[0] = '0';
				prefix[1] = 'c';
				prefix[2] = '\0';
			}

		// If hexadecminal, take our char and add the "0x" prefix
		case 16:
			{
				prefix[0] = '0';
				prefix[1] = 'x';
				prefix[2] = '\0';
			}
	}
	
	while (l != 0)
	{	
		number[length] = table[l % base];

		l /= base;
		length++;

		number = (char*)realloc(number, length);
	}
	
    for (a = 0; a < length/2; a++)  
    {
		char tmp;

		tmp = number[a];  
		number[a] = number[length - a - 1];  
		number[length - a - 1] = tmp;  
    }

	number = (char*)realloc(number, length + 1);
	length++;
	number[length] = '\0';

	if (base != 10)
	{
		number = (char*)realloc(number, length + 2);
		number = strcat(prefix, number);
	}

	if (negative)
	{
		minus[0] = '-';
		minus[1] = '\0';

		number = (char*)realloc(number, length + 1);
		number = strcat(minus, number);
	}

	s = number;
	return s;     
}

char* CharMethods::IntToString(int i, char* s, unsigned int base)
{
	const char* table = "0123456789abcdef";	

	unsigned int length = 0;
	unsigned int a = 0;

	char prefix[3];
	char minus[2];

	char* number = (char*)malloc(0);

	bool negative = false;

	// If zero, just return null
	if (base == 0 || base > 16)
	{
		return nullptr;
	}

	if (i < 0 && base == 10)
	{
		i = -i;
		negative = true;
	}

	switch (base)
	{	
		// If binary, take our char and add the "0b" prefix
		case 2:
			{
				prefix[0] = '0';
				prefix[1] = 'b';
				prefix[2] = '\0';	
			}

		// If octal, take our char and add the "0c" prefix
		// I'm not fucking using "0" as the prefix fuck you
		case 8:
			{
				prefix[0] = '0';
				prefix[1] = 'c';
				prefix[2] = '\0';
			}

		// If hexadecminal, take our char and add the "0x" prefix
		case 16:
			{
				prefix[0] = '0';
				prefix[1] = 'x';
				prefix[2] = '\0';
			}
	}
	
	while (i != 0)
	{	
		number[length] = table[i % base];

		i /= base;
		length++;

		number = (char*)realloc(number, length);
	}
	
    for (a = 0; a < length/2; a++)  
    {
		char tmp;

		tmp = number[a];  
		number[a] = number[length - a - 1];  
		number[length - a - 1] = tmp;  
    }

	number = (char*)realloc(number, length + 1);
	length++;
	number[length] = '\0';

	if (base != 10)
	{
		number = (char*)realloc(number, length + 2);
		number = strcat(prefix, number);
	}

	if (negative)
	{
		minus[0] = '-';
		minus[1] = '\0';

		number = (char*)realloc(number, length + 1);
		number = strcat(minus, number);
	}

	s = number;
	return s;   
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