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

#شامل <array>
#شامل <optional>

#شامل "unicode/utypes.h"
#شامل "unicode/uniset.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uscript.h"
#شامل "unicode/symtable.h"
#شامل "unicode/uset.h"
#شامل "unicode/locid.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/utfiterator.h"
#شامل "uset_imp.h"
#شامل "ruleiter.h"
#شامل "cmemory.h"
#شامل "ucln_cmn.h"
#شامل "util.h"
#شامل "uvector.h"
#شامل "uprops.h"
#شامل "patternprops.h"
#شامل "propname.h"
#شامل "normalizer2impl.h"
#شامل "uinvchar.h"
#شامل "uprops.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "umutex.h"
#شامل "uassert.h"
#شامل "hash.h"

U_NAMESPACE_USE

فضاینام {

// Special property set IDs
constexpr نویسه ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr نویسه ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr نویسه ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ایستا UBool U_CALLCONV uset_cleanup();

ایستا UnicodeSet *uni32Singleton;
ایستا icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ایستا UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    بازگشت درست;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

فضاینام {

// Cache some sets for other services -------------------------------------- ***
تهی U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = جدید UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    اگر(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    بازگشت uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

درونخطی UBool
isPerlOpen(ثابت UnicodeString &pattern, int32_t pos) {
    char16_t c;
    بازگشت pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

درونخطی UBool
isNameOpen(ثابت UnicodeString &pattern, int32_t pos) {
    بازگشت pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

درونخطی UBool
isPOSIXOpen(ثابت UnicodeString &pattern, int32_t pos) {
    بازگشت pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
    برای (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        اگر (U_SUCCESS(ec)) {
            بازگشت ch;
        }
    }
    بازگشت U_SENTINEL;
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
    _dbgct(این);
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
    اگر (U_FAILURE(status)) بازگشت *این;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, درست);
    اگر (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت *این;
}

تهی
UnicodeSet::applyPatternIgnoreSpace(ثابت UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ثابت نماد_Table* symbols,
                                    UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    اگر (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        بازگشت;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    اگر (U_FAILURE(status)) بازگشت;
    اگر (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        بازگشت;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ثابت UnicodeString& pattern, int32_t pos) {
    بازگشت ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

رده UnicodeSet::Lexer {
  عمومی:
    Lexer(ثابت UnicodeString &pattern,
          ثابت ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ثابت نماد_Table *ثابت symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    رده LexicalElement {
      عمومی:
        منطقی isSetOperator(ثابت char16_t op) ثابت {
            بازگشت U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        منطقی isStringLiteral() ثابت {
            بازگشت U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        منطقی isNamedElement() ثابت {
            بازگشت U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        منطقی isBracketedElement() ثابت {
            بازگشت U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            اگر (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                بازگشت string_;
            }
            بازگشت std::nullopt;
        }

        std::optional<UChar32> codePoint() ثابت {
            اگر (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                بازگشت string_.char32At(0);
            }
            بازگشت std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت UnicodeSet *set() ثابت {
            اگر (U_FAILURE(errorCode_)) {
                بازگشت nullptr;
            }
            اگر (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                اگر (precomputedSet_ != nullptr) {
                    بازگشت precomputedSet_;
                } وگرنه {
                    بازگشت &set_;
                }
            }
            بازگشت nullptr;
        }

        ثابت UErrorCode& errorCode() ثابت{
          بازگشت errorCode_;
        }

#اگر U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString result;
            اگر (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            بازگشت result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      خصوصی:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        شمارش Category : std::uint8_t {
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
        ایستا constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       std::u16string_view sourceText, منطقی separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            اگر (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                بازگشت;
            }
            اگر (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } وگرنه اگر (preceding->category_ == ESCAPED_ELEMENT) {
                اگر (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } وگرنه اگر (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    اگر (preceding->sourceText_[1] == u'x') {
                        اگر ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } وگرنه اگر (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        اگر (cp >= u'0' && cp <= u'7') {
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

        friend رده Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        بازگشت pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    منطقی acceptSetOperator(char16_t op) {
        اگر (lookahead().isSetOperator(op)) {
            advance();
            بازگشت درست;
        }
        بازگشت نادرست;
    }

    ثابت LexicalElement &lookahead() {
        اگر (!ahead_.has_value()) {
            ثابت RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        بازگشت *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        اگر (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        بازگشت *ahead2_;
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
        بازگشت chars_;
    }

    int32_t charsOptions() {
        بازگشت charsOptions_;
    }

    منطقی atEnd() ثابت {
        بازگشت chars_.atEnd();
    }

    تهی advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خصوصی:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ثابت {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        بازگشت result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> ثابت& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        منطقی separated = نادرست;
        اگر (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            برای (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                اگر (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    بشکن;
                }
                اگر (s != u'\u200E' && s != u'\u200F') {
                    separated = درست;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        اگر (chars_.atEnd()) {
            بازگشت LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
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
        اگر (first == u'[' || first == u'\\') {
            ثابت RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            اگر ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                اگر (second == u'N') {
                    UChar32 ثابت queryResult = scanNamedElementBrackets(errorCode);
                    بازگشت LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } وگرنه {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    بازگشت LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        اگر (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            اگر (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                ثابت std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                ثابت UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                اگر (precomputedSet != nullptr) {
                    بازگشت LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/درست,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                اگر (expression == nullptr) {
                    بازگشت LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/درست,
                        /*preceding=*/std::nullopt);
                }
                بازگشت evaluateVariable(*expression, source);
            }
        }
        انتخاب (first) {
        حالت u'[':
            بازگشت LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        حالت u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            بازگشت LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        حالت u'&':
        حالت u'-':
        حالت u']':
        حالت u'^':
        حالت u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            بازگشت LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        حالت u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_شمار = 0;
            وقتی (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ثابت RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                اگر (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    اگر (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = درست;
                    } وگرنه اگر (afterBackslash == u'p' || afterBackslash == u'P') {
                        بازگشت LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } وگرنه {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } وگرنه {
#اگر U_ICU_VERSION_MAJOR_رقم < 81
                    اگر (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#وگرنه
#error Remove این transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = نادرست;
                }
                اگر (!escaped && next == u'}') {
                    بازگشت LexicalElement(
                        code_Point_شمار == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_شمار += 1;
            }
            بازگشت LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        پیشفرض:
            بازگشت LexicalElement(
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
        اگر (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            وقتی (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                اگر (last == u':') {
                    اگر (!hex.has_value()) {
                        hex.emplace();
                        اگر (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            بازگشت {};
                        }
                        برای (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            اگر (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } وگرنه {
                                digit = digit & ~0x20;
                                اگر (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } وگرنه {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    بازگشت {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            اگر (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                بازگشت {};
                            }
                        }
                    } وگرنه اگر (!literal.has_value()) {
                        ثابت auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        اگر (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            بازگشت {};
                        }
                    } وگرنه {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        بازگشت {};
                    }
                    start = parsePosition_.getIndex();
                } وگرنه اگر (last == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    ثابت UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    اگر (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        بازگشت {};
                    }
                    اگر (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        بازگشت {};
                    }
                    بازگشت result;
                }
            }
        }
        اگر (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        بازگشت {};
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
        اگر (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_مقدار =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_مقدار.setPattern(rebuiltPattern);
            اگر (!expressionLexer.atEnd()) {
                بازگشت LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/درست,
                    /*preceding=*/std::nullopt);
            }
            بازگشت LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_مقدار),
                source,
                /*separated=*/درست,
                /*preceding=*/std::nullopt);
        } وگرنه {
            expressionLexer.advance();
            اگر (!expressionLexer.atEnd()) {
                بازگشت LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/درست,
                    /*preceding=*/std::nullopt);
            }
            انتخاب (variableToken.category_) {
            حالت LexicalElement::LITERAL_ELEMENT:
            حالت LexicalElement::ESCAPED_ELEMENT:
            حالت LexicalElement::NAMED_ELEMENT:
            حالت LexicalElement::BRACKETED_ELEMENT:
            حالت LexicalElement::STRING_LITERAL:
            حالت LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                بازگشت LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/درست,
                    /*preceding=*/std::nullopt);
            پیشفرض:
                بازگشت LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/درست,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        منطقی exteriorlyNegated = نادرست;
        منطقی interiorlyNegated = نادرست;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        اگر (first == u'\\') {
            اگر (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } وگرنه {
            اگر (third == u'^') {
                exteriorlyNegated = درست;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        وقتی (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ثابت RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            اگر (penultimateUnescaped == u'\\') {
                اگر (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    اگر (!U_SUCCESS(errorCode)) {
                        بازگشت {};
                    }
                } وگرنه {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } وگرنه اگر (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } وگرنه اگر (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                اگر (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    بازگشت {};
                }
                interiorlyNegated = درست;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } وگرنه اگر ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
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
                اگر (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    اگر (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        بازگشت {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                اگر (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                بازگشت result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت {};
    }

    ثابت UnicodeString &pattern_;
    ثابت ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ثابت uint32_t unicodeSetOptions_;
    ثابت int32_t charsOptions_;
    ثابت نماد_Table *ثابت symbols_;
    UnicodeSet &(UnicodeSet::* ثابت caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

فضاینام {

constexpr int32_t MAX_DEPTH = 100;

#اگر U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    انجامبده {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        اگر (U_FAILURE(ec)) {                                                                       \
            اگر (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } وگرنه اگر (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            بازگشت result;                                                                         \
        }                                                                                          \
    } وقتی (نادرست)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    انجامبده {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        اگر (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } وگرنه {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        بازگشت result;                                                                             \
    } وقتی (نادرست)

#وگرنه

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    انجامبده {                                                                                           \
        اگر (U_FAILURE(ec)) {                                                                       \
            بازگشت result;                                                                         \
        }                                                                                          \
    } وقتی (نادرست)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    انجامبده {                                                                                           \
        اگر (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } وگرنه {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        بازگشت result;                                                                             \
    } وقتی (نادرست)

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

تهی UnicodeSet::applyPattern(ثابت UnicodeString &pattern,
                              ثابت ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ثابت نماد_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    اگر (U_FAILURE(ec)) بازگشت;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *این = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    اگر (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    منطقی isComplement = نادرست;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقی preserveSyntaxInPattern = نادرست;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    اگر (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/نادرست);
        lexer.advance();
        preserveSyntaxInPattern = درست;
    } وگرنه {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        اگر (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            اگر (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = درست;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            اگر (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } وگرنه {
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
    اگر ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    اگر (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    اگر (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } وگرنه {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/نادرست);
    }
    بازگشت result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    منطقی &containsSetOperation,
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
    اگر (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    وقتی (!lexer.atEnd()) {
        اگر (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            بازگشت result;
        } وگرنه اگر (lexer.lookahead().isSetOperator(u'$')) {
            اگر (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = درست;
                بازگشت result;
            }
        }
        اگر (lexer.lookahead().isSetOperator(u']')) {
            بازگشت result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    بازگشت result;
}

تهی UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               منطقی &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    اگر (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = درست;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } وگرنه {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

تهی UnicodeSet::parseSetOperations(Lexer &lexer,
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
    ثابت UnicodeSet چپ_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(چپ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    برای (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        اگر (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت UnicodeSet راست_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(راست_Hand_Side);
        } وگرنه اگر (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            اگر (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                بازگشت;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت UnicodeSet راست_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(راست_Hand_Side);
        } وگرنه {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            بازگشت;
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
    اگر (lexer.lookahead().isStringLiteral()) {
        ثابت UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/نادرست);
        rebuiltPat.append(u'}');
        lexer.advance();
        بازگشت UnicodeSet().add(element);
    }
    UChar32 first;
    اگر (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } وگرنه اگر (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } وگرنه {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/نادرست);
    اگر (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        بازگشت UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    اگر (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        بازگشت UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    اگر (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        اگر (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } وگرنه اگر (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } وگرنه {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    اگر (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/نادرست);
    بازگشت UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

فضاینام {

UBool numeric_مقدار_Filter(UChar32 ch, تهی* context) {
    بازگشت u_get_Numeric_مقدار(ch) == *static_cast<دوگانه*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, تهی* context) {
    int32_t value = *static_cast<int32_t*>(context);
    بازگشت (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, تهی* context) {
    ایستا ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    بازگشت uprv_memcmp(&v, &none, اندازه(v)) > 0 && uprv_memcmp(&v, version, اندازه(v)) <= 0;
}

تعریفنوع ساختار {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, تهی* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    بازگشت u_get_Int_Property_مقدار(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, تهی* context) {
    بازگشت uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, تهی* context) {
    بازگشت u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
تهی UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             تهی* context,
                             ثابت UnicodeSet* inclusions,
                             UErrorCode &status) {
    اگر (U_FAILURE(status)) بازگشت;

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
    int32_t limitRange = inclusions->get_Range_شمار();

    برای (صحیح j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        برای (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            اگر ((*filter)(ch, context)) {
                اگر (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } وگرنه اگر (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    اگر (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    اگر (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

فضاینام {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    بازگشت *این; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_مقدار(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    اگر (U_FAILURE(ec) || isFrozen()) { بازگشت *این; }
    اگر (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } وگرنه اگر (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } وگرنه اگر (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } وگرنه اگر (0 <= prop && prop < UCHAR_دودویی_LIMIT) {
        اگر (value == 0 || value == 1) {
            ثابت USet *set = u_get_دودویی_Property_Set(prop, &ec);
            اگر (U_FAILURE(ec)) { بازگشت *این; }
            copyFrom(*UnicodeSet::fromUSet(set), درست);
            اگر (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } وگرنه {
            clear();
        }
    } وگرنه اگر (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ثابت UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } وگرنه {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    بازگشت *این;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    اگر (U_FAILURE(ec) || isFrozen()) بازگشت *این;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    اگر( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    اگر (U_FAILURE(ec)) بازگشت *این;

    UProperty p;
    int32_t v;
    UBool invert = نادرست;

    اگر (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        اگر (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        اگر (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        اگر ((p >= UCHAR_دودویی_START && p < UCHAR_دودویی_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_مقدار_Enum(p, vname.data());
            اگر (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                اگر (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    نویسه* end;
                    دوگانه val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    اگر (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } وگرنه {
                    FAIL(ec);
                }
            }
        }

        وگرنه {

            انتخاب (p) {
            حالت UCHAR_NUMERIC_مقدار:
                {
                    نویسه* end;
                    دوگانه val = uprv_strtod(vname.data(), &end);
                    اگر (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_مقدار_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    بازگشت *این;
                }
            حالت UCHAR_NAME:
                {
                    ثابت UChar32 ch = getCharacterByName(vname);
                    اگر (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    بازگشت *این;
                }
            حالت UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالت UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    بازگشت *این;
                }
            حالت UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_مقدار_Enum(UCHAR_SCRIPT, vname.data());
                اگر (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                بشکن;
            حالت UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_مقدار_Enum(p, vname.data());
                اگر (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                بشکن;
            پیشفرض:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    وگرنه {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_مقدار_Enum(p, pname.data());
        اگر (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_مقدار_Enum(p, pname.data());
            اگر (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                اگر (p >= UCHAR_دودویی_START && p < UCHAR_دودویی_LIMIT) {
                    v = 1;
                } وگرنه اگر (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_مقدار, MAX_مقدار);
                    بازگشت *این;
                } وگرنه اگر (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    بازگشت *این;
                } وگرنه اگر (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = درست;
                } وگرنه {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_مقدار(p, v, ec);
    اگر(invert) {
        complement().removeAllStrings();  // code point complement
    }

    اگر (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت *این;
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
    اگر ((pos+5) > pattern.length()) {
        بازگشت نادرست;
    }

    // Look for an opening [:, [:^, \p, or \P
    بازگشت isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
