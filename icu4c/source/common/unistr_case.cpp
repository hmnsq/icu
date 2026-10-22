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

#شامل "unicode/utypes.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/edits.h"
#شامل "unicode/putil.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uchar.h"
#شامل "uassert.h"
#شامل "ucasemap_imp.h"
#شامل "uelement.h"

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
  اگر(isBogus()) {
    بازگشت -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  اگر(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  ثابت char16_t *chars = getArrayStart();

  chars += start;
  اگر(srcStart!=0) {
    srcChars += srcStart;
  }

  اگر(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    اگر(result!=0) {
      بازگشت static_cast<int8_t>(result >> 24 | 1);
    }
  } وگرنه {
    // get the srcLength if necessary
    اگر(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    اگر(length != srcLength) {
      بازگشت static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  بازگشت 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  اگر(isEmpty() || !isWritable()) {
    // nothing to do
    بازگشت *این;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#اگر !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  اگر (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    اگر (writable) {
      capacity = getCapacity();
    } وگرنه {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      اگر (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ نادرست)) {
        بازگشت *این;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_پشته_Buffer);
      buffer = fUnion.f_پشته_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#اگر !UCONFIG_NO_BREAK_ITERATION
    اگر (iter != nullptr) {
      oldString.setTo(نادرست, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    اگر (U_SUCCESS(errorCode)) {
      setLength(newLength);
      بازگشت *این;
    } وگرنه اگر (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } وگرنه {
      setToBogus();
      بازگشت *این;
    }
  } وگرنه {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#اگر !UCONFIG_NO_BREAK_ITERATION
    اگر (iter != nullptr) {
      oldString.setTo(نادرست, oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    اگر (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      اگر (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        بازگشت *این;
      }
      برای (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      اگر (U_FAILURE(errorCode)) {
        setToBogus();
      }
      بازگشت *این;
    } وگرنه اگر (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } وگرنه {
      setToBogus();
      بازگشت *این;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  اگر (!cloneArrayIfNeeded(newLength, newLength, نادرست, &bufferToDelete, درست)) {
    بازگشت *این;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  اگر (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  اگر (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } وگرنه {
    setToBogus();
  }
  بازگشت *این;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  بازگشت caseMap(UCASE_LOC_ریشه, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(ثابت UElement کلید) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.pointer;
    اگر (str == nullptr) {
        بازگشت 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    بازگشت copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    U_NAMESPACE_USE
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.pointer;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.pointer;
    اگر (str1 == str2) {
        بازگشت درست;
    }
    اگر (str1 == nullptr || str2 == nullptr) {
        بازگشت نادرست;
    }
    بازگشت str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
