// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_swp.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003sep10
*   created by: Markus W. Scherer
*
*   Swap collation binaries.
*/

#ضمن "unicode/udata.h" /* UDataInfo */
#ضمن "utrie.h"
#ضمن "utrie2.h"
#ضمن "udataswp.h"
#ضمن "cmemory.h"
#ضمن "ucol_data.h"
#ضمن "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#إذا !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ثنائی(ثابت UDataSwapper *ds,
                              ثابت فراغ *وچ_انکڑے, int32_t length) {
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<-1) {
        أرجع خطأ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    (فراغ)udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, -1, خالی_سوچک, &error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ)) {
        ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
        إذا(info.انکڑے_Format[0]==0x55 &&   // dataFormat="UCol"
                info.انکڑے_Format[1]==0x43 &&
                info.انکڑے_Format[2]==0x6f &&
                info.انکڑے_Format[3]==0x6c) {
            أرجع سچ;
        }
    }

    // Else check for format version 3.
    ثابت UCATable_سرخی *وچ_سرخی=(ثابت UCATable_سرخی *)وچ_انکڑے;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_سرخی سرخی;
    uprv_memset(&سرخی, 0, حجم(سرخی));
    إذا(length<0) {
        سرخی.size=udata_readInt32(ds, وچ_سرخی->size);
    } وإلا إذا((length<(42*4) || length<(سرخی.size=udata_readInt32(ds, وچ_سرخی->size)))) {
        أرجع خطأ;
    }

    سرخی.magic=ds->readUInt32(وچ_سرخی->magic);
    إذا(!(
        سرخی.magic==UCOL_سرخی_MAGIC &&
        وچ_سرخی->format_نسخہ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        أرجع خطأ;
    }

    إذا(وچ_سرخی->isBigEndian!=ds->وچ_Is_Big_Endian || وچ_سرخی->char_سیٹ_Family!=ds->وچ_Charset) {
        أرجع خطأ;
    }

    أرجع سچ;
}

