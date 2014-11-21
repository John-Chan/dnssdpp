/************************************************************************
//  header only UTF8 string library 
//  --------------------------------------------------------------------
//  
//  taken from http://grigory.info/UTF8Strings.About.html
************************************************************************/

#ifndef UFT8_STRINGS_INLINE_HPP__
#define UFT8_STRINGS_INLINE_HPP__

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <ostream>

#include <string.h>
#include <stdlib.h>
#include <errno.h>


/* stdint.h does not exist on FreeBSD 4.x; its types are defined in sys/types.h instead */
#if defined(__FreeBSD__) && (__FreeBSD__ < 5)
#include <sys/types.h>

/* Likewise, on Sun, standard integer types are in sys/types.h */
#elif defined(__sun__)
#include <sys/types.h>

/* EFI does not have stdint.h, or anything else equivalent */
#elif defined(EFI32) || defined(EFI64) || defined(EFIX64)
#include "Tiano.h"
#if !defined(_STDINT_H_)
typedef UINT8       uint8_t;
typedef INT8        int8_t;
typedef UINT16      uint16_t;
typedef INT16       int16_t;
typedef UINT32      uint32_t;
typedef INT32       int32_t;
typedef UINT64		uint64_t;
typedef INT64		int64_t;
#endif
/* Windows has its own differences */
#elif defined(_WIN32)
#include <windows.h>
#define _UNUSED
#ifndef _MSL_STDINT_H
typedef UINT8       uint8_t;
typedef INT8        int8_t;
typedef UINT16      uint16_t;
typedef INT16       int16_t;
typedef UINT32      uint32_t;
typedef INT32       int32_t;
typedef UINT64		uint64_t;
typedef INT64		int64_t;

#endif

/* All other Posix platforms use stdint.h */
#else
#include <stdint.h>
#endif

namespace UTF8 {

    /**
     *  Exception class. When something bad happens it is thowed by UTF8::String.
     */
    class Exception {
    public:

		enum{
			UnspecifiedException = 1,
			StringToIntConversionError = 2,
			StringToDoubleConversionError = 3,
			FileNotFound = 4,
			StringIsNotACharacter = 5,
		};

        /*static const int UnspecifiedException = 1;
        static const int StringToIntConversionError = 2;
        static const int StringToDoubleConversionError = 3;
        static const int FileNotFound = 4;
        static const int StringIsNotACharacter = 5;*/

        /**
         *  Just a constructor
         */
        Exception(std::string error);

        /// Just a constructor
        Exception(const std::string &error, const int &StatusCode);

        /// Copying constructor
        Exception(const Exception &e);

        /// Returns error string
        std::string GetErrorString() const;

        /// Returns error code
        int GetErrorCode() const;

        /// Assing operator
        Exception & operator =(const Exception &);

    private:
        std::string error;
        int StatusCode;

    };
	
    /**
     * The only string class containing everything to work with UTF8 strings
     */
    class String {
    public:
        static const int SearchDirectionFromLeftToRight = 1;
        static const int SearchDirectionFromRightToLeft = 2;

        /**
         * Search substring in string
         * @param StartPosition Position to start search
         * @param Direction Search forward or backward, uses SearchDirectionFromLeftToRight and SearchDirectionFromRightToLeft
         * @return Returns position of substring if found. Otherwise returns -1
         */
        long Search(const UTF8::String &SubString, unsigned int StartPosition = 0, int Direction = SearchDirectionFromLeftToRight) const;

        /// Simple constructor only initiates buffers
        String();

        /**
         * Create string object from UTF8 char * string
         */
        String(const char *str);

        /**
         * Create string object from UTF-32 string
         */
        String(const uint32_t *);

        /**
         * Create string object from UTF8 std::string
         */
        String(const std::string &);

        /**
         * Copying constructor. Feel free to such things UTF8::String s2=s1;
         */
        String(const String& orig);

        /**
         * Converting from long constructor. Automatically generates string from number.
         */
        String(const long double d,const UTF8::String &ThousandSeparator = "", const UTF8::String &FractionSeparator = ".", const int IntegerPartLength = 0, const int FractionPartLength = 0);

        ~String();

        /**
         * Converts UTF8::String to std::string
         */
        std::string ToString() const;

        /**
         * Reads content from a file and returns as UTF8::String
         */
        static String FromFile(const UTF8::String &Path);

