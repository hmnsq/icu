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

#شامل "unicode/udata.h" /* UDataInfo */
#شامل "utrie.h"
#شامل "utrie2.h"
#شامل "udataswp.h"
#شامل "cmemory.h"
#شامل "ucol_data.h"
#شامل "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#اگر !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ثنائی(ثابت UDataSwapper *ds,
                              ثابت خالی *اندر_ڈیٹا, int32_t length) {
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<-1) {
        واپس جھوٹ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_رمز error_رمز=U_ZERO_ERROR;
    (خالی)udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, -1, خالی_اشاریہ, &error_رمز);
    اگر(U_SUCCESS(error_رمز)) {
        ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
        اگر(info.ڈیٹا_Format[0]==0x55 &&   // dataFormat="UCol"
                info.ڈیٹا_Format[1]==0x43 &&
                info.ڈیٹا_Format[2]==0x6f &&
                info.ڈیٹا_Format[3]==0x6c) {
            واپس سچ;
        }
    }

    // Else check for format version 3.
    ثابت UCATable_سرخی *اندر_سرخی=(ثابت UCATable_سرخی *)اندر_ڈیٹا;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_سرخی سرخی;
    uprv_memset(&سرخی, 0, حجم(سرخی));
    اگر(length<0) {
        سرخی.size=udata_readInt32(ds, اندر_سرخی->size);
    } ورنہ اگر((length<(42*4) || length<(سرخی.size=udata_readInt32(ds, اندر_سرخی->size)))) {
        واپس جھوٹ;
    }

    سرخی.magic=ds->readUInt32(اندر_سرخی->magic);
    اگر(!(
        سرخی.magic==UCOL_سرخی_MAGIC &&
        اندر_سرخی->format_نسخہ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        واپس جھوٹ;
    }

    اگر(اندر_سرخی->isBigEndian!=ds->اندر_Is_Big_Endian || اندر_سرخی->char_سیٹ_Family!=ds->اندر_Charset) {
        واپس جھوٹ;
    }

    واپس سچ;
}

