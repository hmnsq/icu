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

#અન્તર્ભાવ "unicode/ucnvsel.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/uchriter.h"
#અન્તર્ભાવ "utrie2.h"
#અન્તર્ભાવ "propsvec.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucmndata.h"
#અન્તર્ભાવ "udataswp.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"

U_NAMESPACE_USE

સંરચના UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_ગણતરી;
  અક્ષર** encodings;          // which encodings did user ask to use?
  int32_t encodings_ગણતરી;
  int32_t encodingStrLength;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

સ્થિર શૂન્ય generateSelectorData(UConverterSelector* result,
                                 UPropsVectors *upvec,
                                 અચલ USet* excludedCodePoints,
                                 અચલ UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  જો (U_FAILURE(*status)) {
    પાછા;
  }

  int32_t columns = (result->encodings_ગણતરી+31)/32;

  // set errorValue to all-ones
  માટે (int32_t col = 0; col < columns; col++) {
    upvec_set_કિંમત(upvec, UPVEC_ERROR_કિંમત_CP, UPVEC_ERROR_કિંમત_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  માટે (int32_t i = 0; i < result->encodings_ગણતરી; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_ગણતરી;
    int32_t j;
    UConverter* test_converter = ucnv_open(result->encodings[i], status);
    જો (U_FAILURE(*status)) {
      પાછા;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    જો (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      પાછા;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_ગણતરી = uset_get_Item_ગણતરી(unicode_point_set);

    માટે (j = 0; j < item_ગણતરી; ++j) {
      UChar32 start_char;
      UChar32 end_char;
      UErrorCode smallStatus = U_ZERO_ERROR;
      uset_getItem(unicode_point_set, j, &start_char, &end_char, nullptr, 0,
                   &smallStatus);
      જો (U_FAILURE(smallStatus)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } નહીંતર {
        upvec_set_કિંમત(upvec, start_char, end_char, column, static_cast<uint32_t>(~0), mask,
                       status);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_point_set);
    જો (U_FAILURE(*status)) {
      પાછા;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  જો (excludedCodePoints) {
    int32_t item_ગણતરી = uset_get_Item_ગણતરી(excludedCodePoints);
    માટે (int32_t j = 0; j < item_ગણતરી; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      માટે (int32_t col = 0; col < columns; col++) {
        upvec_set_કિંમત(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  result->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  result->pv = upvec_cloneArray(upvec, &result->pv_ગણતરી, nullptr, status);
  result->pv_ગણતરી *= columns;  // number of uint32_t = rows * columns
  result->ownPv = સાચું;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(અચલ અક્ષર* અચલ*  converterList, int32_t converterListSize,
             અચલ USet* excludedCodePoints,
             અચલ UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }
  // ensure args make sense!
  જો (converterListSize < 0 || (converterList == nullptr && converterListSize != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(કદ(UConverterSelector)));
  જો (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, કદ(UConverterSelector));

  જો (converterListSize == 0) {
    converterList = nullptr;
    converterListSize = ucnv_ગણતરી_Available();
  }
  newSelector->encodings =
    (અક્ષર**)uprv_malloc(converterListSize * કદ(અક્ષર*));
  જો (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t totalSize = 0;
  int32_t i;
  માટે (i = 0; i < converterListSize; i++) {
    totalSize +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = totalSize & 3;
  જો (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encodingStrLength = totalSize += encodingStrPadding;
  અક્ષર* allStrings = (અક્ષર*) uprv_malloc(totalSize);
  જો (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }

  માટે (i = 0; i < converterListSize; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  જ્યારે (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = સાચું;
  newSelector->encodings_ગણતરી = converterListSize;
  UPropsVectors *upvec = upvec_open((converterListSize+31)/32, status);
  generateSelectorData(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }

  પાછા newSelector.orphan();
}

/* close opened selector */
U_CAPI શૂન્ય U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  જો (!sel) {
    પાછા;
  }
  જો (sel->ownEncodingStrings) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  જો (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

સ્થિર અચલ UDataInfo dataInfo = {
  કદ(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

ગણના {
  UCNVSEL_INDEX_TRIE_SIZE,      // trie size in bytes
  UCNVSEL_INDEX_PV_ગણતરી,       // number of uint32_t in the bit vectors
  UCNVSEL_INDEX_NAMES_ગણતરી,    // number of encoding names
  UCNVSEL_INDEX_NAMES_LENGTH,   // number of encoding name bytes including padding
  UCNVSEL_INDEX_SIZE = 15,      // bytes following the DataHeader
  UCNVSEL_INDEX_ગણતરી = 16
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
ucnvsel_serialize(અચલ UConverterSelector* sel,
                  શૂન્ય* buffer, int32_t bufferCapacity, UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)buffer;
  જો (bufferCapacity < 0 ||
      (bufferCapacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા 0;
  }
  // add up the size of the serialized form
  int32_t serializedTrieSize = utrie2_serialize(sel->trie, nullptr, 0, status);
  જો (*status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(*status)) {
    પાછા 0;
  }
  *status = U_ZERO_ERROR;

  DataHeader header;
  uprv_memset(&header, 0, કદ(header));
  header.dataHeader.headerSize = (uint16_t)((કદ(header) + 15) & ~15);
  header.dataHeader.magic1 = 0xda;
  header.dataHeader.magic2 = 0x27;
  uprv_memcpy(&header.info, &dataInfo, કદ(dataInfo));

  int32_t indexes[UCNVSEL_INDEX_ગણતરી] = {
    serializedTrieSize,
    sel->pv_ગણતરી,
    sel->encodings_ગણતરી,
    sel->encodingStrLength
  };

  int32_t totalSize =
    header.dataHeader.headerSize +
    (int32_t)કદ(indexes) +
    serializedTrieSize +
    sel->pv_ગણતરી * 4 +
    sel->encodingStrLength;
  indexes[UCNVSEL_INDEX_SIZE] = totalSize - header.dataHeader.headerSize;
  જો (totalSize > bufferCapacity) {
    *status = U_BUFFER_OVERFLOW_ERROR;
    પાછા totalSize;
  }
  // ok, save!
  int32_t length = header.dataHeader.headerSize;
  uprv_memcpy(p, &header, કદ(header));
  uprv_memset(p + કદ(header), 0, length - કદ(header));
  p += length;

  length = (int32_t)કદ(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serializedTrieSize, status);
  p += serializedTrieSize;

  length = sel->pv_ગણતરી * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encodingStrLength);
  p += sel->encodingStrLength;

  પાછા totalSize;
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
સ્થિર int32_t
ucnvsel_swap(અચલ UDataSwapper *ds,
             અચલ શૂન્ય *inData, int32_t length,
             શૂન્ય *outData, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t headerSize = udata_swapDataHeader(ds, inData, length, outData, status);
  જો(U_FAILURE(*status)) {
    પાછા 0;
  }

  /* check data format and format version */
  અચલ UDataInfo* pInfo = reinterpret_cast<અચલ UDataInfo*>(static_cast<અચલ અક્ષર*>(inData) + 4);
  જો(!(
    pInfo->dataFormat[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->dataFormat[1] == 0x53 &&
    pInfo->dataFormat[2] == 0x65 &&
    pInfo->dataFormat[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->dataFormat[0], pInfo->dataFormat[1],
                     pInfo->dataFormat[2], pInfo->dataFormat[3]);
    *status = U_INVALID_FORMAT_ERROR;
    પાછા 0;
  }
  જો(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    પાછા 0;
  }

  જો(length >= 0) {
    length -= headerSize;
    જો(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      પાછા 0;
    }
  }

  અચલ uint8_t* inBytes = static_cast<અચલ uint8_t*>(inData) + headerSize;
  uint8_t* outBytes = static_cast<uint8_t*>(outData) + headerSize;

  /* read the indexes */
  અચલ int32_t* inIndexes = reinterpret_cast<અચલ int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  માટે(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_INDEX_SIZE];
  જો(length >= 0) {
    જો(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      પાછા 0;
    }

    /* copy the data for inaccessible bytes */
    જો(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_INDEX_ગણતરી*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_INDEX_TRIE_SIZE];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_INDEX_PV_ગણતરી]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  પાછા headerSize + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(અચલ શૂન્ય* buffer U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }
  // ensure args make sense!
  અચલ uint8_t *p = (અચલ uint8_t *)buffer;
  જો (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }
  // header
  જો (length < 32) {
    // not even enough space for a minimal header
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    પાછા nullptr;
  }
  અચલ DataHeader *pHeader = (અચલ DataHeader *)p;
  જો (!(
    pHeader->dataHeader.magic1==0xda &&
    pHeader->dataHeader.magic2==0x27 &&
    pHeader->info.dataFormat[0] == 0x43 &&
    pHeader->info.dataFormat[1] == 0x53 &&
    pHeader->info.dataFormat[2] == 0x65 &&
    pHeader->info.dataFormat[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    પાછા nullptr;
  }
  જો (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    પાછા nullptr;
  }
  uint8_t* swapped = nullptr;
  જો (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_આગત_Data(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t totalSize = ucnvsel_swap(ds, p, -1, nullptr, status);
    જો (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      પાછા nullptr;
    }
    જો (length < totalSize) {
      udata_closeSwapper(ds);
      *status = U_INDEX_OUTOFBOUNDS_ERROR;
      પાછા nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(totalSize);
    જો (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      પાછા nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    જો (U_FAILURE(*status)) {
      uprv_free(swapped);
      પાછા nullptr;
    }
    p = swapped;
    pHeader = (અચલ DataHeader *)p;
  }
  જો (length < (pHeader->dataHeader.headerSize + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    પાછા nullptr;
  }
  p += pHeader->dataHeader.headerSize;
  length -= pHeader->dataHeader.headerSize;
  // indexes
  અચલ int32_t *indexes = (અચલ int32_t *)p;
  જો (length < indexes[UCNVSEL_INDEX_SIZE]) {
    uprv_free(swapped);
    *status = U_INDEX_OUTOFBOUNDS_ERROR;
    પાછા nullptr;
  }
  p += UCNVSEL_INDEX_ગણતરી * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(કદ(UConverterSelector));
  અક્ષર **encodings =
    (અક્ષર **)uprv_malloc(
      indexes[UCNVSEL_INDEX_NAMES_ગણતરી] * કદ(અક્ષર *));
  જો (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  uprv_memset(sel, 0, કદ(UConverterSelector));
  sel->pv_ગણતરી = indexes[UCNVSEL_INDEX_PV_ગણતરી];
  sel->encodings = encodings;
  sel->encodings_ગણતરી = indexes[UCNVSEL_INDEX_NAMES_ગણતરી];
  sel->encodingStrLength = indexes[UCNVSEL_INDEX_NAMES_LENGTH];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_કિંમત_BITS,
                                        p, indexes[UCNVSEL_INDEX_TRIE_SIZE], nullptr,
                                        status);
  p += indexes[UCNVSEL_INDEX_TRIE_SIZE];
  જો (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    પાછા nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ગણતરી * 4;
  // encoding names
  અક્ષર* s = (અક્ષર*)p;
  માટે (int32_t i = 0; i < sel->encodings_ગણતરી; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encodingStrLength;

  પાછા sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
સંરચના Enumerator {
  int16_t* index;
  int16_t length;
  int16_t cur;
  અચલ UConverterSelector* sel;
};

U_CDECL_BEGIN

સ્થિર શૂન્ય U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->index);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


સ્થિર int32_t U_CALLCONV
ucnvsel_ગણતરી_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા 0;
  }
  પાછા ((Enumerator*)(enumerator->context))->length;
}


સ્થિર અચલ અક્ષર* U_CALLCONV ucnvsel_next_encoding(UEnumeration* enumerator,
                                                 int32_t* resultLength,
                                                 UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  અચલ UConverterSelector* sel;
  અચલ અક્ષર* result;
  જો (cur >= ((Enumerator*)(enumerator->context))->length) {
    પાછા nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  result = sel->encodings[((Enumerator*)(enumerator->context))->index[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  જો (resultLength) {
    *resultLength = (int32_t)uprv_strlen(result);
  }
  પાછા result;
}

સ્થિર શૂન્ય U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


સ્થિર અચલ UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_ગણતરી_encodings,
    uenum_unextDefault,
    ucnvsel_next_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
સ્થિર UBool intersectMasks(uint32_t* dest, અચલ uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  માટે (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  પાછા oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
સ્થિર int16_t ગણતરી_Ones(uint32_t* mask, int32_t len) {
  int32_t i, totalOnes = 0;
  માટે (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    માટે (; ent; totalOnes++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  પાછા static_cast<int16_t>(totalOnes);
}


/* internal function! */
સ્થિર UEnumeration*
selectForMask(અચલ UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> result(static_cast<Enumerator *>(uprv_malloc(કદ(Enumerator))));
  જો (result.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  result->index = nullptr;  // this will be allocated later!
  result->length = result->cur = 0;
  result->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(કદ(UEnumeration))));
  જો (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, કદ(UEnumeration));
  
  int32_t columns = (sel->encodings_ગણતરી+31)/32;
  int16_t અંક_Ones = ગણતરી_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  જો (અંક_Ones > 0) {
    result->index = static_cast<int16_t*>(uprv_malloc(અંક_Ones * કદ(int16_t)));
    જો (result->index == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      પાછા nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    માટે (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      માટે (i = 0 ; i < 32 && k < sel->encodings_ગણતરી; i++, k++) {
        જો ((v & 1) != 0) {
          result->index[result->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = result.orphan();
  પાછા en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(અચલ UConverterSelector* sel U_LIFETIME_BOUND,
                        અચલ char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }
  // ensure args make sense!
  જો (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }

  int32_t columns = (sel->encodings_ગણતરી+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  જો (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  જો(s!=nullptr) {
    અચલ char16_t *limit;
    જો (length >= 0) {
      limit = s + length;
    } નહીંતર {
      limit = nullptr;
    }
    
    જ્યારે (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pvIndex;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pvIndex);
      જો (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        તોડો;
      }
    }
  }
  પાછા selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(અચલ UConverterSelector* sel U_LIFETIME_BOUND,
                      અચલ અક્ષર* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  જો (U_FAILURE(*status)) {
    પાછા nullptr;
  }
  // ensure args make sense!
  જો (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    પાછા nullptr;
  }

  int32_t columns = (sel->encodings_ગણતરી+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  જો (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    પાછા nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  જો (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  જો(s!=nullptr) {
    અચલ અક્ષર *limit = s + length;
    
    જ્યારે (s != limit) {
      uint16_t pvIndex;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pvIndex);
      જો (intersectMasks(mask, sel->pv+pvIndex, columns)) {
        તોડો;
      }
    }
  }
  પાછા selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
