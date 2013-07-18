#pragma once

#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <assert.h>
#include <iostream>

#include "System.h"

#ifdef ___GNU_LINUX
#include <string.h>
#include <stdarg.h>
#include <linux/types.h>
#include <sys/types.h>
#endif

/// Note that this file is named "Strings.h" due to much trouble making clear
/// to various compilers that this is not to be confused with the c library
/// "string.h" library. This is mostly a problem for file systems which are
/// not case sensitive.

namespace ToolChest
{

   class String
   {
   private:
      char * _s;

      /// Private constructor, creates an uninitialized string of given length
      /// The dummy arguments are merely to distinguish it from the proper
      /// constructors which initializes to the value of an integer.
      inline String(int length, int dummy0, int dummy1) : _s(NULL)
      {
         assert(length >= 0);
         _s = new char[length+1]; _s[length] = '\0';
      }

      static String GetNumericChars()
      {
         const static String numeric = "0123456789";
         return numeric;
      }

   public:

      /// Max field width is the field into which numerical values can
      /// be formatted with the String() constructors
      static const int MAX_FIELD_WIDTH = 14;


      //////////////////
      // Constructors //
      //////////////////

      /// Generic templated constructor for non-primitive objects which
      /// implement a toString() method
      template <class T> inline String(const T& obj) : _s(NULL)
      { *this = obj.ToString(); }
      template <class T> inline String(const T& obj, int prec) : _s(NULL)
      { *this = obj.ToString(prec); }

      /// Default constructor, gives an empty string
      inline String() { _s = new char[1]; _s[0] = '\0'; }

      /// Copy constructor
      inline String(const String& s) : _s(NULL) { assert(s._s); allocate(s._s); }
      inline String(const char * s)  : _s(NULL) { allocate(s); }
      inline String(char * const s)  : _s(NULL) { allocate(s); }
      inline String(char c) { _s = new char[2]; _s[0] = c; _s[1] = '\0'; }

      /// \brief Floating point constructors
      /// \param prec determines the number of digits printed after the decimal
      inline String(float f, int prec = 3) : _s(NULL) { Format((double)f, prec); }
      inline String(double f, int prec = 3) : _s(NULL) { Format(f, prec); }

      static String Format(const char * s, ...)
      {
         va_list argptr;
         va_start(argptr, s);
         char buffer[2048];
         vsprintf(buffer, s, argptr);
         va_end(argptr);
         int len = (int)strlen(s);
         int nullChar = (len < 2047 ? len : 2047); //   MIN((int)strlen(s), 2047);
         buffer[nullChar] = '\0';
         return String(buffer);
      }



      /// 16-bit integer constructors
      /// \param fieldWidth gives the number of characters to be printed. If it is
      /// shorter than needed to print the number, it will be ignored. If it is
      /// longer than necessary, the integer is printed left-justified and padded
      /// with spaces. Applies to 32- and 64-bit integer constructors as well.
      inline String(unsigned short i, int fieldWidth = 0) : _s(NULL)
      { Format((unsigned int)i, fieldWidth, GetUnsigned32Formats()); }
      inline String(short i, int fieldWidth = 0) : _s(NULL)
      { Format((int)i, fieldWidth, GetSigned32Formats()); }

      /// 32-bit integer constructors
      inline String(unsigned int i, int fieldWidth = 0) : _s(NULL)
      { Format((unsigned int)i, fieldWidth, GetUnsigned32Formats()); }
      inline String(int i, int fieldWidth = 0) : _s(NULL)
      { Format((int)i, fieldWidth, GetSigned32Formats()); }
      inline String(unsigned long int i, int fieldWidth = 0) : _s(NULL)
      { Format((unsigned int)i, fieldWidth, GetUnsigned32Formats()); }
      inline String(long int i, int fieldWidth = 0) : _s(NULL)
      { Format((int)i, fieldWidth, GetSigned32Formats()); }

      /// 64-bit integer constructors
      inline String(unsigned long long int i, int fieldWidth = 0) : _s(NULL)
      { Format(i, fieldWidth, GetUnsigned64Formats()); }
      inline String(long long int i, int fieldWidth = 0) : _s(NULL)
      { Format(i, fieldWidth, GetSigned64Formats()); }

      /// boolean constructor
      inline String(bool b)
      {
         _s = new char[6];
         if (b) { strcpy(_s, "true");  _s[4] = '\0'; }
         else   { strcpy(_s, "false"); _s[5] = '\0'; }
      }

