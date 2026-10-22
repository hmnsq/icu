// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2003-2011, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: September 24 2003
* Since: ICU 2.8
**********************************************************************
*/
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _RULEITER_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _RULEITER_H_

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

U_NAMESPACE_BEGIN

class UnicodeString;
class ParsePosition;
class ਚਿੰਨ੍ਹ_ਸਾਰਣੀ;

/**
 * An iterator that returns 32-bit code points.  This class is deliberately
 * <em>not</em> related to any of the ICU character iterator classes
 * in order to minimize complexity.
 * @author Alan Liu
 * @since ICU 2.8
 */
class ਨਿਯਮ_Character_Iterator : ਜਨਤਕ UMemory {

    // TODO: Ideas for later.  (Do not implement if not needed, lest the
    // code coverage numbers go down due to unused methods.)
    // 1. Add a copy constructor, operator==() method.
    // 2. Rather than return DONE, throw an exception if the end
    // is reached -- this is an alternate usage model, probably not useful.

ਨਿਜੀ:
    /**
     * Text being iterated.
     */    
    ਅਚਲ UnicodeString& ਪਾਠ;

    /**
     * Position of iterator.
     */
    ParsePosition& pos;

    /**
     * Symbol table used to parse and dereference variables.  May be 0.
     */
    ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* sym;
    
    /**
     * Current variable expansion, or 0 if none.
     */
    ਅਚਲ UnicodeString* buf;

    /**
     * Position within buf.  Meaningless if buf == 0.
     */
    int32_t bufPos;

ਜਨਤਕ:
    /**
     * Value returned when there are no more characters to iterate.
     */
    ਸਥਿਰ constexpr int32_t ਪੂਰਾ_2 = -1;

    /**
     * Bitmask option to enable parsing of variable names.  If (options &
     * PARSE_VARIABLES) != 0, then an embedded variable will be expanded to
     * its value.  Variables are parsed using the SymbolTable API.
     */
    ਸਥਿਰ constexpr int32_t PARSE_VARIABLES = 1;

    /**
     * Bitmask option to enable parsing of escape sequences.  If (options &
     * PARSE_ESCAPES) != 0, then an embedded escape sequence will be expanded
     * to its value.  Escapes are parsed using Utility.unescapeAt().
     */
    ਸਥਿਰ constexpr int32_t PARSE_ESCAPES   = 2;

    /**
     * Bitmask option to enable skipping of whitespace.  If (options &
     * SKIP_WHITESPACE) != 0, then Pattern_White_Space characters will be silently
     * skipped, as if they were not present in the input.
     */
    ਸਥਿਰ constexpr int32_t SKIP_WHITESPACE = 4;

    /**
     * Constructs an iterator over the given text, starting at the given
     * position.
     * @param text the text to be iterated
     * @param sym the symbol table, or null if there is none.  If sym is null,
     * then variables will not be dereferenced, even if the PARSE_VARIABLES
     * option is set.
     * @param pos upon input, the index of the next character to return.  If a
     * variable has been dereferenced, then pos will <em>not</em> increment as
     * characters of the variable value are iterated.
     */
    ਨਿਯਮ_Character_Iterator(ਅਚਲ UnicodeString& ਪਾਠ, ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* sym,
                          ParsePosition& pos);
    
    /**
     * Returns true if this iterator has no more characters to return.
     */
    UBool at_ਸਿਰਾ() ਅਚਲ;

    /**
     * Returns the next character using the given options, or DONE if there
     * are no more characters, and advance the position to the next
     * character.
     * @param options one or more of the following options, bitwise-OR-ed
     * together: PARSE_VARIABLES, PARSE_ESCAPES, SKIP_WHITESPACE.
     * @param isEscaped output parameter set to true if the character
     * was escaped
     * @param ec input-output error code.  An error will only be set by
     * this routing if options includes PARSE_VARIABLES and an unknown
     * variable name is seen, or if options includes PARSE_ESCAPES and
     * an invalid escape sequence is seen.
     * @return the current 32-bit code point, or DONE
     */
    UChar32 ਅਗਲਾ(int32_t options, UBool& isEscaped, UError_ਕੋਡ& ec);

