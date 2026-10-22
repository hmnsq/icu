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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/casemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucasemap_imp.h"
#ଅନ୍ତର୍ଭାଵ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             ଅଚଳ char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) ଅଚଳ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  ଯଦି(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ଅଚଳ char16_t *chars = getArrayStart();

  chars += start;
  ଯଦି(srcStart!=0) {
    srcChars += srcStart;
  }

  ଯଦି(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    ଯଦି(result!=0) {
      ଫେରନ୍ତୁ static_cast<int8_t>(result >> 24 | 1);
    }
  } ଅନ୍ୟଥା {
    // get the srcLength if necessary
    ଯଦି(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    ଯଦି(length != srcLength) {
      ଫେରନ୍ତୁ static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  ଫେରନ୍ତୁ 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  ଯଦି(isEmpty() || !isWritable()) {
    // nothing to do
    ଫେରନ୍ତୁ *ଏହି;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#ଯଦି !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  ଯଦି (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    ଯଦି (writable) {
      capacity = getCapacity();
    } ଅନ୍ୟଥା {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      ଯଦି (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ ମିଥ୍ୟା)) {
        ଫେରନ୍ତୁ *ଏହି;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_ଗଦା_Buffer);
      buffer = fUnion.f_ଗଦା_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    ଯଦି (iter != nullptr) {
      oldString.setTo(ମିଥ୍ୟା, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    ଯଦି (U_SUCCESS(errorCode)) {
      setLength(newLength);
      ଫେରନ୍ତୁ *ଏହି;
    } ଅନ୍ୟଥା ଯଦି (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } ଅନ୍ୟଥା {
      setToBogus();
      ଫେରନ୍ତୁ *ଏହି;
    }
  } ଅନ୍ୟଥା {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    ଯଦି (iter != nullptr) {
      oldString.setTo(ମିଥ୍ୟା, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    ଯଦି (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      ଯଦି (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        ଫେରନ୍ତୁ *ଏହି;
      }
      ପାଇଁ (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      ଯଦି (U_FAILURE(errorCode)) {
        setToBogus();
      }
      ଫେରନ୍ତୁ *ଏହି;
    } ଅନ୍ୟଥା ଯଦି (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } ଅନ୍ୟଥା {
      setToBogus();
      ଫେରନ୍ତୁ *ଏହି;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  ଯଦି (!cloneArrayIfNeeded(newLength, newLength, ମିଥ୍ୟା, &bufferToDelete, ସତ୍ୟ)) {
    ଫେରନ୍ତୁ *ଏହି;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  ଯଦି (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  ଯଦି (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } ଅନ୍ୟଥା {
    setToBogus();
  }
  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  ଫେରନ୍ତୁ caseMap(UCASE_LOC_ମୂଳ, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ଅଚଳ UElement ଚାବି) {
    U_NAMESPACE_USE
    ଅଚଳ UnicodeString *str = (ଅଚଳ UnicodeString*) ଚାବି.pointer;
    ଯଦି (str == nullptr) {
        ଫେରନ୍ତୁ 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    ଫେରନ୍ତୁ copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ଅଚଳ UElement key1, ଅଚଳ UElement key2) {
    U_NAMESPACE_USE
    ଅଚଳ UnicodeString *str1 = (ଅଚଳ UnicodeString*) key1.pointer;
    ଅଚଳ UnicodeString *str2 = (ଅଚଳ UnicodeString*) key2.pointer;
    ଯଦି (str1 == str2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (str1 == nullptr || str2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
