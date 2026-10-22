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

#அந்தர்பாவ <array>
#அந்தர்பாவ <optional>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/parsepos.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/symtable.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/utfiterator.h"
#அந்தர்பாவ "uset_imp.h"
#அந்தர்பாவ "ruleiter.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "util.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "uprops.h"
#அந்தர்பாவ "patternprops.h"
#அந்தர்பாவ "propname.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "uprops.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "hash.h"

U_NAMESPACE_USE

நாமாகாஶ {

// Special property set IDs
constexpr எழுத்து ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr எழுத்து ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr எழுத்து ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
நிலையான UBool U_CALLCONV uset_cleanup();

நிலையான UnicodeSet *uni32Singleton;
நிலையான icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
நிலையான UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    திரும்பு உண்மை;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

நாமாகாஶ {

// Cache some sets for other services -------------------------------------- ***
வெறுமை U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = புதிய UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    எனில்(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    திரும்பு uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

உள்வரி UBool
isPerlOpen(மாறிலி UnicodeString &pattern, int32_t pos) {
    char16_t c;
    திரும்பு pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

உள்வரி UBool
isNameOpen(மாறிலி UnicodeString &pattern, int32_t pos) {
    திரும்பு pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

உள்வரி UBool
isPOSIXOpen(மாறிலி UnicodeString &pattern, int32_t pos) {
    திரும்பு pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(மாறிலி CharString& name) {
    க்கு (மாறிலி UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        எனில் (U_SUCCESS(ec)) {
            திரும்பு ch;
        }
    }
    திரும்பு U_SENTINEL;
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
UnicodeSet::UnicodeSet(மாறிலி UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(இது);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(மாறிலி UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    எனில் (U_FAILURE(status)) திரும்பு *இது;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, உண்மை);
    எனில் (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    திரும்பு *இது;
}

வெறுமை
UnicodeSet::applyPatternIgnoreSpace(மாறிலி UnicodeString& pattern,
                                    ParsePosition& pos,
                                    மாறிலி குறியீடு_Table* symbols,
                                    UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    எனில் (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        திரும்பு;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    எனில் (U_FAILURE(status)) திரும்பு;
    எனில் (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        திரும்பு;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(மாறிலி UnicodeString& pattern, int32_t pos) {
    திரும்பு ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

வகுப்பு UnicodeSet::Lexer {
  பொது:
    Lexer(மாறிலி UnicodeString &pattern,
          மாறிலி ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          மாறிலி குறியீடு_Table *மாறிலி symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    வகுப்பு LexicalElement {
      பொது:
        தருக்க isSetOperator(மாறிலி char16_t op) மாறிலி {
            திரும்பு U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        தருக்க isStringLiteral() மாறிலி {
            திரும்பு U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        தருக்க isNamedElement() மாறிலி {
            திரும்பு U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        தருக்க isBracketedElement() மாறிலி {
            திரும்பு U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() மாறிலி {
            எனில் (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                திரும்பு string_;
            }
            திரும்பு std::nullopt;
        }

        std::optional<UChar32> codePoint() மாறிலி {
            எனில் (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                திரும்பு string_.char32At(0);
            }
            திரும்பு std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        மாறிலி UnicodeSet *set() மாறிலி {
            எனில் (U_FAILURE(errorCode_)) {
                திரும்பு nullptr;
            }
            எனில் (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                எனில் (precomputedSet_ != nullptr) {
                    திரும்பு precomputedSet_;
                } இல்லையெனில் {
                    திரும்பு &set_;
                }
            }
            திரும்பு nullptr;
        }

        மாறிலி UErrorCode& errorCode() மாறிலி{
          திரும்பு errorCode_;
        }

#எனில் U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() மாறிலி {
            UnicodeString result;
            எனில் (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            திரும்பு result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      தனிப்பட்ட:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        எண்ணிக்கை Category : std::uint8_t {
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
        நிலையான constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       மாறிலி UErrorCode errorCode, மாறிலி UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, தருக்க separated,
                       std::optional<LexicalElement> மாறிலி &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            எனில் (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                திரும்பு;
            }
            எனில் (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } இல்லையெனில் எனில் (preceding->category_ == ESCAPED_ELEMENT) {
                எனில் (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } இல்லையெனில் எனில் (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    எனில் (preceding->sourceText_[1] == u'x') {
                        எனில் ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } இல்லையெனில் எனில் (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        எனில் (cp >= u'0' && cp <= u'7') {
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
        மாறிலி UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend வகுப்பு Lexer;
    };

    UnicodeString getPositionForDebugging() மாறிலி {
        திரும்பு pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    தருக்க acceptSetOperator(char16_t op) {
        எனில் (lookahead().isSetOperator(op)) {
            advance();
            திரும்பு உண்மை;
        }
        திரும்பு பொய்;
    }

    மாறிலி LexicalElement &lookahead() {
        எனில் (!ahead_.has_value()) {
            மாறிலி RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        திரும்பு *ahead_;
    }

    மாறிலி LexicalElement &lookahead2() {
        எனில் (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            மாறிலி RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        திரும்பு *ahead2_;
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
        திரும்பு chars_;
    }

    int32_t charsOptions() {
        திரும்பு charsOptions_;
    }

    தருக்க atEnd() மாறிலி {
        திரும்பு chars_.atEnd();
    }

    வெறுமை advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  தனிப்பட்ட:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() மாறிலி {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        திரும்பு result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> மாறிலி& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        தருக்க separated = பொய்;
        எனில் (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            க்கு (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                எனில் (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    நிறுத்து;
                }
                எனில் (s != u'\u200E' && s != u'\u200F') {
                    separated = உண்மை;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        எனில் (chars_.atEnd()) {
            திரும்பு LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        மாறிலி int32_t start = parsePosition_.getIndex();
        மாறிலி RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        மாறிலி UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        எனில் (first == u'[' || first == u'\\') {
            மாறிலி RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            மாறிலி UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            எனில் ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                எனில் (second == u'N') {
                    UChar32 மாறிலி queryResult = scanNamedElementBrackets(errorCode);
                    திரும்பு LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } இல்லையெனில் {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    திரும்பு LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        எனில் (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            எனில் (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                மாறிலி std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                மாறிலி UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                எனில் (precomputedSet != nullptr) {
                    திரும்பு LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/உண்மை,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                மாறிலி UnicodeString *மாறிலி expression = symbols_->lookup(name);
                எனில் (expression == nullptr) {
                    திரும்பு LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/உண்மை,
                        /*preceding=*/std::nullopt);
                }
                திரும்பு evaluateVariable(*expression, source);
            }
        }
        தேர்வு (first) {
        நிலை u'[':
            திரும்பு LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        நிலை u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            திரும்பு LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        நிலை u'&':
        நிலை u'-':
        நிலை u']':
        நிலை u'^':
        நிலை u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            திரும்பு LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        நிலை u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_மொத்தம் = 0;
            போது (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                மாறிலி RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                எனில் (next == u'\\') {
                    மாறிலி UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    எனில் (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = உண்மை;
                    } இல்லையெனில் எனில் (afterBackslash == u'p' || afterBackslash == u'P') {
                        திரும்பு LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } இல்லையெனில் {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } இல்லையெனில் {
#எனில் U_ICU_VERSION_MAJOR_இலக்கம் < 81
                    எனில் (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#இல்லையெனில்
#error Remove இது transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = பொய்;
                }
                எனில் (!escaped && next == u'}') {
                    திரும்பு LexicalElement(
                        code_Point_மொத்தம் == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_மொத்தம் += 1;
            }
            திரும்பு LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        இயல்பு:
            திரும்பு LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        மாறிலி UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        எனில் (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            போது (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                எனில் (last == u':') {
                    எனில் (!hex.has_value()) {
                        hex.emplace();
                        எனில் (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            திரும்பு {};
                        }
                        க்கு (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            எனில் (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } இல்லையெனில் {
                                digit = digit & ~0x20;
                                எனில் (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } இல்லையெனில் {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    திரும்பு {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            எனில் (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                திரும்பு {};
                            }
                        }
                    } இல்லையெனில் எனில் (!literal.has_value()) {
                        மாறிலி auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        எனில் (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            திரும்பு {};
                        }
                    } இல்லையெனில் {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        திரும்பு {};
                    }
                    start = parsePosition_.getIndex();
                } இல்லையெனில் எனில் (last == u'}') {
                    மாறிலி std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    மாறிலி UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    எனில் (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        திரும்பு {};
                    }
                    எனில் (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        திரும்பு {};
                    }
                    திரும்பு result;
                }
            }
        }
        எனில் (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        திரும்பு {};
    }

    LexicalElement evaluateVariable(மாறிலி UnicodeString &expression,
                                    மாறிலி std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        எனில் (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_மதிப்பு =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_மதிப்பு.setPattern(rebuiltPattern);
            எனில் (!expressionLexer.atEnd()) {
                திரும்பு LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/உண்மை,
                    /*preceding=*/std::nullopt);
            }
            திரும்பு LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_மதிப்பு),
                source,
                /*separated=*/உண்மை,
                /*preceding=*/std::nullopt);
        } இல்லையெனில் {
            expressionLexer.advance();
            எனில் (!expressionLexer.atEnd()) {
                திரும்பு LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/உண்மை,
                    /*preceding=*/std::nullopt);
            }
            தேர்வு (variableToken.category_) {
            நிலை LexicalElement::LITERAL_ELEMENT:
            நிலை LexicalElement::ESCAPED_ELEMENT:
            நிலை LexicalElement::NAMED_ELEMENT:
            நிலை LexicalElement::BRACKETED_ELEMENT:
            நிலை LexicalElement::STRING_LITERAL:
            நிலை LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                திரும்பு LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/உண்மை,
                    /*preceding=*/std::nullopt);
            இயல்பு:
                திரும்பு LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/உண்மை,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        தருக்க exteriorlyNegated = பொய்;
        தருக்க interiorlyNegated = பொய்;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        மாறிலி UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        எனில் (first == u'\\') {
            எனில் (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } இல்லையெனில் {
            எனில் (third == u'^') {
                exteriorlyNegated = உண்மை;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        போது (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            மாறிலி RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            எனில் (penultimateUnescaped == u'\\') {
                எனில் (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    எனில் (!U_SUCCESS(errorCode)) {
                        திரும்பு {};
                    }
                } இல்லையெனில் {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } இல்லையெனில் எனில் (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } இல்லையெனில் எனில் (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                எனில் (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    திரும்பு {};
                }
                interiorlyNegated = உண்மை;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } இல்லையெனில் எனில் ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                மாறிலி int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                எனில் (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    எனில் (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        திரும்பு {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                எனில் (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                திரும்பு result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு {};
    }

    மாறிலி UnicodeString &pattern_;
    மாறிலி ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    மாறிலி uint32_t unicodeSetOptions_;
    மாறிலி int32_t charsOptions_;
    மாறிலி குறியீடு_Table *மாறிலி symbols_;
    UnicodeSet &(UnicodeSet::* மாறிலி caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

நாமாகாஶ {

constexpr int32_t MAX_DEPTH = 100;

#எனில் U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    செய் {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        எனில் (U_FAILURE(ec)) {                                                                       \
            எனில் (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } இல்லையெனில் எனில் (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            திரும்பு result;                                                                         \
        }                                                                                          \
    } போது (பொய்)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    செய் {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        எனில் (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } இல்லையெனில் {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        திரும்பு result;                                                                             \
    } போது (பொய்)

#இல்லையெனில்

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    செய் {                                                                                           \
        எனில் (U_FAILURE(ec)) {                                                                       \
            திரும்பு result;                                                                         \
        }                                                                                          \
    } போது (பொய்)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    செய் {                                                                                           \
        எனில் (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } இல்லையெனில் {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        திரும்பு result;                                                                             \
    } போது (பொய்)

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

வெறுமை UnicodeSet::applyPattern(மாறிலி UnicodeString &pattern,
                              மாறிலி ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              மாறிலி குறியீடு_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    எனில் (U_FAILURE(ec)) திரும்பு;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *இது = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    எனில் (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    தருக்க isComplement = பொய்;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    தருக்க preserveSyntaxInPattern = பொய்;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    எனில் (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/பொய்);
        lexer.advance();
        preserveSyntaxInPattern = உண்மை;
    } இல்லையெனில் {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        எனில் (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            எனில் (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = உண்மை;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            எனில் (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } இல்லையெனில் {
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
    எனில் ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    எனில் (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    எனில் (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } இல்லையெனில் {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/பொய்);
    }
    திரும்பு result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    தருக்க &containsSetOperation,
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
    எனில் (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    போது (!lexer.atEnd()) {
        எனில் (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            திரும்பு result;
        } இல்லையெனில் எனில் (lexer.lookahead().isSetOperator(u'$')) {
            எனில் (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = உண்மை;
                திரும்பு result;
            }
        }
        எனில் (lexer.lookahead().isSetOperator(u']')) {
            திரும்பு result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    திரும்பு result;
}

வெறுமை UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               தருக்க &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    எனில் (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = உண்மை;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } இல்லையெனில் {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

வெறுமை UnicodeSet::parseSetOperations(Lexer &lexer,
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
    மாறிலி UnicodeSet இடது_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(இடது_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    க்கு (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        எனில் (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            மாறிலி UnicodeSet வலது_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(வலது_Hand_Side);
        } இல்லையெனில் எனில் (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            எனில் (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                திரும்பு;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            மாறிலி UnicodeSet வலது_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(வலது_Hand_Side);
        } இல்லையெனில் {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            திரும்பு;
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
    எனில் (lexer.lookahead().isStringLiteral()) {
        மாறிலி UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/பொய்);
        rebuiltPat.append(u'}');
        lexer.advance();
        திரும்பு UnicodeSet().add(element);
    }
    UChar32 first;
    எனில் (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } இல்லையெனில் எனில் (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } இல்லையெனில் {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/பொய்);
    எனில் (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        திரும்பு UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    எனில் (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        திரும்பு UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    எனில் (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        எனில் (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } இல்லையெனில் எனில் (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } இல்லையெனில் {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    எனில் (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/பொய்);
    திரும்பு UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

நாமாகாஶ {

UBool numeric_மதிப்பு_Filter(UChar32 ch, வெறுமை* context) {
    திரும்பு u_get_Numeric_மதிப்பு(ch) == *static_cast<இரட்டை*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, வெறுமை* context) {
    int32_t value = *static_cast<int32_t*>(context);
    திரும்பு (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, வெறுமை* context) {
    நிலையான மாறிலி UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    திரும்பு uprv_memcmp(&v, &none, அளவு(v)) > 0 && uprv_memcmp(&v, version, அளவு(v)) <= 0;
}

வகைவரை கட்டமைப்பு {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, வெறுமை* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    திரும்பு u_get_Int_Property_மதிப்பு(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, வெறுமை* context) {
    திரும்பு uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, வெறுமை* context) {
    திரும்பு u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
வெறுமை UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             வெறுமை* context,
                             மாறிலி UnicodeSet* inclusions,
                             UErrorCode &status) {
    எனில் (U_FAILURE(status)) திரும்பு;

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
    int32_t limitRange = inclusions->get_Range_மொத்தம்();

    க்கு (முழு j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        க்கு (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            எனில் ((*filter)(ch, context)) {
                எனில் (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } இல்லையெனில் எனில் (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    எனில் (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    எனில் (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

நாமாகாஶ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    திரும்பு *இது; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_மதிப்பு(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(ec) || isFrozen()) { திரும்பு *இது; }
    எனில் (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        மாறிலி UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } இல்லையெனில் எனில் (prop == UCHAR_SCRIPT_EXTENSIONS) {
        மாறிலி UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } இல்லையெனில் எனில் (prop == UCHAR_IDENTIFIER_TYPE) {
        மாறிலி UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } இல்லையெனில் எனில் (0 <= prop && prop < UCHAR_இருமம்_LIMIT) {
        எனில் (value == 0 || value == 1) {
            மாறிலி USet *set = u_get_இருமம்_Property_Set(prop, &ec);
            எனில் (U_FAILURE(ec)) { திரும்பு *இது; }
            copyFrom(*UnicodeSet::fromUSet(set), உண்மை);
            எனில் (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } இல்லையெனில் {
            clear();
        }
    } இல்லையெனில் எனில் (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        மாறிலி UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } இல்லையெனில் {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    திரும்பு *இது;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(மாறிலி UnicodeString& prop,
                               மாறிலி UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(ec) || isFrozen()) திரும்பு *இது;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    எனில்( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    எனில் (U_FAILURE(ec)) திரும்பு *இது;

    UProperty p;
    int32_t v;
    UBool invert = பொய்;

    எனில் (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        எனில் (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        எனில் (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        எனில் ((p >= UCHAR_இருமம்_START && p < UCHAR_இருமம்_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_மதிப்பு_Enum(p, vname.data());
            எனில் (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                எனில் (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    எழுத்து* end;
                    இரட்டை val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    எனில் (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } இல்லையெனில் {
                    FAIL(ec);
                }
            }
        }

        இல்லையெனில் {

            தேர்வு (p) {
            நிலை UCHAR_NUMERIC_மதிப்பு:
                {
                    எழுத்து* end;
                    இரட்டை val = uprv_strtod(vname.data(), &end);
                    எனில் (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_மதிப்பு_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    திரும்பு *இது;
                }
            நிலை UCHAR_NAME:
                {
                    மாறிலி UChar32 ch = getCharacterByName(vname);
                    எனில் (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    திரும்பு *இது;
                }
            நிலை UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            நிலை UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    திரும்பு *இது;
                }
            நிலை UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_மதிப்பு_Enum(UCHAR_SCRIPT, vname.data());
                எனில் (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                நிறுத்து;
            நிலை UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_மதிப்பு_Enum(p, vname.data());
                எனில் (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                நிறுத்து;
            இயல்பு:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    இல்லையெனில் {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_மதிப்பு_Enum(p, pname.data());
        எனில் (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_மதிப்பு_Enum(p, pname.data());
            எனில் (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                எனில் (p >= UCHAR_இருமம்_START && p < UCHAR_இருமம்_LIMIT) {
                    v = 1;
                } இல்லையெனில் எனில் (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_மதிப்பு, MAX_மதிப்பு);
                    திரும்பு *இது;
                } இல்லையெனில் எனில் (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    திரும்பு *இது;
                } இல்லையெனில் எனில் (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = உண்மை;
                } இல்லையெனில் {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_மதிப்பு(p, v, ec);
    எனில்(invert) {
        complement().removeAllStrings();  // code point complement
    }

    எனில் (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு *இது;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(மாறிலி UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    எனில் ((pos+5) > pattern.length()) {
        திரும்பு பொய்;
    }

    // Look for an opening [:, [:^, \p, or \P
    திரும்பு isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
