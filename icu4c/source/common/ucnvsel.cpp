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
  int32_t pv_گنتی;
  حرف** encodings;          // which encodings did user ask to use?
  int32_t encodings_گنتی;
  int32_t encoding_Str_لمائی;
  uint8_t* swapped;
  UBool ownPv, own_Encoding_لڑیاں;
};

ساكن فراغ generate_Selector_انکڑے(UConverterSelector* نتیجہ,
                                 UPropsVectors *upvec,
                                 ثابت USet* excluded_کوڈ_Points,
                                 ثابت UConverter_Unicode_سیٹ which_سیٹ,
                                 UError_کوڈ* حالت) {
  إذا (U_FAILURE(*حالت)) {
    أرجع;
  }

  int32_t columns = (نتیجہ->encodings_گنتی+31)/32;

  // set errorValue to all-ones
  لكل (int32_t خانہ = 0; خانہ < columns; خانہ++) {
    upvec_سیٹ_قدر(upvec, UPVEC_ERROR_قدر_CP, UPVEC_ERROR_قدر_CP,
                   خانہ, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0), حالت);
  }

  لكل (int32_t i = 0; i < نتیجہ->encodings_گنتی; ++i) {
    uint32_t mask;
    uint32_t کالم;
    int32_t شے_گنتی;
    int32_t j;
    UConverter* test_converter = ucnv_open(نتیجہ->encodings[i], حالت);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
    USet* unicode_نقطہ_سیٹ;
    unicode_نقطہ_سیٹ = uset_open(1, 0);  // empty set

    ucnv_get_Unicode_سیٹ(test_converter, unicode_نقطہ_سیٹ,
                       which_سیٹ, حالت);
    إذا (U_FAILURE(*حالت)) {
      ucnv_close(test_converter);
      أرجع;
    }

    کالم = i / 32;
    mask = 1 << (i%32);
    // now iterate over intervals on set i!
    شے_گنتی = uset_get_شے_گنتی(unicode_نقطہ_سیٹ);

    لكل (j = 0; j < شے_گنتی; ++j) {
      UChar32 شروع_char;
      UChar32 سرا_char;
      UError_کوڈ small_حالت = U_ZERO_ERROR;
      uset_get_شے(unicode_نقطہ_سیٹ, j, &شروع_char, &سرا_char, خالی_سوچک, 0,
                   &small_حالت);
      إذا (U_FAILURE(small_حالت)) {
        // this will be reached for the converters that fill the set with
        // strings. Those should be ignored by our system
      } وإلا {
        upvec_سیٹ_قدر(upvec, شروع_char, سرا_char, کالم, static_cast<uint32_t>(~0), mask,
                       حالت);
      }
    }
    ucnv_close(test_converter);
    uset_close(unicode_نقطہ_سیٹ);
    إذا (U_FAILURE(*حالت)) {
      أرجع;
    }
  }

  // handle excluded encodings! Simply set their values to all 1's in the upvec
  إذا (excluded_کوڈ_Points) {
    int32_t شے_گنتی = uset_get_شے_گنتی(excluded_کوڈ_Points);
    لكل (int32_t j = 0; j < شے_گنتی; ++j) {
      UChar32 شروع_char;
      UChar32 سرا_char;

      uset_get_شے(excluded_کوڈ_Points, j, &شروع_char, &سرا_char, خالی_سوچک, 0,
                   حالت);
      لكل (int32_t خانہ = 0; خانہ < columns; خانہ++) {
        upvec_سیٹ_قدر(upvec, شروع_char, سرا_char, خانہ, static_cast<uint32_t>(~0), static_cast<uint32_t>(~0),
                      حالت);
      }
    }
  }

  // alright. Now, let's put things in the same exact form you'd get when you
  // unserialize things.
  نتیجہ->trie = upvec_compact_منزل_UTrie2_With_صف_Indexes(upvec, حالت);
  نتیجہ->pv = upvec_cloneArray(upvec, &نتیجہ->pv_گنتی, خالی_سوچک, حالت);
  نتیجہ->pv_گنتی *= columns;  // number of uint32_t = rows * columns
  نتیجہ->ownPv = سچ;
}

