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

#شامل "unicode/ucnvsel.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل <string.h>

#شامل "unicode/uchar.h"
#شامل "unicode/uniset.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchriter.h"
#شامل "utrie2.h"
#شامل "propsvec.h"
#شامل "uassert.h"
#شامل "ucmndata.h"
#شامل "udataswp.h"
#شامل "uenumimp.h"
#شامل "cmemory.h"
#شامل "cstring.h"

U_NAMESPACE_USE

ساختار UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_شمار;
  نویسه** encodings;          // which encodings did user ask to use?
  int32_t encodings_شمار;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

ایستا تهی generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 ثابت USet* excludedCodePoints,
                                 ثابت UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  اگر (U_FAILURE(*status)) {
    بازگشت;
  }

  int32_t columns = (result->encodings_شمار+31)/32;

  // set errorValue to all-ones
  برای (int32_t col = 0; col < columns; col++) {
    upvec_set_مقدار(upvec, UPVEC_ERROR_مقدار_CP, UPVEC_ERROR_مقدار_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  برای (int32_t i = 0; i < result->encodings_شمار; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_شمار;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    اگر (U_FAILURE(*status)) {
      بازگشت;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    اگر (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      بازگشت;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_شمار = uset_get_Item_شمار(unicode_point_set);

    برای (j = 0; j < item_شمار; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      اگر (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } وگرنه {
        upvec_set_مقدار(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    اگر (U_FAILURE(*status)) {
      بازگشت;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  اگر (excludedCodePoints) {
    int32_t item_شمار = uset_get_Item_شمار(excludedCodePoints);
    برای (int32_t j = 0; j < item_شمار; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      برای (int32_t col = 0; col < columns; col++) {
        upvec_set_مقدار(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_شمار, nullptr, status);
  result->pv_شمار *= columns;  // number of uint32_t = rows * columns
  result->ownPv = درست;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت نویسه* ثابت*  converterList, int32_t converterListSize,
             ثابت USet* excludedCodePoints,
             ثابت UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  // ensure args make sense!
  اگر (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(اندازه(UConverterSelector)));
  اگر (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, اندازه(UConverterSelector));

  اگر (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_شمار_Available();
  }
  newSelector->encodings =
    (نویسه**)uprv_malloc(converterListSize * اندازه(نویسه*));
  اگر (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  برای (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  اگر (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  نویسه* allStrings = (نویسه*) uprv_malloc(totalSize);
  اگر (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }

  برای (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  وقتی (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = درست;
  newSelector->encodings_شمار = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }

  بازگشت newSelector.orphan();
}

/* close opened selector */
U_CAPI تهی U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  اگر (!sel) {
    بازگشت;
  }
  اگر (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  اگر (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ایستا ثابت UDataInfo dataInfo = {
  اندازه(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

شمارش {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_شمار,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_شمار,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_شمار = 16
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
ucnvsel_serialize(ثابت UConverterSelector* sel,
                  تهی* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  اگر (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  اگر (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    بازگشت 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, اندازه(header));
  header.dataHeader.headerSize = (uint16_t)((اندازه(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, اندازه(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_شمار] = {
    serializedTrieSize,
    sel->pv_شمار,
    sel->encodings_شمار,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)اندازه(indexes) +
    serializedTrieSize +
    sel->pv_شمار * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  اگر (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    بازگشت totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, اندازه(header));
  uprv_memset(p + اندازه(header), 0, length - اندازه(header));
  p += length;

  length = (int32_t)اندازه(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_شمار * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  بازگشت totalSize;
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
ایستا int32_t
ucnvsel_swap(ثابت UDataSwapper *ds,
             ثابت تهی *inData, int32_t length,
             تهی *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  اگر(U_FAILURE(*status)) {
    بازگشت 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت نویسه*>(inData) + 4);
  اگر(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    بازگشت 0;
  }
  اگر(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    بازگشت 0;
  }

  اگر(length >= 0) {
    length -= headerSize;
    اگر(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      بازگشت 0;
    }
  }

  ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  برای(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  اگر(length >= 0) {
    اگر(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      بازگشت 0;
    }

    /* copy the data for inaccessible bytes */
    اگر(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_شمار*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_شمار]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  بازگشت headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت تهی* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)buffer;
  اگر (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }
  // header
  اگر (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    بازگشت nullptr;
  }
  ثابت DataHeader *pHeader = (ثابت DataHeader *)p;
  اگر (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    بازگشت nullptr;
  }
  اگر (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    بازگشت nullptr;
  }
  uint8_t* swapped = nullptr;
  اگر (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_درون_داد_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    اگر (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      بازگشت nullptr;
    }
    اگر (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      بازگشت nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    اگر (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      بازگشت nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    اگر (U_FAILURE(*status)) {
      uprv_free(swapped);
      بازگشت nullptr;
    }
    p = swapped;
    pHeader = (ثابت DataHeader *)p;
  }
  اگر (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    بازگشت nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  اگر (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    بازگشت nullptr;
  }
  p += UCNVSEL_INDEX_شمار * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(اندازه(UConverterSelector));
  نویسه **encodings =
    (نویسه **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_شمار] * اندازه(نویسه *));
  اگر (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  uprv_memset(sel, 0, اندازه(UConverterSelector));
  sel->pv_شمار = indexes[UCNVSEL_INDEX_PV_شمار];
  sel->encodings = encodings;
  sel->encodings_شمار = indexes[UCNVSEL_INDEX_NAMES_شمار];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_مقدار_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  اگر (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    بازگشت nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_شمار * 4;
  // encoding names
  نویسه* s = (نویسه*)p;
  برای (int32_t i = 0; i < sel->encodings_شمار; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  بازگشت sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ساختار Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ایستا تهی U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ایستا int32_t U_CALLCONV
ucnvsel_شمار_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت 0;
  }
  بازگشت ((Enumerator*)(enumerator->context))->length;
}


ایستا ثابت نویسه* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت نویسه* result;
  اگر (cur >= ((Enumerator*)(enumerator->context))->length) {
    بازگشت nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  اگر (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  بازگشت result;
}

ایستا تهی U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


ایستا ثابت UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_شمار_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ایستا UBool intersectMasks(uint32_t* dest, ثابت uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  برای (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  بازگشت oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ایستا int16_t شمار_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  برای (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    برای (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  بازگشت static_cast<int16_t>(totalOnes);
}


/* internal function! */
ایستا UEnumeration*
selectForMask(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(اندازه(Enumerator))));
  اگر (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(اندازه(UEnumeration))));
  اگر (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, اندازه(UEnumeration));
  
  int32_t columns = (sel->encodings_شمار+31)/32;
  int16_t رقم_Ones = شمار_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  اگر (رقم_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(رقم_Ones * اندازه(int16_t)));
    اگر (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      بازگشت nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    برای (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      برای (i = 0 ; i < 32 && k < sel->encodings_شمار; i++, k++) {
        اگر ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  بازگشت en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  // ensure args make sense!
  اگر (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }

  int32_t columns = (sel->encodings_شمار+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  اگر (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  اگر(s!=nullptr) {
    ثابت char16_t *limit;
    اگر (length >= 0) {
      limit = s + length;
    } وگرنه {
      limit = nullptr;
    }
    
    وقتی (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      اگر (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        بشکن;
      }
    }
  }
  بازگشت selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                      ثابت نویسه* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  اگر (U_FAILURE(*status)) {
    بازگشت nullptr;
  }
  // ensure args make sense!
  اگر (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    بازگشت nullptr;
  }

  int32_t columns = (sel->encodings_شمار+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  اگر (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    بازگشت nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  اگر (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  اگر(s!=nullptr) {
    ثابت نویسه *limit = s + length;
    
    وقتی (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      اگر (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        بشکن;
      }
    }
  }
  بازگشت selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