        /**
         * Converts UTF8::String to long
         */
        int64_t ToLong() const;

        /**
         * Converts UTF8::String to double
         */
        double ToDouble() const;

        /**
         * Converts UTF8::String to const char *
         */
        const char * ToConstCharPtr() const;

        /**
         * Separates string using given separator and returns vector
         */
        std::vector <String> Explode(const String &Separator) const;

        /**
         * Creating String from array of String adding separator between them.
         */
        static String Implode(const std::vector <String> &Strings, const String &Separator);

        /**
         * Sum operator. Provides String1+String2 exression.
         */
        String operator+(const String &) const;

        /**
         * Sum operator. Provides String1+"Str" exression.
         */
        String operator+(const char *) const;

        /**
         * Unary sum operator. Provides String1+=String2 expression.
         */
        String & operator+=(const String &);

        /** 
         * Assign operator. Provides String1=String2 expression.
         */
        String & operator=(const String &);

        /**
         * Assign operator. Provides String1="New value" expression.
         */
        String & operator=(const char *);

        /**
         * Assign operator. Provides String1=(uint32_t*) UTF32_StringPointer expression.
         * Automatically converts UNICODE to UTF-8 ans stores in itself
         */
        String & operator=(const uint32_t*);

        /**
         * Assign operator. Provides String1=(long double) expression.
         */
        String & operator=(long double);

        /**
         * Returns substring of current string.
         * @param Start Start position of substring
         * @param Count Number of sybmols after start position. If number==0 string from Start till end is returned.
         */
        String Substring(unsigned int Start, unsigned int Count = 0) const;

        /**
         * Replaces one text peace by another and returns result
         * @param Search Search string
         * @param Replace Replace string
         * @return Returns result of replacement
         */
        String Replace(const String &Search, const String &Replace) const;

        /**
         * Returns trimmed string. Removes whitespaces from left and right
         */
        String Trim() const;

        /**
         * Returns string with nice quotes like this « ». 
         */
        String Quote() const;

        /**
         * Replaces region of string by text peace and returns result.
         * @param Search Search string
         * @param Replace Replace string
         * @return Returns result of replacement
         */
        String SubstringReplace(unsigned int Start, unsigned int Count, const String &Replace) const;


        /**
         * Returns position of substring in current string.
         * @param Start Position to start search. Default is 0.
         * @return If substring not found returns -1.
         */
        long GetSubstringPosition(const UTF8::String &SubString, unsigned int Start = 0) const;

        /**
         * Get one char operator. Provides UTF8::String c=String1[1];
         */
        String operator[](unsigned int const) const;

        /**
         * Test operator. Provides String1==String2 expression.
         */
        bool operator==(const UTF8::String &) const;

        /**
         * Test operator. Provides String1!=String2 expression.
         */
        bool operator!=(const UTF8::String &) const;

        /**
         * Test operator. Provides String1=="Test" expression.
         */
        bool operator==(const char *) const;

        /**
         * Test operator. Provides String1!="Test" expression.
         */
        bool operator!=(const char *) const;

        /** Test operator. Provides String1<String2 expression.
         * Operator compares left characters of two strings.
         * If String1[0] value is less then the String2[0] returns true.
         * If they are equal then goes to second character and so on.
         * Can be used to sort strings alphabetical.
         */
        bool operator<(const UTF8::String &) const;

        /** Test operator. Provides String1>String2 expression.
         * Operator compares left characters of two strings.
         * If String1[0] value is greater then the String2[0] returns true.
         * If they are equal then goes to second character and so on.
         * Can be used to sort strings alphabetical.
         */
        bool operator>(const UTF8::String &) const;

        /**
         * Returns current string length. Also see DataLength to get buffer
         * size
         */
        unsigned int Length() const;

        /**
         * Returns current char data array length, containig UTF8 string.
         * As one character in UTF8 can be stored by more then one byte use
         * this function to know how much memory allocated for the string.
         */
        unsigned int DataLength() const;

        /**
         * Clears current string as if it is just created
         */
        void Empty();

        /**
         * If string is a one character check if it is one of given
         */
        bool CharacterIsOneOfThese(const UTF8::String &Characters) const;

        /**
         * Checks if this string contains given another string
         */
        bool HasThisString(const UTF8::String &Str) const;


        /**
         * Special function to convert from very big integers
         * Normally it is ok to assing UTF8::String to number. Or construct from it.
         * This function exists only for very very big integers conversion.
         */
        void ConvertFromInt64(int64_t n);


