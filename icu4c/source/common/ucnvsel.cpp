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

কাঠামো UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_গুনতি;
  অক্ষর** encodings;          // which encodings did user ask to use?
  int32_t encodings_গুনতি;
  int32_t encoding_Str_দৈর্ঘ্য;
  uint8_t* swapped;
  UBool ownPv, ownEncodingStrings;
};

স্থির শূন্য generate_Selector_তথ্য(UConverterSelector* ফলাফল,
                                 UPropsVectors *upvec,
                                 ধ্রুবক USet* excludedCodePoints,
                                 ধ্রুবক UConverterUnicodeSet whichSet,
                                 UErrorCode* status) {
  যদি (U_FAILURE(*status)) {
    ফেরত;
  }

  int32_t columns = (ফলাফল->encodings_গুনতি+31)/32;

  // set errorValue to all-ones
  জন্য (int32_t col = 0; col < columns; col++) {
    upvec_set_মান(upvec, UPVEC_ERROR_মান_CP, UPVEC_ERROR_মান_CP,
                   col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), status);
  }

  জন্য (int32_t i = 0; i < ফলাফল->encodings_গুনতি; ++i) {
    uint32_t mask;
    uint32_t column;
    int32_t item_গুনতি;
    int32_t j;
    UConverter* test_converter = ucnv_open(ফলাফল->encodings[i], status);
    যদি (U_FAILURE(*status)) {
      ফেরত;
    }
    USet* unicode_point_set;
    unicode_point_set = uset_open(1, 0);  // empty set

    ucnv_getUnicodeSet(test_converter, unicode_point_set,
                       whichSet, status);
    যদি (U_FAILURE(*status)) {
      ucnv_close(test_converter);
      ফেরত;
    }

    column = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    item_গুনতি = uset_get_Item_গুনতি(unicode_point_set);

    জন্য (j = 0; j < item_গুনতি; ++j) {
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
      ফেরত;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  যদি (excludedCodePoints) {
    int32_t item_গুনতি = uset_get_Item_গুনতি(excludedCodePoints);
    জন্য (int32_t j = 0; j < item_গুনতি; ++j) {
      UChar32 start_char;
      UChar32 end_char;

      uset_getItem(excludedCodePoints, j, &start_char, &end_char, nullptr, 0,
                   status);
      জন্য (int32_t col = 0; col < columns; col++) {
        upvec_set_মান(upvec, start_char, end_char, col, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      status);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  ফলাফল->trie = upvec_compactToUTrie2WithRowIndexes(upvec, status);
  ফলাফল->pv = upvec_cloneArray(upvec, &ফলাফল->pv_গুনতি, nullptr, status);
  ফলাফল->pv_গুনতি *= columns;  // number of uint32_t = rows * columns
  ফলাফল->ownPv = সত্য;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ধ্রুবক অক্ষর* ধ্রুবক*  converterList, int32_t converter_List_আকার,
             ধ্রুবক USet* excludedCodePoints,
             ধ্রুবক UConverterUnicodeSet whichSet, UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  // ensure args make sense!
  যদি (converter_List_আকার < 0 || (converterList == nullptr && converter_List_আকার != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }

  // allocate a new converter
  LocalUConverterSelectorPointer newSelector(
    (UConverterSelector*)uprv_malloc(আকার(UConverterSelector)));
  যদি (newSelector.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  uprv_memset(newSelector.getAlias(), 0, আকার(UConverterSelector));

  যদি (converter_List_আকার == 0) {
    converterList = nullptr;
    converter_List_আকার = ucnv_গুনতি_Available();
  }
  newSelector->encodings =
    (অক্ষর**)uprv_malloc(converter_List_আকার * আকার(অক্ষর*));
  যদি (!newSelector->encodings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  newSelector->encodings[0] = nullptr;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t যোগ_আকার = 0;
  int32_t i;
  জন্য (i = 0; i < converter_List_আকার; i++) {
    যোগ_আকার +=
      (int32_t)uprv_strlen(converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = যোগ_আকার & 3;
  যদি (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_দৈর্ঘ্য = যোগ_আকার += encodingStrPadding;
  অক্ষর* allStrings = (অক্ষর*) uprv_malloc(যোগ_আকার);
  যদি (!allStrings) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }

  জন্য (i = 0; i < converter_List_আকার; i++) {
    newSelector->encodings[i] = allStrings;
    uprv_strcpy(newSelector->encodings[i],
                converterList != nullptr ? converterList[i] : ucnv_getAvailableName(i));
    allStrings += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  যখন (encodingStrPadding > 0) {
    *allStrings++ = 0;
    --encodingStrPadding;
  }

  newSelector->ownEncodingStrings = সত্য;
  newSelector->encodings_গুনতি = converter_List_আকার;
  UPropsVectors *upvec = upvec_open((converter_List_আকার+31)/32, status);
  generate_Selector_তথ্য(newSelector.getAlias(), upvec, excludedCodePoints, whichSet, status);
  upvec_close(upvec);

  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }

  ফেরত newSelector.orphan();
}

/* close opened selector */
U_CAPI শূন্য U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  যদি (!sel) {
    ফেরত;
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

স্থির ধ্রুবক UDataInfo তথ্য_Info = {
  আকার(UDataInfo),
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
  UCNVSEL_সূচক_TRIE_আকার,      // trie size in bytes
  UCNVSEL_সূচক_PV_গুনতি,       // number of uint32_t in the bit vectors
  UCNVSEL_সূচক_NAMES_গুনতি,    // number of encoding names
  UCNVSEL_সূচক_NAMES_দৈর্ঘ্য,   // number of encoding name bytes including padding
  UCNVSEL_সূচক_আকার = 15,      // bytes following the DataHeader
  UCNVSEL_সূচক_গুনতি = 16
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
ucnvsel_serialize(ধ্রুবক UConverterSelector* sel,
                  শূন্য* বাফার, int32_t বাফার_Capacity, UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)বাফার;
  যদি (বাফার_Capacity < 0 ||
      (বাফার_Capacity > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_আকার = utrie2_serialize(sel->trie, nullptr, 0, status);
  যদি (*status != U_বাফার_OVERFLOW_ERROR && U_FAILURE(*status)) {
    ফেরত 0;
  }
  *status = U_ZERO_ERROR;

  তথ্য_Header header;
  uprv_memset(&header, 0, আকার(header));
  header.তথ্য_Header_2.header_আকার = (uint16_t)((আকার(header) + 15) & ~15);
  header.তথ্য_Header_2.magic1 = 0xda;
  header.তথ্য_Header_2.magic2 = 0x27;
  uprv_memcpy(&header.info, &তথ্য_Info, আকার(তথ্য_Info));

  int32_t indexes[UCNVSEL_সূচক_গুনতি] = {
    serialized_Trie_আকার,
    sel->pv_গুনতি,
    sel->encodings_গুনতি,
    sel->encoding_Str_দৈর্ঘ্য
  };

  int32_t যোগ_আকার =
    header.তথ্য_Header_2.header_আকার +
    (int32_t)আকার(indexes) +
    serialized_Trie_আকার +
    sel->pv_গুনতি * 4 +
    sel->encoding_Str_দৈর্ঘ্য;
  indexes[UCNVSEL_সূচক_আকার] = যোগ_আকার - header.তথ্য_Header_2.header_আকার;
  যদি (যোগ_আকার > বাফার_Capacity) {
    *status = U_বাফার_OVERFLOW_ERROR;
    ফেরত যোগ_আকার;
  }
  // ok, save!
  int32_t length = header.তথ্য_Header_2.header_আকার;
  uprv_memcpy(p, &header, আকার(header));
  uprv_memset(p + আকার(header), 0, length - আকার(header));
  p += length;

  length = (int32_t)আকার(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_আকার, status);
  p += serialized_Trie_আকার;

  length = sel->pv_গুনতি * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_দৈর্ঘ্য);
  p += sel->encoding_Str_দৈর্ঘ্য;

  ফেরত যোগ_আকার;
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
ucnvsel_swap(ধ্রুবক UDataSwapper *ds,
             ধ্রুবক শূন্য *in_তথ্য, int32_t length,
             শূন্য *out_তথ্য, UErrorCode *status) {
  /* udata_swapDataHeader checks the arguments */
  int32_t header_আকার = udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, status);
  যদি(U_FAILURE(*status)) {
    ফেরত 0;
  }

  /* check data format and format version */
  ধ্রুবক UDataInfo* pInfo = reinterpret_cast<ধ্রুবক UDataInfo*>(static_cast<ধ্রুবক অক্ষর*>(in_তথ্য) + 4);
  যদি(!(
    pInfo->তথ্য_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->তথ্য_Format[1] == 0x53 &&
    pInfo->তথ্য_Format[2] == 0x65 &&
    pInfo->তথ্য_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                     pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3]);
    *status = U_INVALID_FORMAT_ERROR;
    ফেরত 0;
  }
  যদি(pInfo->formatVersion[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->formatVersion[0]);
    *status = U_UNSUPPORTED_ERROR;
    ফেরত 0;
  }

  যদি(length >= 0) {
    length -= header_আকার;
    যদি(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *status = U_সূচক_OUTOFBOUNDS_ERROR;
      ফেরত 0;
    }
  }

  ধ্রুবক uint8_t* inBytes = static_cast<ধ্রুবক uint8_t*>(in_তথ্য) + header_আকার;
  uint8_t* outBytes = static_cast<uint8_t*>(out_তথ্য) + header_আকার;

  /* read the indexes */
  ধ্রুবক int32_t* inIndexes = reinterpret_cast<ধ্রুবক int32_t*>(inBytes);
  int32_t indexes[16];
  int32_t i;
  জন্য(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, inIndexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_সূচক_আকার];
  যদি(length >= 0) {
    যদি(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *status = U_সূচক_OUTOFBOUNDS_ERROR;
      ফেরত 0;
    }

    /* copy the data for inaccessible bytes */
    যদি(inBytes != outBytes) {
      uprv_memcpy(outBytes, inBytes, size);
    }

    int32_t offset = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_সূচক_গুনতি*4;
    ds->swapArray32(ds, inBytes, count, outBytes, status);
    offset += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_সূচক_TRIE_আকার];
    utrie2_swap(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_সূচক_PV_গুনতি]*4;
    ds->swapArray32(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_সূচক_NAMES_দৈর্ঘ্য];
    ds->swapInvChars(ds, inBytes + offset, count, outBytes + offset, status);
    offset += count;

    U_ASSERT(offset == size);
  }

  ফেরত header_আকার + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ধ্রুবক শূন্য* বাফার U_LIFETIME_BOUND,
                           int32_t length,
                           UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  // ensure args make sense!
  ধ্রুবক uint8_t *p = (ধ্রুবক uint8_t *)বাফার;
  যদি (length <= 0 ||
      (length > 0 && (p == nullptr || (U_POINTER_MASK_LSB(p, 3) != 0)))
  ) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }
  // header
  যদি (length < 32) {
    // not even enough space for a minimal header
    *status = U_সূচক_OUTOFBOUNDS_ERROR;
    ফেরত nullptr;
  }
  ধ্রুবক তথ্য_Header *pHeader = (ধ্রুবক তথ্য_Header *)p;
  যদি (!(
    pHeader->তথ্য_Header_2.magic1==0xda &&
    pHeader->তথ্য_Header_2.magic2==0x27 &&
    pHeader->info.তথ্য_Format[0] == 0x43 &&
    pHeader->info.তথ্য_Format[1] == 0x53 &&
    pHeader->info.তথ্য_Format[2] == 0x65 &&
    pHeader->info.তথ্য_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *status = U_INVALID_FORMAT_ERROR;
    ফেরত nullptr;
  }
  যদি (pHeader->info.formatVersion[0] != 1) {
    *status = U_UNSUPPORTED_ERROR;
    ফেরত nullptr;
  }
  uint8_t* swapped = nullptr;
  যদি (pHeader->info.isBigEndian != U_IS_BIG_ENDIAN ||
      pHeader->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_প্রবেশ_তথ্য(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, status);
    int32_t যোগ_আকার = ucnvsel_swap(ds, p, -1, nullptr, status);
    যদি (U_FAILURE(*status)) {
      udata_closeSwapper(ds);
      ফেরত nullptr;
    }
    যদি (length < যোগ_আকার) {
      udata_closeSwapper(ds);
      *status = U_সূচক_OUTOFBOUNDS_ERROR;
      ফেরত nullptr;
    }
    swapped = (uint8_t*)uprv_malloc(যোগ_আকার);
    যদি (swapped == nullptr) {
      udata_closeSwapper(ds);
      *status = U_MEMORY_ALLOCATION_ERROR;
      ফেরত nullptr;
    }
    ucnvsel_swap(ds, p, length, swapped, status);
    udata_closeSwapper(ds);
    যদি (U_FAILURE(*status)) {
      uprv_free(swapped);
      ফেরত nullptr;
    }
    p = swapped;
    pHeader = (ধ্রুবক তথ্য_Header *)p;
  }
  যদি (length < (pHeader->তথ্য_Header_2.header_আকার + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *status = U_সূচক_OUTOFBOUNDS_ERROR;
    ফেরত nullptr;
  }
  p += pHeader->তথ্য_Header_2.header_আকার;
  length -= pHeader->তথ্য_Header_2.header_আকার;
  // indexes
  ধ্রুবক int32_t *indexes = (ধ্রুবক int32_t *)p;
  যদি (length < indexes[UCNVSEL_সূচক_আকার]) {
    uprv_free(swapped);
    *status = U_সূচক_OUTOFBOUNDS_ERROR;
    ফেরত nullptr;
  }
  p += UCNVSEL_সূচক_গুনতি * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(আকার(UConverterSelector));
  অক্ষর **encodings =
    (অক্ষর **)uprv_malloc(
      indexes[UCNVSEL_সূচক_NAMES_গুনতি] * আকার(অক্ষর *));
  যদি (sel == nullptr || encodings == nullptr) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  uprv_memset(sel, 0, আকার(UConverterSelector));
  sel->pv_গুনতি = indexes[UCNVSEL_সূচক_PV_গুনতি];
  sel->encodings = encodings;
  sel->encodings_গুনতি = indexes[UCNVSEL_সূচক_NAMES_গুনতি];
  sel->encoding_Str_দৈর্ঘ্য = indexes[UCNVSEL_সূচক_NAMES_দৈর্ঘ্য];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_মান_BITS,
                                        p, indexes[UCNVSEL_সূচক_TRIE_আকার], nullptr,
                                        status);
  p += indexes[UCNVSEL_সূচক_TRIE_আকার];
  যদি (U_FAILURE(*status)) {
    ucnvsel_close(sel);
    ফেরত nullptr;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_গুনতি * 4;
  // encoding names
  অক্ষর* s = (অক্ষর*)p;
  জন্য (int32_t i = 0; i < sel->encodings_গুনতি; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_দৈর্ঘ্য;

  ফেরত sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
কাঠামো Enumerator {
  int16_t* সূচক;
  int16_t length;
  int16_t cur;
  ধ্রুবক UConverterSelector* sel;
};

U_CDECL_BEGIN

স্থির শূন্য U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->সূচক);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


স্থির int32_t U_CALLCONV
ucnvsel_গুনতি_encodings(UEnumeration *enumerator, UErrorCode *status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত 0;
  }
  ফেরত ((Enumerator*)(enumerator->context))->length;
}


স্থির ধ্রুবক অক্ষর* U_CALLCONV ucnvsel_পরবর্তী_encoding(UEnumeration* enumerator,
                                                 int32_t* ফলাফল_দৈর্ঘ্য,
                                                 UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ধ্রুবক UConverterSelector* sel;
  ধ্রুবক অক্ষর* ফলাফল;
  যদি (cur >= ((Enumerator*)(enumerator->context))->length) {
    ফেরত nullptr;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  ফলাফল = sel->encodings[((Enumerator*)(enumerator->context))->সূচক[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  যদি (ফলাফল_দৈর্ঘ্য) {
    *ফলাফল_দৈর্ঘ্য = (int32_t)uprv_strlen(ফলাফল);
  }
  ফেরত ফলাফল;
}

স্থির শূন্য U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_END


স্থির ধ্রুবক UEnumeration defaultEncodings = {
  nullptr,
    nullptr,
    ucnvsel_close_selector_iterator,
    ucnvsel_গুনতি_encodings,
    uenum_unextDefault,
    ucnvsel_পরবর্তী_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
স্থির UBool intersectMasks(uint32_t* dest, ধ্রুবক uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  জন্য (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  ফেরত oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
স্থির int16_t গুনতি_Ones(uint32_t* mask, int32_t len) {
  int32_t i, যোগ_Ones = 0;
  জন্য (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    জন্য (; ent; যোগ_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  ফেরত static_cast<int16_t>(যোগ_Ones);
}


/* internal function! */
স্থির UEnumeration*
selectForMask(ধ্রুবক UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* theMask,
              UErrorCode* status) {
  LocalMemory<uint32_t> mask(theMask);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  LocalMemory<Enumerator> ফলাফল(static_cast<Enumerator *>(uprv_malloc(আকার(Enumerator))));
  যদি (ফলাফল.isNull()) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  ফলাফল->সূচক = nullptr;  // this will be allocated later!
  ফলাফল->length = ফলাফল->cur = 0;
  ফলাফল->sel = sel;

  LocalMemory<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(আকার(UEnumeration))));
  যদি (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  memcpy(en.getAlias(), &defaultEncodings, আকার(UEnumeration));
  
  int32_t columns = (sel->encodings_গুনতি+31)/32;
  int16_t অঙ্ক_Ones = গুনতি_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  যদি (অঙ্ক_Ones > 0) {
    ফলাফল->সূচক = static_cast<int16_t*>(uprv_malloc(অঙ্ক_Ones * আকার(int16_t)));
    যদি (ফলাফল->সূচক == nullptr) {
      *status = U_MEMORY_ALLOCATION_ERROR;
      ফেরত nullptr;
    }
    int32_t i, j;
    int16_t k = 0;
    জন্য (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      জন্য (i = 0 ; i < 32 && k < sel->encodings_গুনতি; i++, k++) {
        যদি ((v & 1) != 0) {
          ফলাফল->সূচক[ফলাফল->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = ফলাফল.orphan();
  ফেরত en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ধ্রুবক UConverterSelector* sel U_LIFETIME_BOUND,
                        ধ্রুবক char16_t* s,
                        int32_t length,
                        UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  // ensure args make sense!
  যদি (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }

  int32_t columns = (sel->encodings_গুনতি+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  যদি (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  যদি(s!=nullptr) {
    ধ্রুবক char16_t *limit;
    যদি (length >= 0) {
      limit = s + length;
    } অন্যথা {
      limit = nullptr;
    }
    
    যখন (limit == nullptr ? *s != 0 : s != limit) {
      UChar32 c;
      uint16_t pv_সূচক;
      UTRIE2_U16_NEXT16(sel->trie, s, limit, c, pv_সূচক);
      যদি (intersectMasks(mask, sel->pv+pv_সূচক, columns)) {
        বিরতি;
      }
    }
  }
  ফেরত selectForMask(sel, mask, status);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ধ্রুবক UConverterSelector* sel U_LIFETIME_BOUND,
                      ধ্রুবক অক্ষর* s,
                      int32_t length,
                      UErrorCode* status) {
  // check if already failed
  যদি (U_FAILURE(*status)) {
    ফেরত nullptr;
  }
  // ensure args make sense!
  যদি (sel == nullptr || (s == nullptr && length != 0)) {
    *status = U_ILLEGAL_ARGUMENT_ERROR;
    ফেরত nullptr;
  }

  int32_t columns = (sel->encodings_গুনতি+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  যদি (mask == nullptr) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    ফেরত nullptr;
  }
  uprv_memset(mask, ~0, columns *4);

  যদি (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  যদি(s!=nullptr) {
    ধ্রুবক অক্ষর *limit = s + length;
    
    যখন (s != limit) {
      uint16_t pv_সূচক;
      UTRIE2_U8_NEXT16(sel->trie, s, limit, pv_সূচক);
      যদি (intersectMasks(mask, sel->pv+pv_সূচক, columns)) {
        বিরতি;
      }
    }
  }
  ফেরত selectForMask(sel, mask, status);
}

#endif  // !UCONFIG_NO_CONVERSION
