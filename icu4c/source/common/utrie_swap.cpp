// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// utrie_swap.cpp
// created: 2018aug08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"
#ಅನ್ತರ್ಭಾವ "utrie2_impl.h"

// These functions for swapping different generations of ICU code point tries are here
// so that their implementation files need not depend on swapper code,
// need not depend on each other, and so that other swapper code
// need not depend on other trie code.

ನಾಮಾಕಾಶ {

constexpr int32_t ASCII_ಎಲ್ಲೆ = 0x80;

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UTrie_ಶೀರ್ಷಿಕೆ *ಒಳ_Trie;
    UTrie_ಶೀರ್ಷಿಕೆ trie;
    int32_t size;
    UBool ದತ್ತಾಂಶ_Is32;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || (length>=0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    ಆದರೆ(length>=0 && (uint32_t)length<ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_Trie=(ಸ್ಥಿರಾಂಕ UTrie_ಶೀರ್ಷಿಕೆ *)ಒಳ_ದತ್ತಾಂಶ;
    trie.signature=ds->readUInt32(ಒಳ_Trie->signature);
    trie.options=ds->readUInt32(ಒಳ_Trie->options);
    trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=udata_readInt32(ds, ಒಳ_Trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
    trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=udata_readInt32(ds, ಒಳ_Trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);

    ಆದರೆ( trie.signature!=0x54726965 ||
        (trie.options&UTRIE_OPTIONS_ಸರಿಸು_ಮುಖವಾಡ)!=UTRIE_ಸರಿಸು ||
        ((trie.options>>UTRIE_OPTIONS_ಸೂಚ್ಯಂಕ_ಸರಿಸು)&UTRIE_OPTIONS_ಸರಿಸು_ಮುಖವಾಡ)!=UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು ||
        trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ<UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ ||
        (trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ&(UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ-1))!=0 ||
        trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ ||
        (trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ&(UTRIE_ದತ್ತಾಂಶ_GRANULARITY-1))!=0 ||
        ((trie.options&UTRIE_OPTIONS_LATIN1_IS_ರೇಖೀಯ)!=0 && trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<(UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ+0x100))
    ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ಹಿಂತಿರುಗಿ 0;
    }

    ದತ್ತಾಂಶ_Is32 = (trie.options & UTRIE_OPTIONS_ದತ್ತಾಂಶ_IS_32_ಬಿಟ್) != 0;
    size=ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2+trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*(ದತ್ತಾಂಶ_Is32?4:2);

    ಆದರೆ(length>=0) {
        UTrie_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_Trie;

        ಆದರೆ(length<size) {
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಹೊರಹೋಗು_Trie=(UTrie_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ;

        /* swap the header */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_Trie, ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ), ಹೊರಹೋಗು_Trie, p_Error_ಸಂಕೇತ);

        /* swap the index and the data */
        ಆದರೆ(ದತ್ತಾಂಶ_Is32) {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_Trie+1, trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2, ಹೊರಹೋಗು_Trie+1, p_Error_ಸಂಕೇತ);
            ds->ಅದಲುಬದಲು_Array32(ds, (ಸ್ಥಿರಾಂಕ uint16_t *)(ಒಳ_Trie+1)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4,
                                     (uint16_t *)(ಹೊರಹೋಗು_Trie+1)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_Trie+1, (trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ)*2, ಹೊರಹೋಗು_Trie+1, p_Error_ಸಂಕೇತ);
        }
    }

    ಹಿಂತಿರುಗಿ size;
}

U_CAPI int32_t U_EXPORT2
utrie2_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UTrie2_ಶೀರ್ಷಿಕೆ *ಒಳ_Trie;
    UTrie2_ಶೀರ್ಷಿಕೆ trie;
    int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, size;
    UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || (length>=0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    ಆದರೆ(length>=0 && length<(int32_t)ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_Trie=(ಸ್ಥಿರಾಂಕ UTrie2_ಶೀರ್ಷಿಕೆ *)ಒಳ_ದತ್ತಾಂಶ;
    trie.signature=ds->readUInt32(ಒಳ_Trie->signature);
    trie.options=ds->readUInt16(ಒಳ_Trie->options);
    trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ds->readUInt16(ಒಳ_Trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
    trie.shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=ds->readUInt16(ಒಳ_Trie->shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);

    ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು=(UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು)(trie.options&UTRIE2_OPTIONS_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ);
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(int32_t)trie.shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<<UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು;

    ಆದರೆ( trie.signature!=UTRIE2_SIG ||
        ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<=ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು ||
        trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ<UTRIE2_ಸೂಚ್ಯಂಕ_1_ಸ್ಥಾನಾಂತರ ||
        ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<UTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ
    ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR; /* not a UTrie */
        ಹಿಂತಿರುಗಿ 0;
    }

    size=ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2;
    ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
    ಸ್ಥಿತಿ UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        size+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        size+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>=0) {
        UTrie2_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_Trie;

        ಆದರೆ(length<size) {
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಹೊರಹೋಗು_Trie=(UTrie2_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ;

        /* swap the header */
        ds->ಅದಲುಬದಲು_Array32(ds, &ಒಳ_Trie->signature, 4, &ಹೊರಹೋಗು_Trie->signature, p_Error_ಸಂಕೇತ);
        ds->ಅದಲುಬದಲು_Array16(ds, &ಒಳ_Trie->options, 12, &ಹೊರಹೋಗು_Trie->options, p_Error_ಸಂಕೇತ);

        /* swap the index and the data */
        ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        ಸ್ಥಿತಿ UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_Trie+1, (trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+ದತ್ತಾಂಶ_ಉದ್ದಳತೆ)*2, ಹೊರಹೋಗು_Trie+1, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_Trie+1, trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2, ಹೊರಹೋಗು_Trie+1, p_Error_ಸಂಕೇತ);
            ds->ಅದಲುಬದಲು_Array32(ds, (ಸ್ಥಿರಾಂಕ uint16_t *)(ಒಳ_Trie+1)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4,
                                     (uint16_t *)(ಹೊರಹೋಗು_Trie+1)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಹಿಂತಿರುಗಿ size;
}

U_CAPI int32_t U_EXPORT2
ucptrie_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
             ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UCPTrie_ಶೀರ್ಷಿಕೆ *ಒಳ_Trie;
    UCPTrie_ಶೀರ್ಷಿಕೆ trie;
    int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, size;
    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಮೌಲ್ಯ_ಅಗಲ;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || (length>=0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    ಆದರೆ(length>=0 && length<(int32_t)ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_Trie=(ಸ್ಥಿರಾಂಕ UCPTrie_ಶೀರ್ಷಿಕೆ *)ಒಳ_ದತ್ತಾಂಶ;
    trie.signature=ds->readUInt32(ಒಳ_Trie->signature);
    trie.options=ds->readUInt16(ಒಳ_Trie->options);
    trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ds->readUInt16(ಒಳ_Trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
    trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ds->readUInt16(ಒಳ_Trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);

    UCPTrieType type = (UCPTrieType)((trie.options >> 6) & 3);
    ಮೌಲ್ಯ_ಅಗಲ = (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)(trie.options & UCPTRIE_OPTIONS_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ);
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ((int32_t)(trie.options & UCPTRIE_OPTIONS_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ಮುಖವಾಡ) << 4) | trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;

    int32_t min_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ = type == UCPTRIE_TYPE_FAST ?
        UCPTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ : UCPTRIE_SMALL_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಆದರೆ( trie.signature!=UCPTRIE_SIG ||
        type > UCPTRIE_TYPE_SMALL ||
        (trie.options & UCPTRIE_OPTIONS_RESERVED_ಮುಖವಾಡ) != 0 ||
        ಮೌಲ್ಯ_ಅಗಲ > UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 ||
        trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ < min_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ ||
        ದತ್ತಾಂಶ_ಉದ್ದಳತೆ < ASCII_ಎಲ್ಲೆ
    ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR; /* not a UCPTrie */
        ಹಿಂತಿರುಗಿ 0;
    }

    size=ಗಾತ್ರ(UCPTrie_ಶೀರ್ಷಿಕೆ)+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2;
    ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಅಗಲ) {
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
        size+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
        size+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
        size+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>=0) {
        UCPTrie_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_Trie;

        ಆದರೆ(length<size) {
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಹೊರಹೋಗು_Trie=(UCPTrie_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ;

        /* swap the header */
        ds->ಅದಲುಬದಲು_Array32(ds, &ಒಳ_Trie->signature, 4, &ಹೊರಹೋಗು_Trie->signature, p_Error_ಸಂಕೇತ);
        ds->ಅದಲುಬದಲು_Array16(ds, &ಒಳ_Trie->options, 12, &ಹೊರಹೋಗು_Trie->options, p_Error_ಸಂಕೇತ);

        /* swap the index */
        ಸ್ಥಿರಾಂಕ uint16_t *ಒಳ_ಸೂಚ್ಯಂಕ=reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t *>(ಒಳ_Trie+1);
        uint16_t *ಹೊರಹೋಗು_ಸೂಚ್ಯಂಕ=reinterpret_cast<uint16_t *>(ಹೊರಹೋಗು_Trie+1);
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಸೂಚ್ಯಂಕ, trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2, ಹೊರಹೋಗು_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);

        /* swap the data */
        ಸ್ಥಿರಾಂಕ uint16_t *ಒಳ_ದತ್ತಾಂಶ=ಒಳ_ಸೂಚ್ಯಂಕ+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
        uint16_t *ಹೊರಹೋಗು_ದತ್ತಾಂಶ=ಹೊರಹೋಗು_ಸೂಚ್ಯಂಕ+trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
        ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಅಗಲ) {
        ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16:
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_32:
            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8:
            ಆದರೆ(ಒಳ_Trie!=ಹೊರಹೋಗು_Trie) {
                uprv_memmove(ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಒಳ_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
            }
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಹಿಂತಿರುಗಿ size;
}

ನಾಮಾಕಾಶ {

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
get_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t length, UBool anyEndianOk) {
    uint32_t signature;
    ಆದರೆ(length<16 || ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ದತ್ತಾಂಶ, 3)!=0)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    signature = *static_cast<ಸ್ಥಿರಾಂಕ uint32_t*>(ದತ್ತಾಂಶ);
    ಆದರೆ(signature==UCPTRIE_SIG) {
        ಹಿಂತಿರುಗಿ 3;
    }
    ಆದರೆ(anyEndianOk && signature==UCPTRIE_OE_SIG) {
        ಹಿಂತಿರುಗಿ 3;
    }
    ಆದರೆ(signature==UTRIE2_SIG) {
        ಹಿಂತಿರುಗಿ 2;
    }
    ಆದರೆ(anyEndianOk && signature==UTRIE2_OE_SIG) {
        ಹಿಂತಿರುಗಿ 2;
    }
    ಆದರೆ(signature==UTRIE_SIG) {
        ಹಿಂತಿರುಗಿ 1;
    }
    ಆದರೆ(anyEndianOk && signature==UTRIE_OE_SIG) {
        ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

}  // namespace

U_CAPI int32_t U_EXPORT2
utrie_ಅದಲುಬದಲು_Any_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    ಆಯ್ಕೆ(get_ಆವೃತ್ತಿ(ಒಳ_ದತ್ತಾಂಶ, length, ನಿಜ)) {
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ utrie_ಅದಲುಬದಲು(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ utrie2_ಅದಲುಬದಲು(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ 3:
        ಹಿಂತಿರುಗಿ ucptrie_ಅದಲುಬದಲು(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
}