    private:
        char *Data;
        unsigned int DataArrayLength;
        unsigned int StringLength;

        unsigned int GetSequenceLength(const char * StartByte) const;
        void CheckIfStringIsCorrect(const char *str) const;
        void CalculateStringLength();

        void InitString();
        void AppendString(const char *str);
        void SetString(const char *str);
        int GetSymbolIndexInDataArray(unsigned int Position) const;

        void ConvertFromUTF32(const uint32_t *);
        void ConvertFromDouble(const long double d, const UTF8::String &ThousandSeparator = "", const UTF8::String &DecimalSeparator = ".", const int IntegerPartCount = 0, const int FractionPartCount = 0);

    };

}
/************************************************************************
//  help stuff
//  --------------------------------------------------------------------
//                                                                  
************************************************************************/


/**
 * Not in class overloaded operator +. Provides "Sample"+String1 expression.
 */
static UTF8::String operator+(const char *, const UTF8::String &);

/**
 * Not in class overloaded operator +. Provides std::string("123")+String1 expression.
 */
static UTF8::String operator+(const std::string &, const UTF8::String &);

/**
 * Not in class overloaded operator +. Provides 123+String1 expression.
 */
static UTF8::String operator+(long, const UTF8::String &);

/**
 * Not in class overloaded operator ==. Provides "Test"==String1 expression.
 */
static bool operator==(const char *, const UTF8::String &);

/**
 * Not in class overloaded operator ==. Provides std::string==String1 expression.
 */
static bool operator==(const std::string &, const UTF8::String &);

/**
 * Not in class overloaded operator !=. Provides "Test"!=String1 expression.
 */
static bool operator!=(const char *, const UTF8::String &);

/**
 * Not in class overloaded operator !=. Provides std::string!=String1 expression.
 */
static bool operator!=(const std::string &, const UTF8::String &);

/**
 * Overloading for cout. Provides std::cout << (UTF8::String) operation;
 */
static std::ostream & operator<<(std::ostream &os, const UTF8::String &s);

/************************************************************************
//  UTF8::Exception impl
//  --------------------------------------------------------------------
//                                                                  
************************************************************************/


//const int UTF8::Exception::UnspecifiedException;
//const int UTF8::Exception::StringToIntConversionError;
//const int UTF8::Exception::StringToDoubleConversionError;
//const int UTF8::Exception::FileNotFound;
//const int UTF8::Exception::StringIsNotACharacter;

UTF8::Exception::Exception(const std::string &error, const int &StatusCode) {
	this->error = error;
	this->StatusCode = StatusCode;
}

UTF8::Exception::Exception(std::string error) {
	this->error = error;
	this->StatusCode = UnspecifiedException;
}

UTF8::Exception::Exception(const UTF8::Exception &e) {
	error = e.error;
	StatusCode = e.StatusCode;
}

std::string UTF8::Exception::GetErrorString() const {
	return error;
}

int UTF8::Exception::GetErrorCode() const {
	return StatusCode;
}

UTF8::Exception & UTF8::Exception::operator =(const UTF8::Exception & e) {
	error = e.error;
	error = e.StatusCode;
	return *this;
}

/************************************************************************
//  UTF8::String impl
//  --------------------------------------------------------------------
//                                                                  
************************************************************************/
#define BIN_0b10			0x2
#define BIN_0b110			0x6
#define BIN_0b1110			0xE
#define BIN_0b11110			0x1E
#define BIN_0b111111		0x3F
#define BIN_0b11000000		0xC0
#define BIN_0b10000000		0x80
#define BIN_0b11100000		0xE0
#define BIN_0b11110000		0xF0

