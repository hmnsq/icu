// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucptrie_impl.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "utrie.h"
#అన్తర్భావ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

నామాకాశ {

constexpr int32_t ASCII_హద్దు = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
           స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
           UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UTrie_శీర్షిక *లోపల_Trie;
    UTrie_శీర్షిక trie;
    int32_t size;
    UBool దత్తాంశం_Is32;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || (length>=0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    ఒకవేళ(length>=0 && (uint32_t)length<పరిమాణం(UTrie_శీర్షిక)) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    లోపల_Trie=(స్థిరరాశి UTrie_శీర్షిక *)లోపల_దత్తాంశం;
    trie.signature=ds->readUInt32(లోపల_Trie->signature);
    trie.options=ds->readUInt32(లోపల_Trie->options);
    trie.సూచీ_నిడివి=udata_readInt32(ds, లోపల_Trie->సూచీ_నిడివి);
    trie.దత్తాంశం_నిడివి=udata_readInt32(ds, లోపల_Trie->దత్తాంశం_నిడివి);

    ఒకవేళ( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_జరుపు_ముసుగు)!=UTRIE_జరుపు ||
        ((trie.options>>UTRIE_OPTIONS_సూచీ_జరుపు)&UTRIE_OPTIONS_జరుపు_ముసుగు)!=UTRIE_సూచీ_జరుపు ||
        trie.సూచీ_నిడివి<UTRIE_BMP_సూచీ_నిడివి ||
        (trie.సూచీ_నిడివి&(UTRIE_SURROGATE_ఖండం_లెక్క-1))!=0 ||
        trie.దత్తాంశం_నిడివి<UTRIE_దత్తాంశం_ఖండం_నిడివి ||
        (trie.దత్తాంశం_నిడివి&(UTRIE_దత్తాంశం_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_రేఖీయ)!=0 && trie.దత్తాంశం_నిడివి<(UTRIE_దత్తాంశం_ఖండం_నిడివి+0x100))
    ) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        తిరిగి 0;
    }

    దత్తాంశం_Is32 = (trie.options & UTRIE_OPTIONS_దత్తాంశం_IS_32_బిట్టు) != 0;
    size=పరిమాణం(UTrie_శీర్షిక)+trie.సూచీ_నిడివి*2+trie.దత్తాంశం_నిడివి*(దత్తాంశం_Is32?4:2);

    ఒకవేళ(length>=0) {
        UTrie_శీర్షిక *బహిర్గతం_Trie;

        ఒకవేళ(length<size) {
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        బహిర్గతం_Trie=(UTrie_శీర్షిక *)బహిర్గతం_దత్తాంశం;

        /* swap the header */
        ds->మార్పిడి_Array32(ds, లోపల_Trie, పరిమాణం(UTrie_శీర్షిక), బహిర్గతం_Trie, p_Error_సంకేతం);

        /* swap the index and the data */
        ఒకవేళ(దత్తాంశం_Is32) {
            ds->మార్పిడి_Array16(ds, లోపల_Trie+1, trie.సూచీ_నిడివి*2, బహిర్గతం_Trie+1, p_Error_సంకేతం);
            ds->మార్పిడి_Array32(ds, (స్థిరరాశి uint16_t *)(లోపల_Trie+1)+trie.సూచీ_నిడివి, trie.దత్తాంశం_నిడివి*4,
                                     (uint16_t *)(బహిర్గతం_Trie+1)+trie.సూచీ_నిడివి, p_Error_సంకేతం);
        } లేకపోతే {
            ds->మార్పిడి_Array16(ds, లోపల_Trie+1, (trie.సూచీ_నిడివి+trie.దత్తాంశం_నిడివి)*2, బహిర్గతం_Trie+1, p_Error_సంకేతం);
        }
    }

    తిరిగి size;
}