/* open a selector. If converterListSize is 0, build for all converters.
   If excludedCodePoints is nullptr, don't exclude any codepoints */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(ثابت حرف* ثابت*  converterList, int32_t converter_List_ناپ,
             ثابت USet* excluded_کوڈ_Points,
             ثابت UConverter_Unicode_سیٹ which_سیٹ, UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  // ensure args make sense!
  إذا (converter_List_ناپ < 0 || (converterList == خالی_سوچک && converter_List_ناپ != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }

  // allocate a new converter
  Local_UConverter_Selector_اشارہ_گر newSelector(
    (UConverterSelector*)uprv_malloc(حجم(UConverterSelector)));
  إذا (newSelector.isNull()) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  uprv_memset(newSelector.getAlias(), 0, حجم(UConverterSelector));

  إذا (converter_List_ناپ == 0) {
    converterList = خالی_سوچک;
    converter_List_ناپ = ucnv_گنتی_دستیاب();
  }
  newSelector->encodings =
    (حرف**)uprv_malloc(converter_List_ناپ * حجم(حرف*));
  إذا (!newSelector->encodings) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  newSelector->encodings[0] = خالی_سوچک;  // now we can call ucnvsel_close()

  // make a backup copy of the list of converters
  int32_t کل_ناپ = 0;
  int32_t i;
  لكل (i = 0; i < converter_List_ناپ; i++) {
    کل_ناپ +=
      (int32_t)uprv_strlen(converterList != خالی_سوچک ? converterList[i] : ucnv_get_دستیاب_Name(i)) + 1;
  }
  // 4-align the totalSize to 4-align the size of the serialized form
  int32_t encodingStrPadding = کل_ناپ & 3;
  إذا (encodingStrPadding != 0) {
    encodingStrPadding = 4 - encodingStrPadding;
  }
  newSelector->encoding_Str_لمائی = کل_ناپ += encodingStrPadding;
  حرف* all_لڑیاں = (حرف*) uprv_malloc(کل_ناپ);
  إذا (!all_لڑیاں) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }

  لكل (i = 0; i < converter_List_ناپ; i++) {
    newSelector->encodings[i] = all_لڑیاں;
    uprv_strcpy(newSelector->encodings[i],
                converterList != خالی_سوچک ? converterList[i] : ucnv_get_دستیاب_Name(i));
    all_لڑیاں += uprv_strlen(newSelector->encodings[i]) + 1;
  }
  طالما (encodingStrPadding > 0) {
    *all_لڑیاں++ = 0;
    --encodingStrPadding;
  }

  newSelector->own_Encoding_لڑیاں = سچ;
  newSelector->encodings_گنتی = converter_List_ناپ;
  UPropsVectors *upvec = upvec_open((converter_List_ناپ+31)/32, حالت);
  generate_Selector_انکڑے(newSelector.getAlias(), upvec, excluded_کوڈ_Points, which_سیٹ, حالت);
  upvec_close(upvec);

  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }

  أرجع newSelector.orphan();
}

