// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  udataswp.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003jun05
*   created by: Markus W. Scherer
*
*   Definitions for ICU data transformations for different platforms,
*   changing between big- and little-endian data and/or between
*   charset families (ASCII<->EBCDIC).
*/

#అన్తర్భావ <stdarg.h>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/udata.h" /* UDataInfo */
#అన్తర్భావ "ucmndata.h" /* DataHeader */
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

స్థిర int32_t U_CALLCONV
uprv_మార్పిడి_Array16(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&1)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    p = static_cast<స్థిరరాశి uint16_t*>(లోపల_దత్తాంశం);
    q = static_cast<uint16_t*>(బహిర్గతం_దత్తాంశం);
    count=length/2;
    అయితే(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
uprv_copyArray16(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&1)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>0 && లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
        uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, length);
    }
    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
uprv_మార్పిడి_Array32(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&3)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    p = static_cast<స్థిరరాశి uint32_t*>(లోపల_దత్తాంశం);
    q = static_cast<uint32_t*>(బహిర్గతం_దత్తాంశం);
    count=length/4;
    అయితే(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
uprv_copyArray32(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&3)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>0 && లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
        uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, length);
    }
    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
uprv_మార్పిడి_Array64(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint64_t *p;
    uint64_t *q;
    int32_t count;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&7)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    p = static_cast<స్థిరరాశి uint64_t*>(లోపల_దత్తాంశం);
    q = static_cast<uint64_t*>(బహిర్గతం_దత్తాంశం);
    count=length/8;
    అయితే(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    తిరిగి length;
}

స్థిర int32_t U_CALLCONV
uprv_copyArray64(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length&7)!=0 || బహిర్గతం_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>0 && లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
        uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, length);
    }
    తిరిగి length;
}

స్థిర uint16_t U_CALLCONV
uprv_read_మార్పిడి_UInt16(uint16_t x) {
    తిరిగి static_cast<uint16_t>((x << 8) | (x >> 8));
}

స్థిర uint16_t U_CALLCONV
uprv_read_ప్రత్యక్ష_UInt16(uint16_t x) {
    తిరిగి x;
}