      inline ~String() { assert(_s); release(); }  ///< Destructor

      //////////////////////////
      // Assignment Operators //
      //////////////////////////

      /// \brief Standard assignment operator(s)
      inline String& operator = (const String& s)
      { assert(s._s); release(); allocate(s._s); return *this; }
      inline String& operator = (const char * s)
      { assert(s); release(); allocate(s); return *this; }


      ///////////////
      // Accessors //
      ///////////////

      /// \brief Dereference operator
      inline const char * operator * () const { return _s; }
      inline const char * Get() const         { return _s; }

      /// \\brief Returns length of the string
      inline int Length() const       { return (int)strlen(_s); }

      /// \brief Accesses a character in the string as if it were an array
      inline const char& operator[](int i) const
      { assert(Length() >= i); return _s[i]; }
      inline char& operator[](int i)
      { assert(Length() >= i); return _s[i]; }

      //////////////////////
      // Append Operators //
      //////////////////////

      /// \brief Concatenation operators
      inline String operator + (const char * s) const
      { return (String(_s) += s); }
      inline String operator + (const String& s) const
      { return (String(_s) += s._s); }
      inline String& operator += (const String& s)
      { assert(s._s); return operator+=(s._s); }

      String& operator += (const char * s); ///< Do not inline, results in code bloat


      //////////////////////////
      // Comparison Operators //
      //////////////////////////

      inline bool operator <  (const char * s) const { return (bool)(strcmp(_s, s) < 0); }
      inline bool operator >  (const char * s) const { return (bool)(strcmp(_s, s) > 0); }
      inline bool operator <= (const char * s) const { return (bool)(strcmp(_s, s) <= 0); }
      inline bool operator >= (const char * s) const { return (bool)(strcmp(_s, s) >= 0); }
      inline bool operator == (const char * s) const { return !strcmp(_s, s); }
      inline bool operator != (const char * s) const { return (bool)strcmp(_s, s); }

      inline bool operator <  (const String& s) const { return (bool)(strcmp(_s, s._s) < 0); }
      inline bool operator >  (const String& s) const { return (bool)(strcmp(_s, s._s) > 0); }
      inline bool operator <= (const String& s) const { return (bool)(strcmp(_s, s._s) <= 0); }
      inline bool operator >= (const String& s) const { return (bool)(strcmp(_s, s._s) >= 0); }
      inline bool operator == (const String& s) const { return !strcmp(_s, s._s); }
      inline bool operator != (const String& s) const { return (bool)strcmp(_s, s._s); }

      /// \brief Case-insensitive equality tests.
      inline bool operator /= (const String& s) const { return operator/=(*s); }
      bool operator /= (const char *s) const;


      /////////////////////////////
      // Miscellaneous Functions //
      /////////////////////////////

      String& Reverse();   ///< Reverses the order of the characters

      void ToUpper(); ///< converts lower-case to upper-case
      void ToLower(); ///< converts upper-case to lower-case

      /// \brief Produces x instances of the character c in a string
      inline static String Replicate(int x, char c)
      { assert(x >= 0); String r; for (int i = 0; i < x; ++i) r += c; return r; }

      String& Trim(); ///< Removes leading and trailing whitespace
      String PadToFieldWidth(uint32 w) const;   ///< Adds trailing whitespace

      inline String TrimAndStripQuotes() const
      {
         if (*this == "") return *this;

         String x = *this;
         x.Trim();
         int first = (x[0] == '\"' || x[0] == '\'' ? 1 : 0);
         int last  = (x[x.Length()-1] == '\"' || x[x.Length()-1] == '\'' ? x.Length()-1 : x.Length());
         return x.SubString((int)first, (int)last).Trim();
      }

      void RemoveAllWhiteSpace();
      String SubString(int a, int b) const;
      int FindFirst(char c) const;     ///< Finds first occurance of c
      int FindFirst(String set) const; ///< Find first occurance of any char in set
      int FindLast(char c) const;      ///< Finds the last occurance of c
      int FindLast(String set) const;  ///< Find last occurance of any char in set

      /// \brief Returns all characters after the last dot, returns "" if there
      /// is no dot (and thus no file extension)
      //String getFileExtension() const;

      /// Parses a filename that has a frame number, such as "Screenshot052.jpg"
      /// returns a triplet containing the head, the tail, and the frame number.
      /// Assumes there is only one integral number in the filename
      //FilenameAnatomy disassemble() const;

      ///////////////////////
      // Internal Routines //
      ///////////////////////

   private:

