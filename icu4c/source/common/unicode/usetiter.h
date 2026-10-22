// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#جيڪڏهن_وصف_نه_ٿيل USETITER_H
#وصف_ڪريو USETITER_H

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"
#ضمن "unicode/unistr.h"

/**
 * \file 
 * \brief C++ API: UnicodeSetIterator iterates over the contents of a UnicodeSet.
 */

U_NAMESPACE_BEGIN

class Unicode_سيٽ;
class UnicodeString;

/**
 *
 * UnicodeSetIterator iterates over the contents of a UnicodeSet.  It
 * iterates over either code points or code point ranges.  After all
 * code points or ranges have been returned, it returns the
 * multicharacter strings of the UnicodeSet, if any.
 *
 * This class is not intended for public subclassing.
 *
 * <p>To iterate over code points and strings, use a loop like this:
 * <pre>
 * UnicodeSetIterator it(set);
 * while (it.next()) {
 *     processItem(it.getString());
 * }
 * </pre>
 * <p>Each item in the set is accessed as a string.  Set elements
 *    consisting of single code points are returned as strings containing
 *    just the one code point.
 *
 * <p>To iterate over code point ranges, instead of individual code points,
 *    use a loop like this:
 * <pre>
 * UnicodeSetIterator it(set);
 * while (it.nextRange()) {
 *   if (it.isString()) {
 *     processString(it.getString());
 *   } else {
 *     processCodepointRange(it.getCodepoint(), it.getCodepointEnd());
 *   }
 * }
 * </pre>
 *
 * To iterate over only the strings, start with <code>skipToStrings()</code>.
 *
 * @author M. Davis
 * @stable ICU 2.4
 */
class U_COMMON_API Unicode_سيٽ_Iterator final : عام UObject {
    /**
     * Value of <tt>codepoint</tt> if the iterator points to a string.
     * If <tt>codepoint == IS_STRING</tt>, then examine
     * <tt>string</tt> for the current iteration result.
     */
    تعداد { IS_STRING = -1 };

    /**
     * Current code point, or the special value <tt>IS_STRING</tt>, if
     * the iterator points to a string.
     */
    UChar32 codepoint;

    /**
     * When iterating over ranges using <tt>nextRange()</tt>,
     * <tt>codepointEnd</tt> contains the inclusive end of the
     * iteration range, if <tt>codepoint != IS_STRING</tt>.  If
     * iterating over code points using <tt>next()</tt>, or if
     * <tt>codepoint == IS_STRING</tt>, then the value of
     * <tt>codepointEnd</tt> is undefined.
     */
    UChar32 codepoint_پڇاڙي;

    /**
     * If <tt>codepoint == IS_STRING</tt>, then <tt>string</tt> points
     * to the current string.  If <tt>codepoint != IS_STRING</tt>, the
     * value of <tt>string</tt> is undefined.
     */
    ثابت UnicodeString* string;

 عام:

    /**
     * Create an iterator over the given set.  The iterator is valid
     * only so long as <tt>set</tt> is valid.
     * @param set set to iterate over
     * @stable ICU 2.4
     */
    Unicode_سيٽ_Iterator(ثابت Unicode_سيٽ& سيٽ U_LIFETIME_BOUND);

    /**
     * Create an iterator over nothing.  <tt>next()</tt> and
     * <tt>nextRange()</tt> return false. This is a convenience
     * constructor allowing the target to be set later.
     * @stable ICU 2.4
     */
    Unicode_سيٽ_Iterator();

    /**
     * Destructor.
     * @stable ICU 2.4
     */
    مجازي ~Unicode_سيٽ_Iterator();

    /**
     * Returns true if the current element is a string.  If so, the
     * caller can retrieve it with <tt>getString()</tt>.  If this
     * method returns false, the current element is a code point or
     * code point range, depending on whether <tt>next()</tt> or
     * <tt>nextRange()</tt> was called.
     * Elements of types string and codepoint can both be retrieved
     * with the function <tt>getString()</tt>.
     * Elements of type codepoint can also be retrieved with
     * <tt>getCodepoint()</tt>.
     * For ranges, <tt>getCodepoint()</tt> returns the starting codepoint
     * of the range, and <tt>getCodepointEnd()</tt> returns the end
     * of the range.
     * @stable ICU 2.4
     */
    مضمن UBool isString() ثابت;

    /**
     * Returns the current code point, if <tt>isString()</tt> returned
     * false.  Otherwise returns an undefined result.
     * @stable ICU 2.4
     */
    مضمن UChar32 getCodepoint() ثابت;

    /**
     * Returns the end of the current code point range, if
     * <tt>isString()</tt> returned false and <tt>nextRange()</tt> was
     * called.  Otherwise returns an undefined result.
     * @stable ICU 2.4
     */
    مضمن UChar32 get_Codepoint_پڇاڙي() ثابت;

