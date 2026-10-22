// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistr_case.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004aug19
*   created by: Markus W. Scherer
*
*   Case-mapping functions moved here from unistr.cpp
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/putil.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uchar.h"
#ضمن "uassert.h"
#ضمن "ucasemap_imp.h"
#ضمن "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ثابت char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  إذا(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = getArrayStart();

  chars += start;
  إذا(srcStart!=0) {
    srcChars += srcStart;
  }

  إذا(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    إذا(result!=0) {
      أرجع static_cast<int8_t>(result >> 24 | 1);
    }
  } وإلا {
    // get the srcLength if necessary
    إذا(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    إذا(length != srcLength) {
      أرجع static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  أرجع 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  إذا(isEmpty() || !isWritable()) {
    // nothing to do
    أرجع *هذا;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#إذا !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  إذا (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    إذا (writable) {
      capacity = getCapacity();
    } وإلا {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      إذا (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ خطأ)) {
        أرجع *هذا;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_تر_Buffer);
      buffer = fUnion.f_تر_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != nullptr) {
      oldString.setTo(خطأ, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    إذا (U_SUCCESS(errorCode)) {
      setLength(newLength);
      أرجع *هذا;
    } وإلا إذا (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } وإلا {
      setToBogus();
      أرجع *هذا;
    }
  } وإلا {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#إذا !UCONFIG_NO_BREAK_ITERATION
    إذا (iter != nullptr) {
      oldString.setTo(خطأ, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    إذا (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      إذا (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        أرجع *هذا;
      }
      لكل (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      إذا (U_FAILURE(errorCode)) {
        setToBogus();
      }
      أرجع *هذا;
    } وإلا إذا (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } وإلا {
      setToBogus();
      أرجع *هذا;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  إذا (!cloneArrayIfNeeded(newLength, newLength, خطأ, &bufferToDelete, true)) {
    أرجع *هذا;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  إذا (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  إذا (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } وإلا {
    setToBogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  أرجع caseMap(UCASE_LOC_سيو, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement مبودي) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) مبودي.pointer;
    إذا (str == nullptr) {
        أرجع 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    أرجع copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.pointer;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.pointer;
    إذا (str1 == str2) {
        أرجع true;
    }
    إذا (str1 == nullptr || str2 == nullptr) {
        أرجع خطأ;
    }
    أرجع str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
