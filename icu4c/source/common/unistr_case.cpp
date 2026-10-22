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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucasemap_imp.h"
#அந்தர்பாவ "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             மாறிலி char16_t *srcChars,
                             int32_t srcStart,
                             int32_t srcLength,
                             uint32_t options) மாறிலி
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  எனில்(isBogus()) {
    திரும்பு -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  எனில்(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  // get the correct pointer
  மாறிலி char16_t *chars = getArrayStart();

  chars += start;
  எனில்(srcStart!=0) {
    srcChars += srcStart;
  }

  எனில்(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t result=u_strcmpFold(chars, length, srcChars, srcLength,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    எனில்(result!=0) {
      திரும்பு static_cast<int8_t>(result >> 24 | 1);
    }
  } இல்லையெனில் {
    // get the srcLength if necessary
    எனில்(srcLength < 0) {
      srcLength = u_strlen(srcChars + srcStart);
    }
    எனில்(length != srcLength) {
      திரும்பு static_cast<int8_t>((length - srcLength) >> 24 | 1);
    }
  }
  திரும்பு 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  எனில்(isEmpty() || !isWritable()) {
    // nothing to do
    திரும்பு *இது;
  }

  char16_t oldBuffer[2 * US_STACKBUF_SIZE];
  char16_t *oldArray;
  int32_t oldLength = length();
  int32_t newLength;
  UBool writable = isBufferWritable();
  UErrorCode errorCode = U_ZERO_ERROR;

#எனில் !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#endif

  // Try to avoid heap-allocating a new character array for this string.
  எனில் (writable ? oldLength <= UPRV_LENGTHOF(oldBuffer) : oldLength < US_STACKBUF_SIZE) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *buffer = getArrayStart();
    int32_t capacity;
    oldArray = oldBuffer;
    u_memcpy(oldBuffer, buffer, oldLength);
    எனில் (writable) {
      capacity = getCapacity();
    } இல்லையெனில் {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      எனில் (!cloneArrayIfNeeded(US_STACKBUF_SIZE, US_STACKBUF_SIZE, /* doCopyArray= */ பொய்)) {
        திரும்பு *இது;
      }
      U_ASSERT(fUnion.fFields.fLengthAndFlags & k_Using_அடுக்கு_Buffer);
      buffer = fUnion.f_அடுக்கு_Fields.fBuffer;
      capacity = US_STACKBUF_SIZE;
    }
#எனில் !UCONFIG_NO_BREAK_ITERATION
    எனில் (iter != nullptr) {
      oldString.setTo(பொய், oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 buffer, capacity,
                                 oldArray, oldLength, nullptr, errorCode);
    எனில் (U_SUCCESS(errorCode)) {
      setLength(newLength);
      திரும்பு *இது;
    } இல்லையெனில் எனில் (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
    } இல்லையெனில் {
      setToBogus();
      திரும்பு *இது;
    }
  } இல்லையெனில் {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#எனில் !UCONFIG_NO_BREAK_ITERATION
    எனில் (iter != nullptr) {
      oldString.setTo(பொய், oldArray, oldLength);
      iter->setText(oldString);
    }
#endif
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, oldLength, &edits, errorCode);
    எனில் (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      newLength = oldLength + edits.lengthDelta();
      எனில் (newLength > oldLength && !cloneArrayIfNeeded(newLength, newLength)) {
        திரும்பு *இது;
      }
      க்கு (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destinationIndex(), ei.oldLength(),
                  replacementChars, ei.replacementIndex(), ei.newLength());
      }
      எனில் (U_FAILURE(errorCode)) {
        setToBogus();
      }
      திரும்பு *இது;
    } இல்லையெனில் எனில் (errorCode == U_BUFFER_OVERFLOW_ERROR) {
      // common overflow handling below
      newLength = oldLength + edits.lengthDelta();
    } இல்லையெனில் {
      setToBogus();
      திரும்பு *இது;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *bufferToDelete = nullptr;
  எனில் (!cloneArrayIfNeeded(newLength, newLength, பொய், &bufferToDelete, உண்மை)) {
    திரும்பு *இது;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  newLength = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, oldLength, nullptr, errorCode);
  எனில் (bufferToDelete) {
    uprv_free(bufferToDelete);
  }
  எனில் (U_SUCCESS(errorCode)) {
    setLength(newLength);
  } இல்லையெனில் {
    setToBogus();
  }
  திரும்பு *இது;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  திரும்பு caseMap(UCASE_LOC_வேர், options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(மாறிலி UElement சாவி) {
    U_NAMESPACE_USE
    மாறிலி UnicodeString *str = (மாறிலி UnicodeString*) சாவி.pointer;
    எனில் (str == nullptr) {
        திரும்பு 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    திரும்பு copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(மாறிலி UElement key1, மாறிலி UElement key2) {
    U_NAMESPACE_USE
    மாறிலி UnicodeString *str1 = (மாறிலி UnicodeString*) key1.pointer;
    மாறிலி UnicodeString *str2 = (மாறிலி UnicodeString*) key2.pointer;
    எனில் (str1 == str2) {
        திரும்பு உண்மை;
    }
    எனில் (str1 == nullptr || str2 == nullptr) {
        திரும்பு பொய்;
    }
    திரும்பு str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