void UTF8::String::ConvertFromDouble(const long double d, const UTF8::String &ThousandSeparator, const UTF8::String &FractionSeparator, const int IntegerPartLength, const int FractionPartLength) {

	std::ostringstream os;
	os.precision(15);
	os << d;


	UTF8::String Number(os.str());
	UTF8::String Integer, Fraction;

	// Extracting integer and fraction
	std::vector <UTF8::String> Extracted = Number.Explode(".");

	unsigned int IntegerLength;
	if (IntegerPartLength) {
		IntegerLength = IntegerPartLength;
	} else {
		IntegerLength = Extracted[0].Length();
	}

	unsigned int FractionLength;
	if (FractionPartLength) {
		FractionLength = FractionPartLength;
	} else {
		if (Extracted.size() > 1) {
			FractionLength = Extracted[1].Length();
		} else {
			FractionLength = 0;
		}
	}

	// Parsing integer
	for (unsigned int i = 0; i < IntegerLength; i++) {
		if ((i > 0) && (i % 3 == 0)) {
			Integer = ThousandSeparator + Integer;
		}

		if (Extracted[0].Length() < i + 1) {
			Integer = "0" + Integer;
		} else {
			Integer = Extracted[0][Extracted[0].Length() - 1 - i] + Integer;
		}
	}


	// Parsing fraction
	if (FractionLength) {
		Fraction = FractionSeparator;
		for (unsigned int i = 0; i < FractionLength; i++) {
			if ((Extracted.size() > 1) && (Extracted[1].Length() > i)) {
				Fraction += Extracted[1][i];
			} else {
				Fraction += "0";
			}
		}
	}



	* this = Integer + Fraction;


}

bool UTF8::String::HasThisString(const UTF8::String &Str) const {

	return GetSubstringPosition(Str) != -1;
}

bool UTF8::String::CharacterIsOneOfThese(const UTF8::String &Characters) const {
	if (Length() == 1) {
		for (unsigned int i = 0; i < Characters.Length(); i++) {
			if (Characters[i] == *this) {
				return true;
			}
		}

		return false;
	} else {

		throw Exception("[CharacterIsOneOfThese] String is more then one character length: \"" + ToString() + "\"", UTF8::Exception::StringIsNotACharacter);
	}
}

UTF8::String UTF8::String::FromFile(const UTF8::String &Path) {
	UTF8::String s;

	std::ifstream File;
	File.open(Path.ToConstCharPtr());


	if (File.is_open()) {

		File.seekg(0, std::ios::end);
		unsigned int Length = File.tellg();
		File.seekg(0, std::ios::beg);

		char *buf = new char[Length + 1];
		memset(buf, 0, Length + 1);

		File.read(buf, Length);
		s.AppendString(buf);

		delete buf;
	} else {
		throw Exception("Cannot open file \"" + Path.ToString() + "\"", UTF8::Exception::FileNotFound);
	}

	File.close();

	return s;
}

long UTF8::String::Search(const UTF8::String &SubString, unsigned int StartPosition, int Direction) const {

	unsigned int SubstringLength = SubString.Length();
	unsigned int n = StartPosition;

	if (n > Length() - SubstringLength) {
		if (Direction == SearchDirectionFromLeftToRight) {
			return -1;
		} else {
			n = Length() - SubstringLength;
		}
	}

	if (n < 0) {
		if (Direction == SearchDirectionFromRightToLeft) {
			return -1;
		} else {
			n = 0;
		}
	}

	while (((Direction == SearchDirectionFromLeftToRight) && (n < Length() - SubstringLength + 1)) || ((Direction == SearchDirectionFromRightToLeft) && (n >= 0))) {

		if (this->Substring(n, SubstringLength) == SubString) {

			return n;
		}

		n += Direction == SearchDirectionFromLeftToRight ? 1 : -1;
	}

	return -1;

}

std::ostream & operator<<(std::ostream &os, const UTF8::String &s) {
	os << s.ToString();

	return os;
}

bool operator==(const char *str, const UTF8::String &StringObj) {

	return StringObj == str;
}

bool operator==(const std::string &str, const UTF8::String &StringObj) {

	return StringObj == str;
}

bool operator!=(const char *str, const UTF8::String &StringObj) {

	return StringObj != str;
}

bool operator!=(const std::string &str, const UTF8::String &StringObj) {

	return StringObj != str;
}

UTF8::String UTF8::String::Quote() const {
	return "<<"+(*this)+">>";
}

UTF8::String UTF8::String::Trim() const {
	UTF8::String result = *this;
	long i = 0;

	while ((result[i] == " ") || (result[i] == "\n") || (result[i] == "\r") || (result[i] == "\t")) {
		i++;
	}

	if (i == result.Length()) {
		return UTF8::String();
	}


	long j = result.Length();
	while ((result[j - 1] == " ") || (result[j - 1] == "\n") || (result[j - 1] == "\r") || (result[j - 1] == "\t")) {
		j--;
	}


	result = result.Substring(i, j - i);

	return result;
}