/* close opened selector */
U_CAPI فراغ U_EXPORT2
ucnvsel_close(UConverterSelector *sel) {
  إذا (!sel) {
    أرجع;
  }
  إذا (sel->own_Encoding_لڑیاں) {
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

ساكن ثابت UDataInfo انکڑے_Info = {
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
  UCNVSEL_اشاریہ_TRIE_ناپ,      // trie size in bytes
  UCNVSEL_اشاریہ_PV_گنتی,       // number of uint32_t in the bit vectors
  UCNVSEL_اشاریہ_ناں_گنتی,    // number of encoding names
  UCNVSEL_اشاریہ_ناں_لمائی,   // number of encoding name bytes including padding
  UCNVSEL_اشاریہ_ناپ = 15,      // bytes following the DataHeader
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
                  فراغ* بفر, int32_t بفر_سمرتھا, UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  // ensure args make sense!
  uint8_t *p = (uint8_t *)بفر;
  إذا (بفر_سمرتھا < 0 ||
      (بفر_سمرتھا > 0 && (p == خالی_سوچک || (U_اشارہ_گر_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع 0;
  }
  // add up the size of the serialized form
  int32_t serialized_Trie_ناپ = utrie2_serialize(sel->trie, خالی_سوچک, 0, حالت);
  إذا (*حالت != U_بفر_حد_توں_ودھ_ERROR && U_FAILURE(*حالت)) {
    أرجع 0;
  }
  *حالت = U_ZERO_ERROR;

  انکڑے_سرخی سرخی;
  uprv_memset(&سرخی, 0, حجم(سرخی));
  سرخی.انکڑے_سرخی_2.سرخی_ناپ = (uint16_t)((حجم(سرخی) + 15) & ~15);
  سرخی.انکڑے_سرخی_2.magic1 = 0xda;
  سرخی.انکڑے_سرخی_2.magic2 = 0x27;
  uprv_memcpy(&سرخی.info, &انکڑے_Info, حجم(انکڑے_Info));

  int32_t indexes[UCNVSEL_اشاریہ_گنتی] = {
    serialized_Trie_ناپ,
    sel->pv_گنتی,
    sel->encodings_گنتی,
    sel->encoding_Str_لمائی
  };

  int32_t کل_ناپ =
    سرخی.انکڑے_سرخی_2.سرخی_ناپ +
    (int32_t)حجم(indexes) +
    serialized_Trie_ناپ +
    sel->pv_گنتی * 4 +
    sel->encoding_Str_لمائی;
  indexes[UCNVSEL_اشاریہ_ناپ] = کل_ناپ - سرخی.انکڑے_سرخی_2.سرخی_ناپ;
  إذا (کل_ناپ > بفر_سمرتھا) {
    *حالت = U_بفر_حد_توں_ودھ_ERROR;
    أرجع کل_ناپ;
  }
  // ok, save!
  int32_t length = سرخی.انکڑے_سرخی_2.سرخی_ناپ;
  uprv_memcpy(p, &سرخی, حجم(سرخی));
  uprv_memset(p + حجم(سرخی), 0, length - حجم(سرخی));
  p += length;

  length = (int32_t)حجم(indexes);
  uprv_memcpy(p, indexes, length);
  p += length;

  utrie2_serialize(sel->trie, p, serialized_Trie_ناپ, حالت);
  p += serialized_Trie_ناپ;

  length = sel->pv_گنتی * 4;
  uprv_memcpy(p, sel->pv, length);
  p += length;

  uprv_memcpy(p, sel->encodings[0], sel->encoding_Str_لمائی);
  p += sel->encoding_Str_لمائی;

  أرجع کل_ناپ;
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
ucnvsel_وٹاندرا(ثابت UDataSwapper *ds,
             ثابت فراغ *وچ_انکڑے, int32_t length,
             فراغ *باہرلا_انکڑے, UError_کوڈ *حالت) {
  /* udata_swapDataHeader checks the arguments */
  int32_t سرخی_ناپ = udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, حالت);
  إذا(U_FAILURE(*حالت)) {
    أرجع 0;
  }

  /* check data format and format version */
  ثابت UDataInfo* pInfo = reinterpret_cast<ثابت UDataInfo*>(static_cast<ثابت حرف*>(وچ_انکڑے) + 4);
  إذا(!(
    pInfo->انکڑے_Format[0] == 0x43 &&  /* dataFormat="CSel" */
    pInfo->انکڑے_Format[1] == 0x53 &&
    pInfo->انکڑے_Format[2] == 0x65 &&
    pInfo->انکڑے_Format[3] == 0x6c
  )) {
    udata_printError(ds, "ucnvsel_swap(): data format %02x.%02x.%02x.%02x is not recognized as UConverterSelector data\n",
                     pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                     pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3]);
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع 0;
  }
  إذا(pInfo->format_نسخہ[0] != 1) {
    udata_printError(ds, "ucnvsel_swap(): format version %02x is not supported\n",
                     pInfo->format_نسخہ[0]);
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع 0;
  }

  إذا(length >= 0) {
    length -= سرخی_ناپ;
    إذا(length < 16*4) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for UConverterSelector data\n",
                       length);
      *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }
  }

  ثابت uint8_t* وچ_بائٹاں = static_cast<ثابت uint8_t*>(وچ_انکڑے) + سرخی_ناپ;
  uint8_t* باہرلا_بائٹاں = static_cast<uint8_t*>(باہرلا_انکڑے) + سرخی_ناپ;

  /* read the indexes */
  ثابت int32_t* وچ_Indexes = reinterpret_cast<ثابت int32_t*>(وچ_بائٹاں);
  int32_t indexes[16];
  int32_t i;
  لكل(i = 0; i < 16; ++i) {
    indexes[i] = udata_readInt32(ds, وچ_Indexes[i]);
  }

  /* get the total length of the data */
  int32_t size = indexes[UCNVSEL_اشاریہ_ناپ];
  إذا(length >= 0) {
    إذا(length < size) {
      udata_printError(ds, "ucnvsel_swap(): too few bytes (%d after header) for all of UConverterSelector data\n",
                       length);
      *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
      أرجع 0;
    }

    /* copy the data for inaccessible bytes */
    إذا(وچ_بائٹاں != باہرلا_بائٹاں) {
      uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, size);
    }

    int32_t وتھ = 0, count;

    /* swap the int32_t indexes[] */
    count = UCNVSEL_اشاریہ_گنتی*4;
    ds->وٹاندرا_Array32(ds, وچ_بائٹاں, count, باہرلا_بائٹاں, حالت);
    وتھ += count;

    /* swap the UTrie2 */
    count = indexes[UCNVSEL_اشاریہ_TRIE_ناپ];
    utrie2_وٹاندرا(ds, وچ_بائٹاں + وتھ, count, باہرلا_بائٹاں + وتھ, حالت);
    وتھ += count;

    /* swap the uint32_t pv[] */
    count = indexes[UCNVSEL_اشاریہ_PV_گنتی]*4;
    ds->وٹاندرا_Array32(ds, وچ_بائٹاں + وتھ, count, باہرلا_بائٹاں + وتھ, حالت);
    وتھ += count;

    /* swap the encoding names */
    count = indexes[UCNVSEL_اشاریہ_ناں_لمائی];
    ds->وٹاندرا_Inv_Chars(ds, وچ_بائٹاں + وتھ, count, باہرلا_بائٹاں + وتھ, حالت);
    وتھ += count;

    U_ASSERT(وتھ == size);
  }

  أرجع سرخی_ناپ + size;
}

/* unserialize a selector */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(ثابت فراغ* بفر U_LIFETIME_BOUND,
                           int32_t length,
                           UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  // ensure args make sense!
  ثابت uint8_t *p = (ثابت uint8_t *)بفر;
  إذا (length <= 0 ||
      (length > 0 && (p == خالی_سوچک || (U_اشارہ_گر_نقاب_LSB(p, 3) != 0)))
  ) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }
  //سرخی
  إذا (length < 32) {
    // not even enough space for a minimal header
    *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
    أرجع خالی_سوچک;
  }
  ثابت انکڑے_سرخی *p_سرخی = (ثابت انکڑے_سرخی *)p;
  إذا (!(
    p_سرخی->انکڑے_سرخی_2.magic1==0xda &&
    p_سرخی->انکڑے_سرخی_2.magic2==0x27 &&
    p_سرخی->info.انکڑے_Format[0] == 0x43 &&
    p_سرخی->info.انکڑے_Format[1] == 0x53 &&
    p_سرخی->info.انکڑے_Format[2] == 0x65 &&
    p_سرخی->info.انکڑے_Format[3] == 0x6c
  )) {
    /* header not valid or dataFormat not recognized */
    *حالت = U_INVALID_FORMAT_ERROR;
    أرجع خالی_سوچک;
  }
  إذا (p_سرخی->info.format_نسخہ[0] != 1) {
    *حالت = U_UNSUPPORTED_ERROR;
    أرجع خالی_سوچک;
  }
  uint8_t* swapped = خالی_سوچک;
  إذا (p_سرخی->info.isBigEndian != U_IS_BIG_ENDIAN ||
      p_سرخی->info.charsetFamily != U_CHARSET_FAMILY
  ) {
    // swap the data
    UDataSwapper *ds =
      udata_open_Swapper_For_داخلہ_انکڑے(p, length, U_IS_BIG_ENDIAN, U_CHARSET_FAMILY, حالت);
    int32_t کل_ناپ = ucnvsel_وٹاندرا(ds, p, -1, خالی_سوچک, حالت);
    إذا (U_FAILURE(*حالت)) {
      udata_closeSwapper(ds);
      أرجع خالی_سوچک;
    }
    إذا (length < کل_ناپ) {
      udata_closeSwapper(ds);
      *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
      أرجع خالی_سوچک;
    }
    swapped = (uint8_t*)uprv_malloc(کل_ناپ);
    إذا (swapped == خالی_سوچک) {
      udata_closeSwapper(ds);
      *حالت = U_حافظہ_ونڈ_ERROR;
      أرجع خالی_سوچک;
    }
    ucnvsel_وٹاندرا(ds, p, length, swapped, حالت);
    udata_closeSwapper(ds);
    إذا (U_FAILURE(*حالت)) {
      uprv_free(swapped);
      أرجع خالی_سوچک;
    }
    p = swapped;
    p_سرخی = (ثابت انکڑے_سرخی *)p;
  }
  إذا (length < (p_سرخی->انکڑے_سرخی_2.سرخی_ناپ + 16 * 4)) {
    // not even enough space for the header and the indexes
    uprv_free(swapped);
    *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
    أرجع خالی_سوچک;
  }
  p += p_سرخی->انکڑے_سرخی_2.سرخی_ناپ;
  length -= p_سرخی->انکڑے_سرخی_2.سرخی_ناپ;
  // indexes
  ثابت int32_t *indexes = (ثابت int32_t *)p;
  إذا (length < indexes[UCNVSEL_اشاریہ_ناپ]) {
    uprv_free(swapped);
    *حالت = U_اشاریہ_OUTOFBOUNDS_ERROR;
    أرجع خالی_سوچک;
  }
  p += UCNVSEL_اشاریہ_گنتی * 4;
  // create and populate the selector object
  UConverterSelector* sel = (UConverterSelector*)uprv_malloc(حجم(UConverterSelector));
  حرف **encodings =
    (حرف **)uprv_malloc(
      indexes[UCNVSEL_اشاریہ_ناں_گنتی] * حجم(حرف *));
  إذا (sel == خالی_سوچک || encodings == خالی_سوچک) {
    uprv_free(swapped);
    uprv_free(sel);
    uprv_free(encodings);
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  uprv_memset(sel, 0, حجم(UConverterSelector));
  sel->pv_گنتی = indexes[UCNVSEL_اشاریہ_PV_گنتی];
  sel->encodings = encodings;
  sel->encodings_گنتی = indexes[UCNVSEL_اشاریہ_ناں_گنتی];
  sel->encoding_Str_لمائی = indexes[UCNVSEL_اشاریہ_ناں_لمائی];
  sel->swapped = swapped;
  // trie
  sel->trie = utrie2_openFromSerialized(UTRIE2_16_قدر_بٹ,
                                        p, indexes[UCNVSEL_اشاریہ_TRIE_ناپ], خالی_سوچک,
                                        حالت);
  p += indexes[UCNVSEL_اشاریہ_TRIE_ناپ];
  إذا (U_FAILURE(*حالت)) {
    ucnvsel_close(sel);
    أرجع خالی_سوچک;
  }
  // bit vectors
  sel->pv = (uint32_t *)p;
  p += sel->pv_گنتی * 4;
  // encoding names
  حرف* s = (حرف*)p;
  لكل (int32_t i = 0; i < sel->encodings_گنتی; ++i) {
    sel->encodings[i] = s;
    s += uprv_strlen(s) + 1;
  }
  p += sel->encoding_Str_لمائی;

  أرجع sel;
}

// a bunch of functions for the enumeration thingie! Nothing fancy here. Just
// iterate over the selected encodings
بنية Enumerator {
  int16_t* اشاریہ;
  int16_t length;
  int16_t cur;
  ثابت UConverterSelector* sel;
};

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV
ucnvsel_close_selector_iterator(UEnumeration *enumerator) {
  uprv_free(((Enumerator*)(enumerator->context))->اشاریہ);
  uprv_free(enumerator->context);
  uprv_free(enumerator);
}


ساكن int32_t U_CALLCONV
ucnvsel_گنتی_encodings(UEnumeration *enumerator, UError_کوڈ *حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع 0;
  }
  أرجع ((Enumerator*)(enumerator->context))->length;
}