స్థిర uint32_t U_CALLCONV
uprv_read_మార్పిడి_UInt32(uint32_t x) {
    తిరిగి (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

స్థిర uint32_t U_CALLCONV
uprv_read_ప్రత్యక్ష_UInt32(uint32_t x) {
    తిరిగి x;
}

స్థిర శూన్య U_CALLCONV
uprv_write_మార్పిడి_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

స్థిర శూన్య U_CALLCONV
uprv_write_ప్రత్యక్ష_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

స్థిర శూన్య U_CALLCONV
uprv_write_మార్పిడి_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

స్థిర శూన్య U_CALLCONV
uprv_write_ప్రత్యక్ష_UInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(స్థిరరాశి UDataSwapper *ds, int16_t x) {
    తిరిగి (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(స్థిరరాశి UDataSwapper *ds, int32_t x) {
    తిరిగి (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_మార్పిడి_Inv_String_ఖండం(స్థిరరాశి UDataSwapper *ds,
                         స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                         UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి అక్షరం *లోపల_Chars;
    int32_t అక్షరశ్రేణులు_నిడివి;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    లోపల_Chars=(స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం;
    అక్షరశ్రేణులు_నిడివి=length;
    అయితే(అక్షరశ్రేణులు_నిడివి>0 && లోపల_Chars[అక్షరశ్రేణులు_నిడివి-1]!=0) {
        --అక్షరశ్రేణులు_నిడివి;
    }

    /* swap up to the last NUL */
    ds->మార్పిడి_Inv_Chars(ds, లోపల_దత్తాంశం, అక్షరశ్రేణులు_నిడివి, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);

    /* copy the bytes after the last NUL */
    ఒకవేళ(లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం && length>అక్షరశ్రేణులు_నిడివి) {
        uprv_memcpy((అక్షరం *)బహిర్గతం_దత్తాంశం+అక్షరశ్రేణులు_నిడివి, లోపల_Chars+అక్షరశ్రేణులు_నిడివి, length-అక్షరశ్రేణులు_నిడివి);
    }

    /* return the length including padding bytes */
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        తిరిగి length;
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI శూన్య U_EXPORT2
udata_printError(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి అక్షరం *fmt,
                 ...) {
    va_list వాదనలు_2;

    ఒకవేళ(ds->printError!=శూన్యసూచిక) {
        va_start(వాదనలు_2, fmt);
        ds->printError(ds->printErrorContext, fmt, వాదనలు_2);
        va_end(వాదనలు_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_మార్పిడి_దత్తాంశం_శీర్షిక(స్థిరరాశి UDataSwapper *ds,
                     స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి దత్తాంశం_శీర్షిక *p_శీర్షిక;
    uint16_t శీర్షిక_కొలత, info_కొలత;

    /* argument checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<-1 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* check minimum length and magic bytes */
    p_శీర్షిక=(స్థిరరాశి దత్తాంశం_శీర్షిక *)లోపల_దత్తాంశం;
    ఒకవేళ( (length>=0 && length<(int32_t)పరిమాణం(దత్తాంశం_శీర్షిక)) ||
        p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic1!=0xda ||
        p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic2!=0x27 ||
        p_శీర్షిక->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    శీర్షిక_కొలత=ds->readUInt16(p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత);
    info_కొలత=ds->readUInt16(p_శీర్షిక->info.size);

    ఒకవేళ( శీర్షిక_కొలత<పరిమాణం(దత్తాంశం_శీర్షిక) ||
        info_కొలత<పరిమాణం(UDataInfo) ||
        శీర్షిక_కొలత<(పరిమాణం(p_శీర్షిక->దత్తాంశం_శీర్షిక_2)+info_కొలత) ||
        (length>=0 && length<శీర్షిక_కొలత)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         శీర్షిక_కొలత, info_కొలత, length);
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>0) {
        దత్తాంశం_శీర్షిక *బహిర్గతం_శీర్షిక;
        స్థిరరాశి అక్షరం *s;
        int32_t max_నిడివి_2;

        /* Most of the fields are just bytes and need no swapping. */
        ఒకవేళ(లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
            uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, శీర్షిక_కొలత);
        }
        బహిర్గతం_శీర్షిక=(దత్తాంశం_శీర్షిక *)బహిర్గతం_దత్తాంశం;

        బహిర్గతం_శీర్షిక->info.isBigEndian = ds->బహిర్గతం_Is_Big_Endian;
        బహిర్గతం_శీర్షిక->info.charsetFamily = ds->బహిర్గతం_Charset;

        /* swap headerSize */
        ds->మార్పిడి_Array16(ds, &p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత, 2, &బహిర్గతం_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత, p_Error_సంకేతం);

        /* swap UDataInfo size and reservedWord */
        ds->మార్పిడి_Array16(ds, &p_శీర్షిక->info.size, 4, &బహిర్గతం_శీర్షిక->info.size, p_Error_సంకేతం);

        /* swap copyright statement after the UDataInfo */
        info_కొలత+=పరిమాణం(p_శీర్షిక->దత్తాంశం_శీర్షిక_2);
        s=(స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+info_కొలత;
        max_నిడివి_2=శీర్షిక_కొలత-info_కొలత;
        /* get the length of the string */
        కొరకు(length=0; length<max_నిడివి_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->మార్పిడి_Inv_Chars(ds, s, length, (అక్షరం *)బహిర్గతం_దత్తాంశం+info_కొలత, p_Error_సంకేతం);
    }

    తిరిగి శీర్షిక_కొలత;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool లోపల_Is_Big_Endian, uint8_t లోపల_Charset,
                  UBool బహిర్గతం_Is_Big_Endian, uint8_t బహిర్గతం_Charset,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UDataSwapper *swapper;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(లోపల_Charset>U_EBCDIC_FAMILY || బహిర్గతం_Charset>U_EBCDIC_FAMILY) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(పరిమాణం(UDataSwapper));
    ఒకవేళ(swapper==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memset(swapper, 0, పరిమాణం(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->లోపల_Is_Big_Endian=లోపల_Is_Big_Endian;
    swapper->లోపల_Charset=లోపల_Charset;
    swapper->బహిర్గతం_Is_Big_Endian=బహిర్గతం_Is_Big_Endian;
    swapper->బహిర్గతం_Charset=బహిర్గతం_Charset;

    swapper->readUInt16= లోపల_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ప్రత్యక్ష_UInt16 : uprv_read_మార్పిడి_UInt16;
    swapper->readUInt32= లోపల_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ప్రత్యక్ష_UInt32 : uprv_read_మార్పిడి_UInt32;

    swapper->writeUInt16= బహిర్గతం_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ప్రత్యక్ష_UInt16 : uprv_write_మార్పిడి_UInt16;
    swapper->writeUInt32= బహిర్గతం_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ప్రత్యక్ష_UInt32 : uprv_write_మార్పిడి_UInt32;

    swapper->compareInvChars= బహిర్గతం_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    ఒకవేళ(లోపల_Is_Big_Endian==బహిర్గతం_Is_Big_Endian) {
        swapper->మార్పిడి_Array16=uprv_copyArray16;
        swapper->మార్పిడి_Array32=uprv_copyArray32;
        swapper->మార్పిడి_Array64=uprv_copyArray64;
    } లేకపోతే {
        swapper->మార్పిడి_Array16=uprv_మార్పిడి_Array16;
        swapper->మార్పిడి_Array32=uprv_మార్పిడి_Array32;
        swapper->మార్పిడి_Array64=uprv_మార్పిడి_Array64;
    }

    ఒకవేళ(లోపల_Charset==U_ASCII_FAMILY) {
        swapper->మార్పిడి_Inv_Chars= బహిర్గతం_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } లేకపోతే /* U_EBCDIC_FAMILY */ {
        swapper->మార్పిడి_Inv_Chars= బహిర్గతం_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    తిరిగి swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ఆదానం_దత్తాంశం(స్థిరరాశి శూన్య *దత్తాంశం, int32_t length,
                              UBool బహిర్గతం_Is_Big_Endian, uint8_t బహిర్గతం_Charset,
                              UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి దత్తాంశం_శీర్షిక *p_శీర్షిక;
    uint16_t శీర్షిక_కొలత, info_కొలత;
    UBool లోపల_Is_Big_Endian;
    int8_t లోపల_Charset;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ( దత్తాంశం==శూన్యసూచిక ||
        (length>=0 && length<(int32_t)పరిమాణం(దత్తాంశం_శీర్షిక)) ||
        బహిర్గతం_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    p_శీర్షిక=(స్థిరరాశి దత్తాంశం_శీర్షిక *)దత్తాంశం;
    ఒకవేళ( (length>=0 && length<(int32_t)పరిమాణం(దత్తాంశం_శీర్షిక)) ||
        p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic1!=0xda ||
        p_శీర్షిక->దత్తాంశం_శీర్షిక_2.magic2!=0x27 ||
        p_శీర్షిక->info.sizeofUChar!=2
    ) {
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    లోపల_Is_Big_Endian=(UBool)p_శీర్షిక->info.isBigEndian;
    లోపల_Charset=p_శీర్షిక->info.charsetFamily;

    ఒకవేళ(లోపల_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        శీర్షిక_కొలత=p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
        info_కొలత=p_శీర్షిక->info.size;
    } లేకపోతే {
        శీర్షిక_కొలత=uprv_read_మార్పిడి_UInt16(p_శీర్షిక->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత);
        info_కొలత=uprv_read_మార్పిడి_UInt16(p_శీర్షిక->info.size);
    }

    ఒకవేళ( శీర్షిక_కొలత<పరిమాణం(దత్తాంశం_శీర్షిక) ||
        info_కొలత<పరిమాణం(UDataInfo) ||
        శీర్షిక_కొలత<(పరిమాణం(p_శీర్షిక->దత్తాంశం_శీర్షిక_2)+info_కొలత) ||
        (length>=0 && length<శీర్షిక_కొలత)
    ) {
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి udata_openSwapper(లోపల_Is_Big_Endian, లోపల_Charset, బహిర్గతం_Is_Big_Endian, బహిర్గతం_Charset, p_Error_సంకేతం);
}

U_CAPI శూన్య U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
