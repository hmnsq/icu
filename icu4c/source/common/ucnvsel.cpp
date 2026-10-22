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

#ضمن "unicode/ucnvsel.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن <string.h>

#ضمن "unicode/uchar.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchriter.h"
#ضمن "utrie2.h"
#ضمن "propsvec.h"
#ضمن "uassert.h"
#ضمن "ucmndata.h"
#ضمن "udataswp.h"
#ضمن "uenumimp.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"

U_NAMESPACE_USE

بنية UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_شمار;
  حرف** encodings;          // which encodings did user ask to use?
  int32_t encodings_شمار;
  int32_t encoding_Str_درازی;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_رشتهها;
};

ساكن فراغ generate_Selector_داده(UConverterSelector* نتیجه,
                                 UPropsVectors *upvec,
                                 ثابت USet* excluded_کد_Points,
                                 ثابت UConverter_Unicode_مجموعه which_مجموعه,
                                 UError_کد* وضعیت) {
  إذا (U_FAILURE(*وضعیت)) {
    أرجع;
  }

  int32_t columns = (نتیجه->encodings_شمار+31)/32;

  // set errorValue to all-ones
  لكل (int32_t ستون = 0; ستون < columns; ستون++) {
    upvec_مجموعه_مقدار(upvec, UPVEC_ERROR_مقدار_CP, UPVEC_ERROR_مقدار_CP,
                   ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), وضعیت);
  }

  لكل (int32_t i = 0; i < نتیجه->encodings_شمار; ++i) {
    uint32_t mask;
    uint32_t ستونی;
    int32_t قلم_شمار;
    int32_t j;
    UConverter* test_converter = ucnv_open(نتیجه->encodings[i], وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
      أرجع;
    }
    USet* unicode_نقطه_مجموعه;
    unicode_نقطه_مجموعه = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_مجموعه(test_converter, unicode_نقطه_مجموعه,
                       which_مجموعه, وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
      ucnv_close(test_converter);
      أرجع;
    }

    ستونی = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    قلم_شمار = uset_get_قلم_شمار(unicode_نقطه_مجموعه);

    لكل (j = 0; j < قلم_شمار; ++j) {
      UChar32 شروع_char;
      UChar32 انتها_char;
      UError_کد small_وضعیت = U_ZERO_ERROR;
      uset_get_قلم(unicode_نقطه_مجموعه, j, &شروع_char, &انتها_char, اشاره_تهی, 0,
                   &small_وضعیت);
      إذا (U_FAILURE(small_وضعیت)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } وإلا {
        upvec_مجموعه_مقدار(upvec, شروع_char, انتها_char, ستونی, static_cast<uint32_t>(~0), mask,
                       وضعیت);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_نقطه_مجموعه);
    إذا (U_FAILURE(*وضعیت)) {
      أرجع;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  إذا (excluded_کد_Points) {
    int32_t قلم_شمار = uset_get_قلم_شمار(excluded_کد_Points);
    لكل (int32_t j = 0; j < قلم_شمار; ++j) {
      UChar32 شروع_char;
      UChar32 انتها_char;

      uset_get_قلم(excluded_کد_Points, j, &شروع_char, &انتها_char, اشاره_تهی, 0,
                   وضعیت);
      لكل (int32_t ستون = 0; ستون < columns; ستون++) {
        upvec_مجموعه_مقدار(upvec, شروع_char, انتها_char, ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      وضعیت);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  نتیجه->trie = upvec_compact_مقصد_UTrie2_With_ردیف_Indexes(upvec, وضعیت);
  نتیجه->pv = upvec_cloneArray(upvec, &نتیجه->pv_شمار, اشاره_تهی, وضعیت);
  نتیجه->pv_شمار *= columns;  // number of uint32_t = rows * columns
  نتیجه->ownPv = درست;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت حرف* ثابت*  converterList, int32_t converter_List_اندازه,
             ثابت USet* excluded_کد_Points,
             ثابت UConverter_Unicode_مجموعه which_مجموعه, UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  // ensure args make sense!
  إذا (converter_List_اندازه < 0 || (converterList == اشاره_تهی && converter_List_اندازه != 0)) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }

  // allocate a new converter
  Local_UConverter_Selector_اشاره_گر newSelector(
    (UConverterSelector*)uprv_malloc(حجم(UConverterSelector)));
  إذا (newSelector.isNull()) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  uprv_memset(newSelector.getAlias(), 0, حجم(UConverterSelector));

  إذا (converter_List_اندازه == 0) {
    converterList = اشاره_تهی;
    converter_List_اندازه = ucnv_شمار_موجود();
  }
  newSelector->encodings =
    (حرف**)uprv_malloc(converter_List_اندازه * حجم(حرف*));
  إذا (!newSelector->encodings) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  newSelector->encodings[0] = اشاره_تهی;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t مجموع_اندازه = 0;
  int32_t i;
  لكل (i = 0; i < converter_List_اندازه; i++) {
    مجموع_اندازه +=
      (int32_t)uprv_strlen(converterList != اشاره_تهی ? converterList[i] : ucnv_get_موجود_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = مجموع_اندازه & 3;
  إذا (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_درازی = مجموع_اندازه += encodingStrPadding;
  حرف* all_رشتهها = (حرف*) uprv_malloc(مجموع_اندازه);
  إذا (!all_رشتهها) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }

  لكل (i = 0; i < converter_List_اندازه; i++) {
    newSelector->encodings[i] = all_رشتهها;
    uprv_strcpy(newSelector->encodings[i],
                converterList != اشاره_تهی ? converterList[i] : ucnv_get_موجود_Name(i));
    all_رشتهها += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  طالما (encodingStrPadding > 0) {
    *all_رشتهها++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_رشتهها = درست;
  newSelector->encodings_شمار = converter_List_اندازه;
  UPropsVectors *upvec = upvec_open((converter_List_اندازه+31)/32, وضعیت);
  generate_Selector_داده(newSelector.getAlias(), upvec, excluded_کد_Points, which_مجموعه, وضعیت);
  upvec_close(upvec);

  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }

  أرجع newSelector.orphan();
}

/* close opened selector */
U_CAPI فراغ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  إذا (!sel) {
    أرجع;
  }
  إذا (sel->own_Encoding_رشتهها) {
    uprv_free(sel->encodings[0]);
  }
  uprv_free(sel->encodings);
  إذا (sel->ownPv) {
    uprv_free(sel->pv);
  }
  utrie2_close(sel->trie);
  uprv_free(sel->swapped);
  uprv_free(sel);
}

ساكن ثابت UDataInfo داده_Info = {
  حجم(UDataInfo),
  0,

  U_IS_BIG_ENDIAN,
  U_CHARSET_FAMILY,
  U_SIZEOF_UCHAR,
  0,

  { 0x43, 0x53, 0x65, 0x6c },   /* dataFormat="CSel" */
  { 1, 0, 0, 0 },               /* formatVersion */
  { 0, 0, 0, 0 }                /* dataVersion */
};

تعداد {
  UCNVSEL_شاخص_TRIE_اندازه,      // trie size in bytes
  UCNVSEL_شاخص_PV_شمار,       // number of uint32_t in the bit vectors
  UCNVSEL_شاخص_نامها_شمار,    // number of encoding names
  UCNVSEL_شاخص_نامها_درازی,   // number of encoding name bytes including padding
  UCNVSEL_شاخص_اندازه = 15,      // bytes following the DataHeader
  UCNVSEL_شاخص_شمار = 16
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
                  فراغ* میانگیر, int32_t میانگیر_ظرفیت, UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)میانگیر;
  إذا (میانگیر_ظرفیت < 0 ||
      (میانگیر_ظرفیت > 0 && (p == اشاره_تهی || (U_اشاره_گر_ماسک_LSB(p, 3) != 0)))
  ) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_اندازه = utrie2_serialize(sel->trie, اشاره_تهی, 0, وضعیت);
  إذا (*وضعیت != U_میانگیر_سرریز_ERROR && U_FAILURE(*وضعیت)) {
    أرجع 0;
  }
  *وضعیت = U_ZERO_ERROR;

  داده_سرآیند سرآیند;
  uprv_memset(&سرآیند, 0, حجم(سرآیند));
  سرآیند.داده_سرآیند_2.سرآیند_اندازه = (uint16_t)((حجم(سرآیند) + 15) & ~15);
  سرآیند.داده_سرآیند_2.magic1 = 0xda;
  سرآیند.داده_سرآیند_2.magic2 = 0x27;
  uprv_memcpy(&سرآیند.info, &داده_Info, حجم(داده_Info));

  int32_t indexes[UCNVSEL_شاخص_شمار] = {
    serialized_Trie_اندازه,
    sel->pv_شمار,
    sel->encodings_شمار,
    sel->encoding_Str_درازی
  };

  int32_t مجموع_اندازه =
    سرآیند.داده_سرآیند_2.سرآیند_اندازه +
    (int32_t)حجم(indexes) +
    serialized_Trie_اندازه +
    sel->pv_شمار * 4 +
    sel->encoding_Str_درازی;
  indexes[UCNVSEL_شاخص_اندازه] = مجموع_اندازه - سرآیند.داده_سرآیند_2.سرآیند_اندازه;
  إذا (مجموع_اندازه > میانگیر_ظرفیت) {
    *وضعیت = U_میانگیر_سرریز_ERROR;
    أرجع مجموع_اندازه;
  }
  // ok, save!
  int32_t length = سرآیند.داده_سرآیند_2.سرآیند_اندازه;
  uprv_memcpy(p, &سرآیند, حجم(سرآیند));
  uprv_memset(p + حجم(سرآیند), 0, length - حجم(سرآیند));
  p += length;

  length = (int32_t)حجم(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_اندازه, وضعیت);
  p += serialized_Trie_اندازه;

  length = sel->pv_شمار * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_درازی);
  p += sel->encoding_Str_درازی;

  أرجع مجموع_اندازه;
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
ساكن int32_t
ucnvsel_جابجا(ثابت UDataSwapper *ds,
             ثابت فراغ *درون_داده, int32_t length,
             فراغ *بیرون_داده, UError_کد *وضعیت) {
  /* udata_swapDataHeader checks the arguments */
  int32_t سرآیند_اندازه = udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, وضعیت);
  إذا(U_FAILURE(*وضعیت)) {
    أرجع 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت حرف*>(درون_داده) + 4);
  إذا(!(
    pInfo->داده_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->داده_Format[1] == 0x53 &&
    pInfo->داده_Format[2] == 0x65 &&
    pInfo->داده_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->داده_Format[0], pInfo->داده_Format[1],
                     pInfo->داده_Format[2], pInfo->داده_Format[3]);
    *وضعیت = U_INVALID_FORMAT_ERROR;
    أرجع 0;
  }
  إذا(pInfo->format_نسخه[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_نسخه[0]);
    *وضعیت = U_UNSUPPORTED_ERROR;
    أرجع 0;
  }

  إذا(length >= 0) {
    length -= سرآیند_اندازه;
    إذا(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }
  }

  ثابت uint8_t* درون_بایتها = static_cast<ثابت uint8_t*>(درون_داده) + سرآیند_اندازه;
  uint8_t* بیرون_بایتها = static_cast<uint8_t*>(بیرون_داده) + سرآیند_اندازه;

  /* read the indexes */
  ثابت int32_t* درون_Indexes = reinterpret_cast<ثابت int32_t*>(درون_بایتها);
  int32_t indexes[16];
  int32_t i;
  لكل(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, درون_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_شاخص_اندازه];
  إذا(length >= 0) {
    إذا(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }

    /* copy the data for inaccessible bytes */
    إذا(درون_بایتها != بیرون_بایتها) {
      uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
    }

    int32_t جابه_جایی = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_شاخص_شمار*4;
    ds->جابجا_Array32(ds, درون_بایتها, count, بیرون_بایتها, وضعیت);
    جابه_جایی += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_شاخص_TRIE_اندازه];
    utrie2_جابجا(ds, درون_بایتها + جابه_جایی, count, بیرون_بایتها + جابه_جایی, وضعیت);
    جابه_جایی += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_شاخص_PV_شمار]*4;
    ds->جابجا_Array32(ds, درون_بایتها + جابه_جایی, count, بیرون_بایتها + جابه_جایی, وضعیت);
    جابه_جایی += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_شاخص_نامها_درازی];
    ds->جابجا_Inv_Chars(ds, درون_بایتها + جابه_جایی, count, بیرون_بایتها + جابه_جایی, وضعیت);
    جابه_جایی += count;

    U_ASSERT(جابه_جایی == size);
  }

  أرجع سرآیند_اندازه + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت فراغ* میانگیر U_LIFETIME_BOUND,
                           int32_t length,
                           UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)میانگیر;
  إذا (length <= 0 ||
      (length > 0 && (p == اشاره_تهی || (U_اشاره_گر_ماسک_LSB(p, 3) != 0)))
  ) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }
  //سرآیند
  إذا (length < 32) {
    // not even enough space for a minimal header
    *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع اشاره_تهی;
  }
  ثابت داده_سرآیند *p_سرآیند = (ثابت داده_سرآیند *)p;
  إذا (!(
    p_سرآیند->داده_سرآیند_2.magic1==0xda &&
    p_سرآیند->داده_سرآیند_2.magic2==0x27 &&
    p_سرآیند->info.داده_Format[0] == 0x43 &&
    p_سرآیند->info.داده_Format[1] == 0x53 &&
    p_سرآیند->info.داده_Format[2] == 0x65 &&
    p_سرآیند->info.داده_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *وضعیت = U_INVALID_FORMAT_ERROR;
    أرجع اشاره_تهی;
  }
  إذا (p_سرآیند->info.format_نسخه[0] != 1) {
    *وضعیت = U_UNSUPPORTED_ERROR;
    أرجع اشاره_تهی;
  }
  uint8_t* swapped = اشاره_تهی;
  إذا (p_سرآیند->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_سرآیند->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_درون_داد_داده(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, وضعیت);
    int32_t مجموع_اندازه = ucnvsel_جابجا(ds, p, -1, اشاره_تهی, وضعیت);
    إذا (U_FAILURE(*وضعیت)) {
      udata_closeSwapper(ds);
      أرجع اشاره_تهی;
    }
    إذا (length < مجموع_اندازه) {
      udata_closeSwapper(ds);
      *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
      أرجع اشاره_تهی;
    }
    swapped = (uint8_t*)uprv_malloc(مجموع_اندازه);
    إذا (swapped == اشاره_تهی) {
      udata_closeSwapper(ds);
      *وضعیت = U_حافظه_تخصیص_ERROR;
      أرجع اشاره_تهی;
    }
    ucnvsel_جابجا(ds, p, length, swapped, وضعیت);
    udata_closeSwapper(ds);
    إذا (U_FAILURE(*وضعیت)) {
      uprv_free(swapped);
      أرجع اشاره_تهی;
    }
    p = swapped;
    p_سرآیند = (ثابت داده_سرآیند *)p;
  }
  إذا (length < (p_سرآیند->داده_سرآیند_2.سرآیند_اندازه + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع اشاره_تهی;
  }
  p += p_سرآیند->داده_سرآیند_2.سرآیند_اندازه;
  length -= p_سرآیند->داده_سرآیند_2.سرآیند_اندازه;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  إذا (length < indexes[UCNVSEL_شاخص_اندازه]) {
    uprv_free(swapped);
    *وضعیت = U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع اشاره_تهی;
  }
  p += UCNVSEL_شاخص_شمار * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(حجم(UConverterSelector));
  حرف **encodings =
    (حرف **)uprv_malloc(
      indexes[UCNVSEL_شاخص_نامها_شمار] * حجم(حرف *));
  إذا (sel == اشاره_تهی || encodings == اشاره_تهی) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  uprv_memset(sel, 0, حجم(UConverterSelector));
  sel->pv_شمار = indexes[UCNVSEL_شاخص_PV_شمار];
  sel->encodings = encodings;
  sel->encodings_شمار = indexes[UCNVSEL_شاخص_نامها_شمار];
  sel->encoding_Str_درازی = indexes[UCNVSEL_شاخص_نامها_درازی];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_مقدار_بیتها,
                                        p, indexes[UCNVSEL_شاخص_TRIE_اندازه], اشاره_تهی,
                                        وضعیت);
  p += indexes[UCNVSEL_شاخص_TRIE_اندازه];
  إذا (U_FAILURE(*وضعیت)) {
    ucnvsel_close(sel);
    أرجع اشاره_تهی;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_شمار * 4;
  // encoding names
  حرف* s = (حرف*)p;
  لكل (int32_t i = 0; i < sel->encodings_شمار; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_درازی;

  أرجع sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
بنية Enumerator {
  int16_t* شاخص;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->شاخص);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ساكن int32_t U_CALLCONV
ucnvsel_شمار_encodings(UEnumeration *enumerator, UError_کد *وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع 0;
  }
  أرجع ((Enumerator*)(enumerator->context))->length;
}


ساكن ثابت حرف* U_CALLCONV ucnvsel_بعدی_encoding(UEnumeration* enumerator,
                                                 int32_t* نتیجه_درازی,
                                                 UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت حرف* نتیجه;
  إذا (cur >= ((Enumerator*)(enumerator->context))->length) {
    أرجع اشاره_تهی;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  نتیجه = sel->encodings[((Enumerator*)(enumerator->context))->شاخص[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  إذا (نتیجه_درازی) {
    *نتیجه_درازی = (int32_t)uprv_strlen(نتیجه);
  }
  أرجع نتیجه;
}

ساكن فراغ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_انتها


ساكن ثابت UEnumeration defaultEncodings = {
  اشاره_تهی,
    اشاره_تهی,
    ucnvsel_close_selector_iterator,
    ucnvsel_شمار_encodings,
    uenum_unextDefault,
    ucnvsel_بعدی_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ساكن UBool intersectMasks(uint32_t* dest, ثابت uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  لكل (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  أرجع oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ساكن int16_t شمار_Ones(uint32_t* mask, int32_t len) {
  int32_t i, مجموع_Ones = 0;
  لكل (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    لكل (; ent; مجموع_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  أرجع static_cast<int16_t>(مجموع_Ones);
}


/* internal function! */
ساكن UEnumeration*
select_For_ماسک(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_ماسک,
              UError_کد* وضعیت) {
  Local_حافظه<uint32_t> mask(the_ماسک);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_حافظه<Enumerator> نتیجه(static_cast<Enumerator *>(uprv_malloc(حجم(Enumerator))));
  إذا (نتیجه.isNull()) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  نتیجه->شاخص = اشاره_تهی;  // this will be allocated later!
  نتیجه->length = نتیجه->cur = 0;
  نتیجه->sel = sel;

  Local_حافظه<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
  إذا (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  کپی_حافظه(en.getAlias(), &defaultEncodings, حجم(UEnumeration));
  
  int32_t columns = (sel->encodings_شمار+31)/32;
  int16_t رقم_Ones = شمار_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  إذا (رقم_Ones > 0) {
    نتیجه->شاخص = static_cast<int16_t*>(uprv_malloc(رقم_Ones * حجم(int16_t)));
    إذا (نتیجه->شاخص == اشاره_تهی) {
      *وضعیت = U_حافظه_تخصیص_ERROR;
      أرجع اشاره_تهی;
    }
    int32_t i, j;
    int16_t k = 0;
    لكل (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      لكل (i = 0 ; i < 32 && k < sel->encodings_شمار; i++, k++) {
        إذا ((v & 1) != 0) {
          نتیجه->شاخص[نتیجه->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = نتیجه.orphan();
  أرجع en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  // ensure args make sense!
  إذا (sel == اشاره_تهی || (s == اشاره_تهی && length != 0)) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }

  int32_t columns = (sel->encodings_شمار+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == اشاره_تهی) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا(s!=اشاره_تهی) {
    ثابت char16_t *حد;
    إذا (length >= 0) {
      حد = s + length;
    } وإلا {
      حد = اشاره_تهی;
    }
    
    طالما (حد == اشاره_تهی ? *s != 0 : s != حد) {
      UChar32 c;
      uint16_t pv_شاخص;
      UTRIE2_U16_NEXT16(sel->trie, s, حد, c, pv_شاخص);
      إذا (intersectMasks(mask, sel->pv+pv_شاخص, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_ماسک(sel, mask, وضعیت);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                      ثابت حرف* s,
                      int32_t length,
                      UError_کد* وضعیت) {
  // check if already failed
  إذا (U_FAILURE(*وضعیت)) {
    أرجع اشاره_تهی;
  }
  // ensure args make sense!
  إذا (sel == اشاره_تهی || (s == اشاره_تهی && length != 0)) {
    *وضعیت = U_ILLEGAL_آرگومان_ERROR;
    أرجع اشاره_تهی;
  }

  int32_t columns = (sel->encodings_شمار+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == اشاره_تهی) {
    *وضعیت = U_حافظه_تخصیص_ERROR;
    أرجع اشاره_تهی;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  إذا(s!=اشاره_تهی) {
    ثابت حرف *حد = s + length;
    
    طالما (s != حد) {
      uint16_t pv_شاخص;
      UTRIE2_U8_NEXT16(sel->trie, s, حد, pv_شاخص);
      إذا (intersectMasks(mask, sel->pv+pv_شاخص, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_ماسک(sel, mask, وضعیت);
}

#پایان_اگر  // !UCONFIG_NO_CONVERSION