      /// \brief Allocates memory for the string in source
      inline void allocate(const char * source)
      {
         release(); int length = (int)strlen(source);
         _s = new char[length + 1];
         strcpy(_s, source); _s[length] = '\0';
      }

      /// \brief allocates memory for a string of certain length, does not initialize
      inline void allocate(int nChars)
      {
         release();
         _s = new char[nChars + 1];
         _s[nChars] = '\0';
      }

      /// \brief Conditionally frees the memory allocated at _s
      inline void release() { if (_s) delete [] _s; _s = NULL; }

      // Internal formatting routines, mostly for constructors
      /// Warning: assumes that _s is NULL
      /// Float constructors cast upwards to double and then perform conversion,
      /// integers shorter than 32-bits are upgraded to 32-bits for conversion
      void Format(double d, int prec);

      template <typename IntegerType>
      void Format(IntegerType i, int fieldWidth, const char ** formats);

      /// Formatting string arrays for use in the format functions

      static const char ** GetFloatFormats()
      {
         static const char * f[MAX_FIELD_WIDTH]
            = {"%g", "%.0f", "%.1f", "%.2f", "%.3f", "%.4f", "%.5f", 
               "%.6f", "%.7f", "%.8f", "%.9f", "%.10f", "%.11f", "%.12f"};
         return f;
      }

      static const char ** GetUnsigned32Formats()
      {
         static const char * f[MAX_FIELD_WIDTH]
            = {"%u", "%1u", "%2u", "%3u", "%4u", "%5u", "%6u", "%7u", 
               "%8u", "%9u", "%10u", "%11u", "%12u", "%13u"};
         return f;
      }

      static const char ** GetUnsigned64Formats()
      {
         static const char * f[MAX_FIELD_WIDTH]
            = {"%llu", "%1llu", "%2llu", "%3llu", "%4llu", "%5llu", "%6llu", 
               "%7llu", "%8llu", "%9llu", "%10llu", "%11llu", "%12llu", "%13llu"};
         return f;
      }

      static const char ** GetSigned32Formats()
      {
         static const char * f[MAX_FIELD_WIDTH]
            = {"%i", "%1i", "%2i", "%3i", "%4i", "%5i", "%6i", "%7i", "%8i", 
               "%9i", "%10i", "%11i", "%12i", "%13i"};
         return f;
      }

      static const char ** GetSigned64Formats()
      {
         static const char * f[MAX_FIELD_WIDTH]
            = {"%lli", "%1lli", "%2lli", "%3lli", "%4lli", "%5lli", "%6lli", 
               "%7lli", "%8lli", "%9lli", "%10lli", "%11lli", "%12lli", "%13lli"};
         return f;
      }

   };

   /// Insertion and extraction operators for C++ I/O Streams
   inline std::ostream& operator << (std::ostream& stream, const String& s)
   { stream << s.Get(); return stream; }
   inline std::istream& operator >> (std::istream& stream, String& s)
   {
      char buffer[512];
      stream.getline(buffer, 512);
      s = buffer;
      return stream;
   }

   //////////////////////
   // Append Operators //
   //////////////////////

   inline String& String::operator += (const char * c)
   {
      // No null pointers please
      assert(_s); assert(c);

      uint32 newLength = (uint32)(strlen(_s) + strlen(c));
      char * temp = new char[newLength+1];
      strcpy(temp, _s);
      strcat(temp, c);
      temp[newLength] = '\0';

      allocate(temp);
      //Memory::free(temp);
      delete [] temp;   // Fixed the leak!?

      return *this;
   }

   ////////////////////////////
   // Miscellaneous Routines //
   ////////////////////////////


   String& String::Reverse()
   {
      assert(_s);
      int length = (int)strlen(_s);
      int halfLength = (length/2);
      for (int i = 0; i < halfLength; ++i)
      {
         char c = _s[i];
         _s[i] = _s[length-i-1];
         _s[length-i-1] = c;
      }

      return *this;
   }


   // Case-insensitive comparison
   bool String::operator /= (const char * s) const
   {
      #ifdef WIN32
      assert(_s);
      return (stricmp(_s, s) == 0);
      #else

      if (strlen(s) != strlen(_s)) return false;
      else
      {
         String t1 = s;     t1.ToUpper();
         String t2 = *this; t2.ToUpper();
         return !strcmp(*t1, *t2);
      }

      #endif
   }

