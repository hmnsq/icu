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

#अन्तर्भाव <array>
#अन्तर्भाव <optional>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/utfiterator.h"
#अन्तर्भाव "uset_imp.h"
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "util.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "patternprops.h"
#अन्तर्भाव "propname.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "hash.h"

U_NAMESPACE_USE

नामाकाश {

// Special property set IDs
constexpr वर्ण ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr वर्ण ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr वर्ण ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uset_cleanup();

स्थिर UnicodeSet *uni32Singleton;
स्थिर icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
स्थिर UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    पटिनिवत्त सच्च;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

नामाकाश {

// Cache some sets for other services -------------------------------------- ***
शून्य U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = नूतन UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    सचे(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    पटिनिवत्त uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

अन्तःस्थ UBool
isPerlOpen(ध्रुव UnicodeString &pattern, int32_t pos) {
    char16_t c;
    पटिनिवत्त pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

अन्तःस्थ UBool
isNameOpen(ध्रुव UnicodeString &pattern, int32_t pos) {
    पटिनिवत्त pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

अन्तःस्थ UBool
isPOSIXOpen(ध्रुव UnicodeString &pattern, int32_t pos) {
    पटिनिवत्त pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(ध्रुव CharString& name) {
    अत्थाय (ध्रुव UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        सचे (U_SUCCESS(ec)) {
            पटिनिवत्त ch;
        }
    }
    पटिनिवत्त U_SENTINEL;
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
UnicodeSet::UnicodeSet(ध्रुव UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(एतत्);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ध्रुव UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    सचे (U_FAILURE(status)) पटिनिवत्त *एतत्;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, सच्च);
    सचे (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    पटिनिवत्त *एतत्;
}

शून्य
UnicodeSet::applyPatternIgnoreSpace(ध्रुव UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ध्रुव चिह्नम्_Table* symbols,
                                    UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    सचे (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        पटिनिवत्त;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    सचे (U_FAILURE(status)) पटिनिवत्त;
    सचे (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        पटिनिवत्त;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ध्रुव UnicodeString& pattern, int32_t pos) {
    पटिनिवत्त ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

वग्ग UnicodeSet::Lexer {
  सार्वजनिक:
    Lexer(ध्रुव UnicodeString &pattern,
          ध्रुव ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ध्रुव चिह्नम्_Table *ध्रुव symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    वग्ग LexicalElement {
      सार्वजनिक:
        तार्किक isSetOperator(ध्रुव char16_t op) ध्रुव {
            पटिनिवत्त U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        तार्किक isStringLiteral() ध्रुव {
            पटिनिवत्त U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        तार्किक isNamedElement() ध्रुव {
            पटिनिवत्त U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        तार्किक isBracketedElement() ध्रुव {
            पटिनिवत्त U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ध्रुव {
            सचे (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                पटिनिवत्त string_;
            }
            पटिनिवत्त std::nullopt;
        }

        std::optional<UChar32> codePoint() ध्रुव {
            सचे (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                पटिनिवत्त string_.char32At(0);
            }
            पटिनिवत्त std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ध्रुव UnicodeSet *set() ध्रुव {
            सचे (U_FAILURE(errorCode_)) {
                पटिनिवत्त nullptr;
            }
            सचे (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                सचे (precomputedSet_ != nullptr) {
                    पटिनिवत्त precomputedSet_;
                } अञ्ञथा {
                    पटिनिवत्त &set_;
                }
            }
            पटिनिवत्त nullptr;
        }

        ध्रुव UErrorCode& errorCode() ध्रुव{
          पटिनिवत्त errorCode_;
        }

#सचे U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ध्रुव {
            UnicodeString result;
            सचे (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            पटिनिवत्त result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      गुप्त:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        गणना Category : std::uint8_t {
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
        स्थिर constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       ध्रुव UErrorCode errorCode, ध्रुव UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, तार्किक separated,
                       std::optional<LexicalElement> ध्रुव &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            सचे (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                पटिनिवत्त;
            }
            सचे (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } अञ्ञथा सचे (preceding->category_ == ESCAPED_ELEMENT) {
                सचे (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } अञ्ञथा सचे (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    सचे (preceding->sourceText_[1] == u'x') {
                        सचे ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } अञ्ञथा सचे (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        सचे (cp >= u'0' && cp <= u'7') {
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
        ध्रुव UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend वग्ग Lexer;
    };

    UnicodeString getPositionForDebugging() ध्रुव {
        पटिनिवत्त pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    तार्किक acceptSetOperator(char16_t op) {
        सचे (lookahead().isSetOperator(op)) {
            advance();
            पटिनिवत्त सच्च;
        }
        पटिनिवत्त मिच्छा;
    }

    ध्रुव LexicalElement &lookahead() {
        सचे (!ahead_.has_value()) {
            ध्रुव RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        पटिनिवत्त *ahead_;
    }

    ध्रुव LexicalElement &lookahead2() {
        सचे (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ध्रुव RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        पटिनिवत्त *ahead2_;
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
        पटिनिवत्त chars_;
    }

    int32_t charsOptions() {
        पटिनिवत्त charsOptions_;
    }

    तार्किक atEnd() ध्रुव {
        पटिनिवत्त chars_.atEnd();
    }

    शून्य advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  गुप्त:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ध्रुव {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        पटिनिवत्त result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> ध्रुव& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        तार्किक separated = मिच्छा;
        सचे (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            अत्थाय (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                सचे (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    भङ्ग;
                }
                सचे (s != u'\u200E' && s != u'\u200F') {
                    separated = सच्च;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        सचे (chars_.atEnd()) {
            पटिनिवत्त LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ध्रुव int32_t start = parsePosition_.getIndex();
        ध्रुव RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        ध्रुव UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        सचे (first == u'[' || first == u'\\') {
            ध्रुव RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ध्रुव UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            सचे ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                सचे (second == u'N') {
                    UChar32 ध्रुव queryResult = scanNamedElementBrackets(errorCode);
                    पटिनिवत्त LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } अञ्ञथा {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    पटिनिवत्त LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        सचे (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            सचे (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                ध्रुव std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                ध्रुव UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                सचे (precomputedSet != nullptr) {
                    पटिनिवत्त LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/सच्च,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ध्रुव UnicodeString *ध्रुव expression = symbols_->lookup(name);
                सचे (expression == nullptr) {
                    पटिनिवत्त LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/सच्च,
                        /*preceding=*/std::nullopt);
                }
                पटिनिवत्त evaluateVariable(*expression, source);
            }
        }
        विकल्प (first) {
        पक्ष u'[':
            पटिनिवत्त LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        पक्ष u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            पटिनिवत्त LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        पक्ष u'&':
        पक्ष u'-':
        पक्ष u']':
        पक्ष u'^':
        पक्ष u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            पटिनिवत्त LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        पक्ष u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_गणनम् = 0;
            याव (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ध्रुव RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                सचे (next == u'\\') {
                    ध्रुव UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    सचे (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = सच्च;
                    } अञ्ञथा सचे (afterBackslash == u'p' || afterBackslash == u'P') {
                        पटिनिवत्त LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } अञ्ञथा {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } अञ्ञथा {
#सचे U_ICU_VERSION_MAJOR_अङ्कः < 81
                    सचे (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#अञ्ञथा
#error Remove एतत् transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = मिच्छा;
                }
                सचे (!escaped && next == u'}') {
                    पटिनिवत्त LexicalElement(
                        code_Point_गणनम् == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_गणनम् += 1;
            }
            पटिनिवत्त LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        स्वभाव:
            पटिनिवत्त LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        ध्रुव UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        सचे (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            याव (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                सचे (last == u':') {
                    सचे (!hex.has_value()) {
                        hex.emplace();
                        सचे (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            पटिनिवत्त {};
                        }
                        अत्थाय (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            सचे (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } अञ्ञथा {
                                digit = digit & ~0x20;
                                सचे (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } अञ्ञथा {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    पटिनिवत्त {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            सचे (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                पटिनिवत्त {};
                            }
                        }
                    } अञ्ञथा सचे (!literal.has_value()) {
                        ध्रुव auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        सचे (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            पटिनिवत्त {};
                        }
                    } अञ्ञथा {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        पटिनिवत्त {};
                    }
                    start = parsePosition_.getIndex();
                } अञ्ञथा सचे (last == u'}') {
                    ध्रुव std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    ध्रुव UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    सचे (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        पटिनिवत्त {};
                    }
                    सचे (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        पटिनिवत्त {};
                    }
                    पटिनिवत्त result;
                }
            }
        }
        सचे (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        पटिनिवत्त {};
    }

    LexicalElement evaluateVariable(ध्रुव UnicodeString &expression,
                                    ध्रुव std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        सचे (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_मानम् =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_मानम्.setPattern(rebuiltPattern);
            सचे (!expressionLexer.atEnd()) {
                पटिनिवत्त LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/सच्च,
                    /*preceding=*/std::nullopt);
            }
            पटिनिवत्त LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_मानम्),
                source,
                /*separated=*/सच्च,
                /*preceding=*/std::nullopt);
        } अञ्ञथा {
            expressionLexer.advance();
            सचे (!expressionLexer.atEnd()) {
                पटिनिवत्त LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/सच्च,
                    /*preceding=*/std::nullopt);
            }
            विकल्प (variableToken.category_) {
            पक्ष LexicalElement::LITERAL_ELEMENT:
            पक्ष LexicalElement::ESCAPED_ELEMENT:
            पक्ष LexicalElement::NAMED_ELEMENT:
            पक्ष LexicalElement::BRACKETED_ELEMENT:
            पक्ष LexicalElement::STRING_LITERAL:
            पक्ष LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                पटिनिवत्त LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/सच्च,
                    /*preceding=*/std::nullopt);
            स्वभाव:
                पटिनिवत्त LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/सच्च,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        तार्किक exteriorlyNegated = मिच्छा;
        तार्किक interiorlyNegated = मिच्छा;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ध्रुव UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        सचे (first == u'\\') {
            सचे (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } अञ्ञथा {
            सचे (third == u'^') {
                exteriorlyNegated = सच्च;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        याव (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ध्रुव RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            सचे (penultimateUnescaped == u'\\') {
                सचे (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    सचे (!U_SUCCESS(errorCode)) {
                        पटिनिवत्त {};
                    }
                } अञ्ञथा {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } अञ्ञथा सचे (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } अञ्ञथा सचे (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                सचे (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    पटिनिवत्त {};
                }
                interiorlyNegated = सच्च;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } अञ्ञथा सचे ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ध्रुव int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                सचे (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    सचे (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        पटिनिवत्त {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                सचे (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                पटिनिवत्त result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त {};
    }

    ध्रुव UnicodeString &pattern_;
    ध्रुव ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ध्रुव uint32_t unicodeSetOptions_;
    ध्रुव int32_t charsOptions_;
    ध्रुव चिह्नम्_Table *ध्रुव symbols_;
    UnicodeSet &(UnicodeSet::* ध्रुव caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

नामाकाश {

constexpr int32_t MAX_DEPTH = 100;

#सचे U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    करोहि {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        सचे (U_FAILURE(ec)) {                                                                       \
            सचे (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } अञ्ञथा सचे (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            पटिनिवत्त result;                                                                         \
        }                                                                                          \
    } याव (मिच्छा)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    करोहि {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        सचे (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अञ्ञथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        पटिनिवत्त result;                                                                             \
    } याव (मिच्छा)

#अञ्ञथा

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    करोहि {                                                                                           \
        सचे (U_FAILURE(ec)) {                                                                       \
            पटिनिवत्त result;                                                                         \
        }                                                                                          \
    } याव (मिच्छा)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    करोहि {                                                                                           \
        सचे (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अञ्ञथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        पटिनिवत्त result;                                                                             \
    } याव (मिच्छा)

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

शून्य UnicodeSet::applyPattern(ध्रुव UnicodeString &pattern,
                              ध्रुव ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ध्रुव चिह्नम्_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    सचे (U_FAILURE(ec)) पटिनिवत्त;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *एतत् = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    सचे (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    तार्किक isComplement = मिच्छा;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    तार्किक preserveSyntaxInPattern = मिच्छा;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    सचे (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/मिच्छा);
        lexer.advance();
        preserveSyntaxInPattern = सच्च;
    } अञ्ञथा {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        सचे (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            सचे (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = सच्च;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            सचे (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } अञ्ञथा {
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
    सचे ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    सचे (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    सचे (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } अञ्ञथा {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/मिच्छा);
    }
    पटिनिवत्त result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    तार्किक &containsSetOperation,
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
    सचे (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    याव (!lexer.atEnd()) {
        सचे (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            पटिनिवत्त result;
        } अञ्ञथा सचे (lexer.lookahead().isSetOperator(u'$')) {
            सचे (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = सच्च;
                पटिनिवत्त result;
            }
        }
        सचे (lexer.lookahead().isSetOperator(u']')) {
            पटिनिवत्त result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    पटिनिवत्त result;
}

शून्य UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               तार्किक &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    सचे (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = सच्च;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } अञ्ञथा {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

शून्य UnicodeSet::parseSetOperations(Lexer &lexer,
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
    ध्रुव UnicodeSet वामः_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(वामः_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    अत्थाय (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        सचे (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ध्रुव UnicodeSet दक्षिणः_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(दक्षिणः_Hand_Side);
        } अञ्ञथा सचे (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            सचे (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                पटिनिवत्त;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ध्रुव UnicodeSet दक्षिणः_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(दक्षिणः_Hand_Side);
        } अञ्ञथा {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            पटिनिवत्त;
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
    सचे (lexer.lookahead().isStringLiteral()) {
        ध्रुव UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/मिच्छा);
        rebuiltPat.append(u'}');
        lexer.advance();
        पटिनिवत्त UnicodeSet().add(element);
    }
    UChar32 first;
    सचे (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } अञ्ञथा सचे (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } अञ्ञथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/मिच्छा);
    सचे (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        पटिनिवत्त UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    सचे (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        पटिनिवत्त UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    सचे (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        सचे (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } अञ्ञथा सचे (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } अञ्ञथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    सचे (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/मिच्छा);
    पटिनिवत्त UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

नामाकाश {

UBool numeric_मानम्_Filter(UChar32 ch, शून्य* context) {
    पटिनिवत्त u_get_Numeric_मानम्(ch) == *static_cast<द्विगुण*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, शून्य* context) {
    int32_t value = *static_cast<int32_t*>(context);
    पटिनिवत्त (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, शून्य* context) {
    स्थिर ध्रुव UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    पटिनिवत्त uprv_memcmp(&v, &none, परिमाण(v)) > 0 && uprv_memcmp(&v, version, परिमाण(v)) <= 0;
}

प्रकारनिर्देश संरचना {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, शून्य* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    पटिनिवत्त u_get_Int_Property_मानम्(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, शून्य* context) {
    पटिनिवत्त uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, शून्य* context) {
    पटिनिवत्त u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
शून्य UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             शून्य* context,
                             ध्रुव UnicodeSet* inclusions,
                             UErrorCode &status) {
    सचे (U_FAILURE(status)) पटिनिवत्त;

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
    int32_t limitRange = inclusions->get_Range_गणनम्();

    अत्थाय (पूर्णाङ्क j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        अत्थाय (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            सचे ((*filter)(ch, context)) {
                सचे (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } अञ्ञथा सचे (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    सचे (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    सचे (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

नामाकाश {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    पटिनिवत्त *एतत्; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_मानम्(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    सचे (U_FAILURE(ec) || isFrozen()) { पटिनिवत्त *एतत्; }
    सचे (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ध्रुव UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } अञ्ञथा सचे (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ध्रुव UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } अञ्ञथा सचे (prop == UCHAR_IDENTIFIER_TYPE) {
        ध्रुव UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } अञ्ञथा सचे (0 <= prop && prop < UCHAR_द्विमान_LIMIT) {
        सचे (value == 0 || value == 1) {
            ध्रुव USet *set = u_get_द्विमान_Property_Set(prop, &ec);
            सचे (U_FAILURE(ec)) { पटिनिवत्त *एतत्; }
            copyFrom(*UnicodeSet::fromUSet(set), सच्च);
            सचे (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } अञ्ञथा {
            clear();
        }
    } अञ्ञथा सचे (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ध्रुव UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } अञ्ञथा {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    पटिनिवत्त *एतत्;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ध्रुव UnicodeString& prop,
                               ध्रुव UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    सचे (U_FAILURE(ec) || isFrozen()) पटिनिवत्त *एतत्;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    सचे( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    सचे (U_FAILURE(ec)) पटिनिवत्त *एतत्;

    UProperty p;
    int32_t v;
    UBool invert = मिच्छा;

    सचे (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        सचे (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        सचे (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        सचे ((p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_मानम्_Enum(p, vname.data());
            सचे (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                सचे (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    वर्ण* end;
                    द्विगुण val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    सचे (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } अञ्ञथा {
                    FAIL(ec);
                }
            }
        }

        अञ्ञथा {

            विकल्प (p) {
            पक्ष UCHAR_NUMERIC_मानम्:
                {
                    वर्ण* end;
                    द्विगुण val = uprv_strtod(vname.data(), &end);
                    सचे (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_मानम्_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    पटिनिवत्त *एतत्;
                }
            पक्ष UCHAR_NAME:
                {
                    ध्रुव UChar32 ch = getCharacterByName(vname);
                    सचे (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    पटिनिवत्त *एतत्;
                }
            पक्ष UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            पक्ष UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    पटिनिवत्त *एतत्;
                }
            पक्ष UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_मानम्_Enum(UCHAR_SCRIPT, vname.data());
                सचे (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                भङ्ग;
            पक्ष UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_मानम्_Enum(p, vname.data());
                सचे (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                भङ्ग;
            स्वभाव:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    अञ्ञथा {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_मानम्_Enum(p, pname.data());
        सचे (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_मानम्_Enum(p, pname.data());
            सचे (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                सचे (p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_LIMIT) {
                    v = 1;
                } अञ्ञथा सचे (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_मानम्, MAX_मानम्);
                    पटिनिवत्त *एतत्;
                } अञ्ञथा सचे (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    पटिनिवत्त *एतत्;
                } अञ्ञथा सचे (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = सच्च;
                } अञ्ञथा {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_मानम्(p, v, ec);
    सचे(invert) {
        complement().removeAllStrings();  // code point complement
    }

    सचे (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    पटिनिवत्त *एतत्;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(ध्रुव UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    सचे ((pos+5) > pattern.length()) {
        पटिनिवत्त मिच्छा;
    }

    // Look for an opening [:, [:^, \p, or \P
    पटिनिवत्त isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