UTF8::String UTF8::String::Replace(const UTF8::String &Search, const UTF8::String &Replace) const {
	UTF8::String result = *this;

	// Long to cover unsigned int and -1
	long pos = 0;
	while ((pos = result.Search(Search, pos)) != -1) {

		result = result.SubstringReplace(pos, Search.Length(), Replace);

		// Next time we search after replacement
		pos += Replace.Length();
	}

	return result;

}

UTF8::String UTF8::String::SubstringReplace(unsigned int Start, unsigned int Count, const UTF8::String &Replace) const {
	if (Start < Length()) {
		return (Start ? Substring(0, Start) : UTF8::String())+Replace + Substring(Start + Count);
	} else {

		return *this;
	}
}

UTF8::String UTF8::String::Implode(const std::vector <UTF8::String> &Strings, const UTF8::String &Separator) {
	if (Strings.size()) {
		UTF8::String Result;

		for (unsigned int i = 0; i < Strings.size(); i++) {
			if (Result.Length()) {
				Result += Separator;
			}

			Result += Strings[i];
		}

		return Result;
	} else {

		return UTF8::String();
	}
}

std::vector <UTF8::String> UTF8::String::Explode(const String &Separator) const {
	std::vector <UTF8::String> v;

	unsigned int prev = 0;

	int i = 0;

	while (i < Length() - Separator.Length() + 1) {
		if (Substring(i, Separator.Length()) == Separator) {
			if (i - prev > 0) {
				v.push_back(Substring(prev, i - prev));
			}
			i += Separator.Length();
			prev = i;
		} else {
			i++;
		}
	}

	if (prev < Length()) {

		v.push_back(Substring(prev, Length() - prev));
	}

	return v;
}

UTF8::String operator+(const char *CharPtr, const UTF8::String &StringObj) {
	UTF8::String s(CharPtr);
	s += StringObj;

	return s;

}

UTF8::String operator+(const std::string & str, const UTF8::String &StringObj) {
	UTF8::String s(str);
	s += StringObj;

	return s;

}

UTF8::String operator+(const long l, const UTF8::String &StringObj) {
	UTF8::String s(l);
	s += StringObj;

	return s;
}

UTF8::String UTF8::String::operator+(const UTF8::String &s) const {
	UTF8::String res(*this);
	res.AppendString(s.Data);

	return res;
}

UTF8::String & UTF8::String::operator+=(const UTF8::String &s) {
	AppendString(s.Data);

	return *this;
}

void UTF8::String::AppendString(const char *str) {
	// The functions that can fill buffer directly:
	//
	//       SetString         AppendString
	//
	// Make shure all preparations are done there

	if (str && strlen(str)) {
		if (DataArrayLength) {
			CheckIfStringIsCorrect(str);

			unsigned int StrLength = strlen(str);

			Data = (char *) realloc(Data, DataArrayLength + StrLength + 1);

			if (Data != NULL) {

				memcpy(Data + DataArrayLength, str, StrLength);
				DataArrayLength += StrLength;
				Data[DataArrayLength] = 0;

				CalculateStringLength();
			} else {
				throw Exception("[AppendString] Cannot realloc any more memory");
			}
		} else {

			SetString(str);
		}
	}
}

void UTF8::String::SetString(const char *str) {
	// The functions that can fill buffer directly:
	//
	//       SetString         AppendString
	//
	// Make shure all preparations are done there

	if (str && strlen(str)) {
		CheckIfStringIsCorrect(str);

		Empty();

		DataArrayLength = strlen(str);
		Data = new char[DataArrayLength + 1];
		Data[DataArrayLength] = 0;

		memcpy(Data, str, DataArrayLength);

		CalculateStringLength();
	} else {

		Empty();
	}
}

void UTF8::String::ConvertFromInt64(int64_t n) {
	Empty();

	if (n) {
		bool minus;
		if (n < 0) {
			n = -n;
			minus = true;
		} else {
			minus = false;
		}

		char tmp[32] = "0";
		const char *num = "0123456789";
		memset(tmp, 0, 32);

		unsigned int i = 30;

		while (n) {
			tmp[i] = num[n % 10];
			n /= 10;
			i--;

			if ((i < 0) || ((i < 1) && minus)) {
				throw Exception("[ConvertFromInt] Cycle terminated, buffer overflow.");
			}
		}

		if (minus) {
			tmp[i] = '-';
			i--;
		}

		SetString(tmp + i + 1);
	} else {

		SetString("0");

	}

	CalculateStringLength();
}

