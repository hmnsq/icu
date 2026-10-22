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

#అన్తర్భావ "unicode/udata.h" /* UDataInfo */
#అన్తర్భావ "utrie.h"
#అన్తర్భావ "utrie2.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucol_data.h"
#అన్తర్భావ "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#ఒకవేళ !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ద్విమాన(స్థిరరాశి UDataSwapper *ds,
                              స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length) {
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<-1) {
        తిరిగి అబద్ధం;
    }

    // First check for format version 4+ which has a standard data header.
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    (శూన్య)udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, -1, శూన్యసూచిక, &error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        స్థిరరాశి UDataInfo &info=*(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
        ఒకవేళ(info.దత్తాంశం_Format[0]==0x55 &&   // dataFormat="UCol"
                info.దత్తాంశం_Format[1]==0x43 &&
                info.దత్తాంశం_Format[2]==0x6f &&
                info.దత్తాంశం_Format[3]==0x6c) {
            తిరిగి నిజం;
        }
    }

    // Else check for format version 3.
    స్థిరరాశి UCATable_శీర్షిక *లోపల_శీర్షిక=(స్థిరరాశి UCATable_శీర్షిక *)లోపల_దత్తాంశం;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATable_శీర్షిక శీర్షిక;
    uprv_memset(&శీర్షిక, 0, పరిమాణం(శీర్షిక));
    ఒకవేళ(length<0) {
        శీర్షిక.size=udata_readInt32(ds, లోపల_శీర్షిక->size);
    } లేకపోతే ఒకవేళ((length<(42*4) || length<(శీర్షిక.size=udata_readInt32(ds, లోపల_శీర్షిక->size)))) {
        తిరిగి అబద్ధం;
    }

    శీర్షిక.magic=ds->readUInt32(లోపల_శీర్షిక->magic);
    ఒకవేళ(!(
        శీర్షిక.magic==UCOL_శీర్షిక_MAGIC &&
        లోపల_శీర్షిక->format_సంస్కరణ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        తిరిగి అబద్ధం;
    }

    ఒకవేళ(లోపల_శీర్షిక->isBigEndian!=ds->లోపల_Is_Big_Endian || లోపల_శీర్షిక->char_సమితి_Family!=ds->లోపల_Charset) {
        తిరిగి అబద్ధం;
    }

    తిరిగి నిజం;
}

