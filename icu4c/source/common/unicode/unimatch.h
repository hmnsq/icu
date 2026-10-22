// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
* Copyright (C) 2001-2005, International Business Machines Corporation and others. All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   07/18/01    aliu        Creation.
**********************************************************************
*/
#నిర్వచితం_కాకపోతే UNIMATCH_H
#నిర్వచించు UNIMATCH_H

#అన్తర్భావ "unicode/utypes.h"

/**
 * \file 
 * \brief C++ API: Unicode Matcher
 */

#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

class Replaceable;
class UnicodeString;
class Unicode_సమితి;

/**
 * Constants returned by <code>UnicodeMatcher::matches()</code>
 * indicating the degree of match.
 * @stable ICU 2.4
 */
గణన UMatchDegree {
    /**
     * Constant returned by <code>matches()</code> indicating a
     * mismatch between the text and this matcher.  The text contains
     * a character which does not match, or the text does not contain
     * all desired characters for a non-incremental match.
     * @stable ICU 2.4
     */
    U_MISMATCH,
    
    /**
     * Constant returned by <code>matches()</code> indicating a
     * partial match between the text and this matcher.  This value is
     * only returned for incremental match operations.  All characters
     * of the text match, but more characters are required for a
     * complete match.  Alternatively, for variable-length matchers,
     * all characters of the text match, and if more characters were
     * supplied at limit, they might also match.
     * @stable ICU 2.4
     */
    U_PARTIAL_సరిపోలిక,
    
    /**
     * Constant returned by <code>matches()</code> indicating a
     * complete match between the text and this matcher.  For an
     * incremental variable-length match, this value is returned if
     * the given text matches, and it is known that additional
     * characters would not alter the extent of the match.
     * @stable ICU 2.4
     */
    U_సరిపోలిక
};

/**
 * <code>UnicodeMatcher</code> defines a protocol for objects that can
 * match a range of characters in a Replaceable string.
 * @stable ICU 2.4
 */
class U_COMMON_API UnicodeMatcher /* not : public UObject because this is an interface/mixin class */ {

బహిరంగ:
    /**
     * Destructor.
     * @stable ICU 2.4
     */
    మాయా ~UnicodeMatcher();

    /**
     * Return a UMatchDegree value indicating the degree of match for
     * the given text at the given offset.  Zero, one, or more
     * characters may be matched.
     *
     * Matching in the forward direction is indicated by limit >
     * offset.  Characters from offset forwards to limit-1 will be
     * considered for matching.
     * 
     * Matching in the reverse direction is indicated by limit <
     * offset.  Characters from offset backwards to limit+1 will be
     * considered for matching.
     *
     * If limit == offset then the only match possible is a zero
     * character match (which subclasses may implement if desired).
     *
     * As a side effect, advance the offset parameter to the limit of
     * the matched substring.  In the forward direction, this will be
     * the index of the last matched character plus one.  In the
     * reverse direction, this will be the index of the last matched
     * character minus one.
     *
     * <p>Note:  This method is not const because some classes may
     * modify their state as the result of a match.
     *
     * @param text the text to be matched
     * @param offset on input, the index into text at which to begin
     * matching.  On output, the limit of the matched text.  The
     * number of matched characters is the output value of offset
     * minus the input value.  Offset should always point to the
     * HIGH SURROGATE (leading code unit) of a pair of surrogates,
     * both on entry and upon return.
     * @param limit the limit index of text to be matched.  Greater
     * than offset for a forward direction match, less than offset for
     * a backward direction match.  The last character to be
     * considered for matching will be text.charAt(limit-1) in the
     * forward direction or text.charAt(limit+1) in the backward
     * direction.
     * @param incremental if true, then assume further characters may
     * be inserted at limit and check for partial matching.  Otherwise
     * assume the text as given is complete.
     * @return a match degree value indicating a full match, a partial
     * match, or a mismatch.  If incremental is false then
     * U_PARTIAL_MATCH should never be returned.
     * @stable ICU 2.4
     */
    మాయా UMatchDegree matches(స్థిరరాశి Replaceable& పాఠ్యం,
                                 int32_t& స్థానభ్రంశం,
                                 int32_t హద్దు,
                                 UBool incremental) = 0;

    /**
     * Returns a string representation of this matcher.  If the result of
     * calling this function is passed to the appropriate parser, it
     * will produce another matcher that is equal to this one.
     * @param result the string to receive the pattern.  Previous
     * contents will be deleted.
     * @param escapeUnprintable if true then convert unprintable
     * character to their hex escape representations, \\uxxxx or
     * \\Uxxxxxxxx.  Unprintable characters are those other than
     * U+000A, U+0020..U+007E.
     * @stable ICU 2.4
     */
    మాయా UnicodeString& గమ్యం_నమూనా(UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                     UBool escapeUnprintable = false) స్థిరరాశి = 0;

    /**
     * Returns true if this matcher will match a character c, where c
     * & 0xFF == v, at offset, in the forward direction (with limit >
     * offset).  This is used by <tt>RuleBasedTransliterator</tt> for
     * indexing.
     * @stable ICU 2.4
     */
    మాయా UBool matches_సూచీ_విలువ(uint8_t v) స్థిరరాశి = 0;

    /**
     * Union the set of all characters that may be matched by this object
     * into the given set.
     * @param toUnionTo the set into which to union the source characters
     * @stable ICU 2.4
     */
    మాయా శూన్య కూడు_సరిపోలిక_సమితి_గమ్యం(Unicode_సమితి& గమ్యం_Union_గమ్యం) స్థిరరాశి = 0;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు
