// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2000-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   02/04/00    aliu        Creation.
**********************************************************************
*/
#నిర్వచితం_కాకపోతే SYMTABLE_H
#నిర్వచించు SYMTABLE_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: An interface that defines both lookup protocol and parsing of
 * symbolic names.
 */

U_NAMESPACE_BEGIN

class ParsePosition;
class UnicodeFunctor;
class Unicode_సమితి;
class UnicodeString;

/**
 * An interface that defines both lookup protocol and parsing of
 * symbolic names.
 *
 * <p>A symbol table maintains two kinds of mappings.  The first is
 * between symbolic names and their values.  For example, if the
 * variable with the name "start" is set to the value "alpha"
 * (perhaps, though not necessarily, through an expression such as
 * "$start=alpha"), then the call lookup("start") will return the
 * char[] array ['a', 'l', 'p', 'h', 'a'].
 *
 * <p>The second kind of mapping is between character values and
 * UnicodeMatcher objects.  This is used by RuleBasedTransliterator,
 * which uses characters in the private use area to represent objects
 * such as UnicodeSets.  If U+E015 is mapped to the UnicodeSet [a-z],
 * then lookupMatcher(0xE015) will return the UnicodeSet [a-z].
 *
 * <p>Finally, a symbol table defines parsing behavior for symbolic
 * names.  All symbolic names start with the SYMBOL_REF character.
 * When a parser encounters this character, it calls parseReference()
 * with the position immediately following the SYMBOL_REF.  The symbol
 * table parses the name, if there is one, and returns it.
 *
 * @stable ICU 2.8
 */
class U_COMMON_API చిహ్నం_పట్టిక /* not : public UObject because this is an interface/mixin class */ {
బహిరంగ:

    /**
     * The character preceding a symbol reference name.
     * @stable ICU 2.8
     */
    గణన { చిహ్నం_REF = 0x0024 /*$*/ };

    /**
     * Destructor.
     * @stable ICU 2.8
     */
    మాయా ~చిహ్నం_పట్టిక();

    /**
     * Lookup the characters associated with this string and return it.
     * Return <tt>nullptr</tt> if no such name exists.  The resultant
     * string may have length zero.
     * @param s the symbolic name to lookup
     * @return a string containing the name's value, or <tt>nullptr</tt> if
     * there is no mapping for s.
     * @stable ICU 2.8
     */
    మాయా స్థిరరాశి UnicodeString* lookup(స్థిరరాశి UnicodeString& s) స్థిరరాశి U_LIFETIME_BOUND = 0;

    /**
     * Returns a pointer to a pre-parsed set associated with the variable with
     * the given name, or null if there is no such set (as is the case for an
     * undefined or string-valued variable).
     * @internal
     */
    మాయా స్థిరరాశి Unicode_సమితి* lookup_సమితి(స్థిరరాశి UnicodeString& /*name*/) స్థిరరాశి {
        తిరిగి nullptr;
    }

    /**
     * Lookup the UnicodeMatcher associated with the given character, and
     * return it.  Return <tt>nullptr</tt> if not found.
     * @param ch a 32-bit code point from 0 to 0x10FFFF inclusive.
     * @return the UnicodeMatcher object represented by the given
     * character, or nullptr if there is no mapping for ch.
     * @stable ICU 2.8
     */
    మాయా స్థిరరాశి UnicodeFunctor* lookupMatcher(UChar32 ch) స్థిరరాశి U_LIFETIME_BOUND = 0;

    /**
     * Parse a symbol reference name from the given string, starting
     * at the given position.  If no valid symbol reference name is
     * found, return the empty string and leave pos unchanged.  That is, if the
     * character at pos cannot start a name, or if pos is at or after
     * text.length(), then return an empty string.  This indicates an
     * isolated SYMBOL_REF character.
     * @param text the text to parse for the name
     * @param pos on entry, the index of the first character to parse.
     * This is the character following the SYMBOL_REF character.  On
     * exit, the index after the last parsed character.  If the parse
     * failed, pos is unchanged on exit.
     * @param limit the index after the last character to be parsed.
     * @return the parsed name, or an empty string if there is no
     * valid symbolic name at the given position.
     * @stable ICU 2.8
     */
    మాయా UnicodeString parseReference(స్థిరరాశి UnicodeString& పాఠ్యం,
                                         ParsePosition& pos, int32_t హద్దు) స్థిరరాశి = 0;
};
U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు
