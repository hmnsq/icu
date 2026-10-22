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
  int32_t pv_ڳڻپ;
  حرف** encodings;          // which encodings did user ask to use?
  int32_t encodings_ڳڻپ;
  int32_t encoding_Str_ڊيگهائي;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_لڙيون;
};

ساكن فراغ generate_Selector_معلومات(UConverterSelector* نتيجو,
                                 UPropsVectors *upvec,
                                 ثابت USet* excluded_ڪوڊ_Points,
                                 ثابت UConverter_Unicode_سيٽ which_سيٽ,
                                 UError_ڪوڊ* حالت) {
  إذا (U_FAILURE(*حالت)) {
    أرجع;
  }

  int32_t columns = (نتيجو->encodings_ڳڻپ+31)/32;

  // set errorValue to all-ones
  لكل (int32_t کانو = 0; کانو < columns; کانو++) {
    upvec_سيٽ_قدر(upvec, UPVEC_ERROR_قدر_CP, UPVEC_ERROR_قدر_CP,
                   کانو, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), حالت);
  }

  لكل (int32_t i = 0; i < نتيجو->encodings_ڳڻپ; ++i) {
    uint32_t mask;
    uint32_t ڪالم;
    int32_t شي_ڳڻپ;
    int32_t j;
    UConverter* test_converter = ucnv_open(نتيجو->encodings[i], حالت);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
    USet* unicode_نقطو_سيٽ;
    unicode_نقطو_سيٽ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_سيٽ(test_converter, unicode_نقطو_سيٽ,
                       which_سيٽ, حالت);
    إذا (U_FAILURE(*حالت)) {
      ucnv_close(test_converter);
      أرجع;
    }

    ڪالم = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    شي_ڳڻپ = uset_get_شي_ڳڻپ(unicode_نقطو_سيٽ);

    لكل (j = 0; j < شي_ڳڻپ; ++j) {
      UChar32 شروعات_char;
      UChar32 پڇاڙي_char;
      UError_ڪوڊ small_حالت = U_ZERO_ERROR;
      uset_get_شي(unicode_نقطو_سيٽ, j, &شروعات_char, &پڇاڙي_char, خالي_اشارو, 0,
                   &small_حالت);
      إذا (U_FAILURE(small_حالت)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } وإلا {
        upvec_سيٽ_قدر(upvec, شروعات_char, پڇاڙي_char, ڪالم, static_cast<uint32_t>(~0), mask,
                       حالت);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_نقطو_سيٽ);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  إذا (excluded_ڪوڊ_Points) {
    int32_t شي_ڳڻپ = uset_get_شي_ڳڻپ(excluded_ڪوڊ_Points);
    لكل (int32_t j = 0; j < شي_ڳڻپ; ++j) {
      UChar32 شروعات_char;
      UChar32 پڇاڙي_char;

      uset_get_شي(excluded_ڪوڊ_Points, j, &شروعات_char, &پڇاڙي_char, خالي_اشارو, 0,
                   حالت);
      لكل (int32_t کانو = 0; کانو < columns; کانو++) {
        upvec_سيٽ_قدر(upvec, شروعات_char, پڇاڙي_char, کانو, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      حالت);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  نتيجو->trie = upvec_compact_منزل_UTrie2_With_صف_Indexes(upvec, حالت);
  نتيجو->pv = upvec_cloneArray(upvec, &نتيجو->pv_ڳڻپ, خالي_اشارو, حالت);
  نتيجو->pv_ڳڻپ *= columns;  // number of uint32_t = rows * columns
  نتيجو->ownPv = سچ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت حرف* ثابت*  converterList, int32_t converter_List_سائيز,
             ثابت USet* excluded_ڪوڊ_Points,
             ثابت UConverter_Unicode_سيٽ which_سيٽ, UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  // ensure args make sense!
  إذا (converter_List_سائيز < 0 || (converterList == خالي_اشارو && converter_List_سائيز != 0)) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }

  // allocate a new converter
  Local_UConverter_Selector_اشارو newSelector(
    (UConverterSelector*)uprv_malloc(حجم(UConverterSelector)));
  إذا (newSelector.isNull()) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  uprv_memset(newSelector.getAlias(), 0, حجم(UConverterSelector));

  إذا (converter_List_سائيز == 0) {
    converterList = خالي_اشارو;
    converter_List_سائيز = ucnv_ڳڻپ_موجود();
  }
  newSelector->encodings =
    (حرف**)uprv_malloc(converter_List_سائيز * حجم(حرف*));
  إذا (!newSelector->encodings) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  newSelector->encodings[0] = خالي_اشارو;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t ڪل_سائيز = 0;
  int32_t i;
  لكل (i = 0; i < converter_List_سائيز; i++) {
    ڪل_سائيز +=
      (int32_t)uprv_strlen(converterList != خالي_اشارو ? converterList[i] : ucnv_get_موجود_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = ڪل_سائيز & 3;
  إذا (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_ڊيگهائي = ڪل_سائيز += encodingStrPadding;
  حرف* all_لڙيون = (حرف*) uprv_malloc(ڪل_سائيز);
  إذا (!all_لڙيون) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }

  لكل (i = 0; i < converter_List_سائيز; i++) {
    newSelector->encodings[i] = all_لڙيون;
    uprv_strcpy(newSelector->encodings[i],
                converterList != خالي_اشارو ? converterList[i] : ucnv_get_موجود_Name(i));
    all_لڙيون += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  طالما (encodingStrPadding > 0) {
    *all_لڙيون++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_لڙيون = سچ;
  newSelector->encodings_ڳڻپ = converter_List_سائيز;
  UPropsVectors *upvec = upvec_open((converter_List_سائيز+31)/32, حالت);
  generate_Selector_معلومات(newSelector.getAlias(), upvec, excluded_ڪوڊ_Points, which_سيٽ, حالت);
  upvec_close(upvec);

  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }

  أرجع newSelector.orphan();
}

/* close opened selector */
U_CAPI فراغ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  إذا (!sel) {
    أرجع;
  }
  إذا (sel->own_Encoding_لڙيون) {
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

ساكن ثابت UDataInfo معلومات_Info = {
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
  UCNVSEL_اشاريو_TRIE_سائيز,      // trie size in bytes
  UCNVSEL_اشاريو_PV_ڳڻپ,       // number of uint32_t in the bit vectors
  UCNVSEL_اشاريو_نالا_ڳڻپ,    // number of encoding names
  UCNVSEL_اشاريو_نالا_ڊيگهائي,   // number of encoding name bytes including padding
  UCNVSEL_اشاريو_سائيز = 15,      // bytes following the DataHeader
  UCNVSEL_اشاريو_ڳڻپ = 16
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
                  فراغ* بفر, int32_t بفر_صلاحيت, UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)بفر;
  إذا (بفر_صلاحيت < 0 ||
      (بفر_صلاحيت > 0 && (p == خالي_اشارو || (U_اشارو_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_سائيز = utrie2_serialize(sel->trie, خالي_اشارو, 0, حالت);
  إذا (*حالت != U_بفر_حد_کان_مٿي_ERROR && U_FAILURE(*حالت)) {
    أرجع 0;
  }
  *حالت = U_ZERO_ERROR;

  معلومات_سرو سرو;
  uprv_memset(&سرو, 0, حجم(سرو));
  سرو.معلومات_سرو_2.سرو_سائيز = (uint16_t)((حجم(سرو) + 15) & ~15);
  سرو.معلومات_سرو_2.magic1 = 0xda;
  سرو.معلومات_سرو_2.magic2 = 0x27;
  uprv_memcpy(&سرو.info, &معلومات_Info, حجم(معلومات_Info));

  int32_t indexes[UCNVSEL_اشاريو_ڳڻپ] = {
    serialized_Trie_سائيز,
    sel->pv_ڳڻپ,
    sel->encodings_ڳڻپ,
    sel->encoding_Str_ڊيگهائي
  };

  int32_t ڪل_سائيز =
    سرو.معلومات_سرو_2.سرو_سائيز +
    (int32_t)حجم(indexes) +
    serialized_Trie_سائيز +
    sel->pv_ڳڻپ * 4 +
    sel->encoding_Str_ڊيگهائي;
  indexes[UCNVSEL_اشاريو_سائيز] = ڪل_سائيز - سرو.معلومات_سرو_2.سرو_سائيز;
  إذا (ڪل_سائيز > بفر_صلاحيت) {
    *حالت = U_بفر_حد_کان_مٿي_ERROR;
    أرجع ڪل_سائيز;
  }
  // ok, save!
  int32_t length = سرو.معلومات_سرو_2.سرو_سائيز;
  uprv_memcpy(p, &سرو, حجم(سرو));
  uprv_memset(p + حجم(سرو), 0, length - حجم(سرو));
  p += length;

  length = (int32_t)حجم(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_سائيز, حالت);
  p += serialized_Trie_سائيز;

  length = sel->pv_ڳڻپ * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_ڊيگهائي);
  p += sel->encoding_Str_ڊيگهائي;

  أرجع ڪل_سائيز;
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
ucnvsel_مٽا_سٽا(ثابت UDataSwapper *ds,
             ثابت فراغ *اندريون_معلومات, int32_t length,
             فراغ *ٻاهريون_معلومات, UError_ڪوڊ *حالت) {
  /* udata_swapDataHeader checks the arguments */
  int32_t سرو_سائيز = udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت حرف*>(اندريون_معلومات) + 4);
  إذا(!(
    pInfo->معلومات_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->معلومات_Format[1] == 0x53 &&
    pInfo->معلومات_Format[2] == 0x65 &&
    pInfo->معلومات_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                     pInfo->معلومات_Format[2], pInfo->معلومات_Format[3]);
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع 0;
  }
  إذا(pInfo->format_نسخو[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_نسخو[0]);
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع 0;
  }

  إذا(length >= 0) {
    length -= سرو_سائيز;
    إذا(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }
  }

  ثابت uint8_t* اندريون_بائيٽون = static_cast<ثابت uint8_t*>(اندريون_معلومات) + سرو_سائيز;
  uint8_t* ٻاهريون_بائيٽون = static_cast<uint8_t*>(ٻاهريون_معلومات) + سرو_سائيز;

  /* read the indexes */
  ثابت int32_t* اندريون_Indexes = reinterpret_cast<ثابت int32_t*>(اندريون_بائيٽون);
  int32_t indexes[16];
  int32_t i;
  لكل(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, اندريون_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_اشاريو_سائيز];
  إذا(length >= 0) {
    إذا(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }

    /* copy the data for inaccessible bytes */
    إذا(اندريون_بائيٽون != ٻاهريون_بائيٽون) {
      uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, size);
    }

    int32_t وٿي = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_اشاريو_ڳڻپ*4;
    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, count, ٻاهريون_بائيٽون, حالت);
    وٿي += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_اشاريو_TRIE_سائيز];
    utrie2_مٽا_سٽا(ds, اندريون_بائيٽون + وٿي, count, ٻاهريون_بائيٽون + وٿي, حالت);
    وٿي += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_اشاريو_PV_ڳڻپ]*4;
    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون + وٿي, count, ٻاهريون_بائيٽون + وٿي, حالت);
    وٿي += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_اشاريو_نالا_ڊيگهائي];
    ds->مٽا_سٽا_Inv_Chars(ds, اندريون_بائيٽون + وٿي, count, ٻاهريون_بائيٽون + وٿي, حالت);
    وٿي += count;

    U_ASSERT(وٿي == size);
  }

  أرجع سرو_سائيز + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت فراغ* بفر U_LIFETIME_BOUND,
                           int32_t length,
                           UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)بفر;
  إذا (length <= 0 ||
      (length > 0 && (p == خالي_اشارو || (U_اشارو_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }
  //سرو
  إذا (length < 32) {
    // not even enough space for a minimal header
    *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع خالي_اشارو;
  }
  ثابت معلومات_سرو *p_سرو = (ثابت معلومات_سرو *)p;
  إذا (!(
    p_سرو->معلومات_سرو_2.magic1==0xda &&
    p_سرو->معلومات_سرو_2.magic2==0x27 &&
    p_سرو->info.معلومات_Format[0] == 0x43 &&
    p_سرو->info.معلومات_Format[1] == 0x53 &&
    p_سرو->info.معلومات_Format[2] == 0x65 &&
    p_سرو->info.معلومات_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع خالي_اشارو;
  }
  إذا (p_سرو->info.format_نسخو[0] != 1) {
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع خالي_اشارو;
  }
  uint8_t* swapped = خالي_اشارو;
  إذا (p_سرو->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_سرو->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_داخلا_معلومات(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, حالت);
    int32_t ڪل_سائيز = ucnvsel_مٽا_سٽا(ds, p, -1, خالي_اشارو, حالت);
    إذا (U_FAILURE(*حالت)) {
      udata_closeSwapper(ds);
      أرجع خالي_اشارو;
    }
    إذا (length < ڪل_سائيز) {
      udata_closeSwapper(ds);
      *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
      أرجع خالي_اشارو;
    }
    swapped = (uint8_t*)uprv_malloc(ڪل_سائيز);
    إذا (swapped == خالي_اشارو) {
      udata_closeSwapper(ds);
      *حالت = U_يادگيري_ورهاست_ERROR;
      أرجع خالي_اشارو;
    }
    ucnvsel_مٽا_سٽا(ds, p, length, swapped, حالت);
    udata_closeSwapper(ds);
    إذا (U_FAILURE(*حالت)) {
      uprv_free(swapped);
      أرجع خالي_اشارو;
    }
    p = swapped;
    p_سرو = (ثابت معلومات_سرو *)p;
  }
  إذا (length < (p_سرو->معلومات_سرو_2.سرو_سائيز + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع خالي_اشارو;
  }
  p += p_سرو->معلومات_سرو_2.سرو_سائيز;
  length -= p_سرو->معلومات_سرو_2.سرو_سائيز;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  إذا (length < indexes[UCNVSEL_اشاريو_سائيز]) {
    uprv_free(swapped);
    *حالت = U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع خالي_اشارو;
  }
  p += UCNVSEL_اشاريو_ڳڻپ * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(حجم(UConverterSelector));
  حرف **encodings =
    (حرف **)uprv_malloc(
      indexes[UCNVSEL_اشاريو_نالا_ڳڻپ] * حجم(حرف *));
  إذا (sel == خالي_اشارو || encodings == خالي_اشارو) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  uprv_memset(sel, 0, حجم(UConverterSelector));
  sel->pv_ڳڻپ = indexes[UCNVSEL_اشاريو_PV_ڳڻپ];
  sel->encodings = encodings;
  sel->encodings_ڳڻپ = indexes[UCNVSEL_اشاريو_نالا_ڳڻپ];
  sel->encoding_Str_ڊيگهائي = indexes[UCNVSEL_اشاريو_نالا_ڊيگهائي];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_قدر_بٽ,
                                        p, indexes[UCNVSEL_اشاريو_TRIE_سائيز], خالي_اشارو,
                                        حالت);
  p += indexes[UCNVSEL_اشاريو_TRIE_سائيز];
  إذا (U_FAILURE(*حالت)) {
    ucnvsel_close(sel);
    أرجع خالي_اشارو;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_ڳڻپ * 4;
  // encoding names
  حرف* s = (حرف*)p;
  لكل (int32_t i = 0; i < sel->encodings_ڳڻپ; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_ڊيگهائي;

  أرجع sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
بنية Enumerator {
  int16_t* اشاريو;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->اشاريو);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ساكن int32_t U_CALLCONV
ucnvsel_ڳڻپ_encodings(UEnumeration *enumerator, UError_ڪوڊ *حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  أرجع ((Enumerator*)(enumerator->context))->length;
}


ساكن ثابت حرف* U_CALLCONV ucnvsel_ايندڙ_encoding(UEnumeration* enumerator,
                                                 int32_t* نتيجو_ڊيگهائي,
                                                 UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت حرف* نتيجو;
  إذا (cur >= ((Enumerator*)(enumerator->context))->length) {
    أرجع خالي_اشارو;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  نتيجو = sel->encodings[((Enumerator*)(enumerator->context))->اشاريو[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  إذا (نتيجو_ڊيگهائي) {
    *نتيجو_ڊيگهائي = (int32_t)uprv_strlen(نتيجو);
  }
  أرجع نتيجو;
}

ساكن فراغ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_پڇاڙي


ساكن ثابت UEnumeration defaultEncodings = {
  خالي_اشارو,
    خالي_اشارو,
    ucnvsel_close_selector_iterator,
    ucnvsel_ڳڻپ_encodings,
    uenum_unextDefault,
    ucnvsel_ايندڙ_encoding, 
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
ساكن int16_t ڳڻپ_Ones(uint32_t* mask, int32_t len) {
  int32_t i, ڪل_Ones = 0;
  لكل (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    لكل (; ent; ڪل_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  أرجع static_cast<int16_t>(ڪل_Ones);
}


/* internal function! */
ساكن UEnumeration*
select_For_نقاب(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_نقاب,
              UError_ڪوڊ* حالت) {
  Local_يادگيري<uint32_t> mask(the_نقاب);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_يادگيري<Enumerator> نتيجو(static_cast<Enumerator *>(uprv_malloc(حجم(Enumerator))));
  إذا (نتيجو.isNull()) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  نتيجو->اشاريو = خالي_اشارو;  // this will be allocated later!
  نتيجو->length = نتيجو->cur = 0;
  نتيجو->sel = sel;

  Local_يادگيري<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
  إذا (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  يادگيري_نقل(en.getAlias(), &defaultEncodings, حجم(UEnumeration));
  
  int32_t columns = (sel->encodings_ڳڻپ+31)/32;
  int16_t انگ_Ones = ڳڻپ_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  إذا (انگ_Ones > 0) {
    نتيجو->اشاريو = static_cast<int16_t*>(uprv_malloc(انگ_Ones * حجم(int16_t)));
    إذا (نتيجو->اشاريو == خالي_اشارو) {
      *حالت = U_يادگيري_ورهاست_ERROR;
      أرجع خالي_اشارو;
    }
    int32_t i, j;
    int16_t k = 0;
    لكل (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      لكل (i = 0 ; i < 32 && k < sel->encodings_ڳڻپ; i++, k++) {
        إذا ((v & 1) != 0) {
          نتيجو->اشاريو[نتيجو->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = نتيجو.orphan();
  أرجع en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  // ensure args make sense!
  إذا (sel == خالي_اشارو || (s == خالي_اشارو && length != 0)) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }

  int32_t columns = (sel->encodings_ڳڻپ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == خالي_اشارو) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا(s!=خالي_اشارو) {
    ثابت char16_t *حد_بندي;
    إذا (length >= 0) {
      حد_بندي = s + length;
    } وإلا {
      حد_بندي = خالي_اشارو;
    }
    
    طالما (حد_بندي == خالي_اشارو ? *s != 0 : s != حد_بندي) {
      UChar32 c;
      uint16_t pv_اشاريو;
      UTRIE2_U16_NEXT16(sel->trie, s, حد_بندي, c, pv_اشاريو);
      إذا (intersectMasks(mask, sel->pv+pv_اشاريو, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_نقاب(sel, mask, حالت);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                      ثابت حرف* s,
                      int32_t length,
                      UError_ڪوڊ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالي_اشارو;
  }
  // ensure args make sense!
  إذا (sel == خالي_اشارو || (s == خالي_اشارو && length != 0)) {
    *حالت = U_ILLEGAL_دليل_ERROR;
    أرجع خالي_اشارو;
  }

  int32_t columns = (sel->encodings_ڳڻپ+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == خالي_اشارو) {
    *حالت = U_يادگيري_ورهاست_ERROR;
    أرجع خالي_اشارو;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  إذا(s!=خالي_اشارو) {
    ثابت حرف *حد_بندي = s + length;
    
    طالما (s != حد_بندي) {
      uint16_t pv_اشاريو;
      UTRIE2_U8_NEXT16(sel->trie, s, حد_بندي, pv_اشاريو);
      إذا (intersectMasks(mask, sel->pv+pv_اشاريو, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_نقاب(sel, mask, حالت);
}

#جيڪڏهن_پورو  // !UCONFIG_NO_CONVERSION
