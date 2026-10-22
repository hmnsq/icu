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

#include "unicode/utypes.h"
#include "unicode/brkiter.h"
#include "unicode/casemap.h"
#include "unicode/edits.h"
#include "unicode/putil.h"
#include "cstring.h"
#include "cmemory.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/uchar.h"
#include "uassert.h"
#include "ucasemap_imp.h"
#include "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             const char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) const
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ᰀᰧ(isBogus()) {
    ᰀᰧᰌ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  ᰀᰧ(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  const char16_t *chars = getArrayStart();

  chars += start;
  ᰀᰧ(srcStart!=0) {
    srcChars += srcStart;
  }

  ᰀᰧ(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    ᰀᰧ(result!=0) {
      ᰀᰧᰌ static_cast<int8_t>(result >> 24 | 1);
    }
  } ᰀᰧᰀ {
    // get the srcLength if necessary
    ᰀᰧ(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    ᰀᰧ(length != srcLength) {
      ᰀᰧᰌ static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  ᰀᰧᰌ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ᰀᰧ(isEmpty() || !isWritable()) {
    // nothing to do
    ᰀᰧᰌ *this;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  ᰀᰧ (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    ᰀᰧ (writable) {
      capacity = getCapacity();
    } ᰀᰧᰀ {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ᰀᰧ (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ false)) {
        ᰀᰧᰌ *this;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & kUsingStackBuffer);
      buffer = fUnion.fStackFields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION
    ᰀᰧ (iter != nullptr) {
      oldString.setTo(false, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    ᰀᰧ (U_SUCCESS(errorCode)) {
      setLength(newLength);
      ᰀᰧᰌ *this;
    } ᰀᰧᰀ ᰀᰧ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } ᰀᰧᰀ {
      setToBogus();
      ᰀᰧᰌ *this;
    }
  } ᰀᰧᰀ {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION
    ᰀᰧ (iter != nullptr) {
      oldString.setTo(false, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    ᰀᰧ (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      ᰀᰧ (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        ᰀᰧᰌ *this;
      }
      for (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      ᰀᰧ (U_FAILURE(errorCode)) {
        setToBogus();
      }
      ᰀᰧᰌ *this;
    } ᰀᰧᰀ ᰀᰧ (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } ᰀᰧᰀ {
      setToBogus();
      ᰀᰧᰌ *this;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  ᰀᰧ (!cloneArrayIfNeeded(newLength, newLength, false, &bufferToDelete, true)) {
    ᰀᰧᰌ *this;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  ᰀᰧ (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  ᰀᰧ (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } ᰀᰧᰀ {
    setToBogus();
  }
  ᰀᰧᰌ *this;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ᰀᰧᰌ caseMap(UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(const UElement key) {
    U_NAMESPACE_USE
    const UnicodeString *str = (const UnicodeString*) key.pointer;
    ᰀᰧ (str == nullptr) {
        ᰀᰧᰌ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ᰀᰧᰌ copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(const UElement key1, const UElement key2) {
    U_NAMESPACE_USE
    const UnicodeString *str1 = (const UnicodeString*) key1.pointer;
    const UnicodeString *str2 = (const UnicodeString*) key2.pointer;
    ᰀᰧ (str1 == str2) {
        ᰀᰧᰌ true;
    }
    ᰀᰧ (str1 == nullptr || str2 == nullptr) {
        ᰀᰧᰌ false;
    }
    ᰀᰧᰌ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
