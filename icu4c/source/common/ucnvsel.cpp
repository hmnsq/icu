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

ساخت UConverterSelector {
  UTrie2 *trie;              // 16 bit trie containing offsets into pv
  uint32_t* pv;              // table of bits!
  int32_t pv_گنتی;
  حرف** encodings;          // which encodings did user ask to use?
  int32_t encodings_گنتی;
  int32_t encoding_Str_لمبائی;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_لڑیاں;
};

ساکن خالی generate_Selector_ڈیٹا(UConverterSelector* نتیجہ,
                                 UPropsVectors *upvec,
                                 ثابت USet* excluded_رمز_Points,
                                 ثابت UConverter_Unicode_سیٹ which_سیٹ,
                                 UError_رمز* حالت_2) {
  اگر (U_FAILURE(*حالت_2)) {
    واپس;
  }

  int32_t columns = (نتیجہ->encodings_گنتی+31)/32;

  // set errorValue to all-ones
  برائے (int32_t ستون = 0; ستون < columns; ستون++) {
    upvec_سیٹ_قدر(upvec, UPVEC_ERROR_قدر_CP, UPVEC_ERROR_قدر_CP,
                   ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), حالت_2);
  }

  برائے (int32_t i = 0; i < نتیجہ->encodings_گنتی; ++i) {
    uint32_t mask;
    uint32_t عمود;
    int32_t شے_گنتی;
    int32_t j;
    UConverter* test_converter = ucnv_open(نتیجہ->encodings[i], حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
      واپس;
    }
    USet* unicode_نقطہ_سیٹ;
    unicode_نقطہ_سیٹ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_سیٹ(test_converter, unicode_نقطہ_سیٹ,
                       which_سیٹ, حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
      ucnv_close(test_converter);
      واپس;
    }

    عمود = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    شے_گنتی = uset_get_شے_گنتی(unicode_نقطہ_سیٹ);

    برائے (j = 0; j < شے_گنتی; ++j) {
      UChar32 آغاز_char;
      UChar32 انتہا_char;
      UError_رمز small_حالت = U_ZERO_ERROR;
      uset_get_شے(unicode_نقطہ_سیٹ, j, &آغاز_char, &انتہا_char, خالی_اشاریہ, 0,
                   &small_حالت);
      اگر (U_FAILURE(small_حالت)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } ورنہ {
        upvec_سیٹ_قدر(upvec, آغاز_char, انتہا_char, عمود, static_cast<uint32_t>(~0), mask,
                       حالت_2);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_نقطہ_سیٹ);
    اگر (U_FAILURE(*حالت_2)) {
      واپس;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  اگر (excluded_رمز_Points) {
    int32_t شے_گنتی = uset_get_شے_گنتی(excluded_رمز_Points);
    برائے (int32_t j = 0; j < شے_گنتی; ++j) {
      UChar32 آغاز_char;
      UChar32 انتہا_char;

      uset_get_شے(excluded_رمز_Points, j, &آغاز_char, &انتہا_char, خالی_اشاریہ, 0,
                   حالت_2);
      برائے (int32_t ستون = 0; ستون < columns; ستون++) {
        upvec_سیٹ_قدر(upvec, آغاز_char, انتہا_char, ستون, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      حالت_2);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  نتیجہ->trie = upvec_compact_مقصد_UTrie2_With_صف_Indexes(upvec, حالت_2);
  نتیجہ->pv = upvec_cloneArray(upvec, &نتیجہ->pv_گنتی, خالی_اشاریہ, حالت_2);
  نتیجہ->pv_گنتی *= columns;  // number of uint32_t = rows * columns
  نتیجہ->ownPv = سچ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت حرف* ثابت*  converterList, int32_t converter_List_سائز,
             ثابت USet* excluded_رمز_Points,
             ثابت UConverter_Unicode_سیٹ which_سیٹ, UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  // ensure args make sense!
  اگر (converter_List_سائز < 0 || (converterList == خالی_اشاریہ && converter_List_سائز != 0)) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }

  // allocate a new converter
  Local_UConverter_Selector_اشارہ_گر newSelector(
    (UConverterSelector*)uprv_malloc(حجم(UConverterSelector)));
  اگر (newSelector.isNull()) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  uprv_memset(newSelector.getAlias(), 0, حجم(UConverterSelector));

  اگر (converter_List_سائز == 0) {
    converterList = خالی_اشاریہ;
    converter_List_سائز = ucnv_گنتی_دستیاب();
  }
  newSelector->encodings =
    (حرف**)uprv_malloc(converter_List_سائز * حجم(حرف*));
  اگر (!newSelector->encodings) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  newSelector->encodings[0] = خالی_اشاریہ;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t کل_سائز = 0;
  int32_t i;
  برائے (i = 0; i < converter_List_سائز; i++) {
    کل_سائز +=
      (int32_t)uprv_strlen(converterList != خالی_اشاریہ ? converterList[i] : ucnv_get_دستیاب_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = کل_سائز & 3;
  اگر (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_لمبائی = کل_سائز += encodingStrPadding;
  حرف* all_لڑیاں = (حرف*) uprv_malloc(کل_سائز);
  اگر (!all_لڑیاں) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }

  برائے (i = 0; i < converter_List_سائز; i++) {
    newSelector->encodings[i] = all_لڑیاں;
    uprv_strcpy(newSelector->encodings[i],
                converterList != خالی_اشاریہ ? converterList[i] : ucnv_get_دستیاب_Name(i));
    all_لڑیاں += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  جب (encodingStrPadding > 0) {
    *all_لڑیاں++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_لڑیاں = سچ;
  newSelector->encodings_گنتی = converter_List_سائز;
  UPropsVectors *upvec = upvec_open((converter_List_سائز+31)/32, حالت_2);
  generate_Selector_ڈیٹا(newSelector.getAlias(), upvec, excluded_رمز_Points, which_سیٹ, حالت_2);
  upvec_close(upvec);

  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }

  واپس newSelector.orphan();
}

/* close opened selector */
U_CAPI خالی U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  اگر (!sel) {
    واپس;
  }
  اگر (sel->own_Encoding_لڑیاں) {
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

ساکن ثابت UDataInfo ڈیٹا_Info = {
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

شمار {
  UCNVSEL_اشاریہ_TRIE_سائز,      // trie size in bytes
  UCNVSEL_اشاریہ_PV_گنتی,       // number of uint32_t in the bit vectors
  UCNVSEL_اشاریہ_نام_گنتی,    // number of encoding names
  UCNVSEL_اشاریہ_نام_لمبائی,   // number of encoding name bytes including padding
  UCNVSEL_اشاریہ_سائز = 15,      // bytes following the DataHeader
  UCNVSEL_اشاریہ_گنتی = 16
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
                  خالی* بفر, int32_t بفر_استعداد, UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)بفر;
  اگر (بفر_استعداد < 0 ||
      (بفر_استعداد > 0 && (p == خالی_اشاریہ || (U_اشارہ_گر_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_سائز = utrie2_serialize(sel->trie, خالی_اشاریہ, 0, حالت_2);
  اگر (*حالت_2 != U_بفر_حد_سے_تجاوز_ERROR && U_FAILURE(*حالت_2)) {
    واپس 0;
  }
  *حالت_2 = U_ZERO_ERROR;

  ڈیٹا_سرخی سرخی;
  uprv_memset(&سرخی, 0, حجم(سرخی));
  سرخی.ڈیٹا_سرخی_2.سرخی_سائز = (uint16_t)((حجم(سرخی) + 15) & ~15);
  سرخی.ڈیٹا_سرخی_2.magic1 = 0xda;
  سرخی.ڈیٹا_سرخی_2.magic2 = 0x27;
  uprv_memcpy(&سرخی.info, &ڈیٹا_Info, حجم(ڈیٹا_Info));

  int32_t indexes[UCNVSEL_اشاریہ_گنتی] = {
    serialized_Trie_سائز,
    sel->pv_گنتی,
    sel->encodings_گنتی,
    sel->encoding_Str_لمبائی
  };

  int32_t کل_سائز =
    سرخی.ڈیٹا_سرخی_2.سرخی_سائز +
    (int32_t)حجم(indexes) +
    serialized_Trie_سائز +
    sel->pv_گنتی * 4 +
    sel->encoding_Str_لمبائی;
  indexes[UCNVSEL_اشاریہ_سائز] = کل_سائز - سرخی.ڈیٹا_سرخی_2.سرخی_سائز;
  اگر (کل_سائز > بفر_استعداد) {
    *حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
    واپس کل_سائز;
  }
  // ok, save!
  int32_t length = سرخی.ڈیٹا_سرخی_2.سرخی_سائز;
  uprv_memcpy(p, &سرخی, حجم(سرخی));
  uprv_memset(p + حجم(سرخی), 0, length - حجم(سرخی));
  p += length;

  length = (int32_t)حجم(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_سائز, حالت_2);
  p += serialized_Trie_سائز;

  length = sel->pv_گنتی * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_لمبائی);
  p += sel->encoding_Str_لمبائی;

  واپس کل_سائز;
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
ساکن int32_t
ucnvsel_تبادلہ(ثابت UDataSwapper *ds,
             ثابت خالی *اندر_ڈیٹا, int32_t length,
             خالی *برآمد_ڈیٹا, UError_رمز *حالت_2) {
  /* udata_swapDataHeader checks the arguments */
  int32_t سرخی_سائز = udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, حالت_2);
  اگر(U_FAILURE(*حالت_2)) {
    واپس 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت حرف*>(اندر_ڈیٹا) + 4);
  اگر(!(
    pInfo->ڈیٹا_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->ڈیٹا_Format[1] == 0x53 &&
    pInfo->ڈیٹا_Format[2] == 0x65 &&
    pInfo->ڈیٹا_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                     pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3]);
    *حالت_2 = U_INVALID_FORMAT_ERROR;
    واپس 0;
  }
  اگر(pInfo->format_نسخہ[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_نسخہ[0]);
    *حالت_2 = U_UNSUPPORTED_ERROR;
    واپس 0;
  }

  اگر(length >= 0) {
    length -= سرخی_سائز;
    اگر(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
      واپس 0;
    }
  }

  ثابت uint8_t* اندر_بائٹیں = static_cast<ثابت uint8_t*>(اندر_ڈیٹا) + سرخی_سائز;
  uint8_t* برآمد_بائٹیں = static_cast<uint8_t*>(برآمد_ڈیٹا) + سرخی_سائز;

  /* read the indexes */
  ثابت int32_t* اندر_Indexes = reinterpret_cast<ثابت int32_t*>(اندر_بائٹیں);
  int32_t indexes[16];
  int32_t i;
  برائے(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, اندر_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_اشاریہ_سائز];
  اگر(length >= 0) {
    اگر(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
      واپس 0;
    }

    /* copy the data for inaccessible bytes */
    اگر(اندر_بائٹیں != برآمد_بائٹیں) {
      uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
    }

    int32_t انحراف = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_اشاریہ_گنتی*4;
    ds->تبادلہ_Array32(ds, اندر_بائٹیں, count, برآمد_بائٹیں, حالت_2);
    انحراف += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_اشاریہ_TRIE_سائز];
    utrie2_تبادلہ(ds, اندر_بائٹیں + انحراف, count, برآمد_بائٹیں + انحراف, حالت_2);
    انحراف += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_اشاریہ_PV_گنتی]*4;
    ds->تبادلہ_Array32(ds, اندر_بائٹیں + انحراف, count, برآمد_بائٹیں + انحراف, حالت_2);
    انحراف += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_اشاریہ_نام_لمبائی];
    ds->تبادلہ_Inv_Chars(ds, اندر_بائٹیں + انحراف, count, برآمد_بائٹیں + انحراف, حالت_2);
    انحراف += count;

    U_ASSERT(انحراف == size);
  }

  واپس سرخی_سائز + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت خالی* بفر U_LIFETIME_BOUND,
                           int32_t length,
                           UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)بفر;
  اگر (length <= 0 ||
      (length > 0 && (p == خالی_اشاریہ || (U_اشارہ_گر_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }
  //سرخی
  اگر (length < 32) {
    // not even enough space for a minimal header
    *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس خالی_اشاریہ;
  }
  ثابت ڈیٹا_سرخی *p_سرخی = (ثابت ڈیٹا_سرخی *)p;
  اگر (!(
    p_سرخی->ڈیٹا_سرخی_2.magic1==0xda &&
    p_سرخی->ڈیٹا_سرخی_2.magic2==0x27 &&
    p_سرخی->info.ڈیٹا_Format[0] == 0x43 &&
    p_سرخی->info.ڈیٹا_Format[1] == 0x53 &&
    p_سرخی->info.ڈیٹا_Format[2] == 0x65 &&
    p_سرخی->info.ڈیٹا_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *حالت_2 = U_INVALID_FORMAT_ERROR;
    واپس خالی_اشاریہ;
  }
  اگر (p_سرخی->info.format_نسخہ[0] != 1) {
    *حالت_2 = U_UNSUPPORTED_ERROR;
    واپس خالی_اشاریہ;
  }
  uint8_t* swapped = خالی_اشاریہ;
  اگر (p_سرخی->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_سرخی->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_داخلہ_ڈیٹا(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, حالت_2);
    int32_t کل_سائز = ucnvsel_تبادلہ(ds, p, -1, خالی_اشاریہ, حالت_2);
    اگر (U_FAILURE(*حالت_2)) {
      udata_closeSwapper(ds);
      واپس خالی_اشاریہ;
    }
    اگر (length < کل_سائز) {
      udata_closeSwapper(ds);
      *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
      واپس خالی_اشاریہ;
    }
    swapped = (uint8_t*)uprv_malloc(کل_سائز);
    اگر (swapped == خالی_اشاریہ) {
      udata_closeSwapper(ds);
      *حالت_2 = U_حافظہ_تخصیص_ERROR;
      واپس خالی_اشاریہ;
    }
    ucnvsel_تبادلہ(ds, p, length, swapped, حالت_2);
    udata_closeSwapper(ds);
    اگر (U_FAILURE(*حالت_2)) {
      uprv_free(swapped);
      واپس خالی_اشاریہ;
    }
    p = swapped;
    p_سرخی = (ثابت ڈیٹا_سرخی *)p;
  }
  اگر (length < (p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس خالی_اشاریہ;
  }
  p += p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز;
  length -= p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  اگر (length < indexes[UCNVSEL_اشاریہ_سائز]) {
    uprv_free(swapped);
    *حالت_2 = U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس خالی_اشاریہ;
  }
  p += UCNVSEL_اشاریہ_گنتی * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(حجم(UConverterSelector));
  حرف **encodings =
    (حرف **)uprv_malloc(
      indexes[UCNVSEL_اشاریہ_نام_گنتی] * حجم(حرف *));
  اگر (sel == خالی_اشاریہ || encodings == خالی_اشاریہ) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  uprv_memset(sel, 0, حجم(UConverterSelector));
  sel->pv_گنتی = indexes[UCNVSEL_اشاریہ_PV_گنتی];
  sel->encodings = encodings;
  sel->encodings_گنتی = indexes[UCNVSEL_اشاریہ_نام_گنتی];
  sel->encoding_Str_لمبائی = indexes[UCNVSEL_اشاریہ_نام_لمبائی];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_قدر_بٹ,
                                        p, indexes[UCNVSEL_اشاریہ_TRIE_سائز], خالی_اشاریہ,
                                        حالت_2);
  p += indexes[UCNVSEL_اشاریہ_TRIE_سائز];
  اگر (U_FAILURE(*حالت_2)) {
    ucnvsel_close(sel);
    واپس خالی_اشاریہ;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_گنتی * 4;
  // encoding names
  حرف* s = (حرف*)p;
  برائے (int32_t i = 0; i < sel->encodings_گنتی; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_لمبائی;

  واپس sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
ساخت Enumerator {
  int16_t* اشاریہ;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ساکن خالی U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->اشاریہ);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ساکن int32_t U_CALLCONV
ucnvsel_گنتی_encodings(UEnumeration *enumerator, UError_رمز *حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس 0;
  }
  واپس ((Enumerator*)(enumerator->context))->length;
}


ساکن ثابت حرف* U_CALLCONV ucnvsel_اگلا_encoding(UEnumeration* enumerator,
                                                 int32_t* نتیجہ_لمبائی,
                                                 UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت حرف* نتیجہ;
  اگر (cur >= ((Enumerator*)(enumerator->context))->length) {
    واپس خالی_اشاریہ;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  نتیجہ = sel->encodings[((Enumerator*)(enumerator->context))->اشاریہ[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  اگر (نتیجہ_لمبائی) {
    *نتیجہ_لمبائی = (int32_t)uprv_strlen(نتیجہ);
  }
  واپس نتیجہ;
}

ساکن خالی U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_انتہا


ساکن ثابت UEnumeration defaultEncodings = {
  خالی_اشاریہ,
    خالی_اشاریہ,
    ucnvsel_close_selector_iterator,
    ucnvsel_گنتی_encodings,
    uenum_unextDefault,
    ucnvsel_اگلا_encoding, 
    ucnvsel_reset_iterator
};


// internal fn to intersect two sets of masks
// returns whether the mask has reduced to all zeros
ساکن UBool intersectMasks(uint32_t* dest, ثابت uint32_t* source1, int32_t len) {
  int32_t i;
  uint32_t oredDest = 0;
  برائے (i = 0 ; i < len ; ++i) {
    oredDest |= (dest[i] &= source1[i]);
  }
  واپس oredDest == 0;
}

// internal fn to count how many 1's are there in a mask
// algorithm taken from  http://graphics.stanford.edu/~seander/bithacks.html
ساکن int16_t گنتی_Ones(uint32_t* mask, int32_t len) {
  int32_t i, کل_Ones = 0;
  برائے (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    برائے (; ent; کل_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  واپس static_cast<int16_t>(کل_Ones);
}


/* internal function! */
ساکن UEnumeration*
select_For_نقاب(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_نقاب,
              UError_رمز* حالت_2) {
  Local_حافظہ<uint32_t> mask(the_نقاب);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_حافظہ<Enumerator> نتیجہ(static_cast<Enumerator *>(uprv_malloc(حجم(Enumerator))));
  اگر (نتیجہ.isNull()) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  نتیجہ->اشاریہ = خالی_اشاریہ;  // this will be allocated later!
  نتیجہ->length = نتیجہ->cur = 0;
  نتیجہ->sel = sel;

  Local_حافظہ<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
  اگر (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  حافظہ_نقل(en.getAlias(), &defaultEncodings, حجم(UEnumeration));
  
  int32_t columns = (sel->encodings_گنتی+31)/32;
  int16_t عدد_Ones = گنتی_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  اگر (عدد_Ones > 0) {
    نتیجہ->اشاریہ = static_cast<int16_t*>(uprv_malloc(عدد_Ones * حجم(int16_t)));
    اگر (نتیجہ->اشاریہ == خالی_اشاریہ) {
      *حالت_2 = U_حافظہ_تخصیص_ERROR;
      واپس خالی_اشاریہ;
    }
    int32_t i, j;
    int16_t k = 0;
    برائے (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      برائے (i = 0 ; i < 32 && k < sel->encodings_گنتی; i++, k++) {
        اگر ((v & 1) != 0) {
          نتیجہ->اشاریہ[نتیجہ->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = نتیجہ.orphan();
  واپس en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  // ensure args make sense!
  اگر (sel == خالی_اشاریہ || (s == خالی_اشاریہ && length != 0)) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }

  int32_t columns = (sel->encodings_گنتی+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  اگر (mask == خالی_اشاریہ) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  uprv_memset(mask, ~0, columns *4);

  اگر(s!=خالی_اشاریہ) {
    ثابت char16_t *حد_بندی;
    اگر (length >= 0) {
      حد_بندی = s + length;
    } ورنہ {
      حد_بندی = خالی_اشاریہ;
    }
    
    جب (حد_بندی == خالی_اشاریہ ? *s != 0 : s != حد_بندی) {
      UChar32 c;
      uint16_t pv_اشاریہ;
      UTRIE2_U16_NEXT16(sel->trie, s, حد_بندی, c, pv_اشاریہ);
      اگر (intersectMasks(mask, sel->pv+pv_اشاریہ, columns)) {
        توڑو;
      }
    }
  }
  واپس select_For_نقاب(sel, mask, حالت_2);
}

/* check a string against the selector - UTF8 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                      ثابت حرف* s,
                      int32_t length,
                      UError_رمز* حالت_2) {
  // check if already failed
  اگر (U_FAILURE(*حالت_2)) {
    واپس خالی_اشاریہ;
  }
  // ensure args make sense!
  اگر (sel == خالی_اشاریہ || (s == خالی_اشاریہ && length != 0)) {
    *حالت_2 = U_ILLEGAL_دلیل_ERROR;
    واپس خالی_اشاریہ;
  }

  int32_t columns = (sel->encodings_گنتی+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  اگر (mask == خالی_اشاریہ) {
    *حالت_2 = U_حافظہ_تخصیص_ERROR;
    واپس خالی_اشاریہ;
  }
  uprv_memset(mask, ~0, columns *4);

  اگر (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  اگر(s!=خالی_اشاریہ) {
    ثابت حرف *حد_بندی = s + length;
    
    جب (s != حد_بندی) {
      uint16_t pv_اشاریہ;
      UTRIE2_U8_NEXT16(sel->trie, s, حد_بندی, pv_اشاریہ);
      اگر (intersectMasks(mask, sel->pv+pv_اشاریہ, columns)) {
        توڑو;
      }
    }
  }
  واپس select_For_نقاب(sel, mask, حالت_2);
}

#اگر_ختم  // !UCONFIG_NO_CONVERSION