U_CAPI int32_t U_EXPORT2
utrie2_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
            స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UTrie2_శీర్షిక *లోపల_Trie;
    UTrie2_శీర్షిక trie;
    int32_t దత్తాంశం_నిడివి, size;
    UTrie2_విలువ_బిట్లు విలువ_బిట్లు;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || (length>=0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    ఒకవేళ(length>=0 && length<(int32_t)పరిమాణం(UTrie2_శీర్షిక)) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    లోపల_Trie=(స్థిరరాశి UTrie2_శీర్షిక *)లోపల_దత్తాంశం;
    trie.signature=ds->readUInt32(లోపల_Trie->signature);
    trie.options=ds->readUInt16(లోపల_Trie->options);
    trie.సూచీ_నిడివి=ds->readUInt16(లోపల_Trie->సూచీ_నిడివి);
    trie.shifted_దత్తాంశం_నిడివి=ds->readUInt16(లోపల_Trie->shifted_దత్తాంశం_నిడివి);

    విలువ_బిట్లు=(UTrie2_విలువ_బిట్లు)(trie.options&UTRIE2_OPTIONS_విలువ_బిట్లు_ముసుగు);
    దత్తాంశం_నిడివి=(int32_t)trie.shifted_దత్తాంశం_నిడివి<<UTRIE2_సూచీ_జరుపు;

    ఒకవేళ( trie.signature!=UTRIE2_SIG ||
        విలువ_బిట్లు<0 || UTRIE2_లెక్క_విలువ_బిట్లు<=విలువ_బిట్లు ||
        trie.సూచీ_నిడివి<UTRIE2_సూచీ_1_స్థానభ్రంశం ||
        దత్తాంశం_నిడివి<UTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం
    ) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        తిరిగి 0;
    }

    size=పరిమాణం(UTrie2_శీర్షిక)+trie.సూచీ_నిడివి*2;
    ఎంపిక(విలువ_బిట్లు) {
    స్థితి UTRIE2_16_విలువ_బిట్లు:
        size+=దత్తాంశం_నిడివి*2;
        ఆపు;
    స్థితి UTRIE2_32_విలువ_బిట్లు:
        size+=దత్తాంశం_నిడివి*4;
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>=0) {
        UTrie2_శీర్షిక *బహిర్గతం_Trie;

        ఒకవేళ(length<size) {
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        బహిర్గతం_Trie=(UTrie2_శీర్షిక *)బహిర్గతం_దత్తాంశం;

        /* swap the header */
        ds->మార్పిడి_Array32(ds, &లోపల_Trie->signature, 4, &బహిర్గతం_Trie->signature, p_Error_సంకేతం);
        ds->మార్పిడి_Array16(ds, &లోపల_Trie->options, 12, &బహిర్గతం_Trie->options, p_Error_సంకేతం);

        /* swap the index and the data */
        ఎంపిక(విలువ_బిట్లు) {
        స్థితి UTRIE2_16_విలువ_బిట్లు:
            ds->మార్పిడి_Array16(ds, లోపల_Trie+1, (trie.సూచీ_నిడివి+దత్తాంశం_నిడివి)*2, బహిర్గతం_Trie+1, p_Error_సంకేతం);
            ఆపు;
        స్థితి UTRIE2_32_విలువ_బిట్లు:
            ds->మార్పిడి_Array16(ds, లోపల_Trie+1, trie.సూచీ_నిడివి*2, బహిర్గతం_Trie+1, p_Error_సంకేతం);
            ds->మార్పిడి_Array32(ds, (స్థిరరాశి uint16_t *)(లోపల_Trie+1)+trie.సూచీ_నిడివి, దత్తాంశం_నిడివి*4,
                                     (uint16_t *)(బహిర్గతం_Trie+1)+trie.సూచీ_నిడివి, p_Error_సంకేతం);
            ఆపు;
        అప్రమేయ:
            *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
            తిరిగి 0;
        }
    }

    తిరిగి size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
             స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
             UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UCPTrie_శీర్షిక *లోపల_Trie;
    UCPTrie_శీర్షిక trie;
    int32_t దత్తాంశం_నిడివి, size;
    UCPTrie_విలువ_వెడల్పు విలువ_వెడల్పు;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || (length>=0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    ఒకవేళ(length>=0 && length<(int32_t)పరిమాణం(UCPTrie_శీర్షిక)) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    లోపల_Trie=(స్థిరరాశి UCPTrie_శీర్షిక *)లోపల_దత్తాంశం;
    trie.signature=ds->readUInt32(లోపల_Trie->signature);
    trie.options=ds->readUInt16(లోపల_Trie->options);
    trie.సూచీ_నిడివి=ds->readUInt16(లోపల_Trie->సూచీ_నిడివి);
    trie.దత్తాంశం_నిడివి = ds->readUInt16(లోపల_Trie->దత్తాంశం_నిడివి);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    విలువ_వెడల్పు = (UCPTrie_విలువ_వెడల్పు)(trie.options & UCPTRIE_OPTIONS_విలువ_బిట్లు_ముసుగు);
    దత్తాంశం_నిడివి = ((int32_t)(trie.options & UCPTRIE_OPTIONS_దత్తాంశం_నిడివి_ముసుగు) << 4) | trie.దత్తాంశం_నిడివి;

    int32_t min_సూచీ_నిడివి = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_సూచీ_నిడివి : UCPTRIE_SMALL_సూచీ_నిడివి;
    ఒకవేళ( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_ముసుగు) != 0 ||
        విలువ_వెడల్పు > UCPTRIE_విలువ_బిట్లు_8 ||
        trie.సూచీ_నిడివి < min_సూచీ_నిడివి ||
        దత్తాంశం_నిడివి < ASCII_హద్దు
    ) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        తిరిగి 0;
    }

    size=పరిమాణం(UCPTrie_శీర్షిక)+trie.సూచీ_నిడివి*2;
    ఎంపిక(విలువ_వెడల్పు) {
    స్థితి UCPTRIE_విలువ_బిట్లు_16:
        size+=దత్తాంశం_నిడివి*2;
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_32:
        size+=దత్తాంశం_నిడివి*4;
        ఆపు;
    స్థితి UCPTRIE_విలువ_బిట్లు_8:
        size+=దత్తాంశం_నిడివి;
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length>=0) {
        UCPTrie_శీర్షిక *బహిర్గతం_Trie;

        ఒకవేళ(length<size) {
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        బహిర్గతం_Trie=(UCPTrie_శీర్షిక *)బహిర్గతం_దత్తాంశం;

        /* swap the header */
        ds->మార్పిడి_Array32(ds, &లోపల_Trie->signature, 4, &బహిర్గతం_Trie->signature, p_Error_సంకేతం);
        ds->మార్పిడి_Array16(ds, &లోపల_Trie->options, 12, &బహిర్గతం_Trie->options, p_Error_సంకేతం);

        /* swap the index */
        స్థిరరాశి uint16_t *లోపల_సూచీ=reinterpret_cast<స్థిరరాశి uint16_t *>(లోపల_Trie+1);
        uint16_t *బహిర్గతం_సూచీ=reinterpret_cast<uint16_t *>(బహిర్గతం_Trie+1);
        ds->మార్పిడి_Array16(ds, లోపల_సూచీ, trie.సూచీ_నిడివి*2, బహిర్గతం_సూచీ, p_Error_సంకేతం);

        /* swap the data */
        స్థిరరాశి uint16_t *లోపల_దత్తాంశం=లోపల_సూచీ+trie.సూచీ_నిడివి;
        uint16_t *బహిర్గతం_దత్తాంశం=బహిర్గతం_సూచీ+trie.సూచీ_నిడివి;
        ఎంపిక(విలువ_వెడల్పు) {
        స్థితి UCPTRIE_విలువ_బిట్లు_16:
            ds->మార్పిడి_Array16(ds, లోపల_దత్తాంశం, దత్తాంశం_నిడివి*2, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
            ఆపు;
        స్థితి UCPTRIE_విలువ_బిట్లు_32:
            ds->మార్పిడి_Array32(ds, లోపల_దత్తాంశం, దత్తాంశం_నిడివి*4, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
            ఆపు;
        స్థితి UCPTRIE_విలువ_బిట్లు_8:
            ఒకవేళ(లోపల_Trie!=బహిర్గతం_Trie) {
                uprv_memmove(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, దత్తాంశం_నిడివి);
            }
            ఆపు;
        అప్రమేయ:
            *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
            తిరిగి 0;
        }
    }

    తిరిగి size;
}

నామాకాశ {

/**
 * Gets the trie version from 32-bit-aligned memory containing the serialized form
 * of a UTrie (version 1), a UTrie2 (version 2), or a UCPTrie (version 3).
 *
 * @param data a pointer to 32-bit-aligned memory containing the serialized form of a trie
 * @param length the number of bytes available at data;
 *               can be more than necessary (see return value)
 * @param anyEndianOk If false, only platform-endian serialized forms are recognized.
 *                    If true, opposite-endian serialized forms are recognized as well.
 * @return the trie version of the serialized form, or 0 if it is not
 *         recognized as a serialized trie
 */
int32_t
get_సంస్కరణ(స్థిరరాశి శూన్య *దత్తాంశం, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ఒకవేళ(length<16 || దత్తాంశం==శూన్యసూచిక || (U_సూచిక_ముసుగు_LSB(దత్తాంశం, 3)!=0)) {
        తిరిగి 0;
    }
    signature = *static_cast<స్థిరరాశి uint32_t*>(దత్తాంశం);
    ఒకవేళ(signature==UCPTRIE_SIG) {
        తిరిగి 3;
    }
    ఒకవేళ(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        తిరిగి 3;
    }
    ఒకవేళ(signature==UTRIE2_SIG) {
        తిరిగి 2;
    }
    ఒకవేళ(anyEndianOk && signature==UTRIE2_OE_SIG) {
        తిరిగి 2;
    }
    ఒకవేళ(signature==UTRIE_SIG) {
        తిరిగి 1;
    }
    ఒకవేళ(anyEndianOk && signature==UTRIE_OE_SIG) {
        తిరిగి 1;
    }
    తిరిగి 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_మార్పిడి_Any_సంస్కరణ(స్థిరరాశి UDataSwapper *ds,
                     స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                     UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) { తిరిగి 0; }
    ఎంపిక(get_సంస్కరణ(లోపల_దత్తాంశం, length, నిజం)) {
    స్థితి 1:
        తిరిగి utrie_మార్పిడి(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    స్థితి 2:
        తిరిగి utrie2_మార్పిడి(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    స్థితి 3:
        తిరిగి ucptrie_మార్పిడి(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    అప్రమేయ:
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి 0;
    }
}
