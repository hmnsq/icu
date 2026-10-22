// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  bmpset.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2007jan29
*   created by: Markus W. Scherer
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __BMPSET_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __BMPSET_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Helper class for frozen UnicodeSets, implements contains() and span()
 * optimized for BMP code points. Structured to be UTF-8-friendly.
 *
 * Latin-1: Look up bytes.
 * 2-byte characters: Bits organized vertically.
 * 3-byte characters: Use zero/one/mixed data per 64-block in U+0000..U+FFFF,
 *                    with mixed for illegal ranges.
 * Supplementary characters: Binary search over
 * the supplementary part of the parent set's inversion list.
 */
class BMPSet : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    BMPSet(ಸ್ಥಿರಾಂಕ int32_t *ಪೋಷಕ_List, int32_t ಪೋಷಕ_List_ಉದ್ದಳತೆ);
    BMPSet(ಸ್ಥಿರಾಂಕ BMPSet &otherBMPSet, ಸ್ಥಿರಾಂಕ int32_t *new_ಪೋಷಕ_List, int32_t new_ಪೋಷಕ_List_ಉದ್ದಳತೆ);
    ವಾಸ್ತವಿಕ ~BMPSet();

    ವಾಸ್ತವಿಕ UBool ಒಳಗೊಂಡಿದೆ(UChar32 c) ಸ್ಥಿರಾಂಕ;

    /*
     * Span the initial substring for which each character c has spanCondition==contains(c).
     * It must be s<limit and spanCondition==0 or 1.
     * @return The string pointer which limits the span.
     */
    ಸ್ಥಿರಾಂಕ char16_t *ಹರವು(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ;

    /*
     * Span the trailing substring for which each character c has spanCondition==contains(c).
     * It must be s<limit and spanCondition==0 or 1.
     * @return The string pointer which starts the span.
     */
    ಸ್ಥಿರಾಂಕ char16_t *ಹರವು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ;

    /*
     * Span the initial substring for which each character c has spanCondition==contains(c).
     * It must be length>0 and spanCondition==0 or 1.
     * @return The string pointer which limits the span.
     */
    ಸ್ಥಿರಾಂಕ uint8_t *ಹರವು_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಉದ್ದಳತೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ;

    /*
     * Span the trailing substring for which each character c has spanCondition==contains(c).
     * It must be length>0 and spanCondition==0 or 1.
     * @return The start of the span.
     */
    int32_t ಹರವು_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t ಉದ್ದಳತೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
    ಶೂನ್ಯ init_ಬಿಟ್ಟುಗಳು();
    ಶೂನ್ಯ overrideIllegal();

    /**
     * Same as UnicodeSet::findCodePoint(UChar32 c) const except that the
     * binary search is restricted for finding code points in a certain range.
     *
     * For restricting the search for finding in the range start..end,
     * pass in
     *   lo=findCodePoint(start) and
     *   hi=findCodePoint(end)
     * with 0<=lo<=hi<len.
     * findCodePoint(c) defaults to lo=0 and hi=len-1.
     *
     * @param c a character in a subrange of MIN_VALUE..MAX_VALUE
     * @param lo The lowest index to be returned.
     * @param hi The highest index to be returned.
     * @return the smallest integer i in the range lo..hi,
     *         inclusive, such that c < list[i]
     */
    int32_t ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(UChar32 c, int32_t lo, int32_t hi) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ UBool ಒಳಗೊಂಡಿದೆ_Slow(UChar32 c, int32_t lo, int32_t hi) ಸ್ಥಿರಾಂಕ;

    /*
     * One byte 0 or 1 per Latin-1 character.
     */
    UBool latin1_ಒಳಗೊಂಡಿದೆ[0x100];

    /* true if contains(U+FFFD). */
    UBool ಒಳಗೊಂಡಿದೆ_FFFD;

    /*
     * One bit per code point from U+0000..U+07FF.
     * The bits are organized vertically; consecutive code points
     * correspond to the same bit positions in consecutive table words.
     * With code point parts
     *   lead=c{10..6}
     *   trail=c{5..0}
     * it is set.contains(c)==(table7FF[trail] bit lead)
     *
     * Bits for 0..7F (non-shortest forms) are set to the result of contains(FFFD)
     * for faster validity checking at runtime.
     */
    uint32_t table7FF[64];

    /*
     * One bit per 64 BMP code points.
     * The bits are organized vertically; consecutive 64-code point blocks
     * correspond to the same bit position in consecutive table words.
     * With code point parts
     *   lead=c{15..12}
     *   t1=c{11..6}
     * test bits (lead+16) and lead in bmpBlockBits[t1].
     * If the upper bit is 0, then the lower bit indicates if contains(c)
     * for all code points in the 64-block.
     * If the upper bit is 1, then the block is mixed and set.contains(c)
     * must be called.
     *
     * Bits for 0..7FF (non-shortest forms) and D800..DFFF are set to
     * the result of contains(FFFD) for faster validity checking at runtime.
     */
    uint32_t bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[64];

    /*
     * Inversion list indexes for restricted binary searches in
     * findCodePoint(), from
     * findCodePoint(U+0800, U+1000, U+2000, .., U+F000, U+10000).
     * U+0800 is the first 3-byte-UTF-8 code point. Code points below U+0800 are
     * always looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    int32_t list4kStarts[18];

    /*
     * The inversion list of the parent set, for the slower contains() implementation
     * for mixed BMP blocks and for supplementary code points.
     * The list is terminated with list[listLength-1]=0x110000.
     */
    ಸ್ಥಿರಾಂಕ int32_t *list;
    int32_t list_ಉದ್ದಳತೆ;
};

ಸಾಲೊಳಗೆ UBool BMPSet::ಒಳಗೊಂಡಿದೆ_Slow(UChar32 c, int32_t lo, int32_t hi) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(c, lo, hi) & 1;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