UTF8::String::String(const long double d, const UTF8::String &ThousandSeparator, const UTF8::String &DecimalSeparator, const int IntegerPartCount, const int FractionPartCount) {

	InitString();
	ConvertFromDouble(d, ThousandSeparator, DecimalSeparator, IntegerPartCount, FractionPartCount);
}

void UTF8::String::InitString() {

	Data = NULL;
	DataArrayLength = 0;
	StringLength = 0;
}

UTF8::String::String() {

	InitString();
}

UTF8::String::String(const std::string & s) {

	InitString();
	CheckIfStringIsCorrect(s.c_str());
	AppendString(s.c_str());
	CalculateStringLength();
}

int UTF8::String::GetSymbolIndexInDataArray(unsigned int Position) const {
	if (Position >= StringLength) {
		throw Exception((UTF8::String("[GetSymbolIndexInDataArray] trying to get position beyond the end of string. StringLength: ") + StringLength + " Position: " + Position + " String: [" + Data + "]").ToString());
	}

	unsigned int n = 0;
	for (unsigned int i = 0; i < Position; i++) {

		n += GetSequenceLength(Data + n);
	}

	return n;

}

long UTF8::String::GetSubstringPosition(const UTF8::String &SubString, unsigned int Start) const {
	if (SubString.Length() > StringLength) {
		return -1;
	}

	unsigned int ScansCount = StringLength - SubString.StringLength + 1 - Start;
	for (unsigned int i = 0; i < ScansCount; i++) {
		if (this->Substring(i + Start, SubString.StringLength) == SubString) {

			return i + Start;
		}
	}

	return -1;
}

UTF8::String UTF8::String::Substring(unsigned int Start, unsigned int Count) const {
	if (Start >= StringLength) {
		return UTF8::String();
	}

	if ((Start + Count > StringLength) || (Count == 0)) {
		Count = StringLength - Start;
	}


	unsigned int StartIndex = GetSymbolIndexInDataArray(Start);
	unsigned int CopyAmount = 0;


	for (unsigned int i = 0; i < Count; i++) {
		CopyAmount += GetSequenceLength(Data + StartIndex + CopyAmount);
	}

	char *tmp = new char[CopyAmount + 1];
	memcpy(tmp, Data + StartIndex, CopyAmount);
	tmp[CopyAmount] = 0;

	UTF8::String r(tmp);
	delete tmp;

	return r;
}

UTF8::String::String(const char * str) {

	InitString();
	SetString(str);
}

UTF8::String::String(const uint32_t * str) {

	InitString();
	ConvertFromUTF32(str);
}

void UTF8::String::ConvertFromUTF32(const uint32_t *s) {
	if (s) {
		unsigned int WideStringLength = 0;
		do {
			WideStringLength++;
			if (WideStringLength == 4294967295UL) {
				throw Exception("[ConvertFromUTF32] Cannot find termination symbol in incoming string.");
			}
		} while (s[WideStringLength]);

		char *tmp = new char[WideStringLength * 4 + 1];
		memset(tmp, 0, WideStringLength * 4 + 1);
		unsigned int pos = 0;

		for (int i = 0; i < WideStringLength; i++) {
			uint32_t wc = s[i];

			if (wc < 0x80) {
				tmp[pos++] = wc;
			} else if (wc < 0x800) {
				tmp[pos++] = (wc >> 6) | BIN_0b11000000;
				tmp[pos++] = (wc & BIN_0b111111) | BIN_0b10000000;
			} else if (wc < 0x10000) {
				tmp[pos++] = (wc >> 12) | BIN_0b11100000;
				tmp[pos++] = ((wc >> 6) & BIN_0b111111) | BIN_0b10000000;
				tmp[pos++] = (wc & BIN_0b111111) | BIN_0b10000000;
			} else {

				tmp[pos++] = (wc >> 18) | BIN_0b11110000;
				tmp[pos++] = ((wc >> 12) & BIN_0b111111) | BIN_0b10000000;
				tmp[pos++] = ((wc >> 6) & BIN_0b111111) | BIN_0b10000000;
				tmp[pos++] = (wc & BIN_0b111111) | BIN_0b10000000;
			}
			/*if (wc < 0x80) {
				tmp[pos++] = wc;
			} else if (wc < 0x800) {
				tmp[pos++] = (wc >> 6) | 0b11000000;
				tmp[pos++] = (wc & 0b111111) | 0b10000000;
			} else if (wc < 0x10000) {
				tmp[pos++] = (wc >> 12) | 0b11100000;
				tmp[pos++] = ((wc >> 6) & 0b111111) | 0b10000000;
				tmp[pos++] = (wc & 0b111111) | 0b10000000;
			} else {

				tmp[pos++] = (wc >> 18) | 0b11110000;
				tmp[pos++] = ((wc >> 12) & 0b111111) | 0b10000000;
				tmp[pos++] = ((wc >> 6) & 0b111111) | 0b10000000;
				tmp[pos++] = (wc & 0b111111) | 0b10000000;
			}*/

		}

		SetString(tmp);

		delete tmp;
	}
}