نامخلا {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
تبادلہ_Format_Version3(ثابت UDataSwapper *ds,
                   ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                   UError_رمز *p_Error_رمز) {
    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    ثابت UCATable_سرخی *اندر_سرخی;
    UCATable_سرخی *برآمد_سرخی;
    UCATable_سرخی سرخی;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<-1 || (length>0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اندر_بائٹیں = static_cast<ثابت uint8_t*>(اندر_ڈیٹا);
    برآمد_بائٹیں = static_cast<uint8_t*>(برآمد_ڈیٹا);

    اندر_سرخی = static_cast<ثابت UCATable_سرخی*>(اندر_ڈیٹا);
    برآمد_سرخی = static_cast<UCATable_سرخی*>(برآمد_ڈیٹا);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&سرخی, 0, حجم(سرخی));
    اگر(length<0) {
        سرخی.size=udata_readInt32(ds, اندر_سرخی->size);
    } ورنہ اگر((length<(42*4) || length<(سرخی.size=udata_readInt32(ds, اندر_سرخی->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    سرخی.magic=ds->readUInt32(اندر_سرخی->magic);
    اگر(!(
        سرخی.magic==UCOL_سرخی_MAGIC &&
        اندر_سرخی->format_نسخہ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         سرخی.magic,
                         اندر_سرخی->format_نسخہ[0], اندر_سرخی->format_نسخہ[1]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اگر(اندر_سرخی->isBigEndian!=ds->اندر_Is_Big_Endian || اندر_سرخی->char_سیٹ_Family!=ds->اندر_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         اندر_سرخی->isBigEndian, اندر_سرخی->char_سیٹ_Family);
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس 0;
    }

    اگر(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, سرخی.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        سرخی.options=                 ds->readUInt32(اندر_سرخی->options);
        سرخی.UCAConsts=               ds->readUInt32(اندر_سرخی->UCAConsts);
        سرخی.contractionUCACombos=    ds->readUInt32(اندر_سرخی->contractionUCACombos);
        سرخی.mappingPosition=         ds->readUInt32(اندر_سرخی->mappingPosition);
        سرخی.expansion=               ds->readUInt32(اندر_سرخی->expansion);
        سرخی.contraction_اشاریہ=        ds->readUInt32(اندر_سرخی->contraction_اشاریہ);
        سرخی.contractionCEs=          ds->readUInt32(اندر_سرخی->contractionCEs);
        سرخی.contraction_سائز=         ds->readUInt32(اندر_سرخی->contraction_سائز);
        سرخی.انتہا_Expansion_CE=          ds->readUInt32(اندر_سرخی->انتہا_Expansion_CE);
        سرخی.expansionCESize=         ds->readUInt32(اندر_سرخی->expansionCESize);
        سرخی.انتہا_Expansion_CECount=     udata_readInt32(ds, اندر_سرخی->انتہا_Expansion_CECount);
        سرخی.contraction_UCACombos_سائز=udata_readInt32(ds, اندر_سرخی->contraction_UCACombos_سائز);
        سرخی.script_مقصد_Lead_Byte=        ds->readUInt32(اندر_سرخی->script_مقصد_Lead_Byte);
        سرخی.lead_Byte_مقصد_Script=        ds->readUInt32(اندر_سرخی->lead_Byte_مقصد_Script);
        
        /* swap the 32-bit integers in the header */
        ds->تبادلہ_Array32(ds, اندر_سرخی, static_cast<int32_t>(reinterpret_cast<ثابت حرف*>(&اندر_سرخی->jamoSpecial) - reinterpret_cast<ثابت حرف*>(اندر_سرخی)),
                           برآمد_سرخی, p_Error_رمز);
        ds->تبادلہ_Array32(ds, &(اندر_سرخی->script_مقصد_Lead_Byte), حجم(سرخی.script_مقصد_Lead_Byte) + حجم(سرخی.lead_Byte_مقصد_Script),
                           &(برآمد_سرخی->script_مقصد_Lead_Byte), p_Error_رمز);
        /* set the output platform properties */
        برآمد_سرخی->isBigEndian=ds->برآمد_Is_Big_Endian;
        برآمد_سرخی->char_سیٹ_Family=ds->برآمد_Charset;

        /* swap the options */
        اگر(سرخی.options!=0) {
            ds->تبادلہ_Array32(ds, اندر_بائٹیں+سرخی.options, سرخی.expansion-سرخی.options,
                               برآمد_بائٹیں+سرخی.options, p_Error_رمز);
        }

        /* swap the expansions */
        اگر(سرخی.mappingPosition!=0 && سرخی.expansion!=0) {
            اگر(سرخی.contraction_اشاریہ!=0) {
                /* expansions bounded by contractions */
                count=سرخی.contraction_اشاریہ-سرخی.expansion;
            } ورنہ {
                /* no contractions: expansions bounded by the main trie */
                count=سرخی.mappingPosition-سرخی.expansion;
            }
            ds->تبادلہ_Array32(ds, اندر_بائٹیں + سرخی.expansion, static_cast<int32_t>(count),
                               برآمد_بائٹیں+سرخی.expansion, p_Error_رمز);
        }

        /* swap the contractions */
        اگر(سرخی.contraction_سائز!=0) {
            /* contractionIndex: char16_t[] */
            ds->تبادلہ_Array16(ds, اندر_بائٹیں+سرخی.contraction_اشاریہ, سرخی.contraction_سائز*2,
                               برآمد_بائٹیں+سرخی.contraction_اشاریہ, p_Error_رمز);

            /* contractionCEs: CEs[] */
            ds->تبادلہ_Array32(ds, اندر_بائٹیں+سرخی.contractionCEs, سرخی.contraction_سائز*4,
                               برآمد_بائٹیں+سرخی.contractionCEs, p_Error_رمز);
        }

        /* swap the main trie */
        اگر(سرخی.mappingPosition!=0) {
            count=سرخی.انتہا_Expansion_CE-سرخی.mappingPosition;
            utrie_تبادلہ(ds, اندر_بائٹیں + سرخی.mappingPosition, static_cast<int32_t>(count),
                          برآمد_بائٹیں+سرخی.mappingPosition, p_Error_رمز);
        }

        /* swap the max expansion table */
        اگر(سرخی.انتہا_Expansion_CECount!=0) {
            ds->تبادلہ_Array32(ds, اندر_بائٹیں+سرخی.انتہا_Expansion_CE, سرخی.انتہا_Expansion_CECount*4,
                               برآمد_بائٹیں+سرخی.انتہا_Expansion_CE, p_Error_رمز);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        اگر(سرخی.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->تبادلہ_Array32(ds, اندر_بائٹیں+سرخی.UCAConsts, سرخی.contractionUCACombos-سرخی.UCAConsts,
                               برآمد_بائٹیں+سرخی.UCAConsts, p_Error_رمز);
        }

        /* swap UCA contractions */
        اگر(سرخی.contraction_UCACombos_سائز!=0) {
            count=سرخی.contraction_UCACombos_سائز*اندر_سرخی->contraction_UCACombos_چوڑائی*U_SIZEOF_UCHAR;
            ds->تبادلہ_Array16(ds, اندر_بائٹیں + سرخی.contractionUCACombos, static_cast<int32_t>(count),
                               برآمد_بائٹیں+سرخی.contractionUCACombos, p_Error_رمز);
        }
        
        /* swap the script to lead bytes */
        اگر(سرخی.script_مقصد_Lead_Byte!=0) {
            صحیح اشاریہ_گنتی = ds->readUInt16(*((uint16_t*)(اندر_بائٹیں+سرخی.script_مقصد_Lead_Byte))); // each entry = 2 * uint16
            صحیح ڈیٹا_گنتی = ds->readUInt16(*((uint16_t*)(اندر_بائٹیں+سرخی.script_مقصد_Lead_Byte + 2))); // each entry = uint16
            ds->تبادلہ_Array16(ds, اندر_بائٹیں+سرخی.script_مقصد_Lead_Byte, 
                                4 + (4 * اشاریہ_گنتی) + (2 * ڈیٹا_گنتی),
                                برآمد_بائٹیں+سرخی.script_مقصد_Lead_Byte, p_Error_رمز);
        }
        
        /* swap the lead byte to scripts */
        اگر(سرخی.lead_Byte_مقصد_Script!=0) {
            صحیح اشاریہ_گنتی = ds->readUInt16(*((uint16_t*)(اندر_بائٹیں+سرخی.lead_Byte_مقصد_Script))); // each entry = uint16
            صحیح ڈیٹا_گنتی = ds->readUInt16(*((uint16_t*)(اندر_بائٹیں+سرخی.lead_Byte_مقصد_Script + 2))); // each entry = uint16
            ds->تبادلہ_Array16(ds, اندر_بائٹیں+سرخی.lead_Byte_مقصد_Script, 
                                4 + (2 * اشاریہ_گنتی) + (2 * ڈیٹا_گنتی),
                                برآمد_بائٹیں+سرخی.lead_Byte_مقصد_Script, p_Error_رمز);
        }
    }

    واپس سرخی.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

شمار {
    IX_INDEXES_لمبائی,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_آغاز,  // 4
    IX_REORDER_CODES_انحراف,
    IX_REORDER_جدول_انحراف,
    IX_TRIE_انحراف,

    IX_RESERVED8_انحراف,  // 8
    IX_CES_انحراف,
    IX_RESERVED10_انحراف,
    IX_CE32_S_انحراف,

    IX_جڑ_ELEMENTS_انحراف,  // 12
    IX_CONTEXTS_انحراف,
    IX_UNSAFE_BWD_انحراف,
    IX_FAST_LATIN_جدول_انحراف,

    IX_SCRIPTS_انحراف,  // 16
    IX_COMPRESSIBLE_بائٹیں_انحراف,
    IX_RESERVED18_انحراف,
    IX_کل_سائز
};

int32_t
تبادلہ_Format_Version4(ثابت UDataSwapper *ds,
                   ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                   UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) { واپس 0; }

    ثابت uint8_t* اندر_بائٹیں = static_cast<ثابت uint8_t*>(اندر_ڈیٹا);
    uint8_t* برآمد_بائٹیں = static_cast<uint8_t*>(برآمد_ڈیٹا);

    ثابت int32_t* اندر_Indexes = reinterpret_cast<ثابت int32_t*>(اندر_بائٹیں);
    int32_t indexes[IX_کل_سائز+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    اگر(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    int32_t indexes_لمبائی=indexes[0]=udata_readInt32(ds, اندر_Indexes[0]);
    اگر(0<=length && length<(indexes_لمبائی*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    برائے(int32_t i=1; i<=IX_کل_سائز && i<indexes_لمبائی; ++i) {
        indexes[i]=udata_readInt32(ds, اندر_Indexes[i]);
    }
    برائے(int32_t i=indexes_لمبائی; i<=IX_کل_سائز; ++i) {
        indexes[i]=-1;
    }
    اندر_Indexes=خالی_اشاریہ;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    اگر(indexes_لمبائی>IX_کل_سائز) {
        size=indexes[IX_کل_سائز];
    } ورنہ اگر(indexes_لمبائی>IX_REORDER_CODES_انحراف) {
        size=indexes[indexes_لمبائی-1];
    } ورنہ {
        size=indexes_لمبائی*4;
    }
    اگر(length<0) { واپس size; }

    اگر(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
        uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
    }

    // Swap the int32_t indexes[].
    ds->تبادلہ_Array32(ds, اندر_بائٹیں, indexes_لمبائی * 4, برآمد_بائٹیں, &error_رمز);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t اشاریہ;  // one of the indexes[] slots
    int32_t انحراف;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    اشاریہ = IX_REORDER_CODES_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array32(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    اشاریہ = IX_TRIE_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        utrie2_تبادلہ(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_RESERVED8_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_رمز = U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اشاریہ = IX_CES_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array64(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_RESERVED10_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_رمز = U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اشاریہ = IX_CE32_S_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array32(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_جڑ_ELEMENTS_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array32(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_CONTEXTS_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array16(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_UNSAFE_BWD_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array16(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_FAST_LATIN_جدول_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array16(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    اشاریہ = IX_SCRIPTS_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        ds->تبادلہ_Array16(ds, اندر_بائٹیں + انحراف, length, برآمد_بائٹیں + انحراف, &error_رمز);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    اشاریہ = IX_RESERVED18_انحراف;
    انحراف = indexes[اشاریہ];
    length = indexes[اشاریہ + 1] - انحراف;
    اگر(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_رمز = U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    واپس size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_تبادلہ(ثابت UDataSwapper *ds,
          ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
          UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) { واپس 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_رمز=U_ZERO_ERROR;
        واپس تبادلہ_Format_Version3(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    }

    /* check data format and format version */
    ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        info.ڈیٹا_Format[0]==0x55 &&   // dataFormat="UCol"
        info.ڈیٹا_Format[1]==0x43 &&
        info.ڈیٹا_Format[2]==0x6f &&
        info.ڈیٹا_Format[3]==0x6c &&
        (3<=info.format_نسخہ[0] && info.format_نسخہ[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.ڈیٹا_Format[0], info.ڈیٹا_Format[1],
                         info.ڈیٹا_Format[2], info.ڈیٹا_Format[3],
                         info.format_نسخہ[0], info.format_نسخہ[1]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_ڈیٹا=(ثابت حرف *)اندر_ڈیٹا+سرخی_سائز;
    اگر(length>=0) { length-=سرخی_سائز; }
    برآمد_ڈیٹا=(برآمد_ڈیٹا == خالی_اشاریہ) ? خالی_اشاریہ : (حرف *)برآمد_ڈیٹا+سرخی_سائز;
    int32_t collation_سائز;
    اگر(info.format_نسخہ[0]>=4) {
        collation_سائز=تبادلہ_Format_Version4(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, *p_Error_رمز);
    } ورنہ {
        collation_سائز=تبادلہ_Format_Version3(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    }
    اگر(U_SUCCESS(*p_Error_رمز)) {
        واپس سرخی_سائز+collation_سائز;
    } ورنہ {
        واپس 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_تبادلہ_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                    UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    ثابت Inverse_UCATable_سرخی *اندر_سرخی;
    Inverse_UCATable_سرخی *برآمد_سرخی;
    Inverse_UCATable_سرخی سرخی={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->ڈیٹا_Format[1]==0x6e &&
        pInfo->ڈیٹا_Format[2]==0x76 &&
        pInfo->ڈیٹا_Format[3]==0x43 &&
        pInfo->format_نسخہ[0]==2 &&
        pInfo->format_نسخہ[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں=(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    برآمد_بائٹیں=(uint8_t *)برآمد_ڈیٹا+سرخی_سائز;

    اندر_سرخی=(ثابت Inverse_UCATable_سرخی *)اندر_بائٹیں;
    برآمد_سرخی=(Inverse_UCATable_سرخی *)برآمد_بائٹیں;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    اگر(length<0) {
        سرخی.byte_سائز=udata_readInt32(ds, اندر_سرخی->byte_سائز);
    } ورنہ اگر(
        ((length-سرخی_سائز)<(8*4) ||
         (uint32_t)(length-سرخی_سائز)<(سرخی.byte_سائز=udata_readInt32(ds, اندر_سرخی->byte_سائز)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اگر(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, سرخی.byte_سائز);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        سرخی.جدول_سائز=   ds->readUInt32(اندر_سرخی->جدول_سائز);
        سرخی.conts_سائز=   ds->readUInt32(اندر_سرخی->conts_سائز);
        سرخی.جدول=       ds->readUInt32(اندر_سرخی->جدول);
        سرخی.conts=       ds->readUInt32(اندر_سرخی->conts);

        /* swap the 32-bit integers in the header */
        ds->تبادلہ_Array32(ds, اندر_سرخی, 5*4, برآمد_سرخی, p_Error_رمز);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->تبادلہ_Array32(ds, اندر_بائٹیں+سرخی.جدول, سرخی.جدول_سائز*3*4,
                           برآمد_بائٹیں+سرخی.جدول, p_Error_رمز);

        /* swap the continuation table; contsSize counts UChars */
        ds->تبادلہ_Array16(ds, اندر_بائٹیں+سرخی.conts, سرخی.conts_سائز*U_SIZEOF_UCHAR,
                           برآمد_بائٹیں+سرخی.conts, p_Error_رمز);
    }

    واپس سرخی_سائز+سرخی.byte_سائز;
}

#اگر_ختم /* #if !UCONFIG_NO_COLLATION */
