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

#समावेश "unicode/utypes.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/edits.h"
#समावेश "unicode/putil.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/uchar.h"
#समावेश "uassert.h"
#समावेश "ucasemap_imp.h"
#समावेश "uelement.h"

U_NAMESPACE_BEGIN

//========================================
// Read-only implementation
//========================================

int8_t
UnicodeString::doCaseCompare(int32_t start,
                             int32_t length,
                             स्थिर char16_t *srcChars,
                             int32_t srcStart,
                             int32_t src_लंबाई,
                             uint32_t options) स्थिर
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  अगर(isBogus()) {
    वापस -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  अगर(srcChars == nullptr) {
    srcStart = src_लंबाई = 0;
  }

  // get the correct pointer
  स्थिर char16_t *chars = getArrayStart();

  chars += start;
  अगर(srcStart!=0) {
    srcChars += srcStart;
  }

  अगर(chars != srcChars) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t परिणाम=u_strcmpFold(chars, length, srcChars, src_लंबाई,
                                options|U_COMPARE_IGNORE_CASE, &errorCode);
    अगर(परिणाम!=0) {
      वापस static_cast<int8_t>(परिणाम >> 24 | 1);
    }
  } अन्यथा {
    // get the srcLength if necessary
    अगर(src_लंबाई < 0) {
      src_लंबाई = u_strlen(srcChars + srcStart);
    }
    अगर(length != src_लंबाई) {
      वापस static_cast<int8_t>((length - src_लंबाई) >> 24 | 1);
    }
  }
  वापस 0;
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::caseMap(
    int32_t caseLocale,
    uint32_t options,
    UCASEMAP_BREAK_ITERATOR_PARAM UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND {
  अगर(isEmpty() || !isWritable()) {
    // nothing to do
    वापस *यह;
  }

  char16_t old_बफर[2 * US_STACKBUF_आकार];
  char16_t *oldArray;
  int32_t old_लंबाई = length();
  int32_t new_लंबाई;
  UBool writable = is_बफर_Writable();
  UErrorCode errorCode = U_ZERO_ERROR;

#अगर !UCONFIG_NO_BREAK_ITERATION
  // Read-only alias to the original string contents for the titlecasing BreakIterator.
  // We cannot set the iterator simply to *this because *this is being modified.
  UnicodeString oldString;
#पूर्ण_अगर

  // Try to avoid heap-allocating a new character array for this string.
  अगर (writable ? old_लंबाई <= UPRV_LENGTHOF(old_बफर) : old_लंबाई < US_STACKBUF_आकार) {
    // Short string: Copy the contents into a temporary buffer and
    // case-map back into the current array, or into the stack buffer.
    char16_t *बफर = getArrayStart();
    int32_t capacity;
    oldArray = old_बफर;
    u_memcpy(old_बफर, बफर, old_लंबाई);
    अगर (writable) {
      capacity = getCapacity();
    } अन्यथा {
      // Switch from the read-only alias or shared heap buffer to the stack buffer.
      अगर (!cloneArrayIfNeeded(US_STACKBUF_आकार, US_STACKBUF_आकार, /* doCopyArray= */ false)) {
        वापस *यह;
      }
      U_ASSERT(fUnion.fFields.f_लंबाई_And_Flags & k_Using_ढेर_बफर);
      बफर = fUnion.f_ढेर_Fields.f_बफर;
      capacity = US_STACKBUF_आकार;
    }
#अगर !UCONFIG_NO_BREAK_ITERATION
    अगर (iter != nullptr) {
      oldString.setTo(false, oldArray, old_लंबाई);
      iter->setText(oldString);
    }
#पूर्ण_अगर
    new_लंबाई = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                 बफर, capacity,
                                 oldArray, old_लंबाई, nullptr, errorCode);
    अगर (U_SUCCESS(errorCode)) {
      set_लंबाई(new_लंबाई);
      वापस *यह;
    } अन्यथा अगर (errorCode == U_बफर_OVERFLOW_ERROR) {
      // common overflow handling below
    } अन्यथा {
      setToBogus();
      वापस *यह;
    }
  } अन्यथा {
    // Longer string or read-only buffer:
    // Collect only changes and then apply them to this string.
    // Case mapping often changes only small parts of a string,
    // and often does not change its length.
    oldArray = getArrayStart();
    Edits edits;
    char16_t replacementChars[200];
#अगर !UCONFIG_NO_BREAK_ITERATION
    अगर (iter != nullptr) {
      oldString.setTo(false, oldArray, old_लंबाई);
      iter->setText(oldString);
    }
#पूर्ण_अगर
    stringCaseMapper(caseLocale, options | U_OMIT_UNCHANGED_TEXT, UCASEMAP_BREAK_ITERATOR
                     replacementChars, UPRV_LENGTHOF(replacementChars),
                     oldArray, old_लंबाई, &edits, errorCode);
    अगर (U_SUCCESS(errorCode)) {
      // Grow the buffer at most once, not for multiple doReplace() calls.
      new_लंबाई = old_लंबाई + edits.लंबाई_Delta();
      अगर (new_लंबाई > old_लंबाई && !cloneArrayIfNeeded(new_लंबाई, new_लंबाई)) {
        वापस *यह;
      }
      क्रम (Edits::Iterator ei = edits.getCoarseChangesIterator(); ei.next(errorCode);) {
        doReplace(ei.destination_सूचकांक(), ei.old_लंबाई(),
                  replacementChars, ei.replacement_सूचकांक(), ei.new_लंबाई());
      }
      अगर (U_FAILURE(errorCode)) {
        setToBogus();
      }
      वापस *यह;
    } अन्यथा अगर (errorCode == U_बफर_OVERFLOW_ERROR) {
      // common overflow handling below
      new_लंबाई = old_लंबाई + edits.लंबाई_Delta();
    } अन्यथा {
      setToBogus();
      वापस *यह;
    }
  }

  // Handle buffer overflow, newLength is known.
  // We need to allocate a new buffer for the internal string case mapping function.
  // This is very similar to how doReplace() keeps the old array pointer
  // and deletes the old array itself after it is done.
  // In addition, we are forcing cloneArrayIfNeeded() to always allocate a new array.
  int32_t *बफर_To_Delete = nullptr;
  अगर (!cloneArrayIfNeeded(new_लंबाई, new_लंबाई, false, &बफर_To_Delete, true)) {
    वापस *यह;
  }
  errorCode = U_ZERO_ERROR;
  // No need to iter->setText() again: The case mapper restarts via iter->first().
  new_लंबाई = stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                               getArrayStart(), getCapacity(),
                               oldArray, old_लंबाई, nullptr, errorCode);
  अगर (बफर_To_Delete) {
    uprv_free(बफर_To_Delete);
  }
  अगर (U_SUCCESS(errorCode)) {
    set_लंबाई(new_लंबाई);
  } अन्यथा {
    setToBogus();
  }
  वापस *यह;
}