   /// Pads a string with whitespace out to fill a field width "w". If the
   /// string currently is >= w in length, nothing will be done to it.
   /// Returned string is guaranteed to be greater than or equal to w in
   /// length
   String String::PadToFieldWidth(uint32 w) const
   {
      uint32 len = (uint32)Length();
      if (len < w) return *this + String::Replicate(w - len, ' ');
      else         return *this;
   }

   String& String::Trim()
   {
      if (strlen(_s) == 0) return *this;

      int beginning = 0;
      int end = (int)strlen(_s)-1;

      int index = 0;
      while ((_s[index] == ' ' || _s[index] == '\t') && index < (int)strlen(_s))
      { beginning++; index++; }

      // Special case, the string was all whitespace
      if (index == (int)strlen(_s))
      {
         release();
         _s = new char[1];
         _s[0] = '\0';
         return *this;
      }

      index = (uint32)(strlen(_s)-1);
      while (_s[index] == ' ' || _s[index] == '\t') { end--; index--; }

      //char * temp = _s;
      allocate(SubString((int)beginning, int(end+1)).Get());   // ??

      return *this;
   }

   void String::RemoveAllWhiteSpace()
   {
      String result;
      int length = (int)strlen(_s);
      for (int i = 0; i < length; ++i)
         if (_s[i] != ' ' && _s[i] != '\t') result += _s[i];

      allocate(result._s);
   }

   // 'a' is the index of the first character, 'b' is one past the last.
   String String::SubString(int a, int b) const
   {
      assert(a >= 0); assert(a <= b);
      assert((unsigned int)b <= strlen(_s));

      if (b-a == 0) return String();

      String x(b-a, 0, 0); //x.allocate(b-a);
      memcpy(&x._s[0], &_s[a], b-a);
      //for (int i = a; i < b; ++i) x[i] += _s[i];
      return x;
   }

   // Finds first occurance of character c
   int String::FindFirst(char c) const
   {
      int length = (int)strlen(_s);
      for (int i = 0; i < length; ++i)
         if (_s[i] == c) return i;
      return -1;   // nothing was found
   }

   // Finds first occurance of any character in "set"
   int String::FindFirst(String set) const
   {
      int length = (int)strlen(_s);
      int length2 = set.Length();
      for (int i = 0; i < length; ++i)
         for (int j = 0; j < length2; ++j)
            if (_s[i] == set[j]) return i;
      return -1;
   }

   // Finds last occurance of character c
   int String::FindLast(char c) const
   {
      int length = (int)strlen(_s);
      for (int i = length-1; i >= 0; --i)
         if (_s[i] == c) return i;
      return -1;   // nothing was found
   }

   // Finds last occurance of any character in "set"
   int String::FindLast(String set) const
   {
      int length = (int)strlen(_s);
      int length2 = set.Length();
      for (int i = length-1; i >= 0; --i)
         for (int j = 0; j < length2; ++j)
            if (_s[i] == set[j]) return i;
      return -1;
   }

   // converts case
   void String::ToUpper()
   {
      #ifdef ___WINDOWS_NT
      strupr(_s);
      #else

      int i = 0;
      while (_s[i] != 0)
      {
         if (_s[i] < 122 && _s[i] > 96)
            _s[i] -= (int)('a' - 'A');
         ++i;
      }

      #endif
   }

void String::ToLower()
{
   #ifdef ___WINDOWS_NT
   strlwr(_s);
   #else

   int i = 0;
   while (_s[i] != 0)
   {
      if (_s[i] < 91 && _s[i] > 64)
         _s[i] += (int)('a' - 'A');
      ++i;
   }

   #endif
}
   /////////////////////////
   // Formatting Routines //
   /////////////////////////

   // Do not force inlining of these functions, but allow the compiler to decide

   inline void String::Format(double d, int prec)
   {
      assert(!_s);

      if (prec > 8) prec = 8;
      char buffer[64];
      sprintf(buffer, GetFloatFormats()[prec+1], d);
      int length = (int)strlen(buffer);
      _s = new char[length+1]; 
      strcpy(_s, buffer); _s[length] = '\0';
   }

   template <typename IntegerType>
   inline void String::Format(IntegerType i, int fieldWidth, const char ** formats)
   {
      assert(!_s);

      if (fieldWidth > MAX_FIELD_WIDTH) fieldWidth = MAX_FIELD_WIDTH;
      char buffer[64];
      sprintf(buffer, formats[fieldWidth], i);
      int length = (int)strlen(buffer);
      _s = new char[length+1]; 
      strcpy(_s, buffer); _s[length] = '\0';
   }

}; // namespace Burns

#endif // GSTRING_H