ساكن ثابت حرف* U_CALLCONV ucnvsel_اگلا_encoding(UEnumeration* enumerator,
                                                 int32_t* نتیجہ_لمائی,
                                                 UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }

  int16_t cur = ((Enumerator*)(enumerator->context))->cur;
  ثابت UConverterSelector* sel;
  ثابت حرف* نتیجہ;
  إذا (cur >= ((Enumerator*)(enumerator->context))->length) {
    أرجع خالی_سوچک;
  }
  sel = ((Enumerator*)(enumerator->context))->sel;
  نتیجہ = sel->encodings[((Enumerator*)(enumerator->context))->اشاریہ[cur] ];
  ((Enumerator*)(enumerator->context))->cur++;
  إذا (نتیجہ_لمائی) {
    *نتیجہ_لمائی = (int32_t)uprv_strlen(نتیجہ);
  }
  أرجع نتیجہ;
}

ساكن فراغ U_CALLCONV ucnvsel_reset_iterator(UEnumeration* enumerator,
                                           UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع ;
  }
  ((Enumerator*)(enumerator->context))->cur = 0;
}

U_CDECL_سرا


ساكن ثابت UEnumeration defaultEncodings = {
  خالی_سوچک,
    خالی_سوچک,
    ucnvsel_close_selector_iterator,
    ucnvsel_گنتی_encodings,
    uenum_unextDefault,
    ucnvsel_اگلا_encoding, 
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
ساكن int16_t گنتی_Ones(uint32_t* mask, int32_t len) {
  int32_t i, کل_Ones = 0;
  لكل (i = 0 ; i < len ; ++i) {
    uint32_t ent = mask[i];
    لكل (; ent; کل_Ones++)
    {
      ent &= ent - 1; // clear the least significant bit set
    }
  }
  أرجع static_cast<int16_t>(کل_Ones);
}


/* internal function! */
ساكن UEnumeration*
select_For_نقاب(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
              uint32_t* the_نقاب,
              UError_کوڈ* حالت) {
  Local_حافظہ<uint32_t> mask(the_نقاب);
  // this is the context we will use. Store a table of indices to which
  // encodings are legit.
  Local_حافظہ<Enumerator> نتیجہ(static_cast<Enumerator *>(uprv_malloc(حجم(Enumerator))));
  إذا (نتیجہ.isNull()) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  نتیجہ->اشاریہ = خالی_سوچک;  // this will be allocated later!
  نتیجہ->length = نتیجہ->cur = 0;
  نتیجہ->sel = sel;

  Local_حافظہ<UEnumeration> en(static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration))));
  إذا (en.isNull()) {
    // TODO(markus): Combine Enumerator and UEnumeration into one struct.
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  حافظہ_نقل(en.getAlias(), &defaultEncodings, حجم(UEnumeration));
  
  int32_t columns = (sel->encodings_گنتی+31)/32;
  int16_t عدد_Ones = گنتی_Ones(mask.getAlias(), columns);
  // now, we know the exact space we need for index
  إذا (عدد_Ones > 0) {
    نتیجہ->اشاریہ = static_cast<int16_t*>(uprv_malloc(عدد_Ones * حجم(int16_t)));
    إذا (نتیجہ->اشاریہ == خالی_سوچک) {
      *حالت = U_حافظہ_ونڈ_ERROR;
      أرجع خالی_سوچک;
    }
    int32_t i, j;
    int16_t k = 0;
    لكل (j = 0 ; j < columns; j++) {
      uint32_t v = mask[j];
      لكل (i = 0 ; i < 32 && k < sel->encodings_گنتی; i++, k++) {
        إذا ((v & 1) != 0) {
          نتیجہ->اشاریہ[نتیجہ->length++] = k;
        }
        v >>= 1;
      }
    }
  } //otherwise, index will remain nullptr (and will never be touched by
    //the enumerator code anyway)
  en->context = نتیجہ.orphan();
  أرجع en.orphan();
}

