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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYMTABLE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYMTABLE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: An interface that defines both lookup protocol and parsing of
 * symbolic names.
 */

U_NAMESPACE_BEGIN

class ParsePosition;
class UnicodeFunctor;
class Unicode_ਸੈੱਟ;
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
class U_COMMON_API ਚਿੰਨ੍ਹ_ਸਾਰਣੀ /* not : public UObject because this is an interface/mixin class */ {
ਜਨਤਕ:

    /**
     * The character preceding a symbol reference name.
     * @stable ICU 2.8
     */
    ਗਿਣਤੀ { ਚਿੰਨ੍ਹ_REF = 0x0024 /*$*/ };

    /**
     * Destructor.
     * @stable ICU 2.8
     */
    ਆਭਾਸੀ ~ਚਿੰਨ੍ਹ_ਸਾਰਣੀ();

    /**
     * Lookup the characters associated with this string and return it.
     * Return <tt>nullptr</tt> if no such name exists.  The resultant
     * string may have length zero.
     * @param s the symbolic name to lookup
     * @return a string containing the name's value, or <tt>nullptr</tt> if
     * there is no mapping for s.
     * @stable ICU 2.8
     */
    ਆਭਾਸੀ ਅਚਲ UnicodeString* lookup(ਅਚਲ UnicodeString& s) ਅਚਲ U_LIFETIME_BOUND = 0;

    /**
     * Returns a pointer to a pre-parsed set associated with the variable with
     * the given name, or null if there is no such set (as is the case for an
     * undefined or string-valued variable).
     * @internal
     */
    ਆਭਾਸੀ ਅਚਲ Unicode_ਸੈੱਟ* lookup_ਸੈੱਟ(ਅਚਲ UnicodeString& /*name*/) ਅਚਲ {
        ਵਾਪਸ nullptr;
    }

    /**
     * Lookup the UnicodeMatcher associated with the given character, and
     * return it.  Return <tt>nullptr</tt> if not found.
     * @param ch a 32-bit code point from 0 to 0x10FFFF inclusive.
     * @return the UnicodeMatcher object represented by the given
     * character, or nullptr if there is no mapping for ch.
     * @stable ICU 2.8
     */
    ਆਭਾਸੀ ਅਚਲ UnicodeFunctor* lookupMatcher(UChar32 ch) ਅਚਲ U_LIFETIME_BOUND = 0;

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
    ਆਭਾਸੀ UnicodeString parseReference(ਅਚਲ UnicodeString& ਪਾਠ,
                                         ParsePosition& pos, int32_t ਹੱਦਬੰਦੀ) ਅਚਲ = 0;
};
U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ
