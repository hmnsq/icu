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
constexpr ವರ್ಣ ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr ವರ್ಣ ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr ವರ್ಣ ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV uset_cleanup();

ಸ್ಥಿರ UnicodeSet *uni32Singleton;
ಸ್ಥಿರ icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ಸ್ಥಿರ UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

ನಾಮಾಕಾಶ {

// Cache some sets for other services -------------------------------------- ***
ಶೂನ್ಯ U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = ನೂತನ UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    ಯದಿ(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    ಪ್ರತ್ಯಾಗಮ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

ಅನ್ತಃಸ್ಥ UBool
isPerlOpen(ಧ್ರುವ UnicodeString &pattern, int32_t pos) {
    char16_t c;
    ಪ್ರತ್ಯಾಗಮ pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

ಅನ್ತಃಸ್ಥ UBool
isNameOpen(ಧ್ರುವ UnicodeString &pattern, int32_t pos) {
    ಪ್ರತ್ಯಾಗಮ pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

ಅನ್ತಃಸ್ಥ UBool
isPOSIXOpen(ಧ್ರುವ UnicodeString &pattern, int32_t pos) {
    ಪ್ರತ್ಯಾಗಮ pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#define _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ಧ್ರುವ CharString& name) {
    ಪ್ರತಿ (ಧ್ರುವ UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        ಯದಿ (U_SUCCESS(ec)) {
            ಪ್ರತ್ಯಾಗಮ ch;
        }
    }
    ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
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
UnicodeSet::UnicodeSet(ಧ್ರುವ UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(ಏತತ್);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ಧ್ರುವ UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, ಸತ್ಯ);
    ಯದಿ (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ
UnicodeSet::applyPatternIgnoreSpace(ಧ್ರುವ UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ಧ್ರುವ ಚಿಹ್ನೆ_Table* symbols,
                                    UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ಪ್ರತ್ಯಾಗಮ;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ಧ್ರುವ UnicodeString& pattern, int32_t pos) {
    ಪ್ರತ್ಯಾಗಮ ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

ವರ್ಗ UnicodeSet::Lexer {
  ಸಾರ್ವಜನಿಕ:
    Lexer(ಧ್ರುವ UnicodeString &pattern,
          ಧ್ರುವ ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ಧ್ರುವ ಚಿಹ್ನೆ_Table *ಧ್ರುವ symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    ವರ್ಗ LexicalElement {
      ಸಾರ್ವಜನಿಕ:
        ತಾರ್ಕಿಕ isSetOperator(ಧ್ರುವ char16_t op) ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        ತಾರ್ಕಿಕ isStringLiteral() ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        ತಾರ್ಕಿಕ isNamedElement() ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        ತಾರ್ಕಿಕ isBracketedElement() ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ಧ್ರುವ {
            ಯದಿ (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ಪ್ರತ್ಯಾಗಮ string_;
            }
            ಪ್ರತ್ಯಾಗಮ std::nullopt;
        }

        std::optional<UChar32> codePoint() ಧ್ರುವ {
            ಯದಿ (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ಪ್ರತ್ಯಾಗಮ string_.char32At(0);
            }
            ಪ್ರತ್ಯಾಗಮ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ಧ್ರುವ UnicodeSet *set() ಧ್ರುವ {
            ಯದಿ (U_FAILURE(errorCode_)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ಯದಿ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ಯದಿ (precomputedSet_ != nullptr) {
                    ಪ್ರತ್ಯಾಗಮ precomputedSet_;
                } ಅನ್ಯಥಾ {
                    ಪ್ರತ್ಯಾಗಮ &set_;
                }
            }
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }

        ಧ್ರುವ UErrorCode& errorCode() ಧ್ರುವ{
          ಪ್ರತ್ಯಾಗಮ errorCode_;
        }

#ಯದಿ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ಧ್ರುವ {
            UnicodeString result;
            ಯದಿ (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            ಪ್ರತ್ಯಾಗಮ result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      ಗುಪ್ತ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ಗಣನಾ Category : std::uint8_t {
            SET_OPERATOR,
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
            END_OF_TEXT,
        };
        ಸ್ಥಿರ constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
        LexicalElement(Category category, UnicodeString string, RuleCharacterIterator::Pos after,
                       ಧ್ರುವ UErrorCode errorCode, ಧ್ರುವ UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, ತಾರ್ಕಿಕ separated,
                       std::optional<LexicalElement> ಧ್ರುವ &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            ಯದಿ (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯದಿ (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } ಅನ್ಯಥಾ ಯದಿ (preceding->category_ == ESCAPED_ELEMENT) {
                ಯದಿ (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } ಅನ್ಯಥಾ ಯದಿ (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    ಯದಿ (preceding->sourceText_[1] == u'x') {
                        ಯದಿ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } ಅನ್ಯಥಾ ಯದಿ (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        ಯದಿ (cp >= u'0' && cp <= u'7') {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        RuleCharacterIterator::Pos after_;
        UErrorCode errorCode_;
        ಧ್ರುವ UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend ವರ್ಗ Lexer;
    };

    UnicodeString getPositionForDebugging() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    ತಾರ್ಕಿಕ acceptSetOperator(char16_t op) {
        ಯದಿ (lookahead().isSetOperator(op)) {
            advance();
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಧ್ರುವ LexicalElement &lookahead() {
        ಯದಿ (!ahead_.has_value()) {
            ಧ್ರುವ RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        ಪ್ರತ್ಯಾಗಮ *ahead_;
    }

    ಧ್ರುವ LexicalElement &lookahead2() {
        ಯದಿ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ಧ್ರುವ RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        ಪ್ರತ್ಯಾಗಮ *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    RuleCharacterIterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        ಪ್ರತ್ಯಾಗಮ chars_;
    }

    int32_t charsOptions() {
        ಪ್ರತ್ಯಾಗಮ charsOptions_;
    }

    ತಾರ್ಕಿಕ atEnd() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ chars_.atEnd();
    }

    ಶೂನ್ಯ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ಗುಪ್ತ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ಧ್ರುವ {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> ಧ್ರುವ& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ತಾರ್ಕಿಕ separated = ಅಸತ್ಯ;
        ಯದಿ (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            ಪ್ರತಿ (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                ಯದಿ (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    ಭಙ್ಗ;
                }
                ಯದಿ (s != u'\u200E' && s != u'\u200F') {
                    separated = ಸತ್ಯ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ಯದಿ (chars_.atEnd()) {
            ಪ್ರತ್ಯಾಗಮ LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ಧ್ರುವ int32_t start = parsePosition_.getIndex();
        ಧ್ರುವ RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        ಧ್ರುವ UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        ಯದಿ (first == u'[' || first == u'\\') {
            ಧ್ರುವ RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ಧ್ರುವ UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            ಯದಿ ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                ಯದಿ (second == u'N') {
                    UChar32 ಧ್ರುವ queryResult = scanNamedElementBrackets(errorCode);
                    ಪ್ರತ್ಯಾಗಮ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } ಅನ್ಯಥಾ {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    ಪ್ರತ್ಯಾಗಮ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        ಯದಿ (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ಯದಿ (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                ಧ್ರುವ std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                ಧ್ರುವ UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                ಯದಿ (precomputedSet != nullptr) {
                    ಪ್ರತ್ಯಾಗಮ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/ಸತ್ಯ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ಧ್ರುವ UnicodeString *ಧ್ರುವ expression = symbols_->lookup(name);
                ಯದಿ (expression == nullptr) {
                    ಪ್ರತ್ಯಾಗಮ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/ಸತ್ಯ,
                        /*preceding=*/std::nullopt);
                }
                ಪ್ರತ್ಯಾಗಮ evaluateVariable(*expression, source);
            }
        }
        ವಿಕಲ್ಪ (first) {
        ಪಕ್ಷ u'[':
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        ಪಕ್ಷ u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        ಪಕ್ಷ u'&':
        ಪಕ್ಷ u'-':
        ಪಕ್ಷ u']':
        ಪಕ್ಷ u'^':
        ಪಕ್ಷ u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        ಪಕ್ಷ u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_ಎಣಿಕೆ = 0;
            ಯಾವತ್ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ಧ್ರುವ RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                ಯದಿ (next == u'\\') {
                    ಧ್ರುವ UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    ಯದಿ (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = ಸತ್ಯ;
                    } ಅನ್ಯಥಾ ಯದಿ (afterBackslash == u'p' || afterBackslash == u'P') {
                        ಪ್ರತ್ಯಾಗಮ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } ಅನ್ಯಥಾ {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } ಅನ್ಯಥಾ {
#ಯದಿ U_ICU_VERSION_MAJOR_ಅಂಕೆ < 81
                    ಯದಿ (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#ಅನ್ಯಥಾ
#error Remove ಏತತ್ transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = ಅಸತ್ಯ;
                }
                ಯದಿ (!escaped && next == u'}') {
                    ಪ್ರತ್ಯಾಗಮ LexicalElement(
                        code_Point_ಎಣಿಕೆ == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_ಎಣಿಕೆ += 1;
            }
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        ಸ್ವಭಾವ:
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        ಧ್ರುವ UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        ಯದಿ (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ಯಾವತ್ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                ಯದಿ (last == u':') {
                    ಯದಿ (!hex.has_value()) {
                        hex.emplace();
                        ಯದಿ (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ಪ್ರತ್ಯಾಗಮ {};
                        }
                        ಪ್ರತಿ (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            ಯದಿ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ಅನ್ಯಥಾ {
                                digit = digit & ~0x20;
                                ಯದಿ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ಅನ್ಯಥಾ {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    ಪ್ರತ್ಯಾಗಮ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ಯದಿ (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                ಪ್ರತ್ಯಾಗಮ {};
                            }
                        }
                    } ಅನ್ಯಥಾ ಯದಿ (!literal.has_value()) {
                        ಧ್ರುವ auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        ಯದಿ (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ಪ್ರತ್ಯಾಗಮ {};
                        }
                    } ಅನ್ಯಥಾ {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ಪ್ರತ್ಯಾಗಮ {};
                    }
                    start = parsePosition_.getIndex();
                } ಅನ್ಯಥಾ ಯದಿ (last == u'}') {
                    ಧ್ರುವ std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    ಧ್ರುವ UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    ಯದಿ (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ಪ್ರತ್ಯಾಗಮ {};
                    }
                    ಯದಿ (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ಪ್ರತ್ಯಾಗಮ {};
                    }
                    ಪ್ರತ್ಯಾಗಮ result;
                }
            }
        }
        ಯದಿ (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ {};
    }

    LexicalElement evaluateVariable(ಧ್ರುವ UnicodeString &expression,
                                    ಧ್ರುವ std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        ಯದಿ (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_ಮೌಲ್ಯ =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_ಮೌಲ್ಯ.setPattern(rebuiltPattern);
            ಯದಿ (!expressionLexer.atEnd()) {
                ಪ್ರತ್ಯಾಗಮ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ಸತ್ಯ,
                    /*preceding=*/std::nullopt);
            }
            ಪ್ರತ್ಯಾಗಮ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_ಮೌಲ್ಯ),
                source,
                /*separated=*/ಸತ್ಯ,
                /*preceding=*/std::nullopt);
        } ಅನ್ಯಥಾ {
            expressionLexer.advance();
            ಯದಿ (!expressionLexer.atEnd()) {
                ಪ್ರತ್ಯಾಗಮ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ಸತ್ಯ,
                    /*preceding=*/std::nullopt);
            }
            ವಿಕಲ್ಪ (variableToken.category_) {
            ಪಕ್ಷ LexicalElement::LITERAL_ELEMENT:
            ಪಕ್ಷ LexicalElement::ESCAPED_ELEMENT:
            ಪಕ್ಷ LexicalElement::NAMED_ELEMENT:
            ಪಕ್ಷ LexicalElement::BRACKETED_ELEMENT:
            ಪಕ್ಷ LexicalElement::STRING_LITERAL:
            ಪಕ್ಷ LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ಪ್ರತ್ಯಾಗಮ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/ಸತ್ಯ,
                    /*preceding=*/std::nullopt);
            ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/ಸತ್ಯ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        ತಾರ್ಕಿಕ exteriorlyNegated = ಅಸತ್ಯ;
        ತಾರ್ಕಿಕ interiorlyNegated = ಅಸತ್ಯ;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ಧ್ರುವ UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        ಯದಿ (first == u'\\') {
            ಯದಿ (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } ಅನ್ಯಥಾ {
            ಯದಿ (third == u'^') {
                exteriorlyNegated = ಸತ್ಯ;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        ಯಾವತ್ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ಧ್ರುವ RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            ಯದಿ (penultimateUnescaped == u'\\') {
                ಯದಿ (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    ಯದಿ (!U_SUCCESS(errorCode)) {
                        ಪ್ರತ್ಯಾಗಮ {};
                    }
                } ಅನ್ಯಥಾ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } ಅನ್ಯಥಾ ಯದಿ (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ಅನ್ಯಥಾ ಯದಿ (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                ಯದಿ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ಪ್ರತ್ಯಾಗಮ {};
                }
                interiorlyNegated = ಸತ್ಯ;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ಅನ್ಯಥಾ ಯದಿ ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ಧ್ರುವ int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ಯದಿ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    ಯದಿ (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ಪ್ರತ್ಯಾಗಮ {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                ಯದಿ (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                ಪ್ರತ್ಯಾಗಮ result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ {};
    }

    ಧ್ರುವ UnicodeString &pattern_;
    ಧ್ರುವ ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ಧ್ರುವ uint32_t unicodeSetOptions_;
    ಧ್ರುವ int32_t charsOptions_;
    ಧ್ರುವ ಚಿಹ್ನೆ_Table *ಧ್ರುವ symbols_;
    UnicodeSet &(UnicodeSet::* ಧ್ರುವ caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

ನಾಮಾಕಾಶ {

constexpr int32_t MAX_DEPTH = 100;

#ಯದಿ U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ಕುರು {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ಯದಿ (U_FAILURE(ec)) {                                                                       \
            ಯದಿ (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ಅನ್ಯಥಾ ಯದಿ (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            ಪ್ರತ್ಯಾಗಮ result;                                                                         \
        }                                                                                          \
    } ಯಾವತ್ (ಅಸತ್ಯ)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ಕುರು {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ಯದಿ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ಅನ್ಯಥಾ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ಪ್ರತ್ಯಾಗಮ result;                                                                             \
    } ಯಾವತ್ (ಅಸತ್ಯ)

#ಅನ್ಯಥಾ

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ಕುರು {                                                                                           \
        ಯದಿ (U_FAILURE(ec)) {                                                                       \
            ಪ್ರತ್ಯಾಗಮ result;                                                                         \
        }                                                                                          \
    } ಯಾವತ್ (ಅಸತ್ಯ)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ಕುರು {                                                                                           \
        ಯದಿ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ಅನ್ಯಥಾ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ಪ್ರತ್ಯಾಗಮ result;                                                                             \
    } ಯಾವತ್ (ಅಸತ್ಯ)

#endif

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

ಶೂನ್ಯ UnicodeSet::applyPattern(ಧ್ರುವ UnicodeString &pattern,
                              ಧ್ರುವ ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ಧ್ರುವ ಚಿಹ್ನೆ_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    ಯದಿ (U_FAILURE(ec)) ಪ್ರತ್ಯಾಗಮ;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *ಏತತ್ = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    ಯದಿ (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    ತಾರ್ಕಿಕ isComplement = ಅಸತ್ಯ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    ತಾರ್ಕಿಕ preserveSyntaxInPattern = ಅಸತ್ಯ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    ಯದಿ (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/ಅಸತ್ಯ);
        lexer.advance();
        preserveSyntaxInPattern = ಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ಯದಿ (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            ಯದಿ (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = ಸತ್ಯ;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            ಯದಿ (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } ಅನ್ಯಥಾ {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    ಯದಿ ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    ಯದಿ (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    ಯದಿ (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } ಅನ್ಯಥಾ {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/ಅಸತ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    ತಾರ್ಕಿಕ &containsSetOperation,
                                    UErrorCode &ec) {
    UnicodeSet result;
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
    ಯದಿ (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ಯಾವತ್ (!lexer.atEnd()) {
        ಯದಿ (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            ಪ್ರತ್ಯಾಗಮ result;
        } ಅನ್ಯಥಾ ಯದಿ (lexer.lookahead().isSetOperator(u'$')) {
            ಯದಿ (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = ಸತ್ಯ;
                ಪ್ರತ್ಯಾಗಮ result;
            }
        }
        ಯದಿ (lexer.lookahead().isSetOperator(u']')) {
            ಪ್ರತ್ಯಾಗಮ result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               ತಾರ್ಕಿಕ &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ಯದಿ (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = ಸತ್ಯ;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ಅನ್ಯಥಾ {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

ಶೂನ್ಯ UnicodeSet::parseSetOperations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    UErrorCode &ec) {
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
    ಧ್ರುವ UnicodeSet ಎಡ_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(ಎಡ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    ಪ್ರತಿ (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ಯದಿ (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ಧ್ರುವ UnicodeSet ಬಲ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ಬಲ_Hand_Side);
        } ಅನ್ಯಥಾ ಯದಿ (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ಯದಿ (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ಪ್ರತ್ಯಾಗಮ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ಧ್ರುವ UnicodeSet ಬಲ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ಬಲ_Hand_Side);
        } ಅನ್ಯಥಾ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
}

UnicodeSet UnicodeSet::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UErrorCode &ec) {
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
    ಯದಿ (lexer.lookahead().isStringLiteral()) {
        ಧ್ರುವ UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/ಅಸತ್ಯ);
        rebuiltPat.append(u'}');
        lexer.advance();
        ಪ್ರತ್ಯಾಗಮ UnicodeSet().add(element);
    }
    UChar32 first;
    ಯದಿ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ಅನ್ಯಥಾ ಯದಿ (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } ಅನ್ಯಥಾ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/ಅಸತ್ಯ);
    ಯದಿ (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        ಪ್ರತ್ಯಾಗಮ UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ಯದಿ (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ಪ್ರತ್ಯಾಗಮ UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    ಯದಿ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        ಯದಿ (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ಅನ್ಯಥಾ ಯದಿ (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } ಅನ್ಯಥಾ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ಯದಿ (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/ಅಸತ್ಯ);
    ಪ್ರತ್ಯಾಗಮ UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

ನಾಮಾಕಾಶ {

UBool numeric_ಮೌಲ್ಯ_Filter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಪ್ರತ್ಯಾಗಮ u_get_Numeric_ಮೌಲ್ಯ(ch) == *static_cast<ದ್ವಿಗುಣ*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ಪ್ರತ್ಯಾಗಮ (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಸ್ಥಿರ ಧ್ರುವ UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    ಪ್ರತ್ಯಾಗಮ uprv_memcmp(&v, &none, ಪರಿಮಾಣ(v)) > 0 && uprv_memcmp(&v, version, ಪರಿಮಾಣ(v)) <= 0;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ಪ್ರತ್ಯಾಗಮ u_get_Int_Property_ಮೌಲ್ಯ(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಪ್ರತ್ಯಾಗಮ uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, ಶೂನ್ಯ* context) {
    ಪ್ರತ್ಯಾಗಮ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
ಶೂನ್ಯ UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             ಶೂನ್ಯ* context,
                             ಧ್ರುವ UnicodeSet* inclusions,
                             UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ;

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

    UChar32 startHasProperty = -1;
    int32_t limitRange = inclusions->get_Range_ಎಣಿಕೆ();

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        ಪ್ರತಿ (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ಯದಿ ((*filter)(ch, context)) {
                ಯದಿ (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } ಅನ್ಯಥಾ ಯದಿ (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    ಯದಿ (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    ಯದಿ (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

ನಾಮಾಕಾಶ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_ಮೌಲ್ಯ(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(ec) || isFrozen()) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ಧ್ರುವ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } ಅನ್ಯಥಾ ಯದಿ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ಧ್ರುವ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ಅನ್ಯಥಾ ಯದಿ (prop == UCHAR_IDENTIFIER_TYPE) {
        ಧ್ರುವ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ಅನ್ಯಥಾ ಯದಿ (0 <= prop && prop < UCHAR_ದ್ವಿಮಾನ_LIMIT) {
        ಯದಿ (value == 0 || value == 1) {
            ಧ್ರುವ USet *set = u_get_ದ್ವಿಮಾನ_Property_Set(prop, &ec);
            ಯದಿ (U_FAILURE(ec)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
            copyFrom(*UnicodeSet::fromUSet(set), ಸತ್ಯ);
            ಯದಿ (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } ಅನ್ಯಥಾ {
            clear();
        }
    } ಅನ್ಯಥಾ ಯದಿ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ಧ್ರುವ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ಅನ್ಯಥಾ {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ಧ್ರುವ UnicodeString& prop,
                               ಧ್ರುವ UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(ec) || isFrozen()) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ಯದಿ( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ಯದಿ (U_FAILURE(ec)) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

    UProperty p;
    int32_t v;
    UBool invert = ಅಸತ್ಯ;

    ಯದಿ (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        ಯದಿ (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        ಯದಿ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        ಯದಿ ((p >= UCHAR_ದ್ವಿಮಾನ_START && p < UCHAR_ದ್ವಿಮಾನ_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_ಮೌಲ್ಯ_Enum(p, vname.data());
            ಯದಿ (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                ಯದಿ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    ವರ್ಣ* end;
                    ದ್ವಿಗುಣ val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ಯದಿ (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ಅನ್ಯಥಾ {
                    FAIL(ec);
                }
            }
        }

        ಅನ್ಯಥಾ {

            ವಿಕಲ್ಪ (p) {
            ಪಕ್ಷ UCHAR_NUMERIC_ಮೌಲ್ಯ:
                {
                    ವರ್ಣ* end;
                    ದ್ವಿಗುಣ val = uprv_strtod(vname.data(), &end);
                    ಯದಿ (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_ಮೌಲ್ಯ_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
                }
            ಪಕ್ಷ UCHAR_NAME:
                {
                    ಧ್ರುವ UChar32 ch = getCharacterByName(vname);
                    ಯದಿ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
                }
            ಪಕ್ಷ UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            ಪಕ್ಷ UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
                }
            ಪಕ್ಷ UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_ಮೌಲ್ಯ_Enum(UCHAR_SCRIPT, vname.data());
                ಯದಿ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ಭಙ್ಗ;
            ಪಕ್ಷ UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_ಮೌಲ್ಯ_Enum(p, vname.data());
                ಯದಿ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ಅನ್ಯಥಾ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_ಮೌಲ್ಯ_Enum(p, pname.data());
        ಯದಿ (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_ಮೌಲ್ಯ_Enum(p, pname.data());
            ಯದಿ (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                ಯದಿ (p >= UCHAR_ದ್ವಿಮಾನ_START && p < UCHAR_ದ್ವಿಮಾನ_LIMIT) {
                    v = 1;
                } ಅನ್ಯಥಾ ಯದಿ (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_ಮೌಲ್ಯ, MAX_ಮೌಲ್ಯ);
                    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
                } ಅನ್ಯಥಾ ಯದಿ (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
                } ಅನ್ಯಥಾ ಯದಿ (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = ಸತ್ಯ;
                } ಅನ್ಯಥಾ {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_ಮೌಲ್ಯ(p, v, ec);
    ಯದಿ(invert) {
        complement().removeAllStrings();  // code point complement
    }

    ಯದಿ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(ಧ್ರುವ UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ಯದಿ ((pos+5) > pattern.length()) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    // Look for an opening [:, [:^, \p, or \P
    ಪ್ರತ್ಯಾಗಮ isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
