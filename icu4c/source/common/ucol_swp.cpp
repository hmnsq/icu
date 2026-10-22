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
ucol_looks_Like_Collation_دوګونی(ثابت UDataSwapper *ds,
                              ثابت فراغ *دننه_معلومات, int32_t length) {
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<-1) {
        أرجع خطأ;
    }

    // First check for format version 4+ which has a standard data header.
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    (فراغ)udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, -1, تش_اشاره, &error_کوډ);
    إذا(U_SUCCESS(error_کوډ)) {
        ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
        إذا(info.معلومات_Format[0]==0x55 &&   // dataFormat="UCol"
                info.معلومات_Format[1]==0x43 &&
                info.معلومات_Format[2]==0x6f &&
                info.معلومات_Format[3]==0x6c) {
            أرجع ریښتیا;
        }
    }

    // Else check for format version 3.
    ثابت UCATable_سرلیک *دننه_سرلیک=(ثابت UCATable_سرلیک *)دننه_معلومات;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_سرلیک سرلیک;
    uprv_memset(&سرلیک, 0, حجم(سرلیک));
    إذا(length<0) {
        سرلیک.size=udata_readInt32(ds, دننه_سرلیک->size);
    } وإلا إذا((length<(42*4) || length<(سرلیک.size=udata_readInt32(ds, دننه_سرلیک->size)))) {
        أرجع خطأ;
    }

    سرلیک.magic=ds->readUInt32(دننه_سرلیک->magic);
    إذا(!(
        سرلیک.magic==UCOL_سرلیک_MAGIC &&
        دننه_سرلیک->format_نسخه[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        أرجع خطأ;
    }

    إذا(دننه_سرلیک->isBigEndian!=ds->دننه_Is_Big_Endian || دننه_سرلیک->char_سېټ_Family!=ds->دننه_Charset) {
        أرجع خطأ;
    }

    أرجع ریښتیا;
}

نطاق {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
بدلون_Format_Version3(ثابت UDataSwapper *ds,
                   ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                   UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    ثابت UCATable_سرلیک *دننه_سرلیک;
    UCATable_سرلیک *بهر_سرلیک;
    UCATable_سرلیک سرلیک;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<-1 || (length>0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    دننه_بایټونه = static_cast<ثابت uint8_t*>(دننه_معلومات);
    بهر_بایټونه = static_cast<uint8_t*>(بهر_معلومات);

    دننه_سرلیک = static_cast<ثابت UCATable_سرلیک*>(دننه_معلومات);
    بهر_سرلیک = static_cast<UCATable_سرلیک*>(بهر_معلومات);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&سرلیک, 0, حجم(سرلیک));
    إذا(length<0) {
        سرلیک.size=udata_readInt32(ds, دننه_سرلیک->size);
    } وإلا إذا((length<(42*4) || length<(سرلیک.size=udata_readInt32(ds, دننه_سرلیک->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    سرلیک.magic=ds->readUInt32(دننه_سرلیک->magic);
    إذا(!(
        سرلیک.magic==UCOL_سرلیک_MAGIC &&
        دننه_سرلیک->format_نسخه[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         سرلیک.magic,
                         دننه_سرلیک->format_نسخه[0], دننه_سرلیک->format_نسخه[1]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    إذا(دننه_سرلیک->isBigEndian!=ds->دننه_Is_Big_Endian || دننه_سرلیک->char_سېټ_Family!=ds->دننه_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         دننه_سرلیک->isBigEndian, دننه_سرلیک->char_سېټ_Family);
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(دننه_بایټونه!=بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, سرلیک.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        سرلیک.options=                 ds->readUInt32(دننه_سرلیک->options);
        سرلیک.UCAConsts=               ds->readUInt32(دننه_سرلیک->UCAConsts);
        سرلیک.contractionUCACombos=    ds->readUInt32(دننه_سرلیک->contractionUCACombos);
        سرلیک.mappingPosition=         ds->readUInt32(دننه_سرلیک->mappingPosition);
        سرلیک.expansion=               ds->readUInt32(دننه_سرلیک->expansion);
        سرلیک.contraction_شاخص=        ds->readUInt32(دننه_سرلیک->contraction_شاخص);
        سرلیک.contractionCEs=          ds->readUInt32(دننه_سرلیک->contractionCEs);
        سرلیک.contraction_اندازه=         ds->readUInt32(دننه_سرلیک->contraction_اندازه);
        سرلیک.پای_ټکی_Expansion_CE=          ds->readUInt32(دننه_سرلیک->پای_ټکی_Expansion_CE);
        سرلیک.expansionCESize=         ds->readUInt32(دننه_سرلیک->expansionCESize);
        سرلیک.پای_ټکی_Expansion_CECount=     udata_readInt32(ds, دننه_سرلیک->پای_ټکی_Expansion_CECount);
        سرلیک.contraction_UCACombos_اندازه=udata_readInt32(ds, دننه_سرلیک->contraction_UCACombos_اندازه);
        سرلیک.script_منزل_Lead_Byte=        ds->readUInt32(دننه_سرلیک->script_منزل_Lead_Byte);
        سرلیک.lead_Byte_منزل_Script=        ds->readUInt32(دننه_سرلیک->lead_Byte_منزل_Script);
        
        /* swap the 32-bit integers in the header */
        ds->بدلون_Array32(ds, دننه_سرلیک, static_cast<int32_t>(reinterpret_cast<ثابت حرف*>(&دننه_سرلیک->jamoSpecial) - reinterpret_cast<ثابت حرف*>(دننه_سرلیک)),
                           بهر_سرلیک, p_Error_کوډ);
        ds->بدلون_Array32(ds, &(دننه_سرلیک->script_منزل_Lead_Byte), حجم(سرلیک.script_منزل_Lead_Byte) + حجم(سرلیک.lead_Byte_منزل_Script),
                           &(بهر_سرلیک->script_منزل_Lead_Byte), p_Error_کوډ);
        /* set the output platform properties */
        بهر_سرلیک->isBigEndian=ds->بهر_Is_Big_Endian;
        بهر_سرلیک->char_سېټ_Family=ds->بهر_Charset;

        /* swap the options */
        إذا(سرلیک.options!=0) {
            ds->بدلون_Array32(ds, دننه_بایټونه+سرلیک.options, سرلیک.expansion-سرلیک.options,
                               بهر_بایټونه+سرلیک.options, p_Error_کوډ);
        }

        /* swap the expansions */
        إذا(سرلیک.mappingPosition!=0 && سرلیک.expansion!=0) {
            إذا(سرلیک.contraction_شاخص!=0) {
                /* expansions bounded by contractions */
                count=سرلیک.contraction_شاخص-سرلیک.expansion;
            } وإلا {
                /* no contractions: expansions bounded by the main trie */
                count=سرلیک.mappingPosition-سرلیک.expansion;
            }
            ds->بدلون_Array32(ds, دننه_بایټونه + سرلیک.expansion, static_cast<int32_t>(count),
                               بهر_بایټونه+سرلیک.expansion, p_Error_کوډ);
        }

        /* swap the contractions */
        إذا(سرلیک.contraction_اندازه!=0) {
            /* contractionIndex: char16_t[] */
            ds->بدلون_Array16(ds, دننه_بایټونه+سرلیک.contraction_شاخص, سرلیک.contraction_اندازه*2,
                               بهر_بایټونه+سرلیک.contraction_شاخص, p_Error_کوډ);

            /* contractionCEs: CEs[] */
            ds->بدلون_Array32(ds, دننه_بایټونه+سرلیک.contractionCEs, سرلیک.contraction_اندازه*4,
                               بهر_بایټونه+سرلیک.contractionCEs, p_Error_کوډ);
        }

        /* swap the main trie */
        إذا(سرلیک.mappingPosition!=0) {
            count=سرلیک.پای_ټکی_Expansion_CE-سرلیک.mappingPosition;
            utrie_بدلون(ds, دننه_بایټونه + سرلیک.mappingPosition, static_cast<int32_t>(count),
                          بهر_بایټونه+سرلیک.mappingPosition, p_Error_کوډ);
        }

        /* swap the max expansion table */
        إذا(سرلیک.پای_ټکی_Expansion_CECount!=0) {
            ds->بدلون_Array32(ds, دننه_بایټونه+سرلیک.پای_ټکی_Expansion_CE, سرلیک.پای_ټکی_Expansion_CECount*4,
                               بهر_بایټونه+سرلیک.پای_ټکی_Expansion_CE, p_Error_کوډ);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        إذا(سرلیک.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->بدلون_Array32(ds, دننه_بایټونه+سرلیک.UCAConsts, سرلیک.contractionUCACombos-سرلیک.UCAConsts,
                               بهر_بایټونه+سرلیک.UCAConsts, p_Error_کوډ);
        }

        /* swap UCA contractions */
        إذا(سرلیک.contraction_UCACombos_اندازه!=0) {
            count=سرلیک.contraction_UCACombos_اندازه*دننه_سرلیک->contraction_UCACombos_پلنوالی*U_SIZEOF_UCHAR;
            ds->بدلون_Array16(ds, دننه_بایټونه + سرلیک.contractionUCACombos, static_cast<int32_t>(count),
                               بهر_بایټونه+سرلیک.contractionUCACombos, p_Error_کوډ);
        }
        
        /* swap the script to lead bytes */
        إذا(سرلیک.script_منزل_Lead_Byte!=0) {
            صحيح شاخص_شمېر = ds->readUInt16(*((uint16_t*)(دننه_بایټونه+سرلیک.script_منزل_Lead_Byte))); // each entry = 2 * uint16
            صحيح معلومات_شمېر = ds->readUInt16(*((uint16_t*)(دننه_بایټونه+سرلیک.script_منزل_Lead_Byte + 2))); // each entry = uint16
            ds->بدلون_Array16(ds, دننه_بایټونه+سرلیک.script_منزل_Lead_Byte, 
                                4 + (4 * شاخص_شمېر) + (2 * معلومات_شمېر),
                                بهر_بایټونه+سرلیک.script_منزل_Lead_Byte, p_Error_کوډ);
        }
        
        /* swap the lead byte to scripts */
        إذا(سرلیک.lead_Byte_منزل_Script!=0) {
            صحيح شاخص_شمېر = ds->readUInt16(*((uint16_t*)(دننه_بایټونه+سرلیک.lead_Byte_منزل_Script))); // each entry = uint16
            صحيح معلومات_شمېر = ds->readUInt16(*((uint16_t*)(دننه_بایټونه+سرلیک.lead_Byte_منزل_Script + 2))); // each entry = uint16
            ds->بدلون_Array16(ds, دننه_بایټونه+سرلیک.lead_Byte_منزل_Script, 
                                4 + (2 * شاخص_شمېر) + (2 * معلومات_شمېر),
                                بهر_بایټونه+سرلیک.lead_Byte_منزل_Script, p_Error_کوډ);
        }
    }

    أرجع سرلیک.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

تعداد {
    IX_INDEXES_اوږدتیا,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_پیل,  // 4
    IX_REORDER_CODES_دننه_واټن,
    IX_REORDER_جدول_دننه_واټن,
    IX_TRIE_دننه_واټن,

    IX_RESERVED8_دننه_واټن,  // 8
    IX_CES_دننه_واټن,
    IX_RESERVED10_دننه_واټن,
    IX_CE32_S_دننه_واټن,

    IX_ریښه_ELEMENTS_دننه_واټن,  // 12
    IX_CONTEXTS_دننه_واټن,
    IX_UNSAFE_BWD_دننه_واټن,
    IX_FAST_LATIN_جدول_دننه_واټن,

    IX_SCRIPTS_دننه_واټن,  // 16
    IX_COMPRESSIBLE_بایټونه_دننه_واټن,
    IX_RESERVED18_دننه_واټن,
    IX_ټول_اندازه
};

int32_t
بدلون_Format_Version4(ثابت UDataSwapper *ds,
                   ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                   UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) { أرجع 0; }

    ثابت uint8_t* دننه_بایټونه = static_cast<ثابت uint8_t*>(دننه_معلومات);
    uint8_t* بهر_بایټونه = static_cast<uint8_t*>(بهر_معلومات);

    ثابت int32_t* دننه_Indexes = reinterpret_cast<ثابت int32_t*>(دننه_بایټونه);
    int32_t indexes[IX_ټول_اندازه+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    إذا(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    int32_t indexes_اوږدتیا=indexes[0]=udata_readInt32(ds, دننه_Indexes[0]);
    إذا(0<=length && length<(indexes_اوږدتیا*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    لكل(int32_t i=1; i<=IX_ټول_اندازه && i<indexes_اوږدتیا; ++i) {
        indexes[i]=udata_readInt32(ds, دننه_Indexes[i]);
    }
    لكل(int32_t i=indexes_اوږدتیا; i<=IX_ټول_اندازه; ++i) {
        indexes[i]=-1;
    }
    دننه_Indexes=تش_اشاره;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    إذا(indexes_اوږدتیا>IX_ټول_اندازه) {
        size=indexes[IX_ټول_اندازه];
    } وإلا إذا(indexes_اوږدتیا>IX_REORDER_CODES_دننه_واټن) {
        size=indexes[indexes_اوږدتیا-1];
    } وإلا {
        size=indexes_اوږدتیا*4;
    }
    إذا(length<0) { أرجع size; }

    إذا(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    إذا(دننه_بایټونه!=بهر_بایټونه) {
        uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
    }

    // Swap the int32_t indexes[].
    ds->بدلون_Array32(ds, دننه_بایټونه, indexes_اوږدتیا * 4, بهر_بایټونه, &error_کوډ);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t شاخص;  // one of the indexes[] slots
    int32_t دننه_واټن;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    شاخص = IX_REORDER_CODES_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array32(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    شاخص = IX_TRIE_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        utrie2_بدلون(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_RESERVED8_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_کوډ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    شاخص = IX_CES_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array64(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_RESERVED10_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_کوډ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    شاخص = IX_CE32_S_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array32(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_ریښه_ELEMENTS_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array32(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_CONTEXTS_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array16(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_UNSAFE_BWD_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array16(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_FAST_LATIN_جدول_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array16(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    شاخص = IX_SCRIPTS_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        ds->بدلون_Array16(ds, دننه_بایټونه + دننه_واټن, length, بهر_بایټونه + دننه_واټن, &error_کوډ);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    شاخص = IX_RESERVED18_دننه_واټن;
    دننه_واټن = indexes[شاخص];
    length = indexes[شاخص + 1] - دننه_واټن;
    إذا(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_کوډ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    أرجع size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_بدلون(ثابت UDataSwapper *ds,
          ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
          UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) { أرجع 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_کوډ=U_ZERO_ERROR;
        أرجع بدلون_Format_Version3(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    }

    /* check data format and format version */
    ثابت UDataInfo &info=*(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        info.معلومات_Format[0]==0x55 &&   // dataFormat="UCol"
        info.معلومات_Format[1]==0x43 &&
        info.معلومات_Format[2]==0x6f &&
        info.معلومات_Format[3]==0x6c &&
        (3<=info.format_نسخه[0] && info.format_نسخه[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.معلومات_Format[0], info.معلومات_Format[1],
                         info.معلومات_Format[2], info.معلومات_Format[3],
                         info.format_نسخه[0], info.format_نسخه[1]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_معلومات=(ثابت حرف *)دننه_معلومات+سرلیک_اندازه;
    إذا(length>=0) { length-=سرلیک_اندازه; }
    بهر_معلومات=(بهر_معلومات == تش_اشاره) ? تش_اشاره : (حرف *)بهر_معلومات+سرلیک_اندازه;
    int32_t collation_اندازه;
    إذا(info.format_نسخه[0]>=4) {
        collation_اندازه=بدلون_Format_Version4(ds, دننه_معلومات, length, بهر_معلومات, *p_Error_کوډ);
    } وإلا {
        collation_اندازه=بدلون_Format_Version3(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    }
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        أرجع سرلیک_اندازه+collation_اندازه;
    } وإلا {
        أرجع 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_بدلون_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                    UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    ثابت Inverse_UCATable_سرلیک *دننه_سرلیک;
    Inverse_UCATable_سرلیک *بهر_سرلیک;
    Inverse_UCATable_سرلیک سرلیک={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x76 &&
        pInfo->معلومات_Format[3]==0x43 &&
        pInfo->format_نسخه[0]==2 &&
        pInfo->format_نسخه[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه=(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    بهر_بایټونه=(uint8_t *)بهر_معلومات+سرلیک_اندازه;

    دننه_سرلیک=(ثابت Inverse_UCATable_سرلیک *)دننه_بایټونه;
    بهر_سرلیک=(Inverse_UCATable_سرلیک *)بهر_بایټونه;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    إذا(length<0) {
        سرلیک.byte_اندازه=udata_readInt32(ds, دننه_سرلیک->byte_اندازه);
    } وإلا إذا(
        ((length-سرلیک_اندازه)<(8*4) ||
         (uint32_t)(length-سرلیک_اندازه)<(سرلیک.byte_اندازه=udata_readInt32(ds, دننه_سرلیک->byte_اندازه)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        إذا(دننه_بایټونه!=بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, سرلیک.byte_اندازه);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        سرلیک.جدول_اندازه=   ds->readUInt32(دننه_سرلیک->جدول_اندازه);
        سرلیک.conts_اندازه=   ds->readUInt32(دننه_سرلیک->conts_اندازه);
        سرلیک.جدول=       ds->readUInt32(دننه_سرلیک->جدول);
        سرلیک.conts=       ds->readUInt32(دننه_سرلیک->conts);

        /* swap the 32-bit integers in the header */
        ds->بدلون_Array32(ds, دننه_سرلیک, 5*4, بهر_سرلیک, p_Error_کوډ);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->بدلون_Array32(ds, دننه_بایټونه+سرلیک.جدول, سرلیک.جدول_اندازه*3*4,
                           بهر_بایټونه+سرلیک.جدول, p_Error_کوډ);

        /* swap the continuation table; contsSize counts UChars */
        ds->بدلون_Array16(ds, دننه_بایټونه+سرلیک.conts, سرلیک.conts_اندازه*U_SIZEOF_UCHAR,
                           بهر_بایټونه+سرلیک.conts, p_Error_کوډ);
    }

    أرجع سرلیک_اندازه+سرلیک.byte_اندازه;
}

#د_که_پای /* #if !UCONFIG_NO_COLLATION */
