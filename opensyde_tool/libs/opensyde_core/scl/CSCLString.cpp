//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       String class

   ANSI C++ string handling class.
   For details cf. documentation in .h file.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.04.2009  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdio>  //for printf
#include <cctype>  //for tolower / toupper
#include <cstdlib> //for strtol
#include <cstdarg>
#include <sstream>

#include "stwtypes.h"
#include "CSCLString.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_scl;

//Unfortunately we need a hack for Visual C++ and MinGW here: vsnprintf is not in the std namespace, so we drag it in
//Officially vsnprintf is only part of std in C++11
#if defined(_MSC_VER) || defined(__MINGW32__)
namespace std
{
using ::vsnprintf;
}
#endif

/* -- Module Global Constants ---------------------------------------------- */
//unfortunately the 64bit printf specifier is not fully portable :-(
#ifdef _WIN32
//MS-VC, Borland C++, MinGW (MinGW also uses the MSVCRT.dll !)
static const charn macn_PRINTF_SPEC_64BITS[] = "%I64i";
static const charn macn_PRINTF_SPEC_64BITU[] = "%I64u";
static const charn macn_PRINTF_SPEC_64BITX[] = "%I64x";
#else
//GCC non-Windows (Borland C++ could also handle this)
static const charn macn_PRINTF_SPEC_64BITS[] = "%lli";
static const charn macn_PRINTF_SPEC_64BITU[] = "%llu";
static const charn macn_PRINTF_SPEC_64BITX[] = "%llx";
#endif

//ASCII lookup table to speed up conversion
const uint8 mau8_ConvTable[256] =
{
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, //    0 .. 9
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   10U, 11U, 12U, 13U, 14U, 15U, //    A .. F
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   10U, 11U, 12U, 13U, 14U, 15U, //    a .. f
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU,
   0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU
};

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