/* check a string against the selector - UTF16 version */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(ثابت UConverterSelector* sel U_LIFETIME_BOUND,
                        ثابت char16_t* s,
                        int32_t length,
                        UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  // ensure args make sense!
  إذا (sel == خالی_سوچک || (s == خالی_سوچک && length != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }

  int32_t columns = (sel->encodings_گنتی+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == خالی_سوچک) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا(s!=خالی_سوچک) {
    ثابت char16_t *حد_بندی;
    إذا (length >= 0) {
      حد_بندی = s + length;
    } وإلا {
      حد_بندی = خالی_سوچک;
    }
    
    طالما (حد_بندی == خالی_سوچک ? *s != 0 : s != حد_بندی) {
      UChar32 c;
      uint16_t pv_اشاریہ;
      UTRIE2_U16_NEXT16(sel->trie, s, حد_بندی, c, pv_اشاریہ);
      إذا (intersectMasks(mask, sel->pv+pv_اشاریہ, columns)) {
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
                      UError_کوڈ* حالت) {
  // check if already failed
  إذا (U_FAILURE(*حالت)) {
    أرجع خالی_سوچک;
  }
  // ensure args make sense!
  إذا (sel == خالی_سوچک || (s == خالی_سوچک && length != 0)) {
    *حالت = U_ILLEGAL_دلیل_ERROR;
    أرجع خالی_سوچک;
  }

  int32_t columns = (sel->encodings_گنتی+31)/32;
  uint32_t* mask = (uint32_t*) uprv_malloc(columns * 4);
  إذا (mask == خالی_سوچک) {
    *حالت = U_حافظہ_ونڈ_ERROR;
    أرجع خالی_سوچک;
  }
  uprv_memset(mask, ~0, columns *4);

  إذا (length < 0) {
    length = (int32_t)uprv_strlen(s);
  }

  إذا(s!=خالی_سوچک) {
    ثابت حرف *حد_بندی = s + length;
    
    طالما (s != حد_بندی) {
      uint16_t pv_اشاریہ;
      UTRIE2_U8_NEXT16(sel->trie, s, حد_بندی, pv_اشاریہ);
      إذا (intersectMasks(mask, sel->pv+pv_اشاریہ, columns)) {
        اكسر;
      }
    }
  }
  أرجع select_For_نقاب(sel, mask, حالت);
}

#جے_انت  // !UCONFIG_NO_CONVERSION