نطاق {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
وٹاندرا_Format_Version3(ثابت UDataSwapper *ds,
                   ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                   UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;

    ثابت UCATable_سرخی *وچ_سرخی;
    UCATable_سرخی *باہرلا_سرخی;
    UCATable_سرخی سرخی;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<-1 || (length>0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں = static_cast<ثابت uint8_t*>(وچ_انکڑے);
    باہرلا_بائٹاں = static_cast<uint8_t*>(باہرلا_انکڑے);

    وچ_سرخی = static_cast<ثابت UCATable_سرخی*>(وچ_انکڑے);
    باہرلا_سرخی = static_cast<UCATable_سرخی*>(باہرلا_انکڑے);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&سرخی, 0, حجم(سرخی));
    إذا(length<0) {
        سرخی.size=udata_readInt32(ds, وچ_سرخی->size);
    } وإلا إذا((length<(42*4) || length<(سرخی.size=udata_readInt32(ds, وچ_سرخی->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    سرخی.magic=ds->readUInt32(وچ_سرخی->magic);
    إذا(!(
        سرخی.magic==UCOL_سرخی_MAGIC &&
        وچ_سرخی->format_نسخہ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         سرخی.magic,
                         وچ_سرخی->format_نسخہ[0], وچ_سرخی->format_نسخہ[1]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    إذا(وچ_سرخی->isBigEndian!=ds->وچ_Is_Big_Endian || وچ_سرخی->char_سیٹ_Family!=ds->وچ_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         وچ_سرخی->isBigEndian, وچ_سرخی->char_سیٹ_Family);
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
            uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, سرخی.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        سرخی.options=                 ds->readUInt32(وچ_سرخی->options);
        سرخی.UCAConsts=               ds->readUInt32(وچ_سرخی->UCAConsts);
        سرخی.contractionUCACombos=    ds->readUInt32(وچ_سرخی->contractionUCACombos);
        سرخی.mappingPosition=         ds->readUInt32(وچ_سرخی->mappingPosition);
        سرخی.expansion=               ds->readUInt32(وچ_سرخی->expansion);
        سرخی.contraction_اشاریہ=        ds->readUInt32(وچ_سرخی->contraction_اشاریہ);
        سرخی.contractionCEs=          ds->readUInt32(وچ_سرخی->contractionCEs);
        سرخی.contraction_ناپ=         ds->readUInt32(وچ_سرخی->contraction_ناپ);
        سرخی.سرا_Expansion_CE=          ds->readUInt32(وچ_سرخی->سرا_Expansion_CE);
        سرخی.expansionCESize=         ds->readUInt32(وچ_سرخی->expansionCESize);
        سرخی.سرا_Expansion_CECount=     udata_readInt32(ds, وچ_سرخی->سرا_Expansion_CECount);
        سرخی.contraction_UCACombos_ناپ=udata_readInt32(ds, وچ_سرخی->contraction_UCACombos_ناپ);
        سرخی.script_منزل_Lead_Byte=        ds->readUInt32(وچ_سرخی->script_منزل_Lead_Byte);
        سرخی.lead_Byte_منزل_Script=        ds->readUInt32(وچ_سرخی->lead_Byte_منزل_Script);
        
        /* swap the 32-bit integers in the header */
        ds->وٹاندرا_Array32(ds, وچ_سرخی, static_cast<int32_t>(reinterpret_cast<ثابت حرف*>(&وچ_سرخی->jamoSpecial) - reinterpret_cast<ثابت حرف*>(وچ_سرخی)),
                           باہرلا_سرخی, p_Error_کوڈ);
        ds->وٹاندرا_Array32(ds, &(وچ_سرخی->script_منزل_Lead_Byte), حجم(سرخی.script_منزل_Lead_Byte) + حجم(سرخی.lead_Byte_منزل_Script),
                           &(باہرلا_سرخی->script_منزل_Lead_Byte), p_Error_کوڈ);
        /* set the output platform properties */
        باہرلا_سرخی->isBigEndian=ds->باہرلا_Is_Big_Endian;
        باہرلا_سرخی->char_سیٹ_Family=ds->باہرلا_Charset;

        /* swap the options */
        إذا(سرخی.options!=0) {
            ds->وٹاندرا_Array32(ds, وچ_بائٹاں+سرخی.options, سرخی.expansion-سرخی.options,
                               باہرلا_بائٹاں+سرخی.options, p_Error_کوڈ);
        }

        /* swap the expansions */
        إذا(سرخی.mappingPosition!=0 && سرخی.expansion!=0) {
            إذا(سرخی.contraction_اشاریہ!=0) {
                /* expansions bounded by contractions */
                count=سرخی.contraction_اشاریہ-سرخی.expansion;
            } وإلا {
                /* no contractions: expansions bounded by the main trie */
                count=سرخی.mappingPosition-سرخی.expansion;
            }
            ds->وٹاندرا_Array32(ds, وچ_بائٹاں + سرخی.expansion, static_cast<int32_t>(count),
                               باہرلا_بائٹاں+سرخی.expansion, p_Error_کوڈ);
        }

        /* swap the contractions */
        إذا(سرخی.contraction_ناپ!=0) {
            /* contractionIndex: char16_t[] */
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں+سرخی.contraction_اشاریہ, سرخی.contraction_ناپ*2,
                               باہرلا_بائٹاں+سرخی.contraction_اشاریہ, p_Error_کوڈ);

            /* contractionCEs: CEs[] */
            ds->وٹاندرا_Array32(ds, وچ_بائٹاں+سرخی.contractionCEs, سرخی.contraction_ناپ*4,
                               باہرلا_بائٹاں+سرخی.contractionCEs, p_Error_کوڈ);
        }

        /* swap the main trie */
        إذا(سرخی.mappingPosition!=0) {
            count=سرخی.سرا_Expansion_CE-سرخی.mappingPosition;
            utrie_وٹاندرا(ds, وچ_بائٹاں + سرخی.mappingPosition, static_cast<int32_t>(count),
                          باہرلا_بائٹاں+سرخی.mappingPosition, p_Error_کوڈ);
        }

        /* swap the max expansion table */
        إذا(سرخی.سرا_Expansion_CECount!=0) {
            ds->وٹاندرا_Array32(ds, وچ_بائٹاں+سرخی.سرا_Expansion_CE, سرخی.سرا_Expansion_CECount*4,
                               باہرلا_بائٹاں+سرخی.سرا_Expansion_CE, p_Error_کوڈ);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        إذا(سرخی.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->وٹاندرا_Array32(ds, وچ_بائٹاں+سرخی.UCAConsts, سرخی.contractionUCACombos-سرخی.UCAConsts,
                               باہرلا_بائٹاں+سرخی.UCAConsts, p_Error_کوڈ);
        }

        /* swap UCA contractions */
        إذا(سرخی.contraction_UCACombos_ناپ!=0) {
            count=سرخی.contraction_UCACombos_ناپ*وچ_سرخی->contraction_UCACombos_چوڑائی*U_SIZEOF_UCHAR;
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں + سرخی.contractionUCACombos, static_cast<int32_t>(count),
                               باہرلا_بائٹاں+سرخی.contractionUCACombos, p_Error_کوڈ);
        }
        
        /* swap the script to lead bytes */
        إذا(سرخی.script_منزل_Lead_Byte!=0) {
            صحيح اشاریہ_گنتی = ds->readUInt16(*((uint16_t*)(وچ_بائٹاں+سرخی.script_منزل_Lead_Byte))); // each entry = 2 * uint16
            صحيح انکڑے_گنتی = ds->readUInt16(*((uint16_t*)(وچ_بائٹاں+سرخی.script_منزل_Lead_Byte + 2))); // each entry = uint16
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں+سرخی.script_منزل_Lead_Byte, 
                                4 + (4 * اشاریہ_گنتی) + (2 * انکڑے_گنتی),
                                باہرلا_بائٹاں+سرخی.script_منزل_Lead_Byte, p_Error_کوڈ);
        }
        
        /* swap the lead byte to scripts */
        إذا(سرخی.lead_Byte_منزل_Script!=0) {
            صحيح اشاریہ_گنتی = ds->readUInt16(*((uint16_t*)(وچ_بائٹاں+سرخی.lead_Byte_منزل_Script))); // each entry = uint16
            صحيح انکڑے_گنتی = ds->readUInt16(*((uint16_t*)(وچ_بائٹاں+سرخی.lead_Byte_منزل_Script + 2))); // each entry = uint16
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں+سرخی.lead_Byte_منزل_Script, 
                                4 + (2 * اشاریہ_گنتی) + (2 * انکڑے_گنتی),
                                باہرلا_بائٹاں+سرخی.lead_Byte_منزل_Script, p_Error_کوڈ);
        }
    }

    أرجع سرخی.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

تعداد {
    IX_INDEXES_لمائی,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_شروع,  // 4
    IX_REORDER_CODES_وتھ,
    IX_REORDER_جدول_وتھ,
    IX_TRIE_وتھ,

    IX_RESERVED8_وتھ,  // 8
    IX_CES_وتھ,
    IX_RESERVED10_وتھ,
    IX_CE32_S_وتھ,

    IX_جڑ_ELEMENTS_وتھ,  // 12
    IX_CONTEXTS_وتھ,
    IX_UNSAFE_BWD_وتھ,
    IX_FAST_LATIN_جدول_وتھ,

    IX_SCRIPTS_وتھ,  // 16
    IX_COMPRESSIBLE_بائٹاں_وتھ,
    IX_RESERVED18_وتھ,
    IX_کل_ناپ
};

int32_t
وٹاندرا_Format_Version4(ثابت UDataSwapper *ds,
                   ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                   UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) { أرجع 0; }

    ثابت uint8_t* وچ_بائٹاں = static_cast<ثابت uint8_t*>(وچ_انکڑے);
    uint8_t* باہرلا_بائٹاں = static_cast<uint8_t*>(باہرلا_انکڑے);

    ثابت int32_t* وچ_Indexes = reinterpret_cast<ثابت int32_t*>(وچ_بائٹاں);
    int32_t indexes[IX_کل_ناپ+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    إذا(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t indexes_لمائی=indexes[0]=udata_readInt32(ds, وچ_Indexes[0]);
    إذا(0<=length && length<(indexes_لمائی*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    لكل(int32_t i=1; i<=IX_کل_ناپ && i<indexes_لمائی; ++i) {
        indexes[i]=udata_readInt32(ds, وچ_Indexes[i]);
    }
    لكل(int32_t i=indexes_لمائی; i<=IX_کل_ناپ; ++i) {
        indexes[i]=-1;
    }
    وچ_Indexes=خالی_سوچک;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    إذا(indexes_لمائی>IX_کل_ناپ) {
        size=indexes[IX_کل_ناپ];
    } وإلا إذا(indexes_لمائی>IX_REORDER_CODES_وتھ) {
        size=indexes[indexes_لمائی-1];
    } وإلا {
        size=indexes_لمائی*4;
    }
    إذا(length<0) { أرجع size; }

    إذا(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
        uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, size);
    }

    // Swap the int32_t indexes[].
    ds->وٹاندرا_Array32(ds, وچ_بائٹاں, indexes_لمائی * 4, باہرلا_بائٹاں, &error_کوڈ);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t اشاریہ;  // one of the indexes[] slots
    int32_t وتھ;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    اشاریہ = IX_REORDER_CODES_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    اشاریہ = IX_TRIE_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        utrie2_وٹاندرا(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_RESERVED8_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_کوڈ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اشاریہ = IX_CES_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array64(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_RESERVED10_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_کوڈ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اشاریہ = IX_CE32_S_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_جڑ_ELEMENTS_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_CONTEXTS_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_UNSAFE_BWD_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_FAST_LATIN_جدول_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    اشاریہ = IX_SCRIPTS_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں + وتھ, length, باہرلا_بائٹاں + وتھ, &error_کوڈ);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    اشاریہ = IX_RESERVED18_وتھ;
    وتھ = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - وتھ;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_کوڈ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_وٹاندرا(ثابت UDataSwapper *ds,
          ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
          UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) { أرجع 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_کوڈ=U_ZERO_ERROR;
        أرجع وٹاندرا_Format_Version3(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    }

    /* check data format and format version */
    ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        info.انکڑے_Format[0]==0x55 &&   // dataFormat="UCol"
        info.انکڑے_Format[1]==0x43 &&
        info.انکڑے_Format[2]==0x6f &&
        info.انکڑے_Format[3]==0x6c &&
        (3<=info.format_نسخہ[0] && info.format_نسخہ[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.انکڑے_Format[0], info.انکڑے_Format[1],
                         info.انکڑے_Format[2], info.انکڑے_Format[3],
                         info.format_نسخہ[0], info.format_نسخہ[1]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_انکڑے=(ثابت حرف *)وچ_انکڑے+سرخی_ناپ;
    إذا(length>=0) { length-=سرخی_ناپ; }
    باہرلا_انکڑے=(باہرلا_انکڑے == خالی_سوچک) ? خالی_سوچک : (حرف *)باہرلا_انکڑے+سرخی_ناپ;
    int32_t collation_ناپ;
    إذا(info.format_نسخہ[0]>=4) {
        collation_ناپ=وٹاندرا_Format_Version4(ds, وچ_انکڑے, length, باہرلا_انکڑے, *p_Error_کوڈ);
    } وإلا {
        collation_ناپ=وٹاندرا_Format_Version3(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    }
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        أرجع سرخی_ناپ+collation_ناپ;
    } وإلا {
        أرجع 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_وٹاندرا_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                    UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;

    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;

    ثابت Inverse_UCATable_سرخی *وچ_سرخی;
    Inverse_UCATable_سرخی *باہرلا_سرخی;
    Inverse_UCATable_سرخی سرخی={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->انکڑے_Format[1]==0x6e &&
        pInfo->انکڑے_Format[2]==0x76 &&
        pInfo->انکڑے_Format[3]==0x43 &&
        pInfo->format_نسخہ[0]==2 &&
        pInfo->format_نسخہ[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں=(ثابت uint8_t *)وچ_انکڑے+سرخی_ناپ;
    باہرلا_بائٹاں=(uint8_t *)باہرلا_انکڑے+سرخی_ناپ;

    وچ_سرخی=(ثابت Inverse_UCATable_سرخی *)وچ_بائٹاں;
    باہرلا_سرخی=(Inverse_UCATable_سرخی *)باہرلا_بائٹاں;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    إذا(length<0) {
        سرخی.byte_ناپ=udata_readInt32(ds, وچ_سرخی->byte_ناپ);
    } وإلا إذا(
        ((length-سرخی_ناپ)<(8*4) ||
         (uint32_t)(length-سرخی_ناپ)<(سرخی.byte_ناپ=udata_readInt32(ds, وچ_سرخی->byte_ناپ)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
            uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, سرخی.byte_ناپ);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        سرخی.جدول_ناپ=   ds->readUInt32(وچ_سرخی->جدول_ناپ);
        سرخی.conts_ناپ=   ds->readUInt32(وچ_سرخی->conts_ناپ);
        سرخی.جدول=       ds->readUInt32(وچ_سرخی->جدول);
        سرخی.conts=       ds->readUInt32(وچ_سرخی->conts);

        /* swap the 32-bit integers in the header */
        ds->وٹاندرا_Array32(ds, وچ_سرخی, 5*4, باہرلا_سرخی, p_Error_کوڈ);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں+سرخی.جدول, سرخی.جدول_ناپ*3*4,
                           باہرلا_بائٹاں+سرخی.جدول, p_Error_کوڈ);

        /* swap the continuation table; contsSize counts UChars */
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں+سرخی.conts, سرخی.conts_ناپ*U_SIZEOF_UCHAR,
                           باہرلا_بائٹاں+سرخی.conts, p_Error_کوڈ);
    }

    أرجع سرخی_ناپ+سرخی.byte_ناپ;
}

#جے_انت /* #if !UCONFIG_NO_COLLATION */