UnicodeString&
UnicodeString::foldCase(uint32_t options) U_LIFETIME_BOUND {
  वापस caseMap(UCASE_LOC_मूल, options, UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalFold);
}

U_NAMESPACE_END

// Defined here to reduce dependencies on break iterator
U_CAPI int32_t U_EXPORT2
uhash_hashCaselessUnicodeString(स्थिर UElement कुंजी) {
    U_NAMESPACE_USE
    स्थिर UnicodeString *str = (स्थिर UnicodeString*) कुंजी.pointer;
    अगर (str == nullptr) {
        वापस 0;
    }
    // Inefficient; a better way would be to have a hash function in
    // UnicodeString that does case folding on the fly.
    UnicodeString copy(*str);
    वापस copy.foldCase().hashCode();
}

// Defined here to reduce dependencies on break iterator
U_CAPI UBool U_EXPORT2
uhash_compareCaselessUnicodeString(स्थिर UElement key1, स्थिर UElement key2) {
    U_NAMESPACE_USE
    स्थिर UnicodeString *str1 = (स्थिर UnicodeString*) key1.pointer;
    स्थिर UnicodeString *str2 = (स्थिर UnicodeString*) key2.pointer;
    अगर (str1 == str2) {
        वापस true;
    }
    अगर (str1 == nullptr || str2 == nullptr) {
        वापस false;
    }
    वापस str1->caseCompare(*str2, U_FOLD_CASE_DEFAULT) == 0;
}