    /**
     * Returns true if this iterator is currently within a variable expansion.
     */
    ਸਤਰਅੰਦਰ UBool ਵਿੱਚ_Variable() ਅਚਲ;

    /**
     * An opaque object representing the position of a RuleCharacterIterator.
     */
    ਬਣਤਰ Pos : ਜਨਤਕ UMemory {
    ਨਿਜੀ:
        ਅਚਲ UnicodeString* buf;
        int32_t pos;
        int32_t bufPos;
        ਮਿੱਤਰ class ਨਿਯਮ_Character_Iterator;
    };

    /**
     * Sets an object which, when later passed to setPos(), will
     * restore this iterator's position.  Usage idiom:
     *
     * RuleCharacterIterator iterator = ...;
     * RuleCharacterIterator::Pos pos;
     * iterator.getPos(pos);
     * for (;;) {
     *   iterator.getPos(pos);
     *   int c = iterator.next(...);
     *   ...
     * }
     * iterator.setPos(pos);
     *
     * @param p a position object to be set to this iterator's
     * current position.
     */
    ਖਾਲੀ getPos(Pos& p) ਅਚਲ;

    /**
     * Restores this iterator to the position it had when getPos()
     * set the given object.
     * @param p a position object previously set by getPos()
     */
    ਖਾਲੀ ਸੈੱਟ_Pos(ਅਚਲ Pos& p);

    /**
     * Skips ahead past any ignored characters, as indicated by the given
     * options.  This is useful in conjunction with the lookahead() method.
     *
     * Currently, this only has an effect for SKIP_WHITESPACE.
     * @param options one or more of the following options, bitwise-OR-ed
     * together: PARSE_VARIABLES, PARSE_ESCAPES, SKIP_WHITESPACE.
     */
    ਖਾਲੀ skipIgnored(int32_t options);

    /**
     * Returns a string containing the remainder of the characters to be
     * returned by this iterator, without any option processing.  If the
     * iterator is currently within a variable expansion, this will only
     * extend to the end of the variable expansion.  This method is provided
     * so that iterators may interoperate with string-based APIs.  The typical
     * sequence of calls is to call skipIgnored(), then call lookahead(), then
     * parse the string returned by lookahead(), then call jumpahead() to
     * resynchronize the iterator.
     * @param result a string to receive the characters to be returned
     * by future calls to next()
     * @param maxLookAhead The maximum to copy into the result.
     * @return a reference to result
     */
    UnicodeString& lookahead(UnicodeString& ਨਤੀਜਾ, int32_t maxLookAhead = -1) ਅਚਲ;

    /**
     * Advances the position by the given number of 16-bit code units.
     * This is useful in conjunction with the lookahead() method.
     * @param count the number of 16-bit code units to jump over
     */
    ਖਾਲੀ jumpahead(int32_t ਗਿਣਤੀ_2);

    /**
     * Returns a string representation of this object, consisting of the
     * characters being iterated, with a '|' marking the current position.
     * Position within an expanded variable is <em>not</em> indicated.
     * @param result output parameter to receive a string
     * representation of this object
     */
//    UnicodeString& toString(UnicodeString& result) const;
    
ਨਿਜੀ:
    /**
     * Returns the current 32-bit code point without parsing escapes, parsing
     * variables, or skipping whitespace.
     * @return the current 32-bit code point
     */
    UChar32 _current() ਅਚਲ;
    
    /**
     * Advances the position by the given amount.
     * @param count the number of 16-bit code units to advance past
     */
    ਖਾਲੀ _advance(int32_t ਗਿਣਤੀ_2);
};

ਸਤਰਅੰਦਰ UBool ਨਿਯਮ_Character_Iterator::ਵਿੱਚ_Variable() ਅਚਲ {
    ਵਾਪਸ buf != nullptr;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // _RULEITER_H_
//eof