నామాకాశ {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
మార్పిడి_Format_Version3(స్థిరరాశి UDataSwapper *ds,
                   స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                   UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    స్థిరరాశి UCATable_శీర్షిక *లోపల_శీర్షిక;
    UCATable_శీర్షిక *బహిర్గతం_శీర్షిక;
    UCATable_శీర్షిక శీర్షిక;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<-1 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(లోపల_దత్తాంశం);
    బహిర్గతం_బైట్లు = static_cast<uint8_t*>(బహిర్గతం_దత్తాంశం);

    లోపల_శీర్షిక = static_cast<స్థిరరాశి UCATable_శీర్షిక*>(లోపల_దత్తాంశం);
    బహిర్గతం_శీర్షిక = static_cast<UCATable_శీర్షిక*>(బహిర్గతం_దత్తాంశం);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&శీర్షిక, 0, పరిమాణం(శీర్షిక));
    ఒకవేళ(length<0) {
        శీర్షిక.size=udata_readInt32(ds, లోపల_శీర్షిక->size);
    } లేకపోతే ఒకవేళ((length<(42*4) || length<(శీర్షిక.size=udata_readInt32(ds, లోపల_శీర్షిక->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    శీర్షిక.magic=ds->readUInt32(లోపల_శీర్షిక->magic);
    ఒకవేళ(!(
        శీర్షిక.magic==UCOL_శీర్షిక_MAGIC &&
        లోపల_శీర్షిక->format_సంస్కరణ[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         శీర్షిక.magic,
                         లోపల_శీర్షిక->format_సంస్కరణ[0], లోపల_శీర్షిక->format_సంస్కరణ[1]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(లోపల_శీర్షిక->isBigEndian!=ds->లోపల_Is_Big_Endian || లోపల_శీర్షిక->char_సమితి_Family!=ds->లోపల_Charset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         లోపల_శీర్షిక->isBigEndian, లోపల_శీర్షిక->char_సమితి_Family);
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, శీర్షిక.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        శీర్షిక.options=                 ds->readUInt32(లోపల_శీర్షిక->options);
        శీర్షిక.UCAConsts=               ds->readUInt32(లోపల_శీర్షిక->UCAConsts);
        శీర్షిక.contractionUCACombos=    ds->readUInt32(లోపల_శీర్షిక->contractionUCACombos);
        శీర్షిక.mappingPosition=         ds->readUInt32(లోపల_శీర్షిక->mappingPosition);
        శీర్షిక.expansion=               ds->readUInt32(లోపల_శీర్షిక->expansion);
        శీర్షిక.contraction_సూచీ=        ds->readUInt32(లోపల_శీర్షిక->contraction_సూచీ);
        శీర్షిక.contractionCEs=          ds->readUInt32(లోపల_శీర్షిక->contractionCEs);
        శీర్షిక.contraction_కొలత=         ds->readUInt32(లోపల_శీర్షిక->contraction_కొలత);
        శీర్షిక.చివర_Expansion_CE=          ds->readUInt32(లోపల_శీర్షిక->చివర_Expansion_CE);
        శీర్షిక.expansionCESize=         ds->readUInt32(లోపల_శీర్షిక->expansionCESize);
        శీర్షిక.చివర_Expansion_CECount=     udata_readInt32(ds, లోపల_శీర్షిక->చివర_Expansion_CECount);
        శీర్షిక.contraction_UCACombos_కొలత=udata_readInt32(ds, లోపల_శీర్షిక->contraction_UCACombos_కొలత);
        శీర్షిక.script_గమ్యం_Lead_Byte=        ds->readUInt32(లోపల_శీర్షిక->script_గమ్యం_Lead_Byte);
        శీర్షిక.lead_Byte_గమ్యం_Script=        ds->readUInt32(లోపల_శీర్షిక->lead_Byte_గమ్యం_Script);
        
        /* swap the 32-bit integers in the header */
        ds->మార్పిడి_Array32(ds, లోపల_శీర్షిక, static_cast<int32_t>(reinterpret_cast<స్థిరరాశి అక్షరం*>(&లోపల_శీర్షిక->jamoSpecial) - reinterpret_cast<స్థిరరాశి అక్షరం*>(లోపల_శీర్షిక)),
                           బహిర్గతం_శీర్షిక, p_Error_సంకేతం);
        ds->మార్పిడి_Array32(ds, &(లోపల_శీర్షిక->script_గమ్యం_Lead_Byte), పరిమాణం(శీర్షిక.script_గమ్యం_Lead_Byte) + పరిమాణం(శీర్షిక.lead_Byte_గమ్యం_Script),
                           &(బహిర్గతం_శీర్షిక->script_గమ్యం_Lead_Byte), p_Error_సంకేతం);
        /* set the output platform properties */
        బహిర్గతం_శీర్షిక->isBigEndian=ds->బహిర్గతం_Is_Big_Endian;
        బహిర్గతం_శీర్షిక->char_సమితి_Family=ds->బహిర్గతం_Charset;

        /* swap the options */
        ఒకవేళ(శీర్షిక.options!=0) {
            ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+శీర్షిక.options, శీర్షిక.expansion-శీర్షిక.options,
                               బహిర్గతం_బైట్లు+శీర్షిక.options, p_Error_సంకేతం);
        }

        /* swap the expansions */
        ఒకవేళ(శీర్షిక.mappingPosition!=0 && శీర్షిక.expansion!=0) {
            ఒకవేళ(శీర్షిక.contraction_సూచీ!=0) {
                /* expansions bounded by contractions */
                count=శీర్షిక.contraction_సూచీ-శీర్షిక.expansion;
            } లేకపోతే {
                /* no contractions: expansions bounded by the main trie */
                count=శీర్షిక.mappingPosition-శీర్షిక.expansion;
            }
            ds->మార్పిడి_Array32(ds, లోపల_బైట్లు + శీర్షిక.expansion, static_cast<int32_t>(count),
                               బహిర్గతం_బైట్లు+శీర్షిక.expansion, p_Error_సంకేతం);
        }

        /* swap the contractions */
        ఒకవేళ(శీర్షిక.contraction_కొలత!=0) {
            /* contractionIndex: char16_t[] */
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+శీర్షిక.contraction_సూచీ, శీర్షిక.contraction_కొలత*2,
                               బహిర్గతం_బైట్లు+శీర్షిక.contraction_సూచీ, p_Error_సంకేతం);

            /* contractionCEs: CEs[] */
            ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+శీర్షిక.contractionCEs, శీర్షిక.contraction_కొలత*4,
                               బహిర్గతం_బైట్లు+శీర్షిక.contractionCEs, p_Error_సంకేతం);
        }

        /* swap the main trie */
        ఒకవేళ(శీర్షిక.mappingPosition!=0) {
            count=శీర్షిక.చివర_Expansion_CE-శీర్షిక.mappingPosition;
            utrie_మార్పిడి(ds, లోపల_బైట్లు + శీర్షిక.mappingPosition, static_cast<int32_t>(count),
                          బహిర్గతం_బైట్లు+శీర్షిక.mappingPosition, p_Error_సంకేతం);
        }

        /* swap the max expansion table */
        ఒకవేళ(శీర్షిక.చివర_Expansion_CECount!=0) {
            ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+శీర్షిక.చివర_Expansion_CE, శీర్షిక.చివర_Expansion_CECount*4,
                               బహిర్గతం_బైట్లు+శీర్షిక.చివర_Expansion_CE, p_Error_సంకేతం);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        ఒకవేళ(శీర్షిక.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+శీర్షిక.UCAConsts, శీర్షిక.contractionUCACombos-శీర్షిక.UCAConsts,
                               బహిర్గతం_బైట్లు+శీర్షిక.UCAConsts, p_Error_సంకేతం);
        }

        /* swap UCA contractions */
        ఒకవేళ(శీర్షిక.contraction_UCACombos_కొలత!=0) {
            count=శీర్షిక.contraction_UCACombos_కొలత*లోపల_శీర్షిక->contraction_UCACombos_వెడల్పు*U_SIZEOF_UCHAR;
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + శీర్షిక.contractionUCACombos, static_cast<int32_t>(count),
                               బహిర్గతం_బైట్లు+శీర్షిక.contractionUCACombos, p_Error_సంకేతం);
        }
        
        /* swap the script to lead bytes */
        ఒకవేళ(శీర్షిక.script_గమ్యం_Lead_Byte!=0) {
            పూర్ణాంకం సూచీ_లెక్క = ds->readUInt16(*((uint16_t*)(లోపల_బైట్లు+శీర్షిక.script_గమ్యం_Lead_Byte))); // each entry = 2 * uint16
            పూర్ణాంకం దత్తాంశం_లెక్క = ds->readUInt16(*((uint16_t*)(లోపల_బైట్లు+శీర్షిక.script_గమ్యం_Lead_Byte + 2))); // each entry = uint16
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+శీర్షిక.script_గమ్యం_Lead_Byte, 
                                4 + (4 * సూచీ_లెక్క) + (2 * దత్తాంశం_లెక్క),
                                బహిర్గతం_బైట్లు+శీర్షిక.script_గమ్యం_Lead_Byte, p_Error_సంకేతం);
        }
        
        /* swap the lead byte to scripts */
        ఒకవేళ(శీర్షిక.lead_Byte_గమ్యం_Script!=0) {
            పూర్ణాంకం సూచీ_లెక్క = ds->readUInt16(*((uint16_t*)(లోపల_బైట్లు+శీర్షిక.lead_Byte_గమ్యం_Script))); // each entry = uint16
            పూర్ణాంకం దత్తాంశం_లెక్క = ds->readUInt16(*((uint16_t*)(లోపల_బైట్లు+శీర్షిక.lead_Byte_గమ్యం_Script + 2))); // each entry = uint16
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+శీర్షిక.lead_Byte_గమ్యం_Script, 
                                4 + (2 * సూచీ_లెక్క) + (2 * దత్తాంశం_లెక్క),
                                బహిర్గతం_బైట్లు+శీర్షిక.lead_Byte_గమ్యం_Script, p_Error_సంకేతం);
        }
    }

    తిరిగి శీర్షిక.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

