// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uniset_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug25
*   created by: Markus W. Scherer
*
*   Character property dependent functions moved here from uniset.cpp
*/

#ਅਨ੍ਤਰ੍ਭਾਵ <array>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/symtable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utfiterator.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ruleiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "patternprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "propname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"

U_NAMESPACE_USE

ਨਾਮਾਕਾਸ਼ {

// Special property set IDs
constexpr ਅੱਖਰ ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr ਅੱਖਰ ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr ਅੱਖਰ ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV uset_cleanup();

ਸਥਿਰ Unicode_ਸੈੱਟ *uni32Singleton;
ਸਥਿਰ icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ਸਥਿਰ UBool U_CALLCONV uset_cleanup() {
    ਮਿਟਾਓ uni32Singleton;
    uni32Singleton = ਖਾਲੀ_ਸੂਚਕ;
    uni32InitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

U_CDECL_ਸਿਰਾ

U_NAMESPACE_BEGIN

ਵਰਤੋਂ U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::utf_String_ਕੋਡ_Points;

ਨਾਮਾਕਾਸ਼ {

// Cache some sets for other services -------------------------------------- ***
ਖਾਲੀ U_CALLCONV create_Uni32_ਸੈੱਟ(UError_ਕੋਡ &error_ਕੋਡ) {
    U_ASSERT(uni32Singleton == ਖਾਲੀ_ਸੂਚਕ);
    uni32Singleton = ਨਵਾਂ Unicode_ਸੈੱਟ(UnicodeString(u"[:age=3.2:]"), error_ਕੋਡ);
    ਜੇ(uni32Singleton==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        uni32Singleton->freeze();
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_ਸੈੱਟ *
uniset_getUnicode32Instance(UError_ਕੋਡ &error_ਕੋਡ) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_ਸੈੱਟ, error_ਕੋਡ);
    ਵਾਪਸ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

ਸਤਰਅੰਦਰ UBool
isPerlOpen(ਅਚਲ UnicodeString &ਨਮੂਨਾ, int32_t pos) {
    char16_t c;
    ਵਾਪਸ ਨਮੂਨਾ.charAt(pos)==u'\\' && ((c=ਨਮੂਨਾ.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

ਸਤਰਅੰਦਰ UBool
isNameOpen(ਅਚਲ UnicodeString &ਨਮੂਨਾ, int32_t pos) {
    ਵਾਪਸ ਨਮੂਨਾ.charAt(pos)==u'\\' && ਨਮੂਨਾ.charAt(pos+1)==u'N';
}

ਸਤਰਅੰਦਰ UBool
isPOSIXOpen(ਅਚਲ UnicodeString &ਨਮੂਨਾ, int32_t pos) {
    ਵਾਪਸ ਨਮੂਨਾ.charAt(pos)==u'[' && ਨਮੂਨਾ.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ਅਚਲ CharString& name) {
    ਲਈ (ਅਚਲ UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_ਕੋਡ ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.ਅੰਕੜੇ(), &ec);
        ਜੇ (U_SUCCESS(ec)) {
            ਵਾਪਸ ch;
        }
    }
    ਵਾਪਸ U_SENTINEL;
}

}  // namespace

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs a set from the given pattern, optionally ignoring
 * white space.  See the class description for the syntax of the
 * pattern language.
 * @param pattern a string specifying what characters are in the set
 */
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                       UError_ਕੋਡ& ਸਥਿਤੀ) {
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, ਸਥਿਤੀ);
    _dbgct(ਇਹ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::apply_ਨਮੂਨਾ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_ਨਮੂਨਾ_Ignore_Space(ਨਮੂਨਾ, pos, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ *ਇਹ;

    int32_t i = pos.get_ਸੂਚਕਾਂਕ();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(ਨਮੂਨਾ, i, ਸੱਚ);
    ਜੇ (i != ਨਮੂਨਾ.length()) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ
Unicode_ਸੈੱਟ::apply_ਨਮੂਨਾ_Ignore_Space(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                    ParsePosition& pos,
                                    ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ* symbols,
                                    UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (isFrozen()) {
        ਸਥਿਤੀ = U_NO_WRITE_PERMISSION;
        ਵਾਪਸ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    ਨਿਯਮ_Character_Iterator chars(ਨਮੂਨਾ, symbols, pos);
    apply_ਨਮੂਨਾ(ਨਮੂਨਾ, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;
    ਜੇ (chars.ਵਿੱਚ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        ਸਥਿਤੀ = U_MALFORMED_ਸੈੱਟ;
        ਵਾਪਸ;
    }
    ਸੈੱਟ_ਨਮੂਨਾ(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_ਸੈੱਟ::resembles_ਨਮੂਨਾ(ਅਚਲ UnicodeString& ਨਮੂਨਾ, int32_t pos) {
    ਵਾਪਸ ((pos+1) < ਨਮੂਨਾ.length() &&
            ਨਮੂਨਾ.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_ਨਮੂਨਾ(ਨਮੂਨਾ, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_DEBUGGING_UNICODESET_PARSING 0

ਵਰਗ Unicode_ਸੈੱਟ::Lexer {
  ਜਨਤਕ:
    Lexer(ਅਚਲ UnicodeString &ਨਮੂਨਾ,
          ਅਚਲ ParsePosition &parsePosition,
          ਨਿਯਮ_Character_Iterator &chars,
          uint32_t unicode_ਸੈੱਟ_Options,
          ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ *ਅਚਲ symbols,
          Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute))
        : ਨਮੂਨਾ_2(ਨਮੂਨਾ), parsePosition_(parsePosition), chars_(chars),
          unicode_ਸੈੱਟ_Options_2(unicode_ਸੈੱਟ_Options),
          charsOptions_(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_ਸੈੱਟ_Options & USET_IGNORE_SPACE) != 0
                             ? ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_ਘੇਰਾ_2(case_ਘੇਰਾ) {}

    ਵਰਗ LexicalElement {
      ਜਨਤਕ:
        ਤਾਰਕਿਕ is_ਸੈੱਟ_Operator(ਅਚਲ char16_t op) ਅਚਲ {
            ਵਾਪਸ U_SUCCESS(error_ਕੋਡ_2) && category_ == ਸੈੱਟ_OPERATOR && string_[0] == op;
        }

        ਤਾਰਕਿਕ isStringLiteral() ਅਚਲ {
            ਵਾਪਸ U_SUCCESS(error_ਕੋਡ_2) && category_ == STRING_LITERAL;
        }

        ਤਾਰਕਿਕ isNamedElement() ਅਚਲ {
            ਵਾਪਸ U_SUCCESS(error_ਕੋਡ_2) && category_ == NAMED_ELEMENT;
        }

        ਤਾਰਕਿਕ isBracketedElement() ਅਚਲ {
            ਵਾਪਸ U_SUCCESS(error_ਕੋਡ_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ਅਚਲ {
            ਜੇ (U_SUCCESS(error_ਕੋਡ_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ਵਾਪਸ string_;
            }
            ਵਾਪਸ std::nullopt;
        }

        std::optional<UChar32> ਕੋਡ_ਬਿੰਦੂ() ਅਚਲ {
            ਜੇ (U_SUCCESS(error_ਕੋਡ_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ਵਾਪਸ string_.char32At(0);
            }
            ਵਾਪਸ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ਅਚਲ Unicode_ਸੈੱਟ *ਸੈੱਟ() ਅਚਲ {
            ਜੇ (U_FAILURE(error_ਕੋਡ_2)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ਜੇ (precomputed_ਸੈੱਟ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ precomputed_ਸੈੱਟ;
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ &ਸੈੱਟ_2;
                }
            }
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }

        ਅਚਲ UError_ਕੋਡ& error_ਕੋਡ() ਅਚਲ{
          ਵਾਪਸ error_ਕੋਡ_2;
        }

#ਜੇ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ਅਚਲ {
            UnicodeString ਨਤੀਜਾ;
            ਜੇ (U_FAILURE(error_ਕੋਡ_2)) {
                ਨਤੀਜਾ.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_ਕੋਡ_2)))
                    .append(u"), possibly ");
            }
            ਵਾਪਸ ਨਤੀਜਾ.append(category_ਨਾਂ[category_])
                .append(u" '")
                .append(ਸਰੋਤ_ਪਾਠ)
                .append(u"'");
        }
#ਜੇ_ਅੰਤ

      ਨਿਜੀ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ਗਿਣਤੀ Category : std::uint8_t {
            ਸੈੱਟ_OPERATOR,
            LITERAL_ELEMENT,
            ESCAPED_ELEMENT,
            NAMED_ELEMENT,
            BRACKETED_ELEMENT,
            STRING_LITERAL,
            PROPERTY_QUERY,
            // Used for ill-formed variables and set-valued variables that are not directly a
            // property-query, e.g., $basicLatinLetters=[A-Za-z].  Variables that expand to a single
            // lexical element instead have the category of that lexical element, e.g., $Ll=\p{Ll} has
            // the category PROPERTY_QUERY, $a=a has the category LITERAL_ELEMENT, and $s={Zeichenkette}
            // has the category STRING_LITERAL.
            VARIABLE,
            ਸਿਰਾ_ਦਾ_ਪਾਠ,
        };
        ਸਥਿਰ constexpr std::array<std::u16string_view, ਸਿਰਾ_ਦਾ_ਪਾਠ + 1> category_ਨਾਂ{{
            u"set-operator",
            u"literal-element",
            u"escaped-element",
            u"named-element",
            u"bracketed-element",
            u"string-literal",
            u"property-query",
            u"variable",
            u"(end of text)",
        }};
        LexicalElement(Category category, UnicodeString string, ਨਿਯਮ_Character_Iterator::Pos after,
                       ਅਚਲ UError_ਕੋਡ error_ਕੋਡ, ਅਚਲ Unicode_ਸੈੱਟ *precomputed_ਸੈੱਟ_2, Unicode_ਸੈੱਟ ਸੈੱਟ,
                       std::u16string_view ਸਰੋਤ_ਪਾਠ_2, ਤਾਰਕਿਕ separated,
                       std::optional<LexicalElement> ਅਚਲ &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_ਕੋਡ_2(error_ਕੋਡ),
              precomputed_ਸੈੱਟ(precomputed_ਸੈੱਟ_2), ਸੈੱਟ_2(ਸੈੱਟ), ਸਰੋਤ_ਪਾਠ(ਸਰੋਤ_ਪਾਠ_2) {
            ਜੇ (U_FAILURE(error_ਕੋਡ_2) || separated || !preceding.has_value()) {
                ਵਾਪਸ;
            }
            ਜੇ (preceding->is_ਸੈੱਟ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *ਕੋਡ_ਬਿੰਦੂ() == u':') {
                error_ਕੋਡ_2 = U_MALFORMED_ਸੈੱਟ;
            } ਨਹੀਂਤਾਂ ਜੇ (preceding->category_ == ESCAPED_ELEMENT) {
                ਜੇ (U_IS_LEAD(*preceding->ਕੋਡ_ਬਿੰਦੂ()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*ਕੋਡ_ਬਿੰਦੂ())) {
                    error_ਕੋਡ_2 = U_MALFORMED_ਸੈੱਟ;
                } ਨਹੀਂਤਾਂ ਜੇ (preceding->ਸਰੋਤ_ਪਾਠ.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *ਕੋਡ_ਬਿੰਦੂ();
                    ਜੇ (preceding->ਸਰੋਤ_ਪਾਠ[1] == u'x') {
                        ਜੇ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_ਕੋਡ_2 = U_MALFORMED_ਸੈੱਟ;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ (preceding->ਸਰੋਤ_ਪਾਠ[1] >= u'0' &&
                               preceding->ਸਰੋਤ_ਪਾਠ[1] <= u'7') {
                        ਜੇ (cp >= u'0' && cp <= u'7') {
                            error_ਕੋਡ_2 = U_MALFORMED_ਸੈੱਟ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        ਨਿਯਮ_Character_Iterator::Pos after_;
        UError_ਕੋਡ error_ਕੋਡ_2;
        ਅਚਲ Unicode_ਸੈੱਟ *precomputed_ਸੈੱਟ;
        Unicode_ਸੈੱਟ ਸੈੱਟ_2;
        std::u16string_view ਸਰੋਤ_ਪਾਠ;

        ਮਿੱਤਰ ਵਰਗ Lexer;
    };

    UnicodeString getPositionForDebugging() ਅਚਲ {
        ਵਾਪਸ ਨਮੂਨਾ_2.ਆਰਜ਼ੀ_Sub_String(0, parsePosition_.get_ਸੂਚਕਾਂਕ()) + u"☞" +
               ਨਮੂਨਾ_2.ਆਰਜ਼ੀ_Sub_String(parsePosition_.get_ਸੂਚਕਾਂਕ(), 60);
    }

    ਤਾਰਕਿਕ accept_ਸੈੱਟ_Operator(char16_t op) {
        ਜੇ (lookahead().is_ਸੈੱਟ_Operator(op)) {
            advance();
            ਵਾਪਸ ਸੱਚ;
        }
        ਵਾਪਸ ਝੂਠ;
    }

    ਅਚਲ LexicalElement &lookahead() {
        ਜੇ (!ahead_.has_value()) {
            ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos before = getPos();
            ahead_.emplace(ਅਗਲਾ_Token(behind_));
            chars_.ਸੈੱਟ_Pos(before);
        }
        ਵਾਪਸ *ahead_;
    }

    ਅਚਲ LexicalElement &lookahead2() {
        ਜੇ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos before = getPos();
            chars_.ਸੈੱਟ_Pos(lookahead().after_);
            ahead2_.emplace(ਅਗਲਾ_Token(ahead_));
            chars_.ਸੈੱਟ_Pos(before);
        }
        ਵਾਪਸ *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    ਨਿਯਮ_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        ਵਾਪਸ chars_;
    }

    int32_t charsOptions() {
        ਵਾਪਸ charsOptions_;
    }

    ਤਾਰਕਿਕ at_ਸਿਰਾ() ਅਚਲ {
        ਵਾਪਸ chars_.at_ਸਿਰਾ();
    }

    ਖਾਲੀ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.ਸੈੱਟ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ਨਿਜੀ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    ਨਿਯਮ_Character_Iterator::Pos getPos() ਅਚਲ {
        ਨਿਯਮ_Character_Iterator::Pos ਨਤੀਜਾ;
        chars_.getPos(ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    LexicalElement ਅਗਲਾ_Token(std::optional<LexicalElement> ਅਚਲ& preceding) {
        UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
        ਤਾਰਕਿਕ separated = ਝੂਠ;
        ਜੇ (charsOptions_ & ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            ਨਿਯਮ_Character_Iterator::Pos pos;
            ਲਈ (;;) {
                UBool ਅਣਵਰਤਿਆ_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                  ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
                ਜੇ (!ਨਮੂਨਾ_Props::isWhiteSpace(s)) {
                    chars_.ਸੈੱਟ_Pos(pos);
                    ਰੋਕੋ;
                }
                ਜੇ (s != u'\u200E' && s != u'\u200F') {
                    separated = ਸੱਚ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ਜੇ (chars_.at_ਸਿਰਾ()) {
            ਵਾਪਸ LexicalElement(LexicalElement::ਸਿਰਾ_ਦਾ_ਪਾਠ, {}, getPos(), error_ਕੋਡ,
                                  /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ਅਚਲ int32_t ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
        ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool ਅਣਵਰਤਿਆ_Escaped;
        ਅਚਲ UChar32 first =
            chars_.next(charsOptions_ & ~ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES, ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
        ਜੇ (first == u'[' || first == u'\\') {
            ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos after_ਪਹਿਲਾ = getPos();
            // This could be a property-query or named-element.
            ਅਚਲ UChar32 ਦੂਜਾ = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                                 ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                               ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
            ਜੇ ((first == u'[' && ਦੂਜਾ == u':') ||
                (first == u'\\' && (ਦੂਜਾ == u'p' || ਦੂਜਾ == u'P' || ਦੂਜਾ == u'N'))) {
                ਜੇ (ਦੂਜਾ == u'N') {
                    UChar32 ਅਚਲ query_ਨਤੀਜਾ = ਜਾਂਚ_Named_Element_Brackets(error_ਕੋਡ);
                    ਵਾਪਸ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_ਨਤੀਜਾ), getPos(), error_ਕੋਡ,
                        /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                        /*set=*/{},
                        std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                        separated, preceding);
                } ਨਹੀਂਤਾਂ {
                    Unicode_ਸੈੱਟ query_ਨਤੀਜਾ = ਜਾਂਚ_Property_Query_After_ਸ਼ੁਰੂ(first, ਦੂਜਾ, ਸ਼ੁਰੂ, error_ਕੋਡ);
                    ਵਾਪਸ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_ਕੋਡ,
                        /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ, /*set=*/std::move(query_ਨਤੀਜਾ),
                        std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.ਸੈੱਟ_Pos(after_ਪਹਿਲਾ);
        }
        ਜੇ (first == u'$' && symbols_ != ਖਾਲੀ_ਸੂਚਕ) {
            ਸਵੈਚਾਲਿਤ name_ਸਿਰਾ = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ਜੇ (UnicodeString name = symbols_->parseReference(ਨਮੂਨਾ_2, name_ਸਿਰਾ, ਨਮੂਨਾ_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                chars_.jumpahead(name_ਸਿਰਾ.get_ਸੂਚਕਾਂਕ() - (ਸ਼ੁਰੂ + 1));
                ਅਚਲ std::u16string_view ਸਰੋਤ =
                    std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ);
                ਅਚਲ Unicode_ਸੈੱਟ *precomputed_ਸੈੱਟ_2 = symbols_->lookup_ਸੈੱਟ(name);
                ਜੇ (precomputed_ਸੈੱਟ_2 != ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_ਸੈੱਟ_2, /*set=*/{}, ਸਰੋਤ, /*separated=*/ਸੱਚ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ਅਚਲ UnicodeString *ਅਚਲ expression = symbols_->lookup(name);
                ਜੇ (expression == ਖਾਲੀ_ਸੂਚਕ) {
                    ਵਾਪਸ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                        /*set=*/{},
                        ਸਰੋਤ,
                        /*separated=*/ਸੱਚ,
                        /*preceding=*/std::nullopt);
                }
                ਵਾਪਸ evaluateVariable(*expression, ਸਰੋਤ);
            }
        }
        ਚੋਣ (first) {
        ਹਾਲਤ u'[':
            ਵਾਪਸ LexicalElement(
                LexicalElement::ਸੈੱਟ_OPERATOR, UnicodeString(u'['), getPos(), error_ਕੋਡ,
                /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/{},
                std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                separated, preceding);
        ਹਾਲਤ u'\\': {
            // Now try to parse the escape.
            chars_.ਸੈੱਟ_Pos(before);
            UChar32 ਕੋਡ_ਬਿੰਦੂ = chars_.next(charsOptions_, ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
            ਵਾਪਸ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(ਕੋਡ_ਬਿੰਦੂ), getPos(), error_ਕੋਡ,
                ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/{},
                std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                separated, preceding);
        }
        ਹਾਲਤ u'&':
        ਹਾਲਤ u'-':
        ਹਾਲਤ u']':
        ਹਾਲਤ u'^':
        ਹਾਲਤ u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ਵਾਪਸ LexicalElement(
                LexicalElement::ਸੈੱਟ_OPERATOR, UnicodeString(first), getPos(), error_ਕੋਡ,
                /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/{},
                std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                separated, preceding);
        ਹਾਲਤ u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t ਕੋਡ_ਬਿੰਦੂ_ਗਿਣਤੀ = 0;
            ਜਦੋਂ (!chars_.at_ਸਿਰਾ() && U_SUCCESS(error_ਕੋਡ)) {
                ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos before_ਅਗਲਾ = getPos();
                next = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                     ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                   ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
                ਜੇ (next == u'\\') {
                    ਅਚਲ UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                      ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                    ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
                    ਜੇ (afterBackslash == u'N') {
                        next = ਜਾਂਚ_Named_Element_Brackets(error_ਕੋਡ);
                        escaped = ਸੱਚ;
                    } ਨਹੀਂਤਾਂ ਜੇ (afterBackslash == u'p' || afterBackslash == u'P') {
                        ਵਾਪਸ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_ਸੈੱਟ,
                                              /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                                              /*set=*/{},
                                              std::u16string_view(ਨਮੂਨਾ_2).substr(
                                                  ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                                              separated, preceding);
                    } ਨਹੀਂਤਾਂ {
                        chars_.ਸੈੱਟ_Pos(before_ਅਗਲਾ);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_ਕੋਡ);
                    }
                } ਨਹੀਂਤਾਂ {
#ਜੇ U_ICU_ਸੰਸਕਰਣ_MAJOR_ਅੰਕ < 81
                    ਜੇ (U_SUCCESS(error_ਕੋਡ) && ਨਮੂਨਾ_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_ਕੋਡ = UError_ਕੋਡ::U_ILLEGAL_ਦਲੀਲ_ERROR;
                    }
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ Remove ਇਹ transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#ਜੇ_ਅੰਤ
                    escaped = ਝੂਠ;
                }
                ਜੇ (!escaped && next == u'}') {
                    ਵਾਪਸ LexicalElement(
                        ਕੋਡ_ਬਿੰਦੂ_ਗਿਣਤੀ == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_ਕੋਡ,
                        /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                        /*set=*/{},
                        std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                        separated, preceding);
                }
                string.append(next);
                ਕੋਡ_ਬਿੰਦੂ_ਗਿਣਤੀ += 1;
            }
            ਵਾਪਸ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_ਸੈੱਟ,
                /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/{},
                std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                separated, preceding);
        }
        ਮੂਲ:
            ਵਾਪਸ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_ਕੋਡ, ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/{},
                std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - ਸ਼ੁਰੂ),
                separated, preceding);
        }
    }

    UChar32 ਜਾਂਚ_Named_Element_Brackets(UError_ਕੋਡ &error_ਕੋਡ) {
        UBool ਅਣਵਰਤਿਆ_Escaped;
        ਅਚਲ UChar32 open = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                           ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                         ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
        ਜੇ (open == u'{') {
            int32_t ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ਜਦੋਂ (!chars_.at_ਸਿਰਾ() && U_SUCCESS(error_ਕੋਡ)) {
                UChar32 ਆਖਰੀ = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                             ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                           ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
                ਜੇ (ਆਖਰੀ == u':') {
                    ਜੇ (!hex.has_value()) {
                        hex.emplace();
                        ਜੇ (ਸ਼ੁਰੂ == parsePosition_.get_ਸੂਚਕਾਂਕ() - 1) {
                            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                            ਵਾਪਸ {};
                        }
                        ਲਈ (char16_t digit : std::u16string_view(ਨਮੂਨਾ_2).substr(
                                 ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - 1 - ਸ਼ੁਰੂ)) {
                            uint8_t nibble;
                            ਜੇ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ਨਹੀਂਤਾਂ {
                                digit = digit & ~0x20;
                                ਜੇ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ਨਹੀਂਤਾਂ {
                                    error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                                    ਵਾਪਸ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ਜੇ (hex > 0x10FFFF) {
                                error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                                ਵਾਪਸ {};
                            }
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ (!literal.has_value()) {
                        ਅਚਲ ਸਵੈਚਾਲਿਤ literal_ਕੋਡ_Points = utf_String_ਕੋਡ_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(ਨਮੂਨਾ_2).substr(ਸ਼ੁਰੂ,
                                                                 parsePosition_.get_ਸੂਚਕਾਂਕ() - 1 - ਸ਼ੁਰੂ));
                        ਸਵੈਚਾਲਿਤ it = literal_ਕੋਡ_Points.begin();
                        ਜੇ (it == literal_ਕੋਡ_Points.ਸਿਰਾ() || !it->wellFormed() ||
                                (literal = it->ਕੋਡ_ਬਿੰਦੂ(), ++it) != literal_ਕੋਡ_Points.ਸਿਰਾ()) {
                            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                            ਵਾਪਸ {};
                        }
                    } ਨਹੀਂਤਾਂ {
                        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                        ਵਾਪਸ {};
                    }
                    ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
                } ਨਹੀਂਤਾਂ ਜੇ (ਆਖਰੀ == u'}') {
                    ਅਚਲ std::u16string_view u16name = std::u16string_view(ਨਮੂਨਾ_2).substr(
                        ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ() - 1 - ਸ਼ੁਰੂ);
                    ਅਚਲ UChar32 ਨਤੀਜਾ = getCharacterByName(CharString().appendInvariantChars(
                        u16name.ਅੰਕੜੇ(), static_cast<int32_t>(u16name.length()), error_ਕੋਡ));
                    ਜੇ (!U_SUCCESS(error_ਕੋਡ)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                        ਵਾਪਸ {};
                    }
                    ਜੇ (ਨਤੀਜਾ < 0 || (hex.has_value() && ਨਤੀਜਾ != hex) ||
                            (literal.has_value() && ਨਤੀਜਾ != literal)) {
                        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                        ਵਾਪਸ {};
                    }
                    ਵਾਪਸ ਨਤੀਜਾ;
                }
            }
        }
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
        ਵਾਪਸ {};
    }

    LexicalElement evaluateVariable(ਅਚਲ UnicodeString &expression,
                                    ਅਚਲ std::u16string_view ਸਰੋਤ) {
        UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
        ParsePosition ਵਿਅੰਜਕ_Position;
        ਨਿਯਮ_Character_Iterator ਵਿਅੰਜਕ_Iterator(expression, symbols_, ਵਿਅੰਜਕ_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer ਵਿਅੰਜਕ_Lexer(expression, ਵਿਅੰਜਕ_Position, ਵਿਅੰਜਕ_Iterator, unicode_ਸੈੱਟ_Options_2,
                              /*symbols=*/ਖਾਲੀ_ਸੂਚਕ, case_ਘੇਰਾ_2);
        ਸਵੈਚਾਲਿਤ variableToken = ਵਿਅੰਜਕ_Lexer.lookahead();
        ਜੇ (variableToken.is_ਸੈੱਟ_Operator(u'[')) {
            UnicodeString rebuilt_ਨਮੂਨਾ;
            Unicode_ਸੈੱਟ ਵਿਅੰਜਕ_ਮੁੱਲ =
                parse_Unicode_ਸੈੱਟ(ਵਿਅੰਜਕ_Lexer, rebuilt_ਨਮੂਨਾ, unicode_ਸੈੱਟ_Options_2, case_ਘੇਰਾ_2,
                                /*depth=*/0, error_ਕੋਡ);
            ਵਿਅੰਜਕ_ਮੁੱਲ.ਸੈੱਟ_ਨਮੂਨਾ(rebuilt_ਨਮੂਨਾ);
            ਜੇ (!ਵਿਅੰਜਕ_Lexer.at_ਸਿਰਾ()) {
                ਵਾਪਸ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                    /*set=*/{},
                    ਸਰੋਤ,
                    /*separated=*/ਸੱਚ,
                    /*preceding=*/std::nullopt);
            }
            ਵਾਪਸ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_ਕੋਡ,
                /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                /*set=*/std::move(ਵਿਅੰਜਕ_ਮੁੱਲ),
                ਸਰੋਤ,
                /*separated=*/ਸੱਚ,
                /*preceding=*/std::nullopt);
        } ਨਹੀਂਤਾਂ {
            ਵਿਅੰਜਕ_Lexer.advance();
            ਜੇ (!ਵਿਅੰਜਕ_Lexer.at_ਸਿਰਾ()) {
                ਵਾਪਸ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                    /*set=*/{},
                    ਸਰੋਤ,
                    /*separated=*/ਸੱਚ,
                    /*preceding=*/std::nullopt);
            }
            ਚੋਣ (variableToken.category_) {
            ਹਾਲਤ LexicalElement::LITERAL_ELEMENT:
            ਹਾਲਤ LexicalElement::ESCAPED_ELEMENT:
            ਹਾਲਤ LexicalElement::NAMED_ELEMENT:
            ਹਾਲਤ LexicalElement::BRACKETED_ELEMENT:
            ਹਾਲਤ LexicalElement::STRING_LITERAL:
            ਹਾਲਤ LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ਵਾਪਸ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_ਕੋਡ_2, variableToken.precomputed_ਸੈੱਟ, std::move(variableToken.ਸੈੱਟ_2), ਸਰੋਤ,
                    /*separated=*/ਸੱਚ,
                    /*preceding=*/std::nullopt);
            ਮੂਲ:
                ਵਾਪਸ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/ਖਾਲੀ_ਸੂਚਕ,
                                      /*set=*/{}, ਸਰੋਤ,
                                      /*separated=*/ਸੱਚ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_ਸੈੱਟ ਜਾਂਚ_Property_Query_After_ਸ਼ੁਰੂ(UChar32 first, UChar32 ਦੂਜਾ, int32_t query_ਸ਼ੁਰੂ, UError_ਕੋਡ &error_ਕੋਡ) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
        ਤਾਰਕਿਕ exteriorlyNegated = ਝੂਠ;
        ਤਾਰਕਿਕ interiorlyNegated = ਝੂਠ;
        UBool ਅਣਵਰਤਿਆ_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ਅਚਲ UChar32 ਤੀਜਾ = chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                                            ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                                          ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
        ਜੇ (first == u'\\') {
            ਜੇ (ਤੀਜਾ != u'{') {
                error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ {};
            }
            exteriorlyNegated = ਦੂਜਾ == u'P';
            query_ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਤੀਜਾ == u'^') {
                exteriorlyNegated = ਸੱਚ;
                query_ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = parsePosition_.get_ਸੂਚਕਾਂਕ();
            }
        }
        ਨਿਯਮ_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                          ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                        ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);

        ਜਦੋਂ (!chars_.at_ਸਿਰਾ() && U_SUCCESS(error_ਕੋਡ)) {
            ਅਚਲ ਨਿਯਮ_Character_Iterator::Pos before_ਆਖਰੀ = getPos();
            UChar32 ਆਖਰੀ_Unescaped =
                chars_.next(charsOptions_ & ~(ਨਿਯਮ_Character_Iterator::PARSE_ESCAPES |
                                              ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE),
                            ਅਣਵਰਤਿਆ_Escaped, error_ਕੋਡ);
            ਜੇ (penultimateUnescaped == u'\\') {
                ਜੇ (ਆਖਰੀ_Unescaped == 'N') {
                    ਜਾਂਚ_Named_Element_Brackets(error_ਕੋਡ);
                    ਜੇ (!U_SUCCESS(error_ਕੋਡ)) {
                        ਵਾਪਸ {};
                    }
                } ਨਹੀਂਤਾਂ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.ਸੈੱਟ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~ਨਿਯਮ_Character_Iterator::SKIP_WHITESPACE, ਅਣਵਰਤਿਆ_Escaped,
                                error_ਕੋਡ);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                ਆਖਰੀ_Unescaped = -1;
            } ਨਹੀਂਤਾਂ ਜੇ (!queryOperatorPosition.has_value() && ਆਖਰੀ_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_ਸੂਚਕਾਂਕ() - 1;
            } ਨਹੀਂਤਾਂ ਜੇ (!queryOperatorPosition.has_value() && ਆਖਰੀ_Unescaped == u'≠') {
                ਜੇ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                    ਵਾਪਸ {};
                }
                interiorlyNegated = ਸੱਚ;
                queryOperatorPosition = parsePosition_.get_ਸੂਚਕਾਂਕ() - 1;
            } ਨਹੀਂਤਾਂ ਜੇ ((first == u'[' && penultimateUnescaped == u':' && ਆਖਰੀ_Unescaped == u']') ||
                       (first == u'\\' && ਆਖਰੀ_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ਅਚਲ int32_t query_ਵਿਅੰਜਕ_ਹੱਦਬੰਦੀ =
                    first == u'[' ? parsePosition_.get_ਸੂਚਕਾਂਕ() - 2 : parsePosition_.get_ਸੂਚਕਾਂਕ() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ਜੇ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        ਨਮੂਨਾ_2.ਆਰਜ਼ੀ_Sub_String_Between(*queryOperatorPosition + 1, query_ਵਿਅੰਜਕ_ਹੱਦਬੰਦੀ);
                    ਜੇ (propertyPredicate.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                        ਵਾਪਸ {};
                    }
                }
                Unicode_ਸੈੱਟ ਨਤੀਜਾ;
                ਨਤੀਜਾ.applyPropertyAlias(
                    ਨਮੂਨਾ_2.ਆਰਜ਼ੀ_Sub_String_Between(query_ਵਿਅੰਜਕ_ਸ਼ੁਰੂ,
                                                  queryOperatorPosition.value_or(query_ਵਿਅੰਜਕ_ਹੱਦਬੰਦੀ)),
                    propertyPredicate, error_ਕੋਡ);
                ਜੇ (exteriorlyNegated != interiorlyNegated) {
                    ਨਤੀਜਾ.complement().remove_All_ਲੜੀਆਂ();
                }
                ਨਤੀਜਾ.ਸੈੱਟ_ਨਮੂਨਾ(ਨਮੂਨਾ_2.ਆਰਜ਼ੀ_Sub_String_Between(query_ਸ਼ੁਰੂ, parsePosition_.get_ਸੂਚਕਾਂਕ()));
                ਵਾਪਸ ਨਤੀਜਾ;
            }
            beforePenultimate = before_ਆਖਰੀ;
            penultimateUnescaped = ਆਖਰੀ_Unescaped;
        }
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ {};
    }

    ਅਚਲ UnicodeString &ਨਮੂਨਾ_2;
    ਅਚਲ ParsePosition &parsePosition_;
    ਨਿਯਮ_Character_Iterator &chars_;
    ਅਚਲ uint32_t unicode_ਸੈੱਟ_Options_2;
    ਅਚਲ int32_t charsOptions_;
    ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ *ਅਚਲ symbols_;
    Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::* ਅਚਲ case_ਘੇਰਾ_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

ਨਾਮਾਕਾਸ਼ {

constexpr int32_t MAX_ਡੂੰਘਾਈ = 100;

#ਜੇ U_DEBUGGING_UNICODESET_PARSING

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_UNICODESET_RETURN_IF_ERROR(ਨਤੀਜਾ, ec)                                                   \
    ਕਰੋ {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ਜੇ (U_FAILURE(ec)) {                                                                       \
            ਜੇ (ਡੂੰਘਾਈ < 5) {                                                                       \
                ਲਿਖੋ("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                ਲਿਖੋ("--- [...]\n");                                                             \
            }                                                                                      \
            ਵਾਪਸ ਨਤੀਜਾ;                                                                         \
        }                                                                                          \
    } ਜਦੋਂ (ਝੂਠ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_UNICODESET_RETURN_WITH_PARSE_ERROR(ਨਤੀਜਾ, expected, actual, lexer, ec)                  \
    ਕਰੋ {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        ਲਿਖੋ("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).ਮੰਜ਼ਿਲ_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().ਮੰਜ਼ਿਲ_UTF8_String(contextUTF8).c_str());                 \
        ਲਿਖੋ("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ਜੇ (U_FAILURE(lexer.lookahead().error_ਕੋਡ())) {                                            \
            (ec) = lexer.lookahead().error_ਕੋਡ();                                                  \
        } ਨਹੀਂਤਾਂ {                                                                                   \
            (ec) = U_MALFORMED_ਸੈੱਟ;                                                                \
        }                                                                                          \
        ਵਾਪਸ ਨਤੀਜਾ;                                                                             \
    } ਜਦੋਂ (ਝੂਠ)

#ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_UNICODESET_RETURN_IF_ERROR(ਨਤੀਜਾ, ec)                                                   \
    ਕਰੋ {                                                                                           \
        ਜੇ (U_FAILURE(ec)) {                                                                       \
            ਵਾਪਸ ਨਤੀਜਾ;                                                                         \
        }                                                                                          \
    } ਜਦੋਂ (ਝੂਠ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_UNICODESET_RETURN_WITH_PARSE_ERROR(ਨਤੀਜਾ, expected, actual, lexer, ec)                  \
    ਕਰੋ {                                                                                           \
        ਜੇ (U_FAILURE(lexer.lookahead().error_ਕੋਡ())) {                                            \
            (ec) = lexer.lookahead().error_ਕੋਡ();                                                  \
        } ਨਹੀਂਤਾਂ {                                                                                   \
            (ec) = U_MALFORMED_ਸੈੱਟ;                                                                \
        }                                                                                          \
        ਵਾਪਸ ਨਤੀਜਾ;                                                                             \
    } ਜਦੋਂ (ਝੂਠ)

#ਜੇ_ਅੰਤ

}  // namespace

/**
 * Parse the pattern from the given RuleCharacterIterator.  The
 * iterator is advanced over the parsed pattern.
 * @param pattern The pattern, only used by debug traces.
 * @param parsePosition The ParsePosition underlying chars, only used by debug traces.
 * @param chars iterator over the pattern characters.  Upon return
 * it will be advanced to the first character after the parsed
 * pattern, or the end of the iteration if all characters are
 * parsed.
 * @param symbols symbol table to use to parse and dereference
 * variables, or null if none.
 * @param rebuiltPat the pattern that was parsed, rebuilt or
 * copied from the input pattern, as appropriate.
 * @param options a bit mask of zero or more of the following:
 * IGNORE_SPACE, CASE.
 */

ਖਾਲੀ Unicode_ਸੈੱਟ::apply_ਨਮੂਨਾ(ਅਚਲ UnicodeString &ਨਮੂਨਾ,
                              ਅਚਲ ParsePosition &parsePosition,
                              ਨਿਯਮ_Character_Iterator &chars,
                              ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute),
                              UError_ਕੋਡ &ec) {
    ਜੇ (U_FAILURE(ec)) ਵਾਪਸ;
    Lexer lexer(ਨਮੂਨਾ, parsePosition, chars, options, symbols, case_ਘੇਰਾ);
    *ਇਹ = parse_Unicode_ਸੈੱਟ(lexer, rebuiltPat, options, case_ਘੇਰਾ, /*depth=*/0, ec);
}

Unicode_ਸੈੱਟ Unicode_ਸੈੱਟ::parse_Unicode_ਸੈੱਟ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_ਸੈੱਟ& (Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute),
                                       int32_t ਡੂੰਘਾਈ,
                                       UError_ਕੋਡ &ec) {
    Unicode_ਸੈੱਟ ਨਤੀਜਾ;

    ਜੇ (ਡੂੰਘਾਈ > MAX_ਡੂੰਘਾਈ) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_ਡੂੰਘਾਈ)).c_str(),
                                             ("depth = " + std::to_string(ਡੂੰਘਾਈ)).c_str(), lexer,
                                             ec);
    }

    ਤਾਰਕਿਕ isComplement = ਝੂਠ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    ਤਾਰਕਿਕ preserve_Syntax_ਵਿੱਚ_ਨਮੂਨਾ = ਝੂਠ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_ਨਮੂਨਾ;
    ਜੇ (lexer.lookahead().ਸੈੱਟ() != ਖਾਲੀ_ਸੂਚਕ) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        ਨਤੀਜਾ = *lexer.lookahead().ਸੈੱਟ();
        ਨਤੀਜਾ.ਮੰਜ਼ਿਲ_ਨਮੂਨਾ_2(pretty_Printed_ਨਮੂਨਾ, /*escapeUnprintable=*/ਝੂਠ);
        lexer.advance();
        preserve_Syntax_ਵਿੱਚ_ਨਮੂਨਾ = ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ਜੇ (lexer.accept_ਸੈੱਟ_Operator(u'[')) {
            pretty_Printed_ਨਮੂਨਾ.append(u'[');
            ਜੇ (lexer.accept_ਸੈੱਟ_Operator(u'^')) {
                pretty_Printed_ਨਮੂਨਾ.append(u'^');
                isComplement = ਸੱਚ;
            }
            ਨਤੀਜਾ = parseContent(lexer, pretty_Printed_ਨਮੂਨਾ, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ,
                                  /*containsSetOperation=*/preserve_Syntax_ਵਿੱਚ_ਨਮੂਨਾ, ec);
            U_UNICODESET_RETURN_IF_ERROR(ਨਤੀਜਾ, ec);
            ਜੇ (!lexer.accept_ਸੈੱਟ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(ਨਤੀਜਾ, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_ਨਮੂਨਾ.append(u']');
        } ਨਹੀਂਤਾਂ {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(ਨਤੀਜਾ, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    ਜੇ ((options & USET_CASE_ਨਕਾਬ) != 0) {
        (ਨਤੀਜਾ.*case_ਘੇਰਾ)(options);
    }
    ਜੇ (isComplement) {
        ਨਤੀਜਾ.complement().remove_All_ਲੜੀਆਂ(); // code point complement
    }
    ਜੇ (preserve_Syntax_ਵਿੱਚ_ਨਮੂਨਾ) {
        rebuiltPat.append(pretty_Printed_ਨਮੂਨਾ);
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.generate_ਨਮੂਨਾ(rebuiltPat, /*escapeUnprintable=*/ਝੂਠ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

Unicode_ਸੈੱਟ Unicode_ਸੈੱਟ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute),
                                    int32_t ਡੂੰਘਾਈ,
                                    ਤਾਰਕਿਕ &ਸ਼ਾਮਲ_ਹੈ_ਸੈੱਟ_Operation,
                                    UError_ਕੋਡ &ec) {
    Unicode_ਸੈੱਟ ਨਤੀਜਾ;
    //   Content ::= ""
    //             | ElementList
    //             | ElementList UnescapedHyphenMinus
    //             -- ICU extensions:
    //             | Æther
    //             | ElementList Æther
    //             | DollarElements UnescapedHyphenMinus
    //             | DollarElements Æther
    //             | ElementList DollarElements UnescapedHyphenMinus
    //             | ElementList DollarElements Æther
    //   Æther ::= $                                          -- ICU extension
    //   ElementList ::= UnescapedHyphenMinus
    //                 | Elements
    //                 | ElementList Elements
    //                 | SetOperation
    //                 | ElementList DollarElements Elements  -- ICU extension
    //   SetOperation ::= Union
    //                  | Intersection
    //                  | Difference
    //   Union ::= UnicodeSet
    //           | ElementList UnicodeSet
    //           | ElementList DollarElements UnicodeSet      -- ICU extension
    //   -- ICU extension:
    //   DollarElements ::= $
    //                    | RangeElement-$
    // But that is not LL (we cannot tell if we have a SetOperation or not by looking at the
    // first Elements), so we parse it as described in the note,
    //   ElementList ::= Mutation Mutations | UnescapedHyphenMinus Mutations
    //   Mutations   ::= "" | Mutation Mutations
    //   Mutation    ::= Elements  | SetOperations
    // Where a Mutation is not a subexpression, but a modification of the enclosing ElementList,
    // either adding or removing characters; this means that `result.parseMutation` adds to or
    // removes from `result`, instead of being static returning a set.
    // In parseSetOperations below, we will describe the logic both in terms of the LR
    // expression grammar, and in terms of the LL grammar.
    // With the DollarElements ICU extension, a non-final Mutation can be DollarElements, so this
    // becomes
    //   ElementList ::= Mutations FinalMutation
    //                 | UnescapedHyphenMinus Mutations FinalMutation
    //                 | UnescapedHyphenMinus
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    ਜੇ (lexer.accept_ਸੈੱਟ_Operator(u'-')) {
        ਨਤੀਜਾ.ਜੋੜੋ(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ਜਦੋਂ (!lexer.at_ਸਿਰਾ()) {
        ਜੇ (lexer.accept_ਸੈੱਟ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            ਨਤੀਜਾ.ਜੋੜੋ(u'-');
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator(u'$')) {
            ਜੇ (lexer.lookahead2().is_ਸੈੱਟ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                ਨਤੀਜਾ.ਜੋੜੋ(U_ETHER);
                ਸ਼ਾਮਲ_ਹੈ_ਸੈੱਟ_Operation = ਸੱਚ;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        }
        ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator(u']')) {
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        // Also handles FinalMutation.
        ਨਤੀਜਾ.parseMutation(lexer, rebuiltPat, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ, ਸ਼ਾਮਲ_ਹੈ_ਸੈੱਟ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(ਨਤੀਜਾ, ec);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Unicode_ਸੈੱਟ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute),
                               int32_t ਡੂੰਘਾਈ,
                               ਤਾਰਕਿਕ &ਸ਼ਾਮਲ_ਹੈ_ਸੈੱਟ_Operation,
                               UError_ਕੋਡ &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator('[') || lexer.lookahead().ਸੈੱਟ() != ਖਾਲੀ_ਸੂਚਕ) {
        ਸ਼ਾਮਲ_ਹੈ_ਸੈੱਟ_Operation = ਸੱਚ;
        parse_ਸੈੱਟ_Operations(lexer, rebuiltPat, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ਨਹੀਂਤਾਂ {
        // Also handles DollarElements.
        ਜੋੜੋ_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

ਖਾਲੀ Unicode_ਸੈੱਟ::parse_ਸੈੱਟ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_ਸੈੱਟ &(Unicode_ਸੈੱਟ::*case_ਘੇਰਾ)(int32_t attribute),
                                    int32_t ਡੂੰਘਾਈ,
                                    UError_ਕੋਡ &ec) {
    // When we return from this function, this object represents a
    // https://www.unicode.org/reports/tr61/#SetOperation:
    //   SetOperation ::= Union
    //                  | Intersection
    //                  | Difference
    //   Union ::= UnicodeSet
    //           | ElementList UnicodeSet
    //           | ElementList DollarElements UnicodeSet  -- ICU extension
    //   Intersection ::= SetOperation & UnicodeSet
    //   Difference   ::= SetOperation - UnicodeSet
    // since we parse top-down, we have already gone past any ElementList in the Union and added
    // those to this object, and we end up with the UnicodeSet of the Union following by any
    // right hand sides.  In the LL grammar from the note, this is:
    //   SetOperations  ::= UnicodeSet RightHandSides
    //   RightHandSides ::= ""
    //                    | & UnicodeSet RightHandSides
    //                    | - UnicodeSet RightHandSides
    // but note that the tree resulting from this LL version is not an expression tree: the
    // operations are left-associative.
    // Start by parsing the first UnicodeSet.
    ਅਚਲ Unicode_ਸੈੱਟ ਖੱਬਾ_Hand_Side =
        parse_Unicode_ਸੈੱਟ(lexer, rebuiltPat, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ + 1, ec);
    ਜੋੜੋ_All(ਖੱਬਾ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    ਲਈ (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ਜੇ (lexer.accept_ਸੈੱਟ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ਅਚਲ Unicode_ਸੈੱਟ ਸੱਜਾ_Hand_Side =
                parse_Unicode_ਸੈੱਟ(lexer, rebuiltPat, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ਸੱਜਾ_Hand_Side);
        } ਨਹੀਂਤਾਂ ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ਜੇ (lexer.lookahead2().is_ਸੈੱਟ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ਵਾਪਸ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ਅਚਲ Unicode_ਸੈੱਟ ਸੱਜਾ_Hand_Side =
                parse_Unicode_ਸੈੱਟ(lexer, rebuiltPat, options, case_ਘੇਰਾ, ਡੂੰਘਾਈ + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ਸੱਜਾ_Hand_Side);
        } ਨਹੀਂਤਾਂ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ਵਾਪਸ;
        }
    }
}

Unicode_ਸੈੱਟ Unicode_ਸੈੱਟ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_ਕੋਡ &ec) {
    // Elements     ::= Element
    //                | Range
    // Range        ::= RangeElement - RangeElement
    //                | $ - RangeElement             -- ICU extension
    // RangeElement ::= literal-element
    //                | escaped-element
    //                | named-element
    //                | bracketed-element
    // Element      ::= RangeElement
    //                | string-literal
    // In addition, this function handles the following ICU extension:
    // DollarElements ::= $
    //                  | RangeElement - $
    // which cannot appear at the end of Content.
    // A Content-final $ would already have been interpreted as an Æther by parseContent, so we
    // only need to check that RangeElement - $ is not Content-final.
    ਜੇ (lexer.lookahead().isStringLiteral()) {
        ਅਚਲ UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_ਮੰਜ਼ਿਲ_Pat(rebuiltPat, element, /*escapeUnprintable=*/ਝੂਠ);
        rebuiltPat.append(u'}');
        lexer.advance();
        ਵਾਪਸ Unicode_ਸੈੱਟ().ਜੋੜੋ(element);
    }
    UChar32 first;
    ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ਨਹੀਂਤਾਂ ਜੇ (lexer.lookahead().ਕੋਡ_ਬਿੰਦੂ().has_value()) {
        first = *lexer.lookahead().ਕੋਡ_ਬਿੰਦੂ();
    } ਨਹੀਂਤਾਂ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_ਮੰਜ਼ਿਲ_Pat(rebuiltPat, first, /*escapeUnprintable=*/ਝੂਠ);
    ਜੇ (!lexer.lookahead().is_ਸੈੱਟ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        ਵਾਪਸ Unicode_ਸੈੱਟ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ਜੇ (lexer.lookahead2().is_ਸੈੱਟ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ਵਾਪਸ Unicode_ਸੈੱਟ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 ਆਖਰੀ;
    ਜੇ (lexer.lookahead().is_ਸੈੱਟ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        ਆਖਰੀ = u'$';
        ਜੇ (lexer.lookahead2().is_ਸੈੱਟ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ਨਹੀਂਤਾਂ ਜੇ (lexer.lookahead().ਕੋਡ_ਬਿੰਦੂ().has_value()) {
        ਆਖਰੀ = *lexer.lookahead().ਕੋਡ_ਬਿੰਦੂ();
    } ਨਹੀਂਤਾਂ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ਜੇ (ਆਖਰੀ <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(ਆਖਰੀ) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_ਮੰਜ਼ਿਲ_Pat(rebuiltPat, ਆਖਰੀ, /*escapeUnprintable=*/ਝੂਠ);
    ਵਾਪਸ Unicode_ਸੈੱਟ(first, ਆਖਰੀ);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

ਨਾਮਾਕਾਸ਼ {

UBool numeric_ਮੁੱਲ_Filter(UChar32 ch, ਖਾਲੀ* context) {
    ਵਾਪਸ u_get_Numeric_ਮੁੱਲ(ch) == *static_cast<ਦੁੱਗਣਾ*>(context);
}

UBool general_Category_ਨਕਾਬ_Filter(UChar32 ch, ਖਾਲੀ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ਵਾਪਸ (U_GET_GC_ਨਕਾਬ((UChar32) ch) & value) != 0;
}

UBool ਸੰਸਕਰਣ_Filter(UChar32 ch, ਖਾਲੀ* context) {
    ਸਥਿਰ ਅਚਲ UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* ਸੰਸਕਰਣ = static_cast<UVersionInfo*>(context);
    ਵਾਪਸ uprv_memcmp(&v, &none, ਆਕਾਰ(v)) > 0 && uprv_memcmp(&v, ਸੰਸਕਰਣ, ਆਕਾਰ(v)) <= 0;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, ਖਾਲੀ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ਵਾਪਸ u_get_Int_Property_ਮੁੱਲ(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, ਖਾਲੀ* context) {
    ਵਾਪਸ uscript_hasScript(ch, *static_cast<UScript_ਕੋਡ*>(context));
}

UBool idTypeFilter(UChar32 ch, ਖਾਲੀ* context) {
    ਵਾਪਸ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::applyFilter(Unicode_ਸੈੱਟ::Filter filter,
                             ਖਾਲੀ* context,
                             ਅਚਲ Unicode_ਸੈੱਟ* inclusions,
                             UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ;

    // Logically, walk through all Unicode characters, noting the start
    // and end of each range for which filter.contain(c) is
    // true.  Add each range to a set.
    //
    // To improve performance, use an inclusions set which
    // encodes information about character ranges that are known
    // to have identical properties.
    // inclusions contains the first characters of
    // same-value ranges for the given property.

    clear();

    UChar32 ਸ਼ੁਰੂ_Has_Property = -1;
    int32_t ਹੱਦਬੰਦੀ_Range = inclusions->get_Range_ਗਿਣਤੀ();

    ਲਈ (ਪੂਰਨ j=0; j<ਹੱਦਬੰਦੀ_Range; ++j) {
        // get current range
        UChar32 ਸ਼ੁਰੂ = inclusions->get_Range_ਸ਼ੁਰੂ(j);
        UChar32 ਸਿਰਾ = inclusions->get_Range_ਸਿਰਾ(j);

        // for all the code points in the range, process
        ਲਈ (UChar32 ch = ਸ਼ੁਰੂ; ch <= ਸਿਰਾ; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ਜੇ ((*filter)(ch, context)) {
                ਜੇ (ਸ਼ੁਰੂ_Has_Property < 0) {
                    ਸ਼ੁਰੂ_Has_Property = ch;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ_Has_Property >= 0) {
                ਜੋੜੋ(ਸ਼ੁਰੂ_Has_Property, ch-1);
                ਸ਼ੁਰੂ_Has_Property = -1;
            }
        }
    }
    ਜੇ (ਸ਼ੁਰੂ_Has_Property >= 0) {
        ਜੋੜੋ(ਸ਼ੁਰੂ_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    ਜੇ (isBogus() && U_SUCCESS(ਸਥਿਤੀ)) {
        // We likely ran out of memory. AHHH!
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}

ਨਾਮਾਕਾਸ਼ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAIL(ec) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ec=U_ILLEGAL_ਦਲੀਲ_ERROR; \
    ਵਾਪਸ *ਇਹ; \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

Unicode_ਸੈੱਟ&
Unicode_ਸੈੱਟ::apply_Int_Property_ਮੁੱਲ(UProperty prop, int32_t value, UError_ਕੋਡ& ec) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(ec) || isFrozen()) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (prop == UCHAR_GENERAL_CATEGORY_ਨਕਾਬ) {
        ਅਚਲ Unicode_ਸੈੱਟ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_ਨਕਾਬ_Filter, &value, inclusions, ec);
    } ਨਹੀਂਤਾਂ ਜੇ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ਅਚਲ Unicode_ਸੈੱਟ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_ਕੋਡ script = static_cast<UScript_ਕੋਡ>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ਨਹੀਂਤਾਂ ਜੇ (prop == UCHAR_IDENTIFIER_TYPE) {
        ਅਚਲ Unicode_ਸੈੱਟ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ਨਹੀਂਤਾਂ ਜੇ (0 <= prop && prop < UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) {
        ਜੇ (value == 0 || value == 1) {
            ਅਚਲ USet *ਸੈੱਟ = u_get_ਦੁਆਧਾਰੀ_Property_ਸੈੱਟ(prop, &ec);
            ਜੇ (U_FAILURE(ec)) { ਵਾਪਸ *ਇਹ; }
            copyFrom(*Unicode_ਸੈੱਟ::fromUSet(ਸੈੱਟ), ਸੱਚ);
            ਜੇ (value == 0) {
                complement().remove_All_ਲੜੀਆਂ();  // code point complement
            }
        } ਨਹੀਂਤਾਂ {
            clear();
        }
    } ਨਹੀਂਤਾਂ ਜੇ (UCHAR_INT_ਸ਼ੁਰੂ <= prop && prop < UCHAR_INT_ਹੱਦਬੰਦੀ) {
        ਅਚਲ Unicode_ਸੈੱਟ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ਨਹੀਂਤਾਂ {
        ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

Unicode_ਸੈੱਟ&
Unicode_ਸੈੱਟ::applyPropertyAlias(ਅਚਲ UnicodeString& prop,
                               ਅਚਲ UnicodeString& value,
                               UError_ਕੋਡ& ec) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(ec) || isFrozen()) ਵਾਪਸ *ਇਹ;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ਜੇ( !uprv_isInvariantUString(prop.get_ਬਫਰ(), prop.length()) ||
        !uprv_isInvariantUString(value.get_ਬਫਰ(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ਜੇ (U_FAILURE(ec)) ਵਾਪਸ *ਇਹ;

    UProperty p;
    int32_t v;
    UBool invert = ਝੂਠ;

    ਜੇ (value.length() > 0) {
        p = u_getPropertyEnum(pname.ਅੰਕੜੇ());
        ਜੇ (p == UCHAR_INVALID_ਕੋਡ) FAIL(ec);

        // Treat gc as gcm
        ਜੇ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_ਨਕਾਬ;
        }

        ਜੇ ((p >= UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ && p < UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) ||
            (p >= UCHAR_INT_ਸ਼ੁਰੂ && p < UCHAR_INT_ਹੱਦਬੰਦੀ) ||
            (p >= UCHAR_ਨਕਾਬ_ਸ਼ੁਰੂ && p < UCHAR_ਨਕਾਬ_ਹੱਦਬੰਦੀ)) {
            v = u_get_Property_ਮੁੱਲ_Enum(p, vname.ਅੰਕੜੇ());
            ਜੇ (v == UCHAR_INVALID_ਕੋਡ) {
                // Handle numeric CCC
                ਜੇ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    ਅੱਖਰ* ਸਿਰਾ;
                    ਦੁੱਗਣਾ val = uprv_strtod(vname.ਅੰਕੜੇ(), &ਸਿਰਾ);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ਜੇ (*ਸਿਰਾ != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ਨਹੀਂਤਾਂ {
                    FAIL(ec);
                }
            }
        }

        ਨਹੀਂਤਾਂ {

            ਚੋਣ (p) {
            ਹਾਲਤ UCHAR_NUMERIC_ਮੁੱਲ:
                {
                    ਅੱਖਰ* ਸਿਰਾ;
                    ਦੁੱਗਣਾ val = uprv_strtod(vname.ਅੰਕੜੇ(), &ਸਿਰਾ);
                    ਜੇ (*ਸਿਰਾ != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_ਮੁੱਲ_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ਵਾਪਸ *ਇਹ;
                }
            ਹਾਲਤ UCHAR_NAME:
                {
                    ਅਚਲ UChar32 ch = getCharacterByName(vname);
                    ਜੇ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    ਜੋੜੋ(ch);
                    ਵਾਪਸ *ਇਹ;
                }
            ਹਾਲਤ UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            ਹਾਲਤ UCHAR_AGE:
                {
                    UVersionInfo ਸੰਸਕਰਣ;
                    u_ਸੰਸਕਰਣ_From_String(ਸੰਸਕਰਣ, vname.ਅੰਕੜੇ());
                    applyFilter(ਸੰਸਕਰਣ_Filter, &ਸੰਸਕਰਣ,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ਵਾਪਸ *ਇਹ;
                }
            ਹਾਲਤ UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_ਮੁੱਲ_Enum(UCHAR_SCRIPT, vname.ਅੰਕੜੇ());
                ਜੇ (v == UCHAR_INVALID_ਕੋਡ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ਰੋਕੋ;
            ਹਾਲਤ UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_ਮੁੱਲ_Enum(p, vname.ਅੰਕੜੇ());
                ਜੇ (v == UCHAR_INVALID_ਕੋਡ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ਰੋਕੋ;
            ਮੂਲ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ਨਹੀਂਤਾਂ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_ਨਕਾਬ;
        v = u_get_Property_ਮੁੱਲ_Enum(p, pname.ਅੰਕੜੇ());
        ਜੇ (v == UCHAR_INVALID_ਕੋਡ) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_ਮੁੱਲ_Enum(p, pname.ਅੰਕੜੇ());
            ਜੇ (v == UCHAR_INVALID_ਕੋਡ) {
                p = u_getPropertyEnum(pname.ਅੰਕੜੇ());
                ਜੇ (p >= UCHAR_ਦੁਆਧਾਰੀ_ਸ਼ੁਰੂ && p < UCHAR_ਦੁਆਧਾਰੀ_ਹੱਦਬੰਦੀ) {
                    v = 1;
                } ਨਹੀਂਤਾਂ ਜੇ (0 == uprv_compare_Property_ਨਾਂ(ANY, pname.ਅੰਕੜੇ())) {
                    ਸੈੱਟ(MIN_ਮੁੱਲ, MAX_ਮੁੱਲ);
                    ਵਾਪਸ *ਇਹ;
                } ਨਹੀਂਤਾਂ ਜੇ (0 == uprv_compare_Property_ਨਾਂ(ASCII, pname.ਅੰਕੜੇ())) {
                    ਸੈੱਟ(0, 0x7F);
                    ਵਾਪਸ *ਇਹ;
                } ਨਹੀਂਤਾਂ ਜੇ (0 == uprv_compare_Property_ਨਾਂ(ASSIGNED, pname.ਅੰਕੜੇ())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_ਨਕਾਬ;
                    v = U_GC_CN_ਨਕਾਬ;
                    invert = ਸੱਚ;
                } ਨਹੀਂਤਾਂ {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_ਮੁੱਲ(p, v, ec);
    ਜੇ(invert) {
        complement().remove_All_ਲੜੀਆਂ();  // code point complement
    }

    ਜੇ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ *ਇਹ;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool Unicode_ਸੈੱਟ::resembles_Property_ਨਮੂਨਾ(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ਜੇ ((pos+5) > ਨਮੂਨਾ.length()) {
        ਵਾਪਸ ਝੂਠ;
    }

    // Look for an opening [:, [:^, \p, or \P
    ਵਾਪਸ isPOSIXOpen(ਨਮੂਨਾ, pos) || isPerlOpen(ਨਮੂਨਾ, pos) || isNameOpen(ਨਮੂਨਾ, pos);
}

U_NAMESPACE_ਸਿਰਾ
