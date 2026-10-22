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
  nếu(isBogus()) {
    trả_về -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  nếu(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  const char16_t *chars = getArrayStart();

  chars += start;
  nếu(srcStart!=0) {
    srcChars += srcStart;
  }

  nếu(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    nếu(result!=0) {
      trả_về static_cast<int8_t>(result >> 24 | 1);
    }
  } không_thì {
    // get the srcLength if necessary
    nếu(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    nếu(length != srcLength) {
      trả_về static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  trả_về 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  nếu(isEmpty() || !isWritable()) {
    // nothing to do
    trả_về *this;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#nếu !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  nếu (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    nếu (writable) {
      capacity = getCapacity();
    } không_thì {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      nếu (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ false)) {
        trả_về *this;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & kUsingStackBuffer);
      buffer = fUnion.fStackFields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#nếu !UCONFIG_NO_BREAK_ITERATION
    nếu (iter != nullptr) {
      oldString.setTo(false, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    nếu (U_SUCCESS(errorCode)) {
      setLength(newLength);
      trả_về *this;
    } không_thì nếu (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } không_thì {
      setToBogus();
      trả_về *this;
    }
  } không_thì {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#nếu !UCONFIG_NO_BREAK_ITERATION
    nếu (iter != nullptr) {
      oldString.setTo(false, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    nếu (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      nếu (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        trả_về *this;
      }
      cho (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      nếu (U_FAILURE(errorCode)) {
        setToBogus();
      }
      trả_về *this;
    } không_thì nếu (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } không_thì {
      setToBogus();
      trả_về *this;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  nếu (!cloneArrayIfNeeded(newLength, newLength, false, &bufferToDelete, true)) {
    trả_về *this;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  nếu (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  nếu (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } không_thì {
    setToBogus();
  }
  trả_về *this;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  trả_về caseMap(UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(const UElement key) {
    U_NAMESPACE_USE
    const UnicodeString *str = (const UnicodeString*) key.pointer;
    nếu (str == nullptr) {
        trả_về 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    trả_về copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(const UElement key1, const UElement key2) {
    U_NAMESPACE_USE
    const UnicodeString *str1 = (const UnicodeString*) key1.pointer;
    const UnicodeString *str2 = (const UnicodeString*) key2.pointer;
    nếu (str1 == str2) {
        trả_về true;
    }
    nếu (str1 == nullptr || str2 == nullptr) {
        trả_về false;
    }
    trả_về str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