గణన {
    IX_INDEXES_నిడివి,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32_S_ప్రారంభం,  // 4
    IX_REORDER_CODES_స్థానభ్రంశం,
    IX_REORDER_పట్టిక_స్థానభ్రంశం,
    IX_TRIE_స్థానభ్రంశం,

    IX_RESERVED8_స్థానభ్రంశం,  // 8
    IX_CES_స్థానభ్రంశం,
    IX_RESERVED10_స్థానభ్రంశం,
    IX_CE32_S_స్థానభ్రంశం,

    IX_మూలం_ELEMENTS_స్థానభ్రంశం,  // 12
    IX_CONTEXTS_స్థానభ్రంశం,
    IX_UNSAFE_BWD_స్థానభ్రంశం,
    IX_FAST_LATIN_పట్టిక_స్థానభ్రంశం,

    IX_SCRIPTS_స్థానభ్రంశం,  // 16
    IX_COMPRESSIBLE_బైట్లు_స్థానభ్రంశం,
    IX_RESERVED18_స్థానభ్రంశం,
    IX_మొత్తం_కొలత
};

int32_t
మార్పిడి_Format_Version4(స్థిరరాశి UDataSwapper *ds,
                   స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                   UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) { తిరిగి 0; }

    స్థిరరాశి uint8_t* లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(లోపల_దత్తాంశం);
    uint8_t* బహిర్గతం_బైట్లు = static_cast<uint8_t*>(బహిర్గతం_దత్తాంశం);

    స్థిరరాశి int32_t* లోపల_Indexes = reinterpret_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
    int32_t indexes[IX_మొత్తం_కొలత+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    ఒకవేళ(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    int32_t indexes_నిడివి=indexes[0]=udata_readInt32(ds, లోపల_Indexes[0]);
    ఒకవేళ(0<=length && length<(indexes_నిడివి*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    కొరకు(int32_t i=1; i<=IX_మొత్తం_కొలత && i<indexes_నిడివి; ++i) {
        indexes[i]=udata_readInt32(ds, లోపల_Indexes[i]);
    }
    కొరకు(int32_t i=indexes_నిడివి; i<=IX_మొత్తం_కొలత; ++i) {
        indexes[i]=-1;
    }
    లోపల_Indexes=శూన్యసూచిక;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    ఒకవేళ(indexes_నిడివి>IX_మొత్తం_కొలత) {
        size=indexes[IX_మొత్తం_కొలత];
    } లేకపోతే ఒకవేళ(indexes_నిడివి>IX_REORDER_CODES_స్థానభ్రంశం) {
        size=indexes[indexes_నిడివి-1];
    } లేకపోతే {
        size=indexes_నిడివి*4;
    }
    ఒకవేళ(length<0) { తిరిగి size; }

    ఒకవేళ(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
        uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
    }

    // Swap the int32_t indexes[].
    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, indexes_నిడివి * 4, బహిర్గతం_బైట్లు, &error_సంకేతం);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t సూచీ;  // one of the indexes[] slots
    int32_t స్థానభ్రంశం;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    సూచీ = IX_REORDER_CODES_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    సూచీ = IX_TRIE_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        utrie2_మార్పిడి(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_RESERVED8_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        error_సంకేతం = U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    సూచీ = IX_CES_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array64(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_RESERVED10_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        error_సంకేతం = U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    సూచీ = IX_CE32_S_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_మూలం_ELEMENTS_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_CONTEXTS_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_UNSAFE_BWD_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_FAST_LATIN_పట్టిక_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    సూచీ = IX_SCRIPTS_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + స్థానభ్రంశం, length, బహిర్గతం_బైట్లు + స్థానభ్రంశం, &error_సంకేతం);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    సూచీ = IX_RESERVED18_స్థానభ్రంశం;
    స్థానభ్రంశం = indexes[సూచీ];
    length = indexes[సూచీ + 1] - స్థానభ్రంశం;
    ఒకవేళ(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        error_సంకేతం = U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    తిరిగి size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) { తిరిగి 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        // Try to swap the old format version which did not have a standard data header.
        *p_Error_సంకేతం=U_ZERO_ERROR;
        తిరిగి మార్పిడి_Format_Version3(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    }

    /* check data format and format version */
    స్థిరరాశి UDataInfo &info=*(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        info.దత్తాంశం_Format[0]==0x55 &&   // dataFormat="UCol"
        info.దత్తాంశం_Format[1]==0x43 &&
        info.దత్తాంశం_Format[2]==0x6f &&
        info.దత్తాంశం_Format[3]==0x6c &&
        (3<=info.format_సంస్కరణ[0] && info.format_సంస్కరణ[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.దత్తాంశం_Format[0], info.దత్తాంశం_Format[1],
                         info.దత్తాంశం_Format[2], info.దత్తాంశం_Format[3],
                         info.format_సంస్కరణ[0], info.format_సంస్కరణ[1]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_దత్తాంశం=(స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    ఒకవేళ(length>=0) { length-=శీర్షిక_కొలత; }
    బహిర్గతం_దత్తాంశం=(బహిర్గతం_దత్తాంశం == శూన్యసూచిక) ? శూన్యసూచిక : (అక్షరం *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;
    int32_t collation_కొలత;
    ఒకవేళ(info.format_సంస్కరణ[0]>=4) {
        collation_కొలత=మార్పిడి_Format_Version4(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, *p_Error_సంకేతం);
    } లేకపోతే {
        collation_కొలత=మార్పిడి_Format_Version3(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    }
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        తిరిగి శీర్షిక_కొలత+collation_కొలత;
    } లేకపోతే {
        తిరిగి 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_మార్పిడి_Inverse_UCA(స్థిరరాశి UDataSwapper *ds,
                    స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                    UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    స్థిరరాశి Inverse_UCATable_శీర్షిక *లోపల_శీర్షిక;
    Inverse_UCATable_శీర్షిక *బహిర్గతం_శీర్షిక;
    Inverse_UCATable_శీర్షిక శీర్షిక={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->దత్తాంశం_Format[1]==0x6e &&
        pInfo->దత్తాంశం_Format[2]==0x76 &&
        pInfo->దత్తాంశం_Format[3]==0x43 &&
        pInfo->format_సంస్కరణ[0]==2 &&
        pInfo->format_సంస్కరణ[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0], pInfo->format_సంస్కరణ[1]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు=(uint8_t *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;

    లోపల_శీర్షిక=(స్థిరరాశి Inverse_UCATable_శీర్షిక *)లోపల_బైట్లు;
    బహిర్గతం_శీర్షిక=(Inverse_UCATable_శీర్షిక *)బహిర్గతం_బైట్లు;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    ఒకవేళ(length<0) {
        శీర్షిక.byte_కొలత=udata_readInt32(ds, లోపల_శీర్షిక->byte_కొలత);
    } లేకపోతే ఒకవేళ(
        ((length-శీర్షిక_కొలత)<(8*4) ||
         (uint32_t)(length-శీర్షిక_కొలత)<(శీర్షిక.byte_కొలత=udata_readInt32(ds, లోపల_శీర్షిక->byte_కొలత)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, శీర్షిక.byte_కొలత);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        శీర్షిక.పట్టిక_కొలత=   ds->readUInt32(లోపల_శీర్షిక->పట్టిక_కొలత);
        శీర్షిక.conts_కొలత=   ds->readUInt32(లోపల_శీర్షిక->conts_కొలత);
        శీర్షిక.పట్టిక=       ds->readUInt32(లోపల_శీర్షిక->పట్టిక);
        శీర్షిక.conts=       ds->readUInt32(లోపల_శీర్షిక->conts);

        /* swap the 32-bit integers in the header */
        ds->మార్పిడి_Array32(ds, లోపల_శీర్షిక, 5*4, బహిర్గతం_శీర్షిక, p_Error_సంకేతం);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+శీర్షిక.పట్టిక, శీర్షిక.పట్టిక_కొలత*3*4,
                           బహిర్గతం_బైట్లు+శీర్షిక.పట్టిక, p_Error_సంకేతం);

        /* swap the continuation table; contsSize counts UChars */
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+శీర్షిక.conts, శీర్షిక.conts_కొలత*U_SIZEOF_UCHAR,
                           బహిర్గతం_బైట్లు+శీర్షిక.conts, p_Error_సంకేతం);
    }

    తిరిగి శీర్షిక_కొలత+శీర్షిక.byte_కొలత;
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_COLLATION */