void UTF8::String::CalculateStringLength() {
	// We are not writing anything to memory so limits are not needed
	if (Data) {
		unsigned int n = 0, count = 0;
		do {
			// We do not need to check line end here, it is checked when string is changed
			n += GetSequenceLength(Data + n);
			count++;
		} while (Data[n]);

		StringLength = count;
	} else {

		StringLength = 0;
	}
}

void UTF8::String::CheckIfStringIsCorrect(const char *str) const {
	if (str) {
		// We are not writing anything to memory so limits are not needed
		unsigned int n = 0, i;
		unsigned int SequenceLength;
		while (str[n]) {
			SequenceLength = GetSequenceLength(str + n);
			for (i = 1; i < SequenceLength; i++) {
				//if ((((unsigned char) str[n + i]) >> 6) != 0b10) {
				if ((((unsigned char) str[n + i]) >> 6) != BIN_0b10) {
					std::string s(str);
					throw Exception("[CheckIfStringIsCorrect] Incorrect byte in UTF8 sequence: \"" + s + "\"");
				}
			}
			n += SequenceLength;
			if (n >= 0xFFFFFFFF - 4) {

				std::string s(str);
				throw Exception("[CheckIfStringIsCorrect] termination char was not found in string: \"" + s + "\"");
			}
		}
	}
}

bool UTF8::String::operator>(const UTF8::String &s) const {
	if (*this == s) {
		return false;
	}

	if (*this<s) {

		return false;
	}


	return true;
}

bool UTF8::String::operator<(const UTF8::String &s) const {
	unsigned int MinLength = StringLength < s.StringLength ? StringLength : s.StringLength;

	//std::cout << "MinLength=" << MinLength;

	unsigned int MyPos = 0, RemotePos = 0;
	unsigned int MySequenceLength, RemoteSequenceLength;
	for (unsigned int i = 0; i < MinLength; i++) {
		MySequenceLength = GetSequenceLength(Data + MyPos);
		RemoteSequenceLength = GetSequenceLength(s.Data + RemotePos);

		if (MySequenceLength < RemoteSequenceLength) {
			return true;
		}

		if (MySequenceLength > RemoteSequenceLength) {
			return false;
		}

		for (unsigned int j = 0; j < MySequenceLength; j++) {
			if (Data[MyPos + j] < s.Data[RemotePos + j]) {
				return true;
			}

			if (Data[MyPos + j] > s.Data[RemotePos + j]) {

				return false;
			}
		}

		MyPos += MySequenceLength;
		RemotePos += RemoteSequenceLength;
	}

	// If this string is substring of s (from left side) then it is lower
	return StringLength < s.StringLength;
}

UTF8::String UTF8::String::operator[](unsigned int const n) const {
	if (n >= StringLength) {
		return UTF8::String();
	}

	if (n < 0) {
		return UTF8::String();
	}

	unsigned int pos = 0;
	for (unsigned int i = 0; i < n; i++) {
		pos += GetSequenceLength(Data + pos);
	}

	char t[5];
	memset(t, 0, 5);
	memcpy(t, Data + pos, GetSequenceLength(Data + pos));

	return UTF8::String(t);
}

