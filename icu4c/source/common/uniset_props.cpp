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

#ضمن <array>
#ضمن <optional>

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/symtable.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/utfiterator.h"
#ضمن "uset_imp.h"
#ضمن "ruleiter.h"
#ضمن "cmemory.h"
#ضمن "ucln_cmn.h"
#ضمن "util.h"
#ضمن "uvector.h"
#ضمن "uprops.h"
#ضمن "patternprops.h"
#ضمن "propname.h"
#ضمن "normalizer2impl.h"
#ضمن "uinvchar.h"
#ضمن "uprops.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "umutex.h"
#ضمن "uassert.h"
#ضمن "hash.h"

U_NAMESPACE_USE

نطاق {

// Special property set IDs
constexpr حرف ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr حرف ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr حرف ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uset_cleanup();

ساكن UnicodeSet *uni32Singleton;
ساكن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساكن UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    ܦܢܐ ܫܪܪܐ;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

نطاق {

// Cache some sets for other services -------------------------------------- ***
فراغ U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = جديد UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    ܐܢ(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } ܐܚܪܢܐ {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    ܦܢܐ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

مضمن UBool
isPerlOpen(ثابت UnicodeString &pattern, int32_t pos) {
    char16_t c;
    ܦܢܐ pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

مضمن UBool
isNameOpen(ثابت UnicodeString &pattern, int32_t pos) {
    ܦܢܐ pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

مضمن UBool
isPOSIXOpen(ثابت UnicodeString &pattern, int32_t pos) {
    ܦܢܐ pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(ثابت CharString& name) {
    لكل (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        ܐܢ (U_SUCCESS(ec)) {
            ܦܢܐ ch;
        }
    }
    ܦܢܐ U_SENTINEL;
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
UnicodeSet::UnicodeSet(ثابت UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ثابت UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    ܐܢ (U_FAILURE(status)) ܦܢܐ *هذا;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, ܫܪܪܐ);
    ܐܢ (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ܦܢܐ *هذا;
}

فراغ
UnicodeSet::applyPatternIgnoreSpace(ثابت UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ثابت SymbolTable* symbols,
                                    UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    ܐܢ (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ܦܢܐ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    ܐܢ (U_FAILURE(status)) ܦܢܐ;
    ܐܢ (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ܦܢܐ;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ثابت UnicodeString& pattern, int32_t pos) {
    ܦܢܐ ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

صنف UnicodeSet::Lexer {
  عام:
    Lexer(ثابت UnicodeString &pattern,
          ثابت ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ثابت SymbolTable *ثابت symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    صنف LexicalElement {
      عام:
        منطقي isSetOperator(ثابت char16_t op) ثابت {
            ܦܢܐ U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        منطقي isStringLiteral() ثابت {
            ܦܢܐ U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        منطقي isNamedElement() ثابت {
            ܦܢܐ U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        منطقي isBracketedElement() ثابت {
            ܦܢܐ U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            ܐܢ (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ܦܢܐ string_;
            }
            ܦܢܐ std::nullopt;
        }

        std::optional<UChar32> codePoint() ثابت {
            ܐܢ (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ܦܢܐ string_.char32At(0);
            }
            ܦܢܐ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت UnicodeSet *set() ثابت {
            ܐܢ (U_FAILURE(errorCode_)) {
                ܦܢܐ nullptr;
            }
            ܐܢ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ܐܢ (precomputedSet_ != nullptr) {
                    ܦܢܐ precomputedSet_;
                } ܐܚܪܢܐ {
                    ܦܢܐ &set_;
                }
            }
            ܦܢܐ nullptr;
        }

        ثابت UErrorCode& errorCode() ثابت{
          ܦܢܐ errorCode_;
        }

#ܐܢ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString result;
            ܐܢ (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            ܦܢܐ result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      خاص:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        تعداد Category : std::uint8_t {
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
        ساكن constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       ثابت UErrorCode errorCode, ثابت UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, منطقي separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            ܐܢ (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                ܦܢܐ;
            }
            ܐܢ (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } ܐܚܪܢܐ ܐܢ (preceding->category_ == ESCAPED_ELEMENT) {
                ܐܢ (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } ܐܚܪܢܐ ܐܢ (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    ܐܢ (preceding->sourceText_[1] == u'x') {
                        ܐܢ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } ܐܚܪܢܐ ܐܢ (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        ܐܢ (cp >= u'0' && cp <= u'7') {
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
        ثابت UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend صنف Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        ܦܢܐ pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    منطقي acceptSetOperator(char16_t op) {
        ܐܢ (lookahead().isSetOperator(op)) {
            advance();
            ܦܢܐ ܫܪܪܐ;
        }
        ܦܢܐ ܕܓܠܐ;
    }

    ثابت LexicalElement &lookahead() {
        ܐܢ (!ahead_.has_value()) {
            ثابت RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        ܦܢܐ *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        ܐܢ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        ܦܢܐ *ahead2_;
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
        ܦܢܐ chars_;
    }

    int32_t charsOptions() {
        ܦܢܐ charsOptions_;
    }

    منطقي atEnd() ثابت {
        ܦܢܐ chars_.atEnd();
    }

    فراغ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خاص:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ثابت {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        ܦܢܐ result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> ثابت& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        منطقي separated = ܕܓܠܐ;
        ܐܢ (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            لكل (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                ܐܢ (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    اكسر;
                }
                ܐܢ (s != u'\u200E' && s != u'\u200F') {
                    separated = ܫܪܪܐ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ܐܢ (chars_.atEnd()) {
            ܦܢܐ LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t start = parsePosition_.getIndex();
        ثابت RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        ܐܢ (first == u'[' || first == u'\\') {
            ثابت RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            ܐܢ ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                ܐܢ (second == u'N') {
                    UChar32 ثابت queryResult = scanNamedElementBrackets(errorCode);
                    ܦܢܐ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } ܐܚܪܢܐ {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    ܦܢܐ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        ܐܢ (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ܐܢ (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                ثابت std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                ثابت UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                ܐܢ (precomputedSet != nullptr) {
                    ܦܢܐ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/ܫܪܪܐ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                ܐܢ (expression == nullptr) {
                    ܦܢܐ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/ܫܪܪܐ,
                        /*preceding=*/std::nullopt);
                }
                ܦܢܐ evaluateVariable(*expression, source);
            }
        }
        بدل (first) {
        حالة u'[':
            ܦܢܐ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        حالة u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            ܦܢܐ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        حالة u'&':
        حالة u'-':
        حالة u']':
        حالة u'^':
        حالة u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ܦܢܐ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        حالة u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t codePointCount = 0;
            ܟܡܐ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ثابت RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                ܐܢ (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    ܐܢ (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = ܫܪܪܐ;
                    } ܐܚܪܢܐ ܐܢ (afterBackslash == u'p' || afterBackslash == u'P') {
                        ܦܢܐ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } ܐܚܪܢܐ {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } ܐܚܪܢܐ {
#ܐܢ U_ICU_VERSION_MAJOR_NUM < 81
                    ܐܢ (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#ܐܚܪܢܐ
#error Remove هذا transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = ܕܓܠܐ;
                }
                ܐܢ (!escaped && next == u'}') {
                    ܦܢܐ LexicalElement(
                        codePointCount == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                codePointCount += 1;
            }
            ܦܢܐ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        افتراضي:
            ܦܢܐ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        ܐܢ (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ܟܡܐ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                ܐܢ (last == u':') {
                    ܐܢ (!hex.has_value()) {
                        hex.emplace();
                        ܐܢ (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ܦܢܐ {};
                        }
                        لكل (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            ܐܢ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ܐܚܪܢܐ {
                                digit = digit & ~0x20;
                                ܐܢ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ܐܚܪܢܐ {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    ܦܢܐ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ܐܢ (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                ܦܢܐ {};
                            }
                        }
                    } ܐܚܪܢܐ ܐܢ (!literal.has_value()) {
                        ثابت auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        ܐܢ (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ܦܢܐ {};
                        }
                    } ܐܚܪܢܐ {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ܦܢܐ {};
                    }
                    start = parsePosition_.getIndex();
                } ܐܚܪܢܐ ܐܢ (last == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    ثابت UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    ܐܢ (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ܦܢܐ {};
                    }
                    ܐܢ (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ܦܢܐ {};
                    }
                    ܦܢܐ result;
                }
            }
        }
        ܐܢ (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ܦܢܐ {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        ܐܢ (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expressionValue =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expressionValue.setPattern(rebuiltPattern);
            ܐܢ (!expressionLexer.atEnd()) {
                ܦܢܐ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ܫܪܪܐ,
                    /*preceding=*/std::nullopt);
            }
            ܦܢܐ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expressionValue),
                source,
                /*separated=*/ܫܪܪܐ,
                /*preceding=*/std::nullopt);
        } ܐܚܪܢܐ {
            expressionLexer.advance();
            ܐܢ (!expressionLexer.atEnd()) {
                ܦܢܐ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ܫܪܪܐ,
                    /*preceding=*/std::nullopt);
            }
            بدل (variableToken.category_) {
            حالة LexicalElement::LITERAL_ELEMENT:
            حالة LexicalElement::ESCAPED_ELEMENT:
            حالة LexicalElement::NAMED_ELEMENT:
            حالة LexicalElement::BRACKETED_ELEMENT:
            حالة LexicalElement::STRING_LITERAL:
            حالة LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ܦܢܐ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/ܫܪܪܐ,
                    /*preceding=*/std::nullopt);
            افتراضي:
                ܦܢܐ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/ܫܪܪܐ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        منطقي exteriorlyNegated = ܕܓܠܐ;
        منطقي interiorlyNegated = ܕܓܠܐ;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        ܐܢ (first == u'\\') {
            ܐܢ (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ܦܢܐ {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } ܐܚܪܢܐ {
            ܐܢ (third == u'^') {
                exteriorlyNegated = ܫܪܪܐ;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        ܟܡܐ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ثابت RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            ܐܢ (penultimateUnescaped == u'\\') {
                ܐܢ (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    ܐܢ (!U_SUCCESS(errorCode)) {
                        ܦܢܐ {};
                    }
                } ܐܚܪܢܐ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } ܐܚܪܢܐ ܐܢ (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ܐܚܪܢܐ ܐܢ (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                ܐܢ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ܦܢܐ {};
                }
                interiorlyNegated = ܫܪܪܐ;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ܐܚܪܢܐ ܐܢ ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ثابت int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ܐܢ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    ܐܢ (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ܦܢܐ {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                ܐܢ (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                ܦܢܐ result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ {};
    }

    ثابت UnicodeString &pattern_;
    ثابت ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ثابت uint32_t unicodeSetOptions_;
    ثابت int32_t charsOptions_;
    ثابت SymbolTable *ثابت symbols_;
    UnicodeSet &(UnicodeSet::* ثابت caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نطاق {

constexpr int32_t MAX_DEPTH = 100;

#ܐܢ U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ܥܒܕ {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ܐܢ (U_FAILURE(ec)) {                                                                       \
            ܐܢ (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ܐܚܪܢܐ ܐܢ (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            ܦܢܐ result;                                                                         \
        }                                                                                          \
    } ܟܡܐ (ܕܓܠܐ)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ܥܒܕ {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ܐܢ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ܐܚܪܢܐ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ܦܢܐ result;                                                                             \
    } ܟܡܐ (ܕܓܠܐ)

#ܐܚܪܢܐ

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ܥܒܕ {                                                                                           \
        ܐܢ (U_FAILURE(ec)) {                                                                       \
            ܦܢܐ result;                                                                         \
        }                                                                                          \
    } ܟܡܐ (ܕܓܠܐ)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ܥܒܕ {                                                                                           \
        ܐܢ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ܐܚܪܢܐ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ܦܢܐ result;                                                                             \
    } ܟܡܐ (ܕܓܠܐ)

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

فراغ UnicodeSet::applyPattern(ثابت UnicodeString &pattern,
                              ثابت ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ثابت SymbolTable *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    ܐܢ (U_FAILURE(ec)) ܦܢܐ;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *هذا = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    ܐܢ (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    منطقي isComplement = ܕܓܠܐ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقي preserveSyntaxInPattern = ܕܓܠܐ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    ܐܢ (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/ܕܓܠܐ);
        lexer.advance();
        preserveSyntaxInPattern = ܫܪܪܐ;
    } ܐܚܪܢܐ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ܐܢ (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            ܐܢ (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = ܫܪܪܐ;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            ܐܢ (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } ܐܚܪܢܐ {
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
    ܐܢ ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    ܐܢ (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    ܐܢ (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } ܐܚܪܢܐ {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/ܕܓܠܐ);
    }
    ܦܢܐ result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    منطقي &containsSetOperation,
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
    ܐܢ (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ܟܡܐ (!lexer.atEnd()) {
        ܐܢ (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            ܦܢܐ result;
        } ܐܚܪܢܐ ܐܢ (lexer.lookahead().isSetOperator(u'$')) {
            ܐܢ (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = ܫܪܪܐ;
                ܦܢܐ result;
            }
        }
        ܐܢ (lexer.lookahead().isSetOperator(u']')) {
            ܦܢܐ result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    ܦܢܐ result;
}

فراغ UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               منطقي &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ܐܢ (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = ܫܪܪܐ;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ܐܚܪܢܐ {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

فراغ UnicodeSet::parseSetOperations(Lexer &lexer,
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
    ثابت UnicodeSet leftHandSide =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(leftHandSide);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    لكل (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ܐܢ (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت UnicodeSet rightHandSide =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(rightHandSide);
        } ܐܚܪܢܐ ܐܢ (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ܐܢ (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ܦܢܐ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت UnicodeSet rightHandSide =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(rightHandSide);
        } ܐܚܪܢܐ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ܦܢܐ;
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
    ܐܢ (lexer.lookahead().isStringLiteral()) {
        ثابت UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/ܕܓܠܐ);
        rebuiltPat.append(u'}');
        lexer.advance();
        ܦܢܐ UnicodeSet().add(element);
    }
    UChar32 first;
    ܐܢ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ܐܚܪܢܐ ܐܢ (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } ܐܚܪܢܐ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/ܕܓܠܐ);
    ܐܢ (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        ܦܢܐ UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ܐܢ (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ܦܢܐ UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    ܐܢ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        ܐܢ (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ܐܚܪܢܐ ܐܢ (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } ܐܚܪܢܐ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ܐܢ (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/ܕܓܠܐ);
    ܦܢܐ UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

نطاق {

UBool numericValueFilter(UChar32 ch, فراغ* context) {
    ܦܢܐ u_getNumericValue(ch) == *static_cast<مزدوج*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, فراغ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ܦܢܐ (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, فراغ* context) {
    ساكن ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    ܦܢܐ uprv_memcmp(&v, &none, حجم(v)) > 0 && uprv_memcmp(&v, version, حجم(v)) <= 0;
}

تعريفنوع بنية {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, فراغ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ܦܢܐ u_getIntPropertyValue(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, فراغ* context) {
    ܦܢܐ uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, فراغ* context) {
    ܦܢܐ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
فراغ UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             فراغ* context,
                             ثابت UnicodeSet* inclusions,
                             UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) ܦܢܐ;

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
    int32_t limitRange = inclusions->getRangeCount();

    لكل (صحيح j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        لكل (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ܐܢ ((*filter)(ch, context)) {
                ܐܢ (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } ܐܚܪܢܐ ܐܢ (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    ܐܢ (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    ܐܢ (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

نطاق {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    ܦܢܐ *هذا; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::applyIntPropertyValue(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    ܐܢ (U_FAILURE(ec) || isFrozen()) { ܦܢܐ *هذا; }
    ܐܢ (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } ܐܚܪܢܐ ܐܢ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ܐܚܪܢܐ ܐܢ (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ܐܚܪܢܐ ܐܢ (0 <= prop && prop < UCHAR_BINARY_LIMIT) {
        ܐܢ (value == 0 || value == 1) {
            ثابت USet *set = u_getBinaryPropertySet(prop, &ec);
            ܐܢ (U_FAILURE(ec)) { ܦܢܐ *هذا; }
            copyFrom(*UnicodeSet::fromUSet(set), ܫܪܪܐ);
            ܐܢ (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } ܐܚܪܢܐ {
            clear();
        }
    } ܐܚܪܢܐ ܐܢ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ܐܚܪܢܐ {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ܦܢܐ *هذا;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    ܐܢ (U_FAILURE(ec) || isFrozen()) ܦܢܐ *هذا;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ܐܢ( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ܐܢ (U_FAILURE(ec)) ܦܢܐ *هذا;

    UProperty p;
    int32_t v;
    UBool invert = ܕܓܠܐ;

    ܐܢ (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        ܐܢ (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        ܐܢ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        ܐܢ ((p >= UCHAR_BINARY_START && p < UCHAR_BINARY_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_getPropertyValueEnum(p, vname.data());
            ܐܢ (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                ܐܢ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* end;
                    مزدوج val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ܐܢ (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ܐܚܪܢܐ {
                    FAIL(ec);
                }
            }
        }

        ܐܚܪܢܐ {

            بدل (p) {
            حالة UCHAR_NUMERIC_VALUE:
                {
                    حرف* end;
                    مزدوج val = uprv_strtod(vname.data(), &end);
                    ܐܢ (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numericValueFilter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ܦܢܐ *هذا;
                }
            حالة UCHAR_NAME:
                {
                    ثابت UChar32 ch = getCharacterByName(vname);
                    ܐܢ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    ܦܢܐ *هذا;
                }
            حالة UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالة UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ܦܢܐ *هذا;
                }
            حالة UCHAR_SCRIPT_EXTENSIONS:
                v = u_getPropertyValueEnum(UCHAR_SCRIPT, vname.data());
                ܐܢ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            حالة UCHAR_IDENTIFIER_TYPE:
                v = u_getPropertyValueEnum(p, vname.data());
                ܐܢ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            افتراضي:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ܐܚܪܢܐ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_getPropertyValueEnum(p, pname.data());
        ܐܢ (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_getPropertyValueEnum(p, pname.data());
            ܐܢ (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                ܐܢ (p >= UCHAR_BINARY_START && p < UCHAR_BINARY_LIMIT) {
                    v = 1;
                } ܐܚܪܢܐ ܐܢ (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_VALUE, MAX_VALUE);
                    ܦܢܐ *هذا;
                } ܐܚܪܢܐ ܐܢ (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    ܦܢܐ *هذا;
                } ܐܚܪܢܐ ܐܢ (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = ܫܪܪܐ;
                } ܐܚܪܢܐ {
                    FAIL(ec);
                }
            }
        }
    }

    applyIntPropertyValue(p, v, ec);
    ܐܢ(invert) {
        complement().removeAllStrings();  // code point complement
    }

    ܐܢ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    ܦܢܐ *هذا;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(ثابت UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ܐܢ ((pos+5) > pattern.length()) {
        ܦܢܐ ܕܓܠܐ;
    }

    // Look for an opening [:, [:^, \p, or \P
    ܦܢܐ isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
