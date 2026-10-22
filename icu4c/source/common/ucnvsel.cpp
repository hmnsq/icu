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

#অন্তর্ভাব "unicode/ucnvsel.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "utrie2.h"
#অন্তর্ভাব "propsvec.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucmndata.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"

U_NAMESPACE_USE

সংরচনা UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_গুনতি;
  বর্ণ** encodings;          // which encodings did user ask to use?
  int32_t encodings_গুনতি;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

স্থির শূন্য generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 ধ্রুব USet* excludedCodePoints,
                                 ধ্রুব UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম;
  }

  int32_t columns = (result->encodings_গুনতি+31)/32;

  // set errorValue to all-ones
  প্রতি (int32_t col = 0; col < columns; col++) {
    upvec_set_মান(upvec, UPVEC_ERROR_মান_CP, UPVEC_ERROR_মান_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  প্রতি (int32_t i = 0; i < result->encodings_গুনতি; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_গুনতি;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    যদি (U_FAILURE(*status)) {
      প্রত্যাগম;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    যদি (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      প্রত্যাগম;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_গুনতি = uset_get_Item_গুনতি(unicode_point_set);

    প্রতি (j = 0; j < item_গুনতি; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      যদি (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } অন্যথা {
        upvec_set_মান(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    যদি (U_FAILURE(*status)) {
      প্রত্যাগম;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  যদি (excludedCodePoints) {
    int32_t item_গুনতি = uset_get_Item_গুনতি(excludedCodePoints);
    প্রতি (int32_t j = 0; j < item_গুনতি; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      প্রতি (int32_t col = 0; col < columns; col++) {
        upvec_set_মান(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_গুনতি, nullptr, status);
  result->pv_গুনতি *= columns;  // number of uint32_t = rows * columns
  result->ownPv = সত্য;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ধ্রুব বর্ণ* ধ্রুব*  converterList, int32_t converterListSize,
             ধ্রুব USet* excludedCodePoints,
             ধ্রুব UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }
  // ensure args make sense!
  যদি (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    প্রত্যাগম nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(পরিমাণ(UConverterSelector)));
  যদি (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, পরিমাণ(UConverterSelector));

  যদি (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_গুনতি_Available();
  }
  newSelector->encodings =
    (বর্ণ**)uprv_malloc(converterListSize * পরিমাণ(বর্ণ*));
  যদি (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  প্রতি (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  যদি (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  বর্ণ* allStrings = (বর্ণ*) uprv_malloc(totalSize);
  যদি (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }

  প্রতি (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  যাবত্ (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = সত্য;
  newSelector->encodings_গুনতি = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }

  প্রত্যাগম newSelector.orphan();
}

/* close opened selector */
U_CAPI শূন্য U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  যদি (!sel) {
    প্রত্যাগম;
  }
  যদি (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  যদি (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

স্থির ধ্রুব UDataInfo dataInfo = {
  পরিমাণ(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

গণনা {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_গুনতি,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_গুনতি,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_গুনতি = 16
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
ucnvsel_serialize(ধ্রুব UConverterSelector* sel,
                  শূন্য* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  যদি (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    প্রত্যাগম 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  যদি (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    প্রত্যাগম 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, পরিমাণ(header));
  header.dataHeader.headerSize = (uint16_t)((পরিমাণ(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, পরিমাণ(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_গুনতি] = {
    serializedTrieSize,
    sel->pv_গুনতি,
    sel->encodings_গুনতি,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)পরিমাণ(indexes) +
    serializedTrieSize +
    sel->pv_গুনতি * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  যদি (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    প্রত্যাগম totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, পরিমাণ(header));
  uprv_memset(p + পরিমাণ(header), 0, length - পরিমাণ(header));
  p += length;

  length = (int32_t)পরিমাণ(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_গুনতি * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  প্রত্যাগম totalSize;
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
স্থির int32_t
ucnvsel_swap(ধ্রুব UDataSwapper *ds,
             ধ্রুব শূন্য *inData, int32_t length,
             শূন্য *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  যদি(U_FAILURE(*status)) {
    প্রত্যাগম 0;
  }

  /* check data format and format version */
  ধ্রুব UDataInfo* pInfo = reinterpret_cast<ধ্রুব UDataInfo*>(static_cast<ধ্রুব বর্ণ*>(inData) + 4);
  যদি(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    প্রত্যাগম 0;
  }
  যদি(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    প্রত্যাগম 0;
  }

  যদি(length >= 0) {
    length -= headerSize;
    যদি(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      প্রত্যাগম 0;
    }
  }

  ধ্রুব uint8_t* inBytes = static_cast<ধ্রুব uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  ধ্রুব int32_t* inIndexes = reinterpret_cast<ধ্রুব int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  প্রতি(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  যদি(length >= 0) {
    যদি(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      প্রত্যাগম 0;
    }

    /* copy the data for inaccessible bytes */
    যদি(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_গুনতি*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_গুনতি]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  প্রত্যাগম headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ধ্রুব শূন্য* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }
  // ensure args make sense!
  ধ্রুব uint8_t *p = (ধ্রুব uint8_t *)buffer;
  যদি (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    প্রত্যাগম nullptr;
  }
  // header
  যদি (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    প্রত্যাগম nullptr;
  }
  ধ্রুব DataHeader *pHeader = (ধ্রুব DataHeader *)p;
  যদি (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    প্রত্যাগম nullptr;
  }
  যদি (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    প্রত্যাগম nullptr;
  }
  uint8_t* swapped = nullptr;
  যদি (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_প্রবেশ_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    যদি (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      প্রত্যাগম nullptr;
    }
    যদি (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      প্রত্যাগম nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    যদি (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      প্রত্যাগম nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    যদি (U_FAILURE(*status)) {
      uprv_free(swapped);
      প্রত্যাগম nullptr;
    }
    p = swapped;
    pHeader = (ধ্রুব DataHeader *)p;
  }
  যদি (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    প্রত্যাগম nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  ধ্রুব int32_t *indexes = (ধ্রুব int32_t *)p;
  যদি (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    প্রত্যাগম nullptr;
  }
  p += UCNVSEL_INDEX_গুনতি * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(পরিমাণ(UConverterSelector));
  বর্ণ **encodings =
    (বর্ণ **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_গুনতি] * পরিমাণ(বর্ণ *));
  যদি (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  uprv_memset(sel, 0, পরিমাণ(UConverterSelector));
  sel->pv_গুনতি = indexes[UCNVSEL_INDEX_PV_গুনতি];
  sel->encodings = encodings;
  sel->encodings_গুনতি = indexes[UCNVSEL_INDEX_NAMES_গুনতি];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_মান_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  যদি (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    প্রত্যাগম nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_গুনতি * 4;
  // encoding names
  বর্ণ* s = (বর্ণ*)p;
  প্রতি (int32_t i = 0; i < sel->encodings_গুনতি; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  প্রত্যাগম sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
সংরচনা Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  ধ্রুব UConverterSelector* sel;
};

U_CDECL_BEGIN

স্থির শূন্য U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


স্থির int32_t U_CALLCONV
ucnvsel_গুনতি_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম 0;
  }
  প্রত্যাগম ((Enumerator*)(enumerator->context))->length;
}


স্থির ধ্রুব বর্ণ* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ধ্রুব UConverterSelector* sel;
  ধ্রুব বর্ণ* result;
  যদি (cur >= ((Enumerator*)(enumerator->context))->length) {
    প্রত্যাগম nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  যদি (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  প্রত্যাগম result;
}

স্থির শূন্য U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


স্থির ধ্রুব UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_গুনতি_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
স্থির UBool intersectMasks(uint32_t* dest, ধ্রুব uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  প্রতি (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  প্রত্যাগম oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
স্থির int16_t গুনতি_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  প্রতি (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    প্রতি (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  প্রত্যাগম static_cast<int16_t>(totalOnes);
}


/* internal function! */
স্থির UEnumeration*
selectForMask(ধ্রুব UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(পরিমাণ(Enumerator))));
  যদি (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(পরিমাণ(UEnumeration))));
  যদি (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, পরিমাণ(UEnumeration));
  
  int32_t columns = (sel->encodings_গুনতি+31)/32;
  int16_t অঙ্ক_Ones = গুনতি_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  যদি (অঙ্ক_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(অঙ্ক_Ones * পরিমাণ(int16_t)));
    যদি (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      প্রত্যাগম nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    প্রতি (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      প্রতি (i = 0 ; i < 32 && k < sel->encodings_গুনতি; i++, k++) {
        যদি ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  প্রত্যাগম en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ধ্রুব UConverterSelector* sel U_LIFETIME_BOUND,
                        ধ্রুব char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }
  // ensure args make sense!
  যদি (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    প্রত্যাগম nullptr;
  }

  int32_t columns = (sel->encodings_গুনতি+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  যদি (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  যদি(s!=nullptr) {
    ধ্রুব char16_t *limit;
    যদি (length >= 0) {
      limit = s + length;
    } অন্যথা {
      limit = nullptr;
    }
    
    যাবত্ (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      যদি (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ভঙ্গ;
      }
    }
  }
  প্রত্যাগম selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ধ্রুব UConverterSelector* sel U_LIFETIME_BOUND,
                      ধ্রুব বর্ণ* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    প্রত্যাগম nullptr;
  }
  // ensure args make sense!
  যদি (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    প্রত্যাগম nullptr;
  }

  int32_t columns = (sel->encodings_গুনতি+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  যদি (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  যদি (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  যদি(s!=nullptr) {
    ধ্রুব বর্ণ *limit = s + length;
    
    যাবত্ (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      যদি (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        ভঙ্গ;
      }
    }
  }
  প্রত্যাগম selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