void C_SCLString::m_ThrowIfOutOfRange(const sint32 os32_Index) const
{
   if ((os32_Index < 1) || (os32_Index > static_cast<sint32>(this->Length())))
   {
      throw "C_SCLString: index out of range";
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to "".

   \created     04.09.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(void) :
   c_String("")
{
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to opcn_InitValue.

   \param[in]  opcn_InitValue      pointer to zero-terminated initial string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const charn * const opcn_InitValue)
{
   c_String.operator = (opcn_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to opcn_InitValue.
   The function will do its best to make sense of the wchar_t array based on the configured LOCALE
   and stop conversion if it finds a character it can't convert.
   For details cf. documentation of wcstombs.

   \param[in]  opwcn_InitValue      pointer to zero-terminated initial string

   \created     26.05.2011  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const wchar_t * const opwcn_InitValue)
{
   charn * pcn_Chars;
   size_t un_Return;
   size_t un_Size = wcslen(opwcn_InitValue) + 1U;

   pcn_Chars = new charn[un_Size];
   un_Return = std::wcstombs(pcn_Chars, opwcn_InitValue, un_Size);

   if (un_Return == (static_cast<size_t>(-1)))
   {
      pcn_Chars[un_Size - 1] = '\0';
   }

   c_String.operator = (pcn_Chars);
   delete[] pcn_Chars;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to oc_InitValue.

   \param[in]  orc_InitValue      initial string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const C_SCLString & orc_InitValue)
{
   c_String.operator = (orc_InitValue.c_str());
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to opcn_InitValue. Number of bytes to use can be specified.

   \param[in]  opcn_InitValue   pointer to string data
   \param[in]  oun_Length       number of bytes from opcn_InitValue to use

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const charn * const opcn_InitValue, const uintn oun_Length)
{
   (void)c_String.assign(opcn_InitValue, oun_Length);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to contain one character.

   \param[in]  ocn_InitValue    initial character

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const charn ocn_InitValue)
{
   this->StringPrintFormatted("%c", ocn_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  os16_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const sint16 os16_InitValue)
{
   this->StringPrintFormatted("%d", os16_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  ou16_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const uint16 ou16_InitValue)
{
   this->StringPrintFormatted("%u", ou16_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  osn_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const sintn osn_InitValue)
{
   this->StringPrintFormatted("%d", osn_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  oun_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const uintn oun_InitValue)
{
   this->StringPrintFormatted("%u", oun_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  os32_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const sint32 os32_InitValue)
{
   this->StringPrintFormatted("%d", os32_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  ou32_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const uint32 ou32_InitValue)
{
   this->StringPrintFormatted("%u", ou32_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  os64_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const sint64 os64_InitValue)
{
   this->StringPrintFormatted(macn_PRINTF_SPEC_64BITS, os64_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of numeric value.

   \param[in]  ou64_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const uint64 ou64_InitValue)
{
   this->StringPrintFormatted(macn_PRINTF_SPEC_64BITU, ou64_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor.

   Initialize string data to string representation of float-value.
   The number of digits after the decimal separator is 6.

   Printf-specs on interpretation of "NAN" and "INF" are partially implementation dependent:    \n

   quote:    \n
   A double argument representing an infinity shall be converted in one of the styles
   "[-]inf" or "[-]infinity" ; which style is implementation-defined. A double argument
   representing a NaN shall be converted in one of the styles "[-]nan(n-char-sequence)" or
   "[-]nan" ; which style, and the meaning of any n-char-sequence, is implementation-defined.
   The F conversion specifier produces "INF", "INFINITY", or "NAN" instead of "inf", "infinity",
   or "nan", respectively.   \n
   /quote    \n

   So this function behaves a little different than VCL's AnsiString.
   Also it does not remove the trailing zeroes, like AnsiString does.

   \param[in]  of64_InitValue    numeric init value

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::C_SCLString(const float64 of64_InitValue)
{
   this->StringPrintFormatted("%f", of64_InitValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Destructor.

   Nothing to do explicitly.

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString::~C_SCLString(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief    Assignment operator.

   Assign string to instance's string data.

   \param[in]  orc_Source    string to assign

   \return
   reference to new string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::operator =(const C_SCLString & orc_Source)
{
   if (this != &orc_Source)
   {
      c_String.operator = (orc_Source.c_str());
   }
   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Append operator.

   Append string to instance's string data.

   \param[in]  orc_Source    string to append

   \return
   reference to new combined string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::operator +=(const C_SCLString & orc_Source)
{
   c_String.operator +=(orc_Source.c_str());
   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Add operator.

   Concatenate 2 strings and return combined strings.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   new combined string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString SCL_PACKAGE stw_scl::operator +(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   std::string c_Temp;
   (void)c_Temp.assign(orc_Par1.c_str());
   c_Temp.operator += (orc_Par2.c_str());
   return c_Temp.c_str();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Compare equals operator.

   Compare 2 strings.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true    -> both strings are identical   \n
   false   -> strings are not identical

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator ==(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return ((orc_Par1.AsStdString()->compare(orc_Par2.c_str())) == 0) ? true : false;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Compare not equals operator.

   Compare 2 strings.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true     -> strings are not identical    \n
   false    -> both strings are identical

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator !=(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return (orc_Par1 == orc_Par2) ? false : true;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Less than operator.

   Compare whether oc_Par1 is less than oc_Par2.
   This is done by comparing the ASCII codes of the individual characters.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true     -> orc_Par1 <  orc_Par2
   false    -> orc_Par1 >= orc_Par2

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator <(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return (orc_Par1.AsStdString()->compare(orc_Par2.c_str()) < 0) ? true : false;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Greater than operator.

   Compare whether oc_Par1 is greater than oc_Par2.
   This is done by comparing the ASCII codes of the individual characters.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true     -> orc_Par1 >  orc_Par2
   false    -> orc_Par1 <= orc_Par2

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator >(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return (orc_Par1.AsStdString()->compare(orc_Par2.c_str()) > 0) ? true : false;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Less equals operator.

   Compare whether oc_Par1 is less than oc_Par2.
   This is done by comparing the ASCII codes of the individual characters.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true     -> orc_Par1 <= orc_Par2
   false    -> orc_Par1 >  orc_Par2

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator <=(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return (orc_Par1 > orc_Par2) ? false : true;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Greater equals operator.

   Compare whether oc_Par1 is greater than or equal to oc_Par2.
   This is done by comparing the ASCII codes of the individual characters.

   \param[in]  orc_Par1    1st string
   \param[in]  orc_Par2    2nd string

   \return
   true     -> orc_Par1 >= orc_Par2
   false    -> orc_Par1 <  orc_Par2

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool SCL_PACKAGE stw_scl::operator >=(const C_SCLString & orc_Par1, const C_SCLString & orc_Par2)
{
   return (orc_Par1 < orc_Par2) ? false : true;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Compare string with other string

   Compare our data with data of a specified string.

   \param[in]  orc_Source    string to compare against

   \return
   0: strings are identical
   <0: "this" string is < than orc_Source
   >0: "this" string is > than orc_Source

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::AnsiCompare(const C_SCLString & orc_Source) const
{
   return (c_String.compare(orc_Source.c_str()));
}

//-----------------------------------------------------------------------------
/*!
   \brief    Compare string with other string and ignore case

   Compare our data with data of a specified string.
   Upper/Lower case will be ignored.

   \param[in]  orc_Source    string to compare against

   \return
   0: strings are identical
   <0: "this" string is < than orc_Source
   >0: "this" string is > than orc_Source

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::AnsiCompareIC(const C_SCLString & orc_Source) const
{
   C_SCLString c_Help;

   c_Help = this->UpperCase();
   return (c_Help.c_String.compare(orc_Source.UpperCase().c_str()));
}

//-----------------------------------------------------------------------------
/*!
   \brief    Compose a string consisting an array of defined characters

   Create a string consisting of "ou32_Count" characters of type "ocn_Char"

   \param[in]  ocn_Char     character to use for composing the string
   \param[in]  ou32_Count   number of times to add character to string

   \return
   String consisting of ou32_Count characters of type ocn_Char

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::StringOfChar(const charn ocn_Char, const uint32 ou32_Count)
{
   std::string c_StdString;
   C_SCLString c_SCLString;
   (void)c_StdString.assign(ou32_Count, ocn_Char);
   c_SCLString.operator = (c_StdString.c_str());
   return c_SCLString;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Utility: get number of required characters for "printf"

   Return number of required characters for "printfing".
   Used for calculating the required size of a dynamic buffer for performing the
    actual printing with m_SNPrintf or m_CatSNPrintf.
   The reason this is split up is that the call to "vsnprintf" can modify the opv_Args parameter.
   So we need another copy for the 2nd call. This copy could be created using the "va_copy" macro.
   Unfortunately it is not portable (available from C99 resp. C++11).

   \param[in]  opcn_Format   format string
   \param[in]  opv_Args      values to print into string

   \return
   number of characters required (without terminating zero)

   \created     05.12.2017  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::mh_GetRequiredPrintfSize(const charn * const opcn_Format, va_list opv_Args)
{
   return std::vsnprintf(NULL, 0U, opcn_Format, opv_Args);
} //lint !e952 //va_list can be const on some targets but not all

//---------------------------------------------------------------------------

void C_SCLString::m_SNPrintf(const stw_types::sintn osn_Size, const stw_types::charn * const opcn_Format,
                                          va_list opv_Args)
{
   charn * const pcn_Buffer = new charn[static_cast<uintn>(osn_Size) + 1U];
   (void)std::vsnprintf(pcn_Buffer, static_cast<uintn>(osn_Size) + 1U, opcn_Format, opv_Args);
   (void)c_String.assign(pcn_Buffer);
   delete[] pcn_Buffer;
} //lint !e952 //va_list can be const on some targets but not all

//---------------------------------------------------------------------------

void C_SCLString::m_CatSNPrintf(const stw_types::sintn osn_Size, const stw_types::charn * const opcn_Format,
                                          va_list opv_Args)
{
   charn * const pcn_Buffer = new charn[static_cast<uintn>(osn_Size) + 1U];
   (void)std::vsnprintf(pcn_Buffer, static_cast<uintn>(osn_Size) + 1U, opcn_Format, opv_Args);
   c_String.operator +=(pcn_Buffer);
   delete[] pcn_Buffer;
} //lint !e952 //va_list can be const on some targets but not all

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Alias for C_SCLString::PrintFormatted.
   Provided for compatibility with VCL AnsiString.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   number of characters in resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::printf(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   sintn sn_Length;
   va_list pv_Args;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_SNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return sn_Length;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Alias for C_SCLString::StringPrintFormatted.
   Provided for compatibility with VCL AnsiString.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::sprintf(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   va_list pv_Args;
   sintn sn_Length;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_SNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Print formatted data to "this" string.
   Regular C printf formatters are permitted.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   number of characters in resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::PrintFormatted(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   sintn sn_Length;
   va_list pv_Args;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_SNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return sn_Length;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Print formatted data to "this" string.
   Regular C printf formatters are permitted.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::StringPrintFormatted(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   va_list pv_Args;
   sintn sn_Length;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_SNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Print formatted data to "this" string.
   Text will be appended to existing string data.
   Regular C printf formatters are permitted.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   length of string after adding new text

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::cat_printf(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   sintn sn_Length;

   va_list pv_Args;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_CatSNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return sn_Length;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Print formatted text to string

   Print formatted data to "this" string.
   Text will be appended to existing string data.
   Regular C printf formatters are permitted.

   \param[in]  opcn_Format   format string
   \param[in]  ...           values to print into string

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::cat_sprintf(const charn * const opcn_Format, ...) //lint !e1960 !e1916
{
   va_list pv_Args;
   sintn sn_Length;

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   sn_Length = mh_GetRequiredPrintfSize(opcn_Format, pv_Args);
   va_end(pv_Args);

   va_start(pv_Args, opcn_Format); //lint !e970 !e1924 !e1773 !e925 !e928 !e1963 !e1960 !e826 //effects of using "..."
   m_CatSNPrintf(sn_Length, opcn_Format, pv_Args);
   va_end(pv_Args);

   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert integer to hexadecimal string

   Compose string from integer value data.
   Does not insert a "0x" prefix before the data.

   Example:
   "IntToHex(0x123, 4)" will return 0123.

   \param[in]  osn_Value    value to convert
   \param[in]  ou32_Digits  number of digits to return (zeroes will be filled in from the left)

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToHex(const sintn osn_Value, const uint32 ou32_Digits)
{
   C_SCLString c_HelperString;

   c_HelperString.PrintFormatted("%x", osn_Value);
   while (c_HelperString.Length() < ou32_Digits)
   {
      c_HelperString = static_cast<C_SCLString>("0") + c_HelperString;
   }
   return c_HelperString;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert integer to hexadecimal string

   Compose string from integer value data.
   Does not insert a "0x" prefix before the data.

   Example:
   "IntToHex(0x123, 4)" will return 0123.

   \param[in]  os64_Value   value to convert
   \param[in]  ou32_Digits  number of digits to return (zeroes will be filled in from the left)

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToHex(const sint64 os64_Value, const uint32 ou32_Digits)
{
   C_SCLString c_HelperString;

   c_HelperString.PrintFormatted(macn_PRINTF_SPEC_64BITX, os64_Value);
   while (c_HelperString.Length() < ou32_Digits)
   {
      c_HelperString = static_cast<C_SCLString>("0") + c_HelperString;
   }
   return c_HelperString;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Insert string into existing string data

   Insert string into existing string at specified position.

   \param[in]  orc_Source   string to insert
   \param[in]  ou32_Index   position to insert string at (1 = beginning of string)

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::Insert(const C_SCLString & orc_Source, const uint32 ou32_Index)
{
   uint32 u32_Index = ou32_Index;

   if (u32_Index == 0U) //be defensive. Assume user means beginning ...
   {
      u32_Index = 1U;
   }
   if (u32_Index > this->Length())
   {
      u32_Index = this->Length() + 1U;
   }
   (void)c_String.insert(static_cast<uintn>(u32_Index) - 1U, orc_Source.c_str());
   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Delete characters from existing string

   Delete characters from existing string.
   If the index is larger than the length of the string or less than 1, no characters are deleted.
   If the existing string is too small for the number of characters to erase the function
    will erase as many characters as possible.

   \param[in]  ou32_Index   start index of characters to remove (1 = beginning of string)
   \param[in]  ou32_Count   number of characters to remove

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::Delete(const uint32 ou32_Index, const uint32 ou32_Count)
{
   if ((ou32_Index > 0) && (ou32_Index <= this->Length()))
   {
      (void)c_String.erase(static_cast<uintn>(ou32_Index) - 1U, ou32_Count);
   }
   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Change length of string.

   Resize string.
   Pre-existing data will be preserved.
   If the new length is greater than the previous length '\0' characters will be inserted.

   \param[in]  ou32_NewLength   new length of string (in characters)

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString & C_SCLString::SetLength(const uint32 ou32_NewLength)
{
   c_String.resize(ou32_NewLength);
   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get first position of specified string in existing string

   Return first position of specified string in existing string data

   Example:
   Assume string contains "abcabc".
   object.LastPos("abc") will return 1.

   \param[in]  orc_SubString   string to find

   \return
   0: string not found
   >0: position of string in exsiting string (1 = beginning of string)

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_SCLString::Pos(const C_SCLString & orc_SubString) const
{
   sintn sn_Return;
   uint32 u32_Return;

   sn_Return = static_cast<sintn>(c_String.find(orc_SubString.c_str(), 0U));
   if (sn_Return >= 0)
   {
      u32_Return = static_cast<uint32>(sn_Return) + 1U;
   }
   else
   {
      u32_Return = 0U;
   }
   return u32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get last position of specified string in existing string

   Return last position of specified string in existing string data

   Example:
   Assume string contains "abcabc".
   object.LastPos("abc") will return 4.

   \param[in]  orc_SubString   string to find

   \return
   0: string not found
   >0: last position of string in exsiting string (1 = beginning of string)

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_SCLString::LastPos(const C_SCLString & orc_SubString) const
{
   sintn sn_Return;
   uint32 u32_Return;

   sn_Return = static_cast<sintn>(c_String.rfind(orc_SubString.c_str(), c_String.length()));
   if (sn_Return >= 0)
   {
      u32_Return = static_cast<uint32>(sn_Return) + 1U;
   }
   else
   {
      u32_Return = 0U;
   }
   return u32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Remove whitespaces around string

   Remove the following whitespaces around string data:
   - blank
   - tab
   - carriage return
   - new line
   - formfeed
   - vertical tab

   Will not touch whitespaces enveloped by other characters.

   Will not modify the existing object, just return the "trimmed" string.

   \return
   String with outer whitespaces removed.

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::Trim(void) const
{
   std::string c_Temp = c_String; //copy over so we can call ::erase
   c_Temp.erase(c_Temp.find_last_not_of(" \t\r\n\v\f") + 1);
   c_Temp.erase(0, c_Temp.find_first_not_of(" \t\r\n\v\f"));
   return c_Temp.c_str();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Remove whitespaces at the beginning of string

   Remove the following whitespaces at the beginning of string data:
   - blank
   - tab
   - carriage return
   - new line
   - formfeed
   - vertical tab

   Will not modify the existing object, just return the "trimmed" string.

   \return
   String with beginning whitespaces removed.

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::TrimLeft(void) const
{
   std::string::size_type un_Pos = c_String.find_first_not_of(" \t\r\n\v\f");

   if (un_Pos != std::string::npos)
   {
      return &(c_String.c_str()[un_Pos]); //return string from first non-whitespace on ...
   }

   return "";
}

//-----------------------------------------------------------------------------
/*!
   \brief    Remove whitespaces at the end of string

   Remove the following whitespaces at the end of string data:
   - blank
   - tab
   - carriage return
   - new line
   - formfeed
   - vertical tab

   Will not modify the existing object, just return the "trimmed" string.

   \return
   String with trailing whitespaces removed.

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::TrimRight(void) const
{
   std::string c_Return = c_String; //copy over so we can call ::erase
   c_Return.erase(this->c_String.find_last_not_of(" \t\r\n\v\f") + 1);
   return c_Return.c_str();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to lower case letters

   Convert all upper case letters to lower case letters.
   Will not modify the existing object, just return the resulting string.

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::LowerCase(void) const
{
   C_SCLString c_Result;
   uint32 u32_Index;
   uint32 u32_Length = this->Length(); //remember to improve performance
   const charn * pcn_Str;

   c_Result = (*this);
   pcn_Str = c_Result.c_str(); //remember to improve performance
   for (u32_Index = 0U; u32_Index < u32_Length; u32_Index++)
   {
      c_Result[static_cast<sintn>(u32_Index) + 1] = static_cast<charn>(std::tolower(pcn_Str[u32_Index]));
   }
   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to upper case letters

   Convert all lower case letters to upper case letters.
   Will not modify the existing object, just return the resulting string.

   \return
   Resulting string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::UpperCase(void) const
{
   C_SCLString c_Result;
   uint32 u32_Index;
   uint32 u32_Length = this->Length(); //remember to improve performance
   const charn * pcn_Str;

   c_Result = (*this);
   pcn_Str = c_Result.c_str(); //remember to improve performance
   for (u32_Index = 0U; u32_Index < u32_Length; u32_Index++)
   {
      c_Result[static_cast<sintn>(u32_Index) + 1] = static_cast<charn>(std::toupper(pcn_Str[u32_Index]));
   }
   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Extract part of the string

   Get a part of the string.
   If more characters are requested than the string contains only the available characters will be returned.

   \param[in]    ou32_Index    start index of text to extract (1 = beginning of string)
   \param[in]    ou32_Count    number of characters to extract

   \return
   Resulting string
   if index is zero: ""

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::SubString(const uint32 ou32_Index, const uint32 ou32_Count) const
{
   std::string c_Temp;
   if (ou32_Index == 0U)
   {
      c_Temp = "";
   }
   else
   {
      (void)c_Temp.assign(c_String, static_cast<uintn>(ou32_Index) - 1U, ou32_Count);
   }
   return c_Temp.c_str();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to sint32 number

   Convert string to sint32 values.
   Will throw exception if string is not a number.
   Accepted formats:
   - decimal positive value
   - decimal negative value
   - hexadecimal value ("0x..."); the "x" and a..f can be upper or lower case
   - hexadecimal negative value ("-0x..."); the "x" and a..f can be upper or lower case

   \return
   string as number

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::ToInt(void) const
{
   sint32 s32_Return = 0;
   uint32 u32_Length;
   uint8 u8_XIndex = 1U; //expected position of "x" or "X" to flag hex-value

   u32_Length = c_String.length();

   if (u32_Length == 0U)
   {
      throw ("C_SCLString::ToInt: string does not contain an integer !");
   }
   else
   {
      const charn * pcn_String;
      bool q_Hex = false;
      pcn_String = c_String.c_str();

      if (pcn_String[0] == '-')
      {
         u8_XIndex++;
      }

      if ((u32_Length > (static_cast<uint32>(u8_XIndex) + 1U)) &&
          ((pcn_String[u8_XIndex] == 'x') || (pcn_String[u8_XIndex] == 'X')))
      {
         //Hex: let pass ...
         q_Hex = true;
      }
      else
      {
         //do NOT interpret a leading "0" as octal (VCL AnsiString doesn't)
         while (((*pcn_String) == '0') && ((*(pcn_String + 1)) != '\0'))
         {
            pcn_String++;
         }
      }

      try
      {
         s32_Return = m_StrTos32(pcn_String, q_Hex);
      }
      catch (...)
      {
         throw ("C_SCLString::ToInt: string does not contain an integer !");
      }
   }
   return s32_Return;
}

//---------------------------------------------------------------------------

sint32 C_SCLString::m_StrTos32(const charn * const opcn_String, const bool oq_Hex)
{
   bool q_IsNegative = false;
   sint32 s32_CharVal;
   const charn * pcn_Act;
   sint32 s32_Result = 0;

   pcn_Act = opcn_String;

   if (*pcn_Act == '-')
   {
      q_IsNegative = true;
      pcn_Act++;
   }

   if (oq_Hex == true)
   {
      pcn_Act += 2; //skip "0x"
   }

   while ((*pcn_Act) != '\0')
   {
      s32_CharVal = mau8_ConvTable[static_cast<uint8>(*pcn_Act)];

      if (s32_CharVal == 0xFF)
      {
         throw "";
      }

      if (oq_Hex == true)
      {
         if (s32_CharVal > 0xF)
         {
            throw "";
         }
         s32_Result = (s32_Result * 16) + s32_CharVal;
      }
      else
      {
         if (s32_CharVal > 10)
         {
            throw "";
         }
         s32_Result = (s32_Result * 10) + s32_CharVal;
      }
      pcn_Act++;
   }

   if (q_IsNegative == true)
   {
      s32_Result = -s32_Result;
   }

   return s32_Result;
}

//---------------------------------------------------------------------------

sint64 C_SCLString::m_StrTos64(const charn * const opcn_String, const bool oq_Hex)
{
   bool q_IsNegative = false;
   sint32 s32_CharVal;
   const charn * pcn_Act;
   sint64 s64_Result = 0;

   pcn_Act = opcn_String;

   if (*pcn_Act == '-')
   {
      q_IsNegative = true;
      pcn_Act++;
   }

   if (oq_Hex == true)
   {
      pcn_Act += 2; //skip "0x"
   }

   while ((*pcn_Act) != '\0')
   {
      s32_CharVal = mau8_ConvTable[static_cast<uint8>(*pcn_Act)];

      if (s32_CharVal == 0xFF)
      {
         throw "";
      }

      if (oq_Hex == true)
      {
         if (s32_CharVal > 0xF)
         {
            throw "";
         }
         s64_Result = (s64_Result * 16) + s32_CharVal;
      }
      else
      {
         if (s32_CharVal > 10)
         {
            throw "";
         }
         s64_Result = (s64_Result * 10) + s32_CharVal;
      }
      pcn_Act++;
   }

   if (q_IsNegative == true)
   {
      s64_Result = -s64_Result;
   }

   return s64_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to sint64 number

   Convert string to sint64 values.
   Will throw exception if string is not a number.
   Accepted formats:
   - decimal positive value
   - decimal negative value
   - hexadecimal value ("0x..."); the "x" and a..f can be upper or lower case
   - hexadecimal negative value ("-0x..."); the "x" and a..f can be upper or lower case

   \return
   string as number

   \created     01.09.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint64 C_SCLString::ToInt64(void) const
{
   sint64 s64_Return = 0;
   uint32 u32_Length;
   uint8 u8_XIndex = 1U; //expected position of "x" or "X" to flag hex-value

   u32_Length = c_String.length();

   if (u32_Length == 0U)
   {
      throw ("C_SCLString::ToInt64: string does not contain an integer !");
   }
   else
   {
      const charn * pcn_String;
      bool q_Hex = false;
      pcn_String = c_String.c_str();

      if (pcn_String[0] == '-')
      {
         u8_XIndex++;
      }

      if ((u32_Length > (static_cast<uint32>(u8_XIndex) + 1U)) &&
          ((pcn_String[u8_XIndex] == 'x') || (pcn_String[u8_XIndex] == 'X')))
      {
         //Hex: let pass ...
         q_Hex = true;
      }
      else
      {
         //do NOT interpret a leading "0" as octal (VCL AnsiString doesn't)
         while (((*pcn_String) == '0') && ((*(pcn_String + 1)) != '\0'))
         {
            pcn_String++;
         }
      }

      try
      {
         s64_Return = m_StrTos64(pcn_String, q_Hex);
      }
      catch (...)
      {
         throw ("C_SCLString::ToInt64: string does not contain an integer !");
      }
   }
   return s64_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to sintn number. Apply default if conversion fails.

   Try to convert string to sintn value.
   If the conversion fails return a specified default instead.

   Accepted formats:
   - decimal positive value
   - decimal negative value
   - hexadecimal value ("0x..."); the "x" and a..f can be upper or lower case
   - hexadecimal negative value ("-0x..."); the "x" and a..f can be upper or lower case

   \param[in]  osn_Default   value to return if conversion fails

   \return
   string as number (or default)

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sintn C_SCLString::ToIntDef(const sintn osn_Default) const
{
   sint32 s32_Return;

   try
   {
      s32_Return = ToInt();
   }
   catch (...)
   {
      s32_Return = osn_Default;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert string to float64 number

   Try to convert string to a float64 value.
   "," and "." separators are accepted.

   If the conversion fails the functions throws.

   \return
   string as float64

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
float64 C_SCLString::ToDouble(void) const
{
   float64 f64_Return;
   std::stringstream c_Stream;
   c_Stream.imbue(std::locale::classic()); //use "C" locale: "." is the decimal separator ...

   //replace up to one "," by "."
   if (this->Pos(",") != 0U)
   {
      C_SCLString c_Help = (*this);
      c_Help[c_Help.Pos(",")] = '.';
      c_Stream << c_Help.c_String;
   }
   else
   {
      c_Stream << c_String.c_str();
   }
   c_Stream >> f64_Return;
   if (c_Stream.fail() != 0)
   {
      throw ("C_SCLString::ToDouble: string does not contain a double value !");
   }

   return f64_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get character at position.

   Return character at specified position.
   If the position is invalid the functions throws.

   \param[in]  osn_Index   index of characters (1 = beginning of string)

   \return
   character at position

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
charn C_SCLString::operator [](const sintn osn_Index) const
{
   m_ThrowIfOutOfRange(osn_Index);
   return c_String.operator [] (static_cast<uintn>(osn_Index) - 1U);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get const character at position.

   Return character at specified position.
   If the position is invalid the functions throws.

   \param[in]  osn_Index   index of characters (1 = beginning of string)

   \return
   character at position

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
charn & C_SCLString::operator [](const sintn osn_Index)
{
   m_ThrowIfOutOfRange(osn_Index);
   return c_String.operator [] (static_cast<uintn>(osn_Index) - 1U);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get string data as std::string

   Returns the std::string encapsulated by C_SCLString.

   \return
   String data as std::string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
std::string * C_SCLString::AsStdString(void)
{
   return &c_String;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get string data as const std::string

   Returns the std::string encapsulated by C_SCLString.

   \return
   String data as std::string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
const std::string * C_SCLString::AsStdString(void) const
{
   return &c_String;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert sint64 value to string

   Converts sint64 number to string.

   \param[in]  os64_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const sint64 os64_Value)
{
   C_SCLString c_Text(os64_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert uint64 value to string

   Converts uint64 number to string.

   \param[in]  ou64_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const uint64 ou64_Value)
{
   C_SCLString c_Text(ou64_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert sint32 value to string

   Converts sint32 number to string.

   \param[in]  os32_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const sint32 os32_Value)
{
   C_SCLString c_Text(os32_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert uint32 value to string

   Converts uint32 number to string.

   \param[in]  ou32_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const uint32 ou32_Value)
{
   C_SCLString c_Text(ou32_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert sintn value to string

   Converts sintn number to string.

   \param[in]  osn_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const sintn osn_Value)
{
   C_SCLString c_Text(osn_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert uintn value to string

   Converts uintn number to string.

   \param[in]  oun_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::IntToStr(const uintn oun_Value)
{
   C_SCLString c_Text(oun_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert float32 value to string

   Converts float32 number to string.
   Will return 6 digits after the decimal separator.

   \param[in]  of32_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::FloatToStr(const float32 of32_Value)
{
   C_SCLString c_Text(static_cast<float64>(of32_Value));

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert of64_Value value to string

   Converts of64_Value number to string.
   Will return 6 digits after the decimal separator.

   \param[in]  of64_Value   value to convert

   \return
   value converted to string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::FloatToStr(const float64 of64_Value)
{
   C_SCLString c_Text(of64_Value);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get pointer to raw string data

   Return pointer to ram string data.

   \return
   pointer to data

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
const charn * C_SCLString::c_str(void) const
{
   return c_String.c_str();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get pointer to raw string data

   Return pointer to ram string data.

   \return
   pointer to data

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
const void * C_SCLString::data(void) const
{
   return c_String.data();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get number of characters in string

   Returns the number of characters in the string (i.e.: number of characters before '\0').

   \return  number of characters in string

   \created     xx.xx.20xx  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_SCLString::Length(void) const
{
   return static_cast<uint32>(c_String.length());
}

//-----------------------------------------------------------------------------
/*!
   \brief    Check whether string contains any characters

   \return
   true: no characters in string
   false: more than zero characters in string

   \created     16.04.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool C_SCLString::IsEmpty(void) const
{
   return c_String.empty();
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get index of rightmost character that contains any of the delimiter characters.

   \param[in]    orc_Delimiters   string containing all possible delimiters

   \return
   1-based rightmost position that contains any of the characters on orc_Delimiters (0 if not found)

   \created     19.05.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_SCLString::LastDelimiter(const C_SCLString & orc_Delimiters) const
{
   sint32 s32_Pos;
   uint32 u32_Return = 0U;
   uint32 u32_Length = this->Length();

   if (u32_Length > 0U)
   {
      for (s32_Pos = (static_cast<sint32>(u32_Length) - 1); s32_Pos >= 0; s32_Pos--)
      {
         if (orc_Delimiters.Pos(c_String.operator [](static_cast<uint32>(s32_Pos))) != 0U)
         {
            u32_Return = static_cast<uint32>(s32_Pos) + 1U;
            break;
         }
      }
   }
   return u32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Convert float64 to a C_SCLString with a configurable number of digits.

   Will always use the point (".") as decimal separator.

   \param[in]    of64_Value   values to convert
   \param[in]    os32_Digits  number of digits to add to the string after the decimal point

   \return
   resulting string

   \created     08.06.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLString::FloatToStr(const float64 of64_Value, const sint32 os32_Digits)
{
   C_SCLString c_Help;

   c_Help.StringPrintFormatted("%.*f", os32_Digits, of64_Value);
   return c_Help;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Tokenize string.

   Parses string data for token-separated fields.
   The parsed data will be returned.
   Will throw if out of memory.
   Can not handle masked (e.g. escaped) delimiters within fields.

   \param[in]    orc_Delimiters       token delimiters (e.g. ";.-" or simply ";")
   \param[out]   orc_TokenizedData    array containing parsed tokens

   \created     08.06.2010  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_SCLString::Tokenize(const C_SCLString & orc_Delimiters, SCLDynamicArray<C_SCLString> & orc_TokenizedData) const
{
   C_SCLString c_Text;
   const std::string * pc_String;

   std::string::size_type un_DelimPos;
   std::string::size_type un_TokenPos;
   std::string::size_type un_Pos = 0U;

   pc_String = this->AsStdString();
   orc_TokenizedData.SetLength(0);

   while (true) //lint !e716  //no problem; we have a clearly defined termination condition
   {
      un_DelimPos = pc_String->find_first_of(orc_Delimiters.c_str(), un_Pos);
      un_TokenPos = pc_String->find_first_not_of(orc_Delimiters.c_str(), un_Pos);

      if (un_DelimPos == un_TokenPos)
      {
         //trailing additional delimiter: no more information for us
         break;
      }

      orc_TokenizedData.IncLength(); //might throw bad_alloc
      if (un_DelimPos != std::string::npos)
      {
         if (un_TokenPos != std::string::npos)
         {
            if (un_TokenPos < un_DelimPos)
            {
               c_Text = pc_String->substr(un_Pos, un_DelimPos - un_Pos).c_str();
            }
            else
            {
               c_Text = "";
            }
         }
         else
         {
            c_Text = "";
         }
         orc_TokenizedData[orc_TokenizedData.GetHigh()] = c_Text;
         un_Pos = un_DelimPos + 1U;
      }
      else
      {
         if (un_TokenPos != std::string::npos)
         {
            c_Text = pc_String->substr(un_Pos).c_str();
         }
         else
         {
            c_Text = "";
         }
         orc_TokenizedData[orc_TokenizedData.GetHigh()] = c_Text;
         break; //lint !e1960  //refactoring the code could cause more problems than it solved; no problems known
      }
   }
}