unsigned int UTF8::String::GetSequenceLength(const char * StartByte) const {
	if (StartByte && strlen(StartByte)) {
		unsigned char Byte = StartByte[0];
		if (Byte < 128) {
			return 1;
		}
		// Here we need back order due to mask operation
		if ((Byte >> 5) == BIN_0b110) {
			return 2;
		}

		if ((Byte >> 4) == BIN_0b1110) {
			return 3;
		}

		if ((Byte >> 3) == BIN_0b11110) {

			return 4;
		}

		throw Exception(std::string("[GetSequenceLength] Invalid UTF8 start byte. My own string is: [") + Data + "] Argument is: [" + StartByte + "]");
	} else {

		throw Exception(std::string("[GetSequenceLength] Invalid UTF8 start byte (it is empty). My own string is: [") + Data + "] Argument is: [" + StartByte + "]");
	}
}

UTF8::String & UTF8::String::operator=(const String &Original) {
	// Check if objects are not same
	if ((unsigned int long) &Original != (unsigned int long) this) {
		Empty();
		SetString(Original.Data);
	}

	return *this;
}

UTF8::String & UTF8::String::operator=(const char *str) {
	Empty();
	SetString(str);

	return *this;
}

UTF8::String & UTF8::String::operator=(const uint32_t *str) {
	Empty();
	ConvertFromUTF32(str);

	return *this;
}

UTF8::String & UTF8::String::operator=(long double d) {
	Empty();
	ConvertFromDouble(d);

	return *this;
}

void UTF8::String::Empty() {
	if (DataArrayLength) {

		delete Data;
		InitString();
	}
}

std::string UTF8::String::ToString() const {
	if (DataArrayLength) {
		return std::string(Data);
	} else {

		return std::string();
	}
}

double UTF8::String::ToDouble() const {
	int64_t mul = 1;
	char c;
	int int_part = 0;
	double prec_part = 0;

	for (int i = DataArrayLength - 1; i >= 0; i--) {
		c = Data[i];
		if ((c >= '0') && (c <= '9')) {
			int_part += (c - '0') * mul;
			mul *= 10;
		} else {
			if (c == '.') {
				prec_part = (double) int_part / (double) mul;
				int_part = 0;
				mul = 1;
			} else {
				if ((c == '-') && (i == 0)) {
					int_part = -int_part;
					prec_part = -prec_part;
				} else {

					UTF8::String err = "Cannot convert \"" + * this+"\" to double.";
					throw UTF8::Exception(err.ToConstCharPtr(), UTF8::Exception::StringToDoubleConversionError);
				}
			}
		}
	}
	return int_part + prec_part;
}

int64_t UTF8::String::ToLong() const {
	int64_t mul = 1;
	char c;
	int64_t number = 0;

	for (int i = DataArrayLength - 1; i >= 0; i--) {
		c = Data[i];
		if ((c >= '0') && (c <= '9')) {
			number += (c - '0') * mul;
			mul *= 10;
		} else {
			if (c == '.') {
				number = 0;
				mul = 1;
			} else {
				if ((c == '-') && (i == 0)) {
					number = -number;
				} else {

					UTF8::String err = "Cannot convert \"" + * this+"\" to number.";
					throw UTF8::Exception(err.ToConstCharPtr(), UTF8::Exception::StringToIntConversionError);
				}
			}
		}
	}

	return number;

}

UTF8::String UTF8::String::operator+(const char *s) const {
	UTF8::String res(*this);
	res.AppendString(s);

	return res;
}

bool UTF8::String::operator==(const UTF8::String &s) const {
	if (DataArrayLength != s.DataArrayLength) {
		return false;
	} else {
		for (int i = 0; i < DataArrayLength; i++) {
			if (Data[i] != s.Data[i]) {

				return false;
			}
		}

		return true;
	}
}

bool UTF8::String::operator!=(const UTF8::String &s) const {

	return !(*this == s);
}

bool UTF8::String::operator==(const char *str) const {
	if (str && strlen(str)) {
		if (DataArrayLength != strlen(str)) {
			return false;
		} else {
			for (int i = 0; i < DataArrayLength; i++) {
				if (Data[i] != str[i]) {
					return false;
				}
			}

			return true;
		}
	} else {

		return StringLength == 0;
	}
}

bool UTF8::String::operator!=(const char *str) const {

	return !(*this == str);
}

const char * UTF8::String::ToConstCharPtr() const {

	return Data;
}

unsigned int UTF8::String::Length() const {

	return StringLength;
}

unsigned int UTF8::String::DataLength() const {

	return DataArrayLength;
}

UTF8::String::~String() {

	Empty();
}

UTF8::String::String(const String& orig) {
	InitString();
	SetString(orig.Data);
}

#endif // UFT8_STRINGS_INLINE_HPP__