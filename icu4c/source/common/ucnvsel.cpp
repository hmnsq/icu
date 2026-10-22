// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2008-2011, International Business Machines
*   Corporation, Google and others.  All Rights Reserved.
*
*******************************************************************************
*/
// Author : eldawy@google.com (Mohamed Eldawy)
// ucnvsel.cpp
//
// Purpose: To generate a list of encodings capable of handling
// a given Unicode text
//
// Started 09-April-2008

/**
 * \file
 *
 * This is an implementation of an encoding selector.
 * The goal is, given a unicode string, find the encodings
 * this string can be mapped to. To make processing faster
 * a trie is built when you call ucnvsel_open() that
 * stores all encodings a codepoint can map to
 */

#अन्तर्भाव "unicode/ucnvsel.h"

#जर !UCONFIG_NO_CONVERSION

#अन्तर्भाव <string.h>

#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "propsvec.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucmndata.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"

U_NAMESPACE_USE

संरचना UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_मोजणी;
  अक्षर** encodings;          // which encodings did user ask to use?
  int32_t encodings_मोजणी;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

स्थिर शून्य generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 स्थिरांक USet* excludedCodePoints,
                                 स्थिरांक UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  जर (U_FAILURE(*status)) {
    परत;
  }

  int32_t columns = (result->encodings_मोजणी+31)/32;

  // set errorValue to all-ones
  साठी (int32_t col = 0; col < columns; col++) {
    upvec_set_मूल्य(upvec, UPVEC_ERROR_मूल्य_CP, UPVEC_ERROR_मूल्य_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  साठी (int32_t i = 0; i < result->encodings_मोजणी; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_मोजणी;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    जर (U_FAILURE(*status)) {
      परत;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    जर (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      परत;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_मोजणी = uset_get_Item_मोजणी(unicode_point_set);

    साठी (j = 0; j < item_मोजणी; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      जर (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } अन्यथा {
        upvec_set_मूल्य(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    जर (U_FAILURE(*status)) {
      परत;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  जर (excludedCodePoints) {
    int32_t item_मोजणी = uset_get_Item_मोजणी(excludedCodePoints);
    साठी (int32_t j = 0; j < item_मोजणी; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      साठी (int32_t col = 0; col < columns; col++) {
        upvec_set_मूल्य(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_मोजणी, nullptr, status);
  result->pv_मोजणी *= columns;  // number of uint32_t = rows * columns
  result->ownPv = सत्य;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(स्थिरांक अक्षर* स्थिरांक*  converterList, int32_t converterListSize,
             स्थिरांक USet* excludedCodePoints,
             स्थिरांक UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }
  // ensure args make sense!
  जर (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(आकार(UConverterSelector)));
  जर (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, आकार(UConverterSelector));

  जर (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_मोजणी_Available();
  }
  newSelector->encodings =
    (अक्षर**)uprv_malloc(converterListSize * आकार(अक्षर*));
  जर (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  साठी (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  जर (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  अक्षर* allStrings = (अक्षर*) uprv_malloc(totalSize);
  जर (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }

  साठी (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  पर्यंत (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = सत्य;
  newSelector->encodings_मोजणी = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  जर (U_FAILURE(*status)) {
    परत nullptr;
  }

  परत newSelector.orphan();
}

/* close opened selector */
U_CAPI शून्य U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  जर (!sel) {
    परत;
  }
  जर (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  जर (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

स्थिर स्थिरांक UDataInfo dataInfo = {
  आकार(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

गणना {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_मोजणी,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_मोजणी,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_मोजणी = 16
};

/*
 * Serialized form of a UConverterSelector, formatVersion 1:
 *
 * The serialized form begins with a standard ICU DataHeader with a UDataInfo
 * as the template above.
 * This is followed by:
 *   int32_t indexes[UCNVSEL_INDEX_COUNT];          // see index entry constants above
 *   serialized UTrie2;                             // indexes[UCNVSEL_INDEX_TRIE_SIZE] bytes
 *   uint32_t pv[indexes[UCNVSEL_INDEX_PV_COUNT]];  // bit vectors
 *   char* encodingNames[indexes[UCNVSEL_INDEX_NAMES_LENGTH]];  // NUL-terminated strings + padding
 */

/* serialize a selector */
U_CAPI int32_t U_EXPORT2
ucnvsel_serialize(स्थिरांक UConverterSelector* sel,
                  शून्य* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  जर (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  जर (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    परत 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, आकार(header));
  header.dataHeader.headerSize = (uint16_t)((आकार(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, आकार(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_मोजणी] = {
    serializedTrieSize,
    sel->pv_मोजणी,
    sel->encodings_मोजणी,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)आकार(indexes) +
    serializedTrieSize +
    sel->pv_मोजणी * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  जर (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    परत totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, आकार(header));
  uprv_memset(p + आकार(header), 0, length - आकार(header));
  p += length;

  length = (int32_t)आकार(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_मोजणी * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  परत totalSize;
}

/**
 * swap a selector into the desired Endianness and Asciiness of
 * the system. Just as FYI, selectors are always saved in the format
 * of the system that created them. They are only converted if used
 * on another system. In other words, selectors created on different
 * system can be different even if the params are identical (endianness
 * and Asciiness differences only)
 *
 * @param ds pointer to data swapper containing swapping info
 * @param inData pointer to incoming data
 * @param length length of inData in bytes
 * @param outData pointer to output data. Capacity should
 *                be at least equal to capacity of inData
 * @param status an in/out ICU UErrorCode
 * @return 0 on failure, number of bytes swapped on success
 *         number of bytes swapped can be smaller than length
 */
स्थिर int32_t
ucnvsel_swap(स्थिरांक UDataSwapper *ds,
             स्थिरांक शून्य *inData, int32_t length,
             शून्य *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  जर(U_FAILURE(*status)) {
    परत 0;
  }

  /* check data format and format version */
  स्थिरांक UDataInfo* pInfo = reinterpret_cast<स्थिरांक UDataInfo*>(static_cast<स्थिरांक अक्षर*>(inData) + 4);
  जर(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    परत 0;
  }
  जर(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    परत 0;
  }

  जर(length >= 0) {
    length -= headerSize;
    जर(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      परत 0;
    }
  }

  स्थिरांक uint8_t* inBytes = static_cast<स्थिरांक uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  स्थिरांक int32_t* inIndexes = reinterpret_cast<स्थिरांक int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  साठी(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  जर(length >= 0) {
    जर(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      परत 0;
    }

    /* copy the data for inaccessible bytes */
    जर(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_मोजणी*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_मोजणी]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  परत headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(स्थिरांक शून्य* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }
  // ensure args make sense!
  स्थिरांक uint8_t *p = (स्थिरांक uint8_t *)buffer;
  जर (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }
  // header
  जर (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    परत nullptr;
  }
  स्थिरांक DataHeader *pHeader = (स्थिरांक DataHeader *)p;
  जर (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    परत nullptr;
  }
  जर (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    परत nullptr;
  }
  uint8_t* swapped = nullptr;
  जर (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_आदान_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    जर (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      परत nullptr;
    }
    जर (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      परत nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    जर (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      परत nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    जर (U_FAILURE(*status)) {
      uprv_free(swapped);
      परत nullptr;
    }
    p = swapped;
    pHeader = (स्थिरांक DataHeader *)p;
  }
  जर (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    परत nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  स्थिरांक int32_t *indexes = (स्थिरांक int32_t *)p;
  जर (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    परत nullptr;
  }
  p += UCNVSEL_INDEX_मोजणी * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(आकार(UConverterSelector));
  अक्षर **encodings =
    (अक्षर **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_मोजणी] * आकार(अक्षर *));
  जर (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  uprv_memset(sel, 0, आकार(UConverterSelector));
  sel->pv_मोजणी = indexes[UCNVSEL_INDEX_PV_मोजणी];
  sel->encodings = encodings;
  sel->encodings_मोजणी = indexes[UCNVSEL_INDEX_NAMES_मोजणी];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_मूल्य_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  जर (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    परत nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_मोजणी * 4;
  // encoding names
  अक्षर* s = (अक्षर*)p;
  साठी (int32_t i = 0; i < sel->encodings_मोजणी; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  परत sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
संरचना Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  स्थिरांक UConverterSelector* sel;
};

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


स्थिर int32_t U_CALLCONV
ucnvsel_मोजणी_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत 0;
  }
  परत ((Enumerator*)(enumerator->context))->length;
}


स्थिर स्थिरांक अक्षर* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  स्थिरांक UConverterSelector* sel;
  स्थिरांक अक्षर* result;
  जर (cur >= ((Enumerator*)(enumerator->context))->length) {
    परत nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  जर (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  परत result;
}

स्थिर शून्य U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


स्थिर स्थिरांक UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_मोजणी_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
स्थिर UBool intersectMasks(uint32_t* dest, स्थिरांक uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  साठी (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  परत oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
स्थिर int16_t मोजणी_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  साठी (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    साठी (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  परत static_cast<int16_t>(totalOnes);
}


/* internal function! */
स्थिर UEnumeration*
selectForMask(स्थिरांक UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(आकार(Enumerator))));
  जर (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(आकार(UEnumeration))));
  जर (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  स_नकल(en.getAlias(), &defaultEncodings, आकार(UEnumeration));
  
  int32_t columns = (sel->encodings_मोजणी+31)/32;
  int16_t अंक_Ones = मोजणी_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  जर (अंक_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(अंक_Ones * आकार(int16_t)));
    जर (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      परत nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    साठी (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      साठी (i = 0 ; i < 32 && k < sel->encodings_मोजणी; i++, k++) {
        जर ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  परत en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(स्थिरांक UConverterSelector* sel U_LIFETIME_BOUND,
                        स्थिरांक char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }
  // ensure args make sense!
  जर (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }

  int32_t columns = (sel->encodings_मोजणी+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  जर (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  जर(s!=nullptr) {
    स्थिरांक char16_t *limit;
    जर (length >= 0) {
      limit = s + length;
    } अन्यथा {
      limit = nullptr;
    }
    
    पर्यंत (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      जर (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        थांबा;
      }
    }
  }
  परत selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(स्थिरांक UConverterSelector* sel U_LIFETIME_BOUND,
                      स्थिरांक अक्षर* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  जर (U_FAILURE(*status)) {
    परत nullptr;
  }
  // ensure args make sense!
  जर (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    परत nullptr;
  }

  int32_t columns = (sel->encodings_मोजणी+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  जर (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    परत nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  जर (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  जर(s!=nullptr) {
    स्थिरांक अक्षर *limit = s + length;
    
    पर्यंत (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      जर (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        थांबा;
      }
    }
  }
  परत selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