    /**
     * Returns the current string, if <tt>isString()</tt> returned
     * true.  If the current iteration item is a code point, a UnicodeString
     * containing that single code point is returned.
     *
     * Ownership of the returned string remains with the iterator.
     * The string is guaranteed to remain valid only until the iterator is
     *   advanced to the next item, or until the iterator is deleted.
     * 
     * @stable ICU 2.4
     */
    ثابت UnicodeString& getString() U_LIFETIME_BOUND;

    /**
     * Skips over the remaining code points/ranges, if any.
     * A following call to next() or nextRange() will yield a string, if there is one.
     * No-op if next() would return false, or if it would yield a string anyway.
     *
     * @return *this
     * @stable ICU 70
     * @see UnicodeSet#strings()
     */
    مضمن Unicode_سيٽ_Iterator& skip_منزل_لڙيون() U_LIFETIME_BOUND {
        // Finish code point/range iteration.
        range = پڇاڙي_Range;
        پڇاڙي_Element = -1;
        ايندڙ_Element = 0;
        أرجع *this;
    }

    /**
     * Advances the iteration position to the next element in the set, 
     * which can be either a single code point or a string.  
     * If there are no more elements in the set, return false.
     *
     * <p>
     * If <tt>isString() == true</tt>, the value is a
     * string, otherwise the value is a
     * single code point.  Elements of either type can be retrieved
     * with the function <tt>getString()</tt>, while elements of
     * consisting of a single code point can be retrieved with
     * <tt>getCodepoint()</tt>
     *
     * <p>The order of iteration is all code points in sorted order,
     * followed by all strings sorted order.    Do not mix
     * calls to <tt>next()</tt> and <tt>nextRange()</tt> without
     * calling <tt>reset()</tt> between them.  The results of doing so
     * are undefined.
     *
     * @return true if there was another element in the set.
     * @stable ICU 2.4
     */
    UBool ايندڙ();

    /**
     * Returns the next element in the set, either a code point range
     * or a string.  If there are no more elements in the set, return
     * false.  If <tt>isString() == true</tt>, the value is a
     * string and can be accessed with <tt>getString()</tt>.  Otherwise the value is a
     * range of one or more code points from <tt>getCodepoint()</tt> to
     * <tt>getCodepointeEnd()</tt> inclusive.
     *
     * <p>The order of iteration is all code points ranges in sorted
     * order, followed by all strings sorted order.  Ranges are
     * disjoint and non-contiguous.  The value returned from <tt>getString()</tt>
     * is undefined unless <tt>isString() == true</tt>.  Do not mix calls to
     * <tt>next()</tt> and <tt>nextRange()</tt> without calling
     * <tt>reset()</tt> between them.  The results of doing so are
     * undefined.
     *
     * @return true if there was another element in the set.
     * @stable ICU 2.4
     */
    UBool ايندڙ_Range();

    /**
     * Sets this iterator to visit the elements of the given set and
     * resets it to the start of that set.  The iterator is valid only
     * so long as <tt>set</tt> is valid.
     * @param set the set to iterate over.
     * @stable ICU 2.4
     */
    فراغ reset(ثابت Unicode_سيٽ& سيٽ);

    /**
     * Resets this iterator to the start of the set.
     * @stable ICU 2.4
     */
    فراغ reset();

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.4
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.4
     */
    مجازي UClassID getDynamicClassID() ثابت override;

    // ======================= PRIVATES ===========================

خاص:

    // endElement and nextElements are really UChar32's, but we keep
    // them as signed int32_t's so we can do comparisons with
    // endElement set to -1.  Leave them as int32_t's.
    /** The set
     */
    ثابت Unicode_سيٽ* سيٽ;
    /** End range
     */
    int32_t پڇاڙي_Range;
    /** Range
     */
    int32_t range;
    /** End element
     */
    int32_t پڇاڙي_Element;
    /** Next element
     */
    int32_t ايندڙ_Element;
    /** Next string
     */
    int32_t ايندڙ_String;
    /** String count
     */
    int32_t string_ڳڻپ;

    /**
     *  Points to the string to use when the caller asks for a
     *  string and the current iteration item is a code point, not a string.
     */
    UnicodeString *cpString;

    /** Copy constructor. Disallowed.
     */
    Unicode_سيٽ_Iterator(ثابت Unicode_سيٽ_Iterator&) = delete;

    /** Assignment operator. Disallowed.
     */
    Unicode_سيٽ_Iterator& عامل=(ثابت Unicode_سيٽ_Iterator&) = delete;

    /** Load range
     */
    فراغ ڀريو_Range(int32_t range);
};

مضمن UBool Unicode_سيٽ_Iterator::isString() ثابت {
    أرجع codepoint < 0;
}

مضمن UChar32 Unicode_سيٽ_Iterator::getCodepoint() ثابت {
    أرجع codepoint;
}

مضمن UChar32 Unicode_سيٽ_Iterator::get_Codepoint_پڇاڙي() ثابت {
    أرجع codepoint_پڇاڙي;
}


U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو
