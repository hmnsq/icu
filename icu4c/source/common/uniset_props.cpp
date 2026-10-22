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

#ಅನ್ತರ್ಭಾವ <array>
#ಅನ್ತರ್ಭಾವ <optional>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/utfiterator.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"
#ಅನ್ತರ್ಭಾವ "ruleiter.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "util.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "propname.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "hash.h"

U_NAMESPACE_USE

ನಾಮಾಕಾಶ {

// Special property set IDs
constexpr ಅಕ್ಷರ ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr ಅಕ್ಷರ ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr ಅಕ್ಷರ ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV uset_cleanup();

ಸ್ಥಿರ Unicode_ಗಣ *uni32Singleton;
ಸ್ಥಿರ icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ಸ್ಥಿರ UBool U_CALLCONV uset_cleanup() {
    ಅಳಿಸು uni32Singleton;
    uni32Singleton = ಶೂನ್ಯಸೂಚಕ;
    uni32InitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CDECL_ಕೊನೆ

U_NAMESPACE_BEGIN

ಬಳಸಿ U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::utf_String_ಸಂಕೇತ_Points;

ನಾಮಾಕಾಶ {

// Cache some sets for other services -------------------------------------- ***
ಶೂನ್ಯ U_CALLCONV create_Uni32_ಗಣ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    U_ASSERT(uni32Singleton == ಶೂನ್ಯಸೂಚಕ);
    uni32Singleton = ಹೊಸ Unicode_ಗಣ(UnicodeString(u"[:age=3.2:]"), error_ಸಂಕೇತ);
    ಆದರೆ(uni32Singleton==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uni32Singleton->freeze();
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_ಗಣ *
uniset_getUnicode32Instance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_ಗಣ, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

ಸಾಲೊಳಗೆ UBool
isPerlOpen(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ, int32_t pos) {
    char16_t c;
    ಹಿಂತಿರುಗಿ ನಮೂನೆ.charAt(pos)==u'\\' && ((c=ನಮೂನೆ.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

ಸಾಲೊಳಗೆ UBool
isNameOpen(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ, int32_t pos) {
    ಹಿಂತಿರುಗಿ ನಮೂನೆ.charAt(pos)==u'\\' && ನಮೂನೆ.charAt(pos+1)==u'N';
}

ಸಾಲೊಳಗೆ UBool
isPOSIXOpen(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ, int32_t pos) {
    ಹಿಂತಿರುಗಿ ನಮೂನೆ.charAt(pos)==u'[' && ನಮೂನೆ.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#ವ್ಯಾಖ್ಯಾನಿಸು _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ಸ್ಥಿರಾಂಕ CharString& name) {
    ಗಾಗಿ (ಸ್ಥಿರಾಂಕ UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.ದತ್ತಾಂಶ(), &ec);
        ಆದರೆ (U_SUCCESS(ec)) {
            ಹಿಂತಿರುಗಿ ch;
        }
    }
    ಹಿಂತಿರುಗಿ U_SENTINEL;
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
Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    apply_ನಮೂನೆ(ನಮೂನೆ, ಸ್ಥಿತಿ_2);
    _dbgct(ಇದು);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_ಗಣ& Unicode_ಗಣ::apply_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_ನಮೂನೆ_Ignore_Space(ನಮೂನೆ, pos, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ *ಇದು;

    int32_t i = pos.get_ಸೂಚ್ಯಂಕ();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(ನಮೂನೆ, i, ನಿಜ);
    ಆದರೆ (i != ನಮೂನೆ.length()) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ
Unicode_ಗಣ::apply_ನಮೂನೆ_Ignore_Space(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                    ParsePosition& pos,
                                    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ* symbols,
                                    UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (isFrozen()) {
        ಸ್ಥಿತಿ_2 = U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    ನಿಯಮ_Character_Iterator chars(ನಮೂನೆ, symbols, pos);
    apply_ನಮೂನೆ(ನಮೂನೆ, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    ಆದರೆ (chars.ಒಳ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        ಸ್ಥಿತಿ_2 = U_MALFORMED_ಗಣ;
        ಹಿಂತಿರುಗಿ;
    }
    ಗಣ_ನಮೂನೆ(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_ಗಣ::resembles_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ, int32_t pos) {
    ಹಿಂತಿರುಗಿ ((pos+1) < ನಮೂನೆ.length() &&
            ನಮೂನೆ.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_ನಮೂನೆ(ನಮೂನೆ, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು U_DEBUGGING_UNICODESET_PARSING 0

ವರ್ಗ Unicode_ಗಣ::Lexer {
  ಸಾರ್ವಜನಿಕ:
    Lexer(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ,
          ಸ್ಥಿರಾಂಕ ParsePosition &parsePosition,
          ನಿಯಮ_Character_Iterator &chars,
          uint32_t unicode_ಗಣ_Options,
          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ *ಸ್ಥಿರಾಂಕ symbols,
          Unicode_ಗಣ &(Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute))
        : ನಮೂನೆ_2(ನಮೂನೆ), parsePosition_(parsePosition), chars_(chars),
          unicode_ಗಣ_Options_2(unicode_ಗಣ_Options),
          charsOptions_(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_ಗಣ_Options & USET_IGNORE_SPACE) != 0
                             ? ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_ಆವರಣ_2(case_ಆವರಣ) {}

    ವರ್ಗ LexicalElement {
      ಸಾರ್ವಜನಿಕ:
        ತಾರ್ಕಿಕ is_ಗಣ_Operator(ಸ್ಥಿರಾಂಕ char16_t op) ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ_2) && category_ == ಗಣ_OPERATOR && string_[0] == op;
        }

        ತಾರ್ಕಿಕ isStringLiteral() ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ_2) && category_ == STRING_LITERAL;
        }

        ತಾರ್ಕಿಕ isNamedElement() ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ_2) && category_ == NAMED_ELEMENT;
        }

        ತಾರ್ಕಿಕ isBracketedElement() ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ಸ್ಥಿರಾಂಕ {
            ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ಹಿಂತಿರುಗಿ string_;
            }
            ಹಿಂತಿರುಗಿ std::nullopt;
        }

        std::optional<UChar32> ಸಂಕೇತ_ಬಿಂದು() ಸ್ಥಿರಾಂಕ {
            ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ಹಿಂತಿರುಗಿ string_.char32At(0);
            }
            ಹಿಂತಿರುಗಿ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *ಗಣ() ಸ್ಥಿರಾಂಕ {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ಆದರೆ (precomputed_ಗಣ != ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ precomputed_ಗಣ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ &ಗಣ_2;
                }
            }
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }

        ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ& error_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ{
          ಹಿಂತಿರುಗಿ error_ಸಂಕೇತ_2;
        }

#ಆದರೆ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ಸ್ಥಿರಾಂಕ {
            UnicodeString ಫಲಿತಾಂಶ;
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) {
                ಫಲಿತಾಂಶ.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_ಸಂಕೇತ_2)))
                    .append(u"), possibly ");
            }
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(category_ಹೆಸರುಗಳು[category_])
                .append(u" '")
                .append(ಮೂಲ_ಪಠ್ಯ)
                .append(u"'");
        }
#ಆದರೆ_ಅಂತ್ಯ

      ಖಾಸಗಿ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ಎಣಿಕೆ Category : std::uint8_t {
            ಗಣ_OPERATOR,
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
            ಕೊನೆ_ದ_ಪಠ್ಯ,
        };
        ಸ್ಥಿರ constexpr std::array<std::u16string_view, ಕೊನೆ_ದ_ಪಠ್ಯ + 1> category_ಹೆಸರುಗಳು{{
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
        LexicalElement(Category category, UnicodeString string, ನಿಯಮ_Character_Iterator::Pos after,
                       ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ error_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *precomputed_ಗಣ_2, Unicode_ಗಣ ಗಣ,
                       std::u16string_view ಮೂಲ_ಪಠ್ಯ_2, ತಾರ್ಕಿಕ separated,
                       std::optional<LexicalElement> ಸ್ಥಿರಾಂಕ &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_ಸಂಕೇತ_2(error_ಸಂಕೇತ),
              precomputed_ಗಣ(precomputed_ಗಣ_2), ಗಣ_2(ಗಣ), ಮೂಲ_ಪಠ್ಯ(ಮೂಲ_ಪಠ್ಯ_2) {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2) || separated || !preceding.has_value()) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ (preceding->is_ಗಣ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *ಸಂಕೇತ_ಬಿಂದು() == u':') {
                error_ಸಂಕೇತ_2 = U_MALFORMED_ಗಣ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (preceding->category_ == ESCAPED_ELEMENT) {
                ಆದರೆ (U_IS_LEAD(*preceding->ಸಂಕೇತ_ಬಿಂದು()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*ಸಂಕೇತ_ಬಿಂದು())) {
                    error_ಸಂಕೇತ_2 = U_MALFORMED_ಗಣ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (preceding->ಮೂಲ_ಪಠ್ಯ.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *ಸಂಕೇತ_ಬಿಂದು();
                    ಆದರೆ (preceding->ಮೂಲ_ಪಠ್ಯ[1] == u'x') {
                        ಆದರೆ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_ಸಂಕೇತ_2 = U_MALFORMED_ಗಣ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (preceding->ಮೂಲ_ಪಠ್ಯ[1] >= u'0' &&
                               preceding->ಮೂಲ_ಪಠ್ಯ[1] <= u'7') {
                        ಆದರೆ (cp >= u'0' && cp <= u'7') {
                            error_ಸಂಕೇತ_2 = U_MALFORMED_ಗಣ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        ನಿಯಮ_Character_Iterator::Pos after_;
        UError_ಸಂಕೇತ error_ಸಂಕೇತ_2;
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *precomputed_ಗಣ;
        Unicode_ಗಣ ಗಣ_2;
        std::u16string_view ಮೂಲ_ಪಠ್ಯ;

        ಮಿತ್ರ ವರ್ಗ Lexer;
    };

    UnicodeString getPositionForDebugging() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ನಮೂನೆ_2.ತಾತ್ಕಾಲಿಕ_Sub_String(0, parsePosition_.get_ಸೂಚ್ಯಂಕ()) + u"☞" +
               ನಮೂನೆ_2.ತಾತ್ಕಾಲಿಕ_Sub_String(parsePosition_.get_ಸೂಚ್ಯಂಕ(), 60);
    }

    ತಾರ್ಕಿಕ accept_ಗಣ_Operator(char16_t op) {
        ಆದರೆ (lookahead().is_ಗಣ_Operator(op)) {
            advance();
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಸ್ಥಿರಾಂಕ LexicalElement &lookahead() {
        ಆದರೆ (!ahead_.has_value()) {
            ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos before = getPos();
            ahead_.emplace(ಮುಂದಿನ_Token(behind_));
            chars_.ಗಣ_Pos(before);
        }
        ಹಿಂತಿರುಗಿ *ahead_;
    }

    ಸ್ಥಿರಾಂಕ LexicalElement &lookahead2() {
        ಆದರೆ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos before = getPos();
            chars_.ಗಣ_Pos(lookahead().after_);
            ahead2_.emplace(ಮುಂದಿನ_Token(ahead_));
            chars_.ಗಣ_Pos(before);
        }
        ಹಿಂತಿರುಗಿ *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    ನಿಯಮ_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        ಹಿಂತಿರುಗಿ chars_;
    }

    int32_t charsOptions() {
        ಹಿಂತಿರುಗಿ charsOptions_;
    }

    ತಾರ್ಕಿಕ at_ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ chars_.at_ಕೊನೆ();
    }

    ಶೂನ್ಯ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.ಗಣ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ಖಾಸಗಿ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    ನಿಯಮ_Character_Iterator::Pos getPos() ಸ್ಥಿರಾಂಕ {
        ನಿಯಮ_Character_Iterator::Pos ಫಲಿತಾಂಶ;
        chars_.getPos(ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    LexicalElement ಮುಂದಿನ_Token(std::optional<LexicalElement> ಸ್ಥಿರಾಂಕ& preceding) {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
        ತಾರ್ಕಿಕ separated = ಸುಳ್ಳು;
        ಆದರೆ (charsOptions_ & ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            ನಿಯಮ_Character_Iterator::Pos pos;
            ಗಾಗಿ (;;) {
                UBool ಬಳಸದ_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                  ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
                ಆದರೆ (!ನಮೂನೆ_Props::isWhiteSpace(s)) {
                    chars_.ಗಣ_Pos(pos);
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (s != u'\u200E' && s != u'\u200F') {
                    separated = ನಿಜ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ಆದರೆ (chars_.at_ಕೊನೆ()) {
            ಹಿಂತಿರುಗಿ LexicalElement(LexicalElement::ಕೊನೆ_ದ_ಪಠ್ಯ, {}, getPos(), error_ಸಂಕೇತ,
                                  /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ಸ್ಥಿರಾಂಕ int32_t ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
        ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool ಬಳಸದ_Escaped;
        ಸ್ಥಿರಾಂಕ UChar32 first =
            chars_.next(charsOptions_ & ~ನಿಯಮ_Character_Iterator::PARSE_ESCAPES, ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
        ಆದರೆ (first == u'[' || first == u'\\') {
            ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos after_ಮೊದಲ = getPos();
            // This could be a property-query or named-element.
            ಸ್ಥಿರಾಂಕ UChar32 ಎರಡನೇ = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                                 ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                               ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
            ಆದರೆ ((first == u'[' && ಎರಡನೇ == u':') ||
                (first == u'\\' && (ಎರಡನೇ == u'p' || ಎರಡನೇ == u'P' || ಎರಡನೇ == u'N'))) {
                ಆದರೆ (ಎರಡನೇ == u'N') {
                    UChar32 ಸ್ಥಿರಾಂಕ query_ಫಲಿತಾಂಶ = ಪರಿಶೀಲನೆ_Named_Element_Brackets(error_ಸಂಕೇತ);
                    ಹಿಂತಿರುಗಿ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_ಫಲಿತಾಂಶ), getPos(), error_ಸಂಕೇತ,
                        /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                        /*set=*/{},
                        std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                        separated, preceding);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    Unicode_ಗಣ query_ಫಲಿತಾಂಶ = ಪರಿಶೀಲನೆ_Property_Query_After_ಆರಂಭ(first, ಎರಡನೇ, ಆರಂಭ, error_ಸಂಕೇತ);
                    ಹಿಂತಿರುಗಿ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_ಸಂಕೇತ,
                        /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ, /*set=*/std::move(query_ಫಲಿತಾಂಶ),
                        std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.ಗಣ_Pos(after_ಮೊದಲ);
        }
        ಆದರೆ (first == u'$' && symbols_ != ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ವಯಂಚಾಲಿತ name_ಕೊನೆ = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ಆದರೆ (UnicodeString name = symbols_->parseReference(ನಮೂನೆ_2, name_ಕೊನೆ, ನಮೂನೆ_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_ಖಾಲಿಗಳು()) {
                chars_.jumpahead(name_ಕೊನೆ.get_ಸೂಚ್ಯಂಕ() - (ಆರಂಭ + 1));
                ಸ್ಥಿರಾಂಕ std::u16string_view ಮೂಲ =
                    std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ);
                ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *precomputed_ಗಣ_2 = symbols_->lookup_ಗಣ(name);
                ಆದರೆ (precomputed_ಗಣ_2 != ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_ಗಣ_2, /*set=*/{}, ಮೂಲ, /*separated=*/ನಿಜ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ಸ್ಥಿರಾಂಕ UnicodeString *ಸ್ಥಿರಾಂಕ expression = symbols_->lookup(name);
                ಆದರೆ (expression == ಶೂನ್ಯಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                        /*set=*/{},
                        ಮೂಲ,
                        /*separated=*/ನಿಜ,
                        /*preceding=*/std::nullopt);
                }
                ಹಿಂತಿರುಗಿ evaluateVariable(*expression, ಮೂಲ);
            }
        }
        ಆಯ್ಕೆ (first) {
        ಸ್ಥಿತಿ u'[':
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::ಗಣ_OPERATOR, UnicodeString(u'['), getPos(), error_ಸಂಕೇತ,
                /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                /*set=*/{},
                std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                separated, preceding);
        ಸ್ಥಿತಿ u'\\': {
            // Now try to parse the escape.
            chars_.ಗಣ_Pos(before);
            UChar32 ಸಂಕೇತ_ಬಿಂದು = chars_.next(charsOptions_, ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(ಸಂಕೇತ_ಬಿಂದು), getPos(), error_ಸಂಕೇತ,
                ಶೂನ್ಯಸೂಚಕ,
                /*set=*/{},
                std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                separated, preceding);
        }
        ಸ್ಥಿತಿ u'&':
        ಸ್ಥಿತಿ u'-':
        ಸ್ಥಿತಿ u']':
        ಸ್ಥಿತಿ u'^':
        ಸ್ಥಿತಿ u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::ಗಣ_OPERATOR, UnicodeString(first), getPos(), error_ಸಂಕೇತ,
                /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                /*set=*/{},
                std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                separated, preceding);
        ಸ್ಥಿತಿ u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t ಸಂಕೇತ_ಬಿಂದು_ಎಣಿಕೆ = 0;
            ಆಗ (!chars_.at_ಕೊನೆ() && U_SUCCESS(error_ಸಂಕೇತ)) {
                ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos before_ಮುಂದಿನ = getPos();
                next = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                     ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                   ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
                ಆದರೆ (next == u'\\') {
                    ಸ್ಥಿರಾಂಕ UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                      ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                    ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
                    ಆದರೆ (afterBackslash == u'N') {
                        next = ಪರಿಶೀಲನೆ_Named_Element_Brackets(error_ಸಂಕೇತ);
                        escaped = ನಿಜ;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (afterBackslash == u'p' || afterBackslash == u'P') {
                        ಹಿಂತಿರುಗಿ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_ಗಣ,
                                              /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                                              /*set=*/{},
                                              std::u16string_view(ನಮೂನೆ_2).substr(
                                                  ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                                              separated, preceding);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        chars_.ಗಣ_Pos(before_ಮುಂದಿನ);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_ಸಂಕೇತ);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ U_ICU_ಆವೃತ್ತಿ_MAJOR_ಅಂಕೆ < 81
                    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ) && ನಮೂನೆ_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_ಸಂಕೇತ = UError_ಸಂಕೇತ::U_ILLEGAL_ವಾದ_ERROR;
                    }
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ Remove ಇದು transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#ಆದರೆ_ಅಂತ್ಯ
                    escaped = ಸುಳ್ಳು;
                }
                ಆದರೆ (!escaped && next == u'}') {
                    ಹಿಂತಿರುಗಿ LexicalElement(
                        ಸಂಕೇತ_ಬಿಂದು_ಎಣಿಕೆ == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_ಸಂಕೇತ,
                        /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                        /*set=*/{},
                        std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                        separated, preceding);
                }
                string.append(next);
                ಸಂಕೇತ_ಬಿಂದು_ಎಣಿಕೆ += 1;
            }
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_ಗಣ,
                /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                /*set=*/{},
                std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                separated, preceding);
        }
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_ಸಂಕೇತ, ಶೂನ್ಯಸೂಚಕ,
                /*set=*/{},
                std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - ಆರಂಭ),
                separated, preceding);
        }
    }

    UChar32 ಪರಿಶೀಲನೆ_Named_Element_Brackets(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        UBool ಬಳಸದ_Escaped;
        ಸ್ಥಿರಾಂಕ UChar32 open = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                           ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                         ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
        ಆದರೆ (open == u'{') {
            int32_t ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ಆಗ (!chars_.at_ಕೊನೆ() && U_SUCCESS(error_ಸಂಕೇತ)) {
                UChar32 ಕೊನೆಯ = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                             ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                           ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
                ಆದರೆ (ಕೊನೆಯ == u':') {
                    ಆದರೆ (!hex.has_value()) {
                        hex.emplace();
                        ಆದರೆ (ಆರಂಭ == parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1) {
                            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                            ಹಿಂತಿರುಗಿ {};
                        }
                        ಗಾಗಿ (char16_t digit : std::u16string_view(ನಮೂನೆ_2).substr(
                                 ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1 - ಆರಂಭ)) {
                            uint8_t nibble;
                            ಆದರೆ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                digit = digit & ~0x20;
                                ಆದರೆ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ಇಲ್ಲದಿದ್ದರೆ {
                                    error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                                    ಹಿಂತಿರುಗಿ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ಆದರೆ (hex > 0x10FFFF) {
                                error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                                ಹಿಂತಿರುಗಿ {};
                            }
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!literal.has_value()) {
                        ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ literal_ಸಂಕೇತ_Points = utf_String_ಸಂಕೇತ_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(ನಮೂನೆ_2).substr(ಆರಂಭ,
                                                                 parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1 - ಆರಂಭ));
                        ಸ್ವಯಂಚಾಲಿತ it = literal_ಸಂಕೇತ_Points.begin();
                        ಆದರೆ (it == literal_ಸಂಕೇತ_Points.ಕೊನೆ() || !it->wellFormed() ||
                                (literal = it->ಸಂಕೇತ_ಬಿಂದು(), ++it) != literal_ಸಂಕೇತ_Points.ಕೊನೆ()) {
                            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                            ಹಿಂತಿರುಗಿ {};
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                        ಹಿಂತಿರುಗಿ {};
                    }
                    ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆಯ == u'}') {
                    ಸ್ಥಿರಾಂಕ std::u16string_view u16name = std::u16string_view(ನಮೂನೆ_2).substr(
                        ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1 - ಆರಂಭ);
                    ಸ್ಥಿರಾಂಕ UChar32 ಫಲಿತಾಂಶ = getCharacterByName(CharString().appendInvariantChars(
                        u16name.ದತ್ತಾಂಶ(), static_cast<int32_t>(u16name.length()), error_ಸಂಕೇತ));
                    ಆದರೆ (!U_SUCCESS(error_ಸಂಕೇತ)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                        ಹಿಂತಿರುಗಿ {};
                    }
                    ಆದರೆ (ಫಲಿತಾಂಶ < 0 || (hex.has_value() && ಫಲಿತಾಂಶ != hex) ||
                            (literal.has_value() && ಫಲಿತಾಂಶ != literal)) {
                        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                        ಹಿಂತಿರುಗಿ {};
                    }
                    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                }
            }
        }
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        }
        ಹಿಂತಿರುಗಿ {};
    }

    LexicalElement evaluateVariable(ಸ್ಥಿರಾಂಕ UnicodeString &expression,
                                    ಸ್ಥಿರಾಂಕ std::u16string_view ಮೂಲ) {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
        ParsePosition ಅಭಿವ್ಯಕ್ತಿ_Position;
        ನಿಯಮ_Character_Iterator ಅಭಿವ್ಯಕ್ತಿ_Iterator(expression, symbols_, ಅಭಿವ್ಯಕ್ತಿ_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer ಅಭಿವ್ಯಕ್ತಿ_Lexer(expression, ಅಭಿವ್ಯಕ್ತಿ_Position, ಅಭಿವ್ಯಕ್ತಿ_Iterator, unicode_ಗಣ_Options_2,
                              /*symbols=*/ಶೂನ್ಯಸೂಚಕ, case_ಆವರಣ_2);
        ಸ್ವಯಂಚಾಲಿತ variableToken = ಅಭಿವ್ಯಕ್ತಿ_Lexer.lookahead();
        ಆದರೆ (variableToken.is_ಗಣ_Operator(u'[')) {
            UnicodeString rebuilt_ನಮೂನೆ;
            Unicode_ಗಣ ಅಭಿವ್ಯಕ್ತಿ_ಮೌಲ್ಯ =
                parse_Unicode_ಗಣ(ಅಭಿವ್ಯಕ್ತಿ_Lexer, rebuilt_ನಮೂನೆ, unicode_ಗಣ_Options_2, case_ಆವರಣ_2,
                                /*depth=*/0, error_ಸಂಕೇತ);
            ಅಭಿವ್ಯಕ್ತಿ_ಮೌಲ್ಯ.ಗಣ_ನಮೂನೆ(rebuilt_ನಮೂನೆ);
            ಆದರೆ (!ಅಭಿವ್ಯಕ್ತಿ_Lexer.at_ಕೊನೆ()) {
                ಹಿಂತಿರುಗಿ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                    /*set=*/{},
                    ಮೂಲ,
                    /*separated=*/ನಿಜ,
                    /*preceding=*/std::nullopt);
            }
            ಹಿಂತಿರುಗಿ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_ಸಂಕೇತ,
                /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                /*set=*/std::move(ಅಭಿವ್ಯಕ್ತಿ_ಮೌಲ್ಯ),
                ಮೂಲ,
                /*separated=*/ನಿಜ,
                /*preceding=*/std::nullopt);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಅಭಿವ್ಯಕ್ತಿ_Lexer.advance();
            ಆದರೆ (!ಅಭಿವ್ಯಕ್ತಿ_Lexer.at_ಕೊನೆ()) {
                ಹಿಂತಿರುಗಿ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                    /*set=*/{},
                    ಮೂಲ,
                    /*separated=*/ನಿಜ,
                    /*preceding=*/std::nullopt);
            }
            ಆಯ್ಕೆ (variableToken.category_) {
            ಸ್ಥಿತಿ LexicalElement::LITERAL_ELEMENT:
            ಸ್ಥಿತಿ LexicalElement::ESCAPED_ELEMENT:
            ಸ್ಥಿತಿ LexicalElement::NAMED_ELEMENT:
            ಸ್ಥಿತಿ LexicalElement::BRACKETED_ELEMENT:
            ಸ್ಥಿತಿ LexicalElement::STRING_LITERAL:
            ಸ್ಥಿತಿ LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ಹಿಂತಿರುಗಿ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_ಸಂಕೇತ_2, variableToken.precomputed_ಗಣ, std::move(variableToken.ಗಣ_2), ಮೂಲ,
                    /*separated=*/ನಿಜ,
                    /*preceding=*/std::nullopt);
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/ಶೂನ್ಯಸೂಚಕ,
                                      /*set=*/{}, ಮೂಲ,
                                      /*separated=*/ನಿಜ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_ಗಣ ಪರಿಶೀಲನೆ_Property_Query_After_ಆರಂಭ(UChar32 first, UChar32 ಎರಡನೇ, int32_t query_ಆರಂಭ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
        ತಾರ್ಕಿಕ exteriorlyNegated = ಸುಳ್ಳು;
        ತಾರ್ಕಿಕ interiorlyNegated = ಸುಳ್ಳು;
        UBool ಬಳಸದ_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ಸ್ಥಿರಾಂಕ UChar32 ಮೂರನೇ = chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                                            ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                                          ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
        ಆದರೆ (first == u'\\') {
            ಆದರೆ (ಮೂರನೇ != u'{') {
                error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ {};
            }
            exteriorlyNegated = ಎರಡನೇ == u'P';
            query_ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಮೂರನೇ == u'^') {
                exteriorlyNegated = ನಿಜ;
                query_ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = parsePosition_.get_ಸೂಚ್ಯಂಕ();
            }
        }
        ನಿಯಮ_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                          ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                        ಬಳಸದ_Escaped, error_ಸಂಕೇತ);

        ಆಗ (!chars_.at_ಕೊನೆ() && U_SUCCESS(error_ಸಂಕೇತ)) {
            ಸ್ಥಿರಾಂಕ ನಿಯಮ_Character_Iterator::Pos before_ಕೊನೆಯ = getPos();
            UChar32 ಕೊನೆಯ_Unescaped =
                chars_.next(charsOptions_ & ~(ನಿಯಮ_Character_Iterator::PARSE_ESCAPES |
                                              ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE),
                            ಬಳಸದ_Escaped, error_ಸಂಕೇತ);
            ಆದರೆ (penultimateUnescaped == u'\\') {
                ಆದರೆ (ಕೊನೆಯ_Unescaped == 'N') {
                    ಪರಿಶೀಲನೆ_Named_Element_Brackets(error_ಸಂಕೇತ);
                    ಆದರೆ (!U_SUCCESS(error_ಸಂಕೇತ)) {
                        ಹಿಂತಿರುಗಿ {};
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.ಗಣ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~ನಿಯಮ_Character_Iterator::SKIP_WHITESPACE, ಬಳಸದ_Escaped,
                                error_ಸಂಕೇತ);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                ಕೊನೆಯ_Unescaped = -1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!queryOperatorPosition.has_value() && ಕೊನೆಯ_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!queryOperatorPosition.has_value() && ಕೊನೆಯ_Unescaped == u'≠') {
                ಆದರೆ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                    ಹಿಂತಿರುಗಿ {};
                }
                interiorlyNegated = ನಿಜ;
                queryOperatorPosition = parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((first == u'[' && penultimateUnescaped == u':' && ಕೊನೆಯ_Unescaped == u']') ||
                       (first == u'\\' && ಕೊನೆಯ_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ಸ್ಥಿರಾಂಕ int32_t query_ಅಭಿವ್ಯಕ್ತಿ_ಎಲ್ಲೆ =
                    first == u'[' ? parsePosition_.get_ಸೂಚ್ಯಂಕ() - 2 : parsePosition_.get_ಸೂಚ್ಯಂಕ() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ಆದರೆ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        ನಮೂನೆ_2.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(*queryOperatorPosition + 1, query_ಅಭಿವ್ಯಕ್ತಿ_ಎಲ್ಲೆ);
                    ಆದರೆ (propertyPredicate.is_ಖಾಲಿಗಳು()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                        ಹಿಂತಿರುಗಿ {};
                    }
                }
                Unicode_ಗಣ ಫಲಿತಾಂಶ;
                ಫಲಿತಾಂಶ.applyPropertyAlias(
                    ನಮೂನೆ_2.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(query_ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ,
                                                  queryOperatorPosition.value_or(query_ಅಭಿವ್ಯಕ್ತಿ_ಎಲ್ಲೆ)),
                    propertyPredicate, error_ಸಂಕೇತ);
                ಆದರೆ (exteriorlyNegated != interiorlyNegated) {
                    ಫಲಿತಾಂಶ.complement().remove_All_ಅಕ್ಷರಸರಣಿಗಳು();
                }
                ಫಲಿತಾಂಶ.ಗಣ_ನಮೂನೆ(ನಮೂನೆ_2.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(query_ಆರಂಭ, parsePosition_.get_ಸೂಚ್ಯಂಕ()));
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            beforePenultimate = before_ಕೊನೆಯ;
            penultimateUnescaped = ಕೊನೆಯ_Unescaped;
        }
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ {};
    }

    ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ_2;
    ಸ್ಥಿರಾಂಕ ParsePosition &parsePosition_;
    ನಿಯಮ_Character_Iterator &chars_;
    ಸ್ಥಿರಾಂಕ uint32_t unicode_ಗಣ_Options_2;
    ಸ್ಥಿರಾಂಕ int32_t charsOptions_;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ *ಸ್ಥಿರಾಂಕ symbols_;
    Unicode_ಗಣ &(Unicode_ಗಣ::* ಸ್ಥಿರಾಂಕ case_ಆವರಣ_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_ಆಳ = 100;

#ಆದರೆ U_DEBUGGING_UNICODESET_PARSING

#ವ್ಯಾಖ್ಯಾನಿಸು U_UNICODESET_RETURN_IF_ERROR(ಫಲಿತಾಂಶ, ec)                                                   \
    ಮಾಡು {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ಆದರೆ (U_FAILURE(ec)) {                                                                       \
            ಆದರೆ (ಆಳ < 5) {                                                                       \
                ಬರೆ("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆಳ == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                ಬರೆ("--- [...]\n");                                                             \
            }                                                                                      \
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;                                                                         \
        }                                                                                          \
    } ಆಗ (ಸುಳ್ಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು U_UNICODESET_RETURN_WITH_PARSE_ERROR(ಫಲಿತಾಂಶ, expected, actual, lexer, ec)                  \
    ಮಾಡು {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        ಬರೆ("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).ಗಮ್ಯ_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().ಗಮ್ಯ_UTF8_String(contextUTF8).c_str());                 \
        ಬರೆ("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ಆದರೆ (U_FAILURE(lexer.lookahead().error_ಸಂಕೇತ())) {                                            \
            (ec) = lexer.lookahead().error_ಸಂಕೇತ();                                                  \
        } ಇಲ್ಲದಿದ್ದರೆ {                                                                                   \
            (ec) = U_MALFORMED_ಗಣ;                                                                \
        }                                                                                          \
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;                                                                             \
    } ಆಗ (ಸುಳ್ಳು)

#ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನಿಸು U_UNICODESET_RETURN_IF_ERROR(ಫಲಿತಾಂಶ, ec)                                                   \
    ಮಾಡು {                                                                                           \
        ಆದರೆ (U_FAILURE(ec)) {                                                                       \
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;                                                                         \
        }                                                                                          \
    } ಆಗ (ಸುಳ್ಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು U_UNICODESET_RETURN_WITH_PARSE_ERROR(ಫಲಿತಾಂಶ, expected, actual, lexer, ec)                  \
    ಮಾಡು {                                                                                           \
        ಆದರೆ (U_FAILURE(lexer.lookahead().error_ಸಂಕೇತ())) {                                            \
            (ec) = lexer.lookahead().error_ಸಂಕೇತ();                                                  \
        } ಇಲ್ಲದಿದ್ದರೆ {                                                                                   \
            (ec) = U_MALFORMED_ಗಣ;                                                                \
        }                                                                                          \
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;                                                                             \
    } ಆಗ (ಸುಳ್ಳು)

#ಆದರೆ_ಅಂತ್ಯ

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

ಶೂನ್ಯ Unicode_ಗಣ::apply_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ,
                              ಸ್ಥಿರಾಂಕ ParsePosition &parsePosition,
                              ನಿಯಮ_Character_Iterator &chars,
                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_ಗಣ &(Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute),
                              UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (U_FAILURE(ec)) ಹಿಂತಿರುಗಿ;
    Lexer lexer(ನಮೂನೆ, parsePosition, chars, options, symbols, case_ಆವರಣ);
    *ಇದು = parse_Unicode_ಗಣ(lexer, rebuiltPat, options, case_ಆವರಣ, /*depth=*/0, ec);
}

Unicode_ಗಣ Unicode_ಗಣ::parse_Unicode_ಗಣ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_ಗಣ& (Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute),
                                       int32_t ಆಳ,
                                       UError_ಸಂಕೇತ &ec) {
    Unicode_ಗಣ ಫಲಿತಾಂಶ;

    ಆದರೆ (ಆಳ > MAX_ಆಳ) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_ಆಳ)).c_str(),
                                             ("depth = " + std::to_string(ಆಳ)).c_str(), lexer,
                                             ec);
    }

    ತಾರ್ಕಿಕ isComplement = ಸುಳ್ಳು;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    ತಾರ್ಕಿಕ preserve_Syntax_ಒಳ_ನಮೂನೆ = ಸುಳ್ಳು;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_ನಮೂನೆ;
    ಆದರೆ (lexer.lookahead().ಗಣ() != ಶೂನ್ಯಸೂಚಕ) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        ಫಲಿತಾಂಶ = *lexer.lookahead().ಗಣ();
        ಫಲಿತಾಂಶ.ಗಮ್ಯ_ನಮೂನೆ_2(pretty_Printed_ನಮೂನೆ, /*escapeUnprintable=*/ಸುಳ್ಳು);
        lexer.advance();
        preserve_Syntax_ಒಳ_ನಮೂನೆ = ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ಆದರೆ (lexer.accept_ಗಣ_Operator(u'[')) {
            pretty_Printed_ನಮೂನೆ.append(u'[');
            ಆದರೆ (lexer.accept_ಗಣ_Operator(u'^')) {
                pretty_Printed_ನಮೂನೆ.append(u'^');
                isComplement = ನಿಜ;
            }
            ಫಲಿತಾಂಶ = parseContent(lexer, pretty_Printed_ನಮೂನೆ, options, case_ಆವರಣ, ಆಳ,
                                  /*containsSetOperation=*/preserve_Syntax_ಒಳ_ನಮೂನೆ, ec);
            U_UNICODESET_RETURN_IF_ERROR(ಫಲಿತಾಂಶ, ec);
            ಆದರೆ (!lexer.accept_ಗಣ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(ಫಲಿತಾಂಶ, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_ನಮೂನೆ.append(u']');
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(ಫಲಿತಾಂಶ, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    ಆದರೆ ((options & USET_CASE_ಮುಖವಾಡ) != 0) {
        (ಫಲಿತಾಂಶ.*case_ಆವರಣ)(options);
    }
    ಆದರೆ (isComplement) {
        ಫಲಿತಾಂಶ.complement().remove_All_ಅಕ್ಷರಸರಣಿಗಳು(); // code point complement
    }
    ಆದರೆ (preserve_Syntax_ಒಳ_ನಮೂನೆ) {
        rebuiltPat.append(pretty_Printed_ನಮೂನೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.generate_ನಮೂನೆ(rebuiltPat, /*escapeUnprintable=*/ಸುಳ್ಳು);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

Unicode_ಗಣ Unicode_ಗಣ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_ಗಣ &(Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute),
                                    int32_t ಆಳ,
                                    ತಾರ್ಕಿಕ &ಒಳಗೊಂಡಿದೆ_ಗಣ_Operation,
                                    UError_ಸಂಕೇತ &ec) {
    Unicode_ಗಣ ಫಲಿತಾಂಶ;
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
    ಆದರೆ (lexer.accept_ಗಣ_Operator(u'-')) {
        ಫಲಿತಾಂಶ.ಕೂಡಿಸು(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ಆಗ (!lexer.at_ಕೊನೆ()) {
        ಆದರೆ (lexer.accept_ಗಣ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            ಫಲಿತಾಂಶ.ಕೂಡಿಸು(u'-');
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator(u'$')) {
            ಆದರೆ (lexer.lookahead2().is_ಗಣ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                ಫಲಿತಾಂಶ.ಕೂಡಿಸು(U_ETHER);
                ಒಳಗೊಂಡಿದೆ_ಗಣ_Operation = ನಿಜ;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        }
        ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator(u']')) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        // Also handles FinalMutation.
        ಫಲಿತಾಂಶ.parseMutation(lexer, rebuiltPat, options, case_ಆವರಣ, ಆಳ, ಒಳಗೊಂಡಿದೆ_ಗಣ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(ಫಲಿತಾಂಶ, ec);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Unicode_ಗಣ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_ಗಣ &(Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute),
                               int32_t ಆಳ,
                               ತಾರ್ಕಿಕ &ಒಳಗೊಂಡಿದೆ_ಗಣ_Operation,
                               UError_ಸಂಕೇತ &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator('[') || lexer.lookahead().ಗಣ() != ಶೂನ್ಯಸೂಚಕ) {
        ಒಳಗೊಂಡಿದೆ_ಗಣ_Operation = ನಿಜ;
        parse_ಗಣ_Operations(lexer, rebuiltPat, options, case_ಆವರಣ, ಆಳ, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Also handles DollarElements.
        ಕೂಡಿಸು_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

ಶೂನ್ಯ Unicode_ಗಣ::parse_ಗಣ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_ಗಣ &(Unicode_ಗಣ::*case_ಆವರಣ)(int32_t attribute),
                                    int32_t ಆಳ,
                                    UError_ಸಂಕೇತ &ec) {
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
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ ಎಡ_Hand_Side =
        parse_Unicode_ಗಣ(lexer, rebuiltPat, options, case_ಆವರಣ, ಆಳ + 1, ec);
    ಕೂಡಿಸು_All(ಎಡ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    ಗಾಗಿ (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ಆದರೆ (lexer.accept_ಗಣ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ಸ್ಥಿರಾಂಕ Unicode_ಗಣ ಬಲ_Hand_Side =
                parse_Unicode_ಗಣ(lexer, rebuiltPat, options, case_ಆವರಣ, ಆಳ + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ಬಲ_Hand_Side);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ಆದರೆ (lexer.lookahead2().is_ಗಣ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ಹಿಂತಿರುಗಿ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ಸ್ಥಿರಾಂಕ Unicode_ಗಣ ಬಲ_Hand_Side =
                parse_Unicode_ಗಣ(lexer, rebuiltPat, options, case_ಆವರಣ, ಆಳ + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ಬಲ_Hand_Side);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ಹಿಂತಿರುಗಿ;
        }
    }
}

Unicode_ಗಣ Unicode_ಗಣ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_ಸಂಕೇತ &ec) {
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
    ಆದರೆ (lexer.lookahead().isStringLiteral()) {
        ಸ್ಥಿರಾಂಕ UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_ಗಮ್ಯ_Pat(rebuiltPat, element, /*escapeUnprintable=*/ಸುಳ್ಳು);
        rebuiltPat.append(u'}');
        lexer.advance();
        ಹಿಂತಿರುಗಿ Unicode_ಗಣ().ಕೂಡಿಸು(element);
    }
    UChar32 first;
    ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lexer.lookahead().ಸಂಕೇತ_ಬಿಂದು().has_value()) {
        first = *lexer.lookahead().ಸಂಕೇತ_ಬಿಂದು();
    } ಇಲ್ಲದಿದ್ದರೆ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_ಗಮ್ಯ_Pat(rebuiltPat, first, /*escapeUnprintable=*/ಸುಳ್ಳು);
    ಆದರೆ (!lexer.lookahead().is_ಗಣ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        ಹಿಂತಿರುಗಿ Unicode_ಗಣ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ಆದರೆ (lexer.lookahead2().is_ಗಣ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ಹಿಂತಿರುಗಿ Unicode_ಗಣ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 ಕೊನೆಯ;
    ಆದರೆ (lexer.lookahead().is_ಗಣ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        ಕೊನೆಯ = u'$';
        ಆದರೆ (lexer.lookahead2().is_ಗಣ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lexer.lookahead().ಸಂಕೇತ_ಬಿಂದು().has_value()) {
        ಕೊನೆಯ = *lexer.lookahead().ಸಂಕೇತ_ಬಿಂದು();
    } ಇಲ್ಲದಿದ್ದರೆ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ಆದರೆ (ಕೊನೆಯ <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(ಕೊನೆಯ) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_ಗಮ್ಯ_Pat(rebuiltPat, ಕೊನೆಯ, /*escapeUnprintable=*/ಸುಳ್ಳು);
    ಹಿಂತಿರುಗಿ Unicode_ಗಣ(first, ಕೊನೆಯ);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

ನಾಮಾಕಾಶ {

UBool numeric_ಮೌಲ್ಯ_Filter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಹಿಂತಿರುಗಿ u_get_Numeric_ಮೌಲ್ಯ(ch) == *static_cast<ದ್ವಿಗುಣ*>(context);
}

UBool general_Category_ಮುಖವಾಡ_Filter(UChar32 ch, ಶೂನ್ಯ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ಹಿಂತಿರುಗಿ (U_GET_GC_ಮುಖವಾಡ((UChar32) ch) & value) != 0;
}

UBool ಆವೃತ್ತಿ_Filter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* ಆವೃತ್ತಿ = static_cast<UVersionInfo*>(context);
    ಹಿಂತಿರುಗಿ uprv_memcmp(&v, &none, ಗಾತ್ರ(v)) > 0 && uprv_memcmp(&v, ಆವೃತ್ತಿ, ಗಾತ್ರ(v)) <= 0;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ಹಿಂತಿರುಗಿ u_get_Int_Property_ಮೌಲ್ಯ(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಹಿಂತಿರುಗಿ uscript_hasScript(ch, *static_cast<UScript_ಸಂಕೇತ*>(context));
}

UBool idTypeFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಹಿಂತಿರುಗಿ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
ಶೂನ್ಯ Unicode_ಗಣ::applyFilter(Unicode_ಗಣ::Filter filter,
                             ಶೂನ್ಯ* context,
                             ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* inclusions,
                             UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

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

    UChar32 ಆರಂಭ_Has_Property = -1;
    int32_t ಎಲ್ಲೆ_Range = inclusions->get_Range_ಎಣಿಕೆ();

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j=0; j<ಎಲ್ಲೆ_Range; ++j) {
        // get current range
        UChar32 ಆರಂಭ = inclusions->get_Range_ಆರಂಭ(j);
        UChar32 ಕೊನೆ = inclusions->get_Range_ಕೊನೆ(j);

        // for all the code points in the range, process
        ಗಾಗಿ (UChar32 ch = ಆರಂಭ; ch <= ಕೊನೆ; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ಆದರೆ ((*filter)(ch, context)) {
                ಆದರೆ (ಆರಂಭ_Has_Property < 0) {
                    ಆರಂಭ_Has_Property = ch;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_Has_Property >= 0) {
                ಕೂಡಿಸು(ಆರಂಭ_Has_Property, ch-1);
                ಆರಂಭ_Has_Property = -1;
            }
        }
    }
    ಆದರೆ (ಆರಂಭ_Has_Property >= 0) {
        ಕೂಡಿಸು(ಆರಂಭ_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    ಆದರೆ (isBogus() && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        // We likely ran out of memory. AHHH!
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

ನಾಮಾಕಾಶ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#ವ್ಯಾಖ್ಯಾನಿಸು FAIL(ec) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ec=U_ILLEGAL_ವಾದ_ERROR; \
    ಹಿಂತಿರುಗಿ *ಇದು; \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

Unicode_ಗಣ&
Unicode_ಗಣ::apply_Int_Property_ಮೌಲ್ಯ(UProperty prop, int32_t value, UError_ಸಂಕೇತ& ec) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(ec) || isFrozen()) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (prop == UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_ಮುಖವಾಡ_Filter, &value, inclusions, ec);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_ಸಂಕೇತ script = static_cast<UScript_ಸಂಕೇತ>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prop == UCHAR_IDENTIFIER_TYPE) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (0 <= prop && prop < UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) {
        ಆದರೆ (value == 0 || value == 1) {
            ಸ್ಥಿರಾಂಕ USet *ಗಣ = u_get_ದ್ವಿಮಾನ_Property_ಗಣ(prop, &ec);
            ಆದರೆ (U_FAILURE(ec)) { ಹಿಂತಿರುಗಿ *ಇದು; }
            copyFrom(*Unicode_ಗಣ::fromUSet(ಗಣ), ನಿಜ);
            ಆದರೆ (value == 0) {
                complement().remove_All_ಅಕ್ಷರಸರಣಿಗಳು();  // code point complement
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            clear();
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (UCHAR_INT_ಆರಂಭ <= prop && prop < UCHAR_INT_ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ec = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Unicode_ಗಣ&
Unicode_ಗಣ::applyPropertyAlias(ಸ್ಥಿರಾಂಕ UnicodeString& prop,
                               ಸ್ಥಿರಾಂಕ UnicodeString& value,
                               UError_ಸಂಕೇತ& ec) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(ec) || isFrozen()) ಹಿಂತಿರುಗಿ *ಇದು;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ಆದರೆ( !uprv_isInvariantUString(prop.get_ಬಫರ್(), prop.length()) ||
        !uprv_isInvariantUString(value.get_ಬಫರ್(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ಆದರೆ (U_FAILURE(ec)) ಹಿಂತಿರುಗಿ *ಇದು;

    UProperty p;
    int32_t v;
    UBool invert = ಸುಳ್ಳು;

    ಆದರೆ (value.length() > 0) {
        p = u_getPropertyEnum(pname.ದತ್ತಾಂಶ());
        ಆದರೆ (p == UCHAR_INVALID_ಸಂಕೇತ) FAIL(ec);

        // Treat gc as gcm
        ಆದರೆ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ;
        }

        ಆದರೆ ((p >= UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ && p < UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) ||
            (p >= UCHAR_INT_ಆರಂಭ && p < UCHAR_INT_ಎಲ್ಲೆ) ||
            (p >= UCHAR_ಮುಖವಾಡ_ಆರಂಭ && p < UCHAR_ಮುಖವಾಡ_ಎಲ್ಲೆ)) {
            v = u_get_Property_ಮೌಲ್ಯ_Enum(p, vname.ದತ್ತಾಂಶ());
            ಆದರೆ (v == UCHAR_INVALID_ಸಂಕೇತ) {
                // Handle numeric CCC
                ಆದರೆ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    ಅಕ್ಷರ* ಕೊನೆ;
                    ದ್ವಿಗುಣ val = uprv_strtod(vname.ದತ್ತಾಂಶ(), &ಕೊನೆ);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ಆದರೆ (*ಕೊನೆ != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    FAIL(ec);
                }
            }
        }

        ಇಲ್ಲದಿದ್ದರೆ {

            ಆಯ್ಕೆ (p) {
            ಸ್ಥಿತಿ UCHAR_NUMERIC_ಮೌಲ್ಯ:
                {
                    ಅಕ್ಷರ* ಕೊನೆ;
                    ದ್ವಿಗುಣ val = uprv_strtod(vname.ದತ್ತಾಂಶ(), &ಕೊನೆ);
                    ಆದರೆ (*ಕೊನೆ != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_ಮೌಲ್ಯ_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ಹಿಂತಿರುಗಿ *ಇದು;
                }
            ಸ್ಥಿತಿ UCHAR_NAME:
                {
                    ಸ್ಥಿರಾಂಕ UChar32 ch = getCharacterByName(vname);
                    ಆದರೆ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    ಕೂಡಿಸು(ch);
                    ಹಿಂತಿರುಗಿ *ಇದು;
                }
            ಸ್ಥಿತಿ UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            ಸ್ಥಿತಿ UCHAR_AGE:
                {
                    UVersionInfo ಆವೃತ್ತಿ;
                    u_ಆವೃತ್ತಿ_From_String(ಆವೃತ್ತಿ, vname.ದತ್ತಾಂಶ());
                    applyFilter(ಆವೃತ್ತಿ_Filter, &ಆವೃತ್ತಿ,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ಹಿಂತಿರುಗಿ *ಇದು;
                }
            ಸ್ಥಿತಿ UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, vname.ದತ್ತಾಂಶ());
                ಆದರೆ (v == UCHAR_INVALID_ಸಂಕೇತ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_ಮೌಲ್ಯ_Enum(p, vname.ದತ್ತಾಂಶ());
                ಆದರೆ (v == UCHAR_INVALID_ಸಂಕೇತ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ಇಲ್ಲದಿದ್ದರೆ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ;
        v = u_get_Property_ಮೌಲ್ಯ_Enum(p, pname.ದತ್ತಾಂಶ());
        ಆದರೆ (v == UCHAR_INVALID_ಸಂಕೇತ) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_ಮೌಲ್ಯ_Enum(p, pname.ದತ್ತಾಂಶ());
            ಆದರೆ (v == UCHAR_INVALID_ಸಂಕೇತ) {
                p = u_getPropertyEnum(pname.ದತ್ತಾಂಶ());
                ಆದರೆ (p >= UCHAR_ದ್ವಿಮಾನ_ಆರಂಭ && p < UCHAR_ದ್ವಿಮಾನ_ಎಲ್ಲೆ) {
                    v = 1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (0 == uprv_compare_Property_ಹೆಸರುಗಳು(ANY, pname.ದತ್ತಾಂಶ())) {
                    ಗಣ(MIN_ಮೌಲ್ಯ, MAX_ಮೌಲ್ಯ);
                    ಹಿಂತಿರುಗಿ *ಇದು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (0 == uprv_compare_Property_ಹೆಸರುಗಳು(ASCII, pname.ದತ್ತಾಂಶ())) {
                    ಗಣ(0, 0x7F);
                    ಹಿಂತಿರುಗಿ *ಇದು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (0 == uprv_compare_Property_ಹೆಸರುಗಳು(ASSIGNED, pname.ದತ್ತಾಂಶ())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_ಮುಖವಾಡ;
                    v = U_GC_CN_ಮುಖವಾಡ;
                    invert = ನಿಜ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_ಮೌಲ್ಯ(p, v, ec);
    ಆದರೆ(invert) {
        complement().remove_All_ಅಕ್ಷರಸರಣಿಗಳು();  // code point complement
    }

    ಆದರೆ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool Unicode_ಗಣ::resembles_Property_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ಆದರೆ ((pos+5) > ನಮೂನೆ.length()) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // Look for an opening [:, [:^, \p, or \P
    ಹಿಂತಿರುಗಿ isPOSIXOpen(ನಮೂನೆ, pos) || isPerlOpen(ನಮೂನೆ, pos) || isNameOpen(ನಮೂನೆ, pos);
}

U_NAMESPACE_ಕೊನೆ
