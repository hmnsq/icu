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

#ଅନ୍ତର୍ଭାଵ <array>
#ଅନ୍ତର୍ଭାଵ <optional>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/symtable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utfiterator.h"
#ଅନ୍ତର୍ଭାଵ "uset_imp.h"
#ଅନ୍ତର୍ଭାଵ "ruleiter.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "util.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"
#ଅନ୍ତର୍ଭାଵ "patternprops.h"
#ଅନ୍ତର୍ଭାଵ "propname.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "hash.h"

U_NAMESPACE_USE

ନାମାକାଶ {

// Special property set IDs
constexpr ଅକ୍ଷର ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr ଅକ୍ଷର ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr ଅକ୍ଷର ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV uset_cleanup();

ସ୍ଥିର UnicodeSet *uni32Singleton;
ସ୍ଥିର icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ସ୍ଥିର UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

ନାମାକାଶ {

// Cache some sets for other services -------------------------------------- ***
ଶୂନ୍ୟ U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = ନୂତନ UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    ଯଦି(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    ଫେରନ୍ତୁ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

ଧାଡ଼ିଭିତରେ UBool
isPerlOpen(ଅଚଳ UnicodeString &pattern, int32_t pos) {
    char16_t c;
    ଫେରନ୍ତୁ pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

ଧାଡ଼ିଭିତରେ UBool
isNameOpen(ଅଚଳ UnicodeString &pattern, int32_t pos) {
    ଫେରନ୍ତୁ pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

ଧାଡ଼ିଭିତରେ UBool
isPOSIXOpen(ଅଚଳ UnicodeString &pattern, int32_t pos) {
    ଫେରନ୍ତୁ pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(ଅଚଳ CharString& name) {
    ପାଇଁ (ଅଚଳ UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        ଯଦି (U_SUCCESS(ec)) {
            ଫେରନ୍ତୁ ch;
        }
    }
    ଫେରନ୍ତୁ U_SENTINEL;
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
UnicodeSet::UnicodeSet(ଅଚଳ UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(ଏହି);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ଅଚଳ UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ *ଏହି;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, ସତ୍ୟ);
    ଯଦି (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ
UnicodeSet::applyPatternIgnoreSpace(ଅଚଳ UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ଅଚଳ ପ୍ରତୀକ_Table* symbols,
                                    UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;
    ଯଦି (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ଫେରନ୍ତୁ;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ଅଚଳ UnicodeString& pattern, int32_t pos) {
    ଫେରନ୍ତୁ ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

ଶ୍ରେଣୀ UnicodeSet::Lexer {
  ସର୍ବସାଧାରଣ:
    Lexer(ଅଚଳ UnicodeString &pattern,
          ଅଚଳ ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ଅଚଳ ପ୍ରତୀକ_Table *ଅଚଳ symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    ଶ୍ରେଣୀ LexicalElement {
      ସର୍ବସାଧାରଣ:
        ତାର୍କିକ isSetOperator(ଅଚଳ char16_t op) ଅଚଳ {
            ଫେରନ୍ତୁ U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        ତାର୍କିକ isStringLiteral() ଅଚଳ {
            ଫେରନ୍ତୁ U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        ତାର୍କିକ isNamedElement() ଅଚଳ {
            ଫେରନ୍ତୁ U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        ତାର୍କିକ isBracketedElement() ଅଚଳ {
            ଫେରନ୍ତୁ U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ଅଚଳ {
            ଯଦି (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ଫେରନ୍ତୁ string_;
            }
            ଫେରନ୍ତୁ std::nullopt;
        }

        std::optional<UChar32> codePoint() ଅଚଳ {
            ଯଦି (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ଫେରନ୍ତୁ string_.char32At(0);
            }
            ଫେରନ୍ତୁ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ଅଚଳ UnicodeSet *set() ଅଚଳ {
            ଯଦି (U_FAILURE(errorCode_)) {
                ଫେରନ୍ତୁ nullptr;
            }
            ଯଦି (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ଯଦି (precomputedSet_ != nullptr) {
                    ଫେରନ୍ତୁ precomputedSet_;
                } ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ &set_;
                }
            }
            ଫେରନ୍ତୁ nullptr;
        }

        ଅଚଳ UErrorCode& errorCode() ଅଚଳ{
          ଫେରନ୍ତୁ errorCode_;
        }

#ଯଦି U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ଅଚଳ {
            UnicodeString result;
            ଯଦି (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            ଫେରନ୍ତୁ result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      ବ୍ୟକ୍ତିଗତ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ଗଣନା Category : std::uint8_t {
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
        ସ୍ଥିର constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       ଅଚଳ UErrorCode errorCode, ଅଚଳ UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, ତାର୍କିକ separated,
                       std::optional<LexicalElement> ଅଚଳ &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            ଯଦି (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                ଫେରନ୍ତୁ;
            }
            ଯଦି (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } ଅନ୍ୟଥା ଯଦି (preceding->category_ == ESCAPED_ELEMENT) {
                ଯଦି (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } ଅନ୍ୟଥା ଯଦି (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    ଯଦି (preceding->sourceText_[1] == u'x') {
                        ଯଦି ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } ଅନ୍ୟଥା ଯଦି (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        ଯଦି (cp >= u'0' && cp <= u'7') {
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
        ଅଚଳ UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend ଶ୍ରେଣୀ Lexer;
    };

    UnicodeString getPositionForDebugging() ଅଚଳ {
        ଫେରନ୍ତୁ pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    ତାର୍କିକ acceptSetOperator(char16_t op) {
        ଯଦି (lookahead().isSetOperator(op)) {
            advance();
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଅଚଳ LexicalElement &lookahead() {
        ଯଦି (!ahead_.has_value()) {
            ଅଚଳ RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        ଫେରନ୍ତୁ *ahead_;
    }

    ଅଚଳ LexicalElement &lookahead2() {
        ଯଦି (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ଅଚଳ RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        ଫେରନ୍ତୁ *ahead2_;
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
        ଫେରନ୍ତୁ chars_;
    }

    int32_t charsOptions() {
        ଫେରନ୍ତୁ charsOptions_;
    }

    ତାର୍କିକ atEnd() ଅଚଳ {
        ଫେରନ୍ତୁ chars_.atEnd();
    }

    ଶୂନ୍ୟ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ବ୍ୟକ୍ତିଗତ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ଅଚଳ {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        ଫେରନ୍ତୁ result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> ଅଚଳ& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ତାର୍କିକ separated = ମିଥ୍ୟା;
        ଯଦି (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            ପାଇଁ (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                ଯଦି (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    ଭାଙ୍ଗ;
                }
                ଯଦି (s != u'\u200E' && s != u'\u200F') {
                    separated = ସତ୍ୟ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ଯଦି (chars_.atEnd()) {
            ଫେରନ୍ତୁ LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ଅଚଳ int32_t start = parsePosition_.getIndex();
        ଅଚଳ RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        ଅଚଳ UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        ଯଦି (first == u'[' || first == u'\\') {
            ଅଚଳ RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ଅଚଳ UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            ଯଦି ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                ଯଦି (second == u'N') {
                    UChar32 ଅଚଳ queryResult = scanNamedElementBrackets(errorCode);
                    ଫେରନ୍ତୁ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } ଅନ୍ୟଥା {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    ଫେରନ୍ତୁ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        ଯଦି (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ଯଦି (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                ଅଚଳ std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                ଅଚଳ UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                ଯଦି (precomputedSet != nullptr) {
                    ଫେରନ୍ତୁ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/ସତ୍ୟ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ଅଚଳ UnicodeString *ଅଚଳ expression = symbols_->lookup(name);
                ଯଦି (expression == nullptr) {
                    ଫେରନ୍ତୁ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/ସତ୍ୟ,
                        /*preceding=*/std::nullopt);
                }
                ଫେରନ୍ତୁ evaluateVariable(*expression, source);
            }
        }
        ବାଛ (first) {
        ସ୍ଥିତି u'[':
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        ସ୍ଥିତି u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        ସ୍ଥିତି u'&':
        ସ୍ଥିତି u'-':
        ସ୍ଥିତି u']':
        ସ୍ଥିତି u'^':
        ସ୍ଥିତି u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        ସ୍ଥିତି u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_ଗଣନା = 0;
            ଯେତେବେଳେ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ଅଚଳ RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                ଯଦି (next == u'\\') {
                    ଅଚଳ UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    ଯଦି (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = ସତ୍ୟ;
                    } ଅନ୍ୟଥା ଯଦି (afterBackslash == u'p' || afterBackslash == u'P') {
                        ଫେରନ୍ତୁ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } ଅନ୍ୟଥା {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } ଅନ୍ୟଥା {
#ଯଦି U_ICU_VERSION_MAJOR_ଅଙ୍କ < 81
                    ଯଦି (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#ଅନ୍ୟଥା
#error Remove ଏହି transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = ମିଥ୍ୟା;
                }
                ଯଦି (!escaped && next == u'}') {
                    ଫେରନ୍ତୁ LexicalElement(
                        code_Point_ଗଣନା == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_ଗଣନା += 1;
            }
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        ଅଚଳ UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        ଯଦି (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ଯେତେବେଳେ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                ଯଦି (last == u':') {
                    ଯଦି (!hex.has_value()) {
                        hex.emplace();
                        ଯଦି (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ଫେରନ୍ତୁ {};
                        }
                        ପାଇଁ (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            ଯଦି (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ଅନ୍ୟଥା {
                                digit = digit & ~0x20;
                                ଯଦି (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ଅନ୍ୟଥା {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    ଫେରନ୍ତୁ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ଯଦି (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                ଫେରନ୍ତୁ {};
                            }
                        }
                    } ଅନ୍ୟଥା ଯଦି (!literal.has_value()) {
                        ଅଚଳ auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        ଯଦି (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ଫେରନ୍ତୁ {};
                        }
                    } ଅନ୍ୟଥା {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ଫେରନ୍ତୁ {};
                    }
                    start = parsePosition_.getIndex();
                } ଅନ୍ୟଥା ଯଦି (last == u'}') {
                    ଅଚଳ std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    ଅଚଳ UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    ଯଦି (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ଫେରନ୍ତୁ {};
                    }
                    ଯଦି (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ଫେରନ୍ତୁ {};
                    }
                    ଫେରନ୍ତୁ result;
                }
            }
        }
        ଯଦି (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ଫେରନ୍ତୁ {};
    }

    LexicalElement evaluateVariable(ଅଚଳ UnicodeString &expression,
                                    ଅଚଳ std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        ଯଦି (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_ମୂଲ୍ୟ =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_ମୂଲ୍ୟ.setPattern(rebuiltPattern);
            ଯଦି (!expressionLexer.atEnd()) {
                ଫେରନ୍ତୁ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ସତ୍ୟ,
                    /*preceding=*/std::nullopt);
            }
            ଫେରନ୍ତୁ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_ମୂଲ୍ୟ),
                source,
                /*separated=*/ସତ୍ୟ,
                /*preceding=*/std::nullopt);
        } ଅନ୍ୟଥା {
            expressionLexer.advance();
            ଯଦି (!expressionLexer.atEnd()) {
                ଫେରନ୍ତୁ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/ସତ୍ୟ,
                    /*preceding=*/std::nullopt);
            }
            ବାଛ (variableToken.category_) {
            ସ୍ଥିତି LexicalElement::LITERAL_ELEMENT:
            ସ୍ଥିତି LexicalElement::ESCAPED_ELEMENT:
            ସ୍ଥିତି LexicalElement::NAMED_ELEMENT:
            ସ୍ଥିତି LexicalElement::BRACKETED_ELEMENT:
            ସ୍ଥିତି LexicalElement::STRING_LITERAL:
            ସ୍ଥିତି LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ଫେରନ୍ତୁ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/ସତ୍ୟ,
                    /*preceding=*/std::nullopt);
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/ସତ୍ୟ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        ତାର୍କିକ exteriorlyNegated = ମିଥ୍ୟା;
        ତାର୍କିକ interiorlyNegated = ମିଥ୍ୟା;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ଅଚଳ UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        ଯଦି (first == u'\\') {
            ଯଦି (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } ଅନ୍ୟଥା {
            ଯଦି (third == u'^') {
                exteriorlyNegated = ସତ୍ୟ;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        ଯେତେବେଳେ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ଅଚଳ RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            ଯଦି (penultimateUnescaped == u'\\') {
                ଯଦି (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    ଯଦି (!U_SUCCESS(errorCode)) {
                        ଫେରନ୍ତୁ {};
                    }
                } ଅନ୍ୟଥା {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } ଅନ୍ୟଥା ଯଦି (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ଅନ୍ୟଥା ଯଦି (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                ଯଦି (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ଫେରନ୍ତୁ {};
                }
                interiorlyNegated = ସତ୍ୟ;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } ଅନ୍ୟଥା ଯଦି ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ଅଚଳ int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ଯଦି (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    ଯଦି (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ଫେରନ୍ତୁ {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                ଯଦି (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                ଫେରନ୍ତୁ result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ {};
    }

    ଅଚଳ UnicodeString &pattern_;
    ଅଚଳ ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ଅଚଳ uint32_t unicodeSetOptions_;
    ଅଚଳ int32_t charsOptions_;
    ଅଚଳ ପ୍ରତୀକ_Table *ଅଚଳ symbols_;
    UnicodeSet &(UnicodeSet::* ଅଚଳ caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

ନାମାକାଶ {

constexpr int32_t MAX_DEPTH = 100;

#ଯଦି U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    କର {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ଯଦି (U_FAILURE(ec)) {                                                                       \
            ଯଦି (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ଅନ୍ୟଥା ଯଦି (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            ଫେରନ୍ତୁ result;                                                                         \
        }                                                                                          \
    } ଯେତେବେଳେ (ମିଥ୍ୟା)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    କର {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ଯଦି (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ଅନ୍ୟଥା {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ଫେରନ୍ତୁ result;                                                                             \
    } ଯେତେବେଳେ (ମିଥ୍ୟା)

#ଅନ୍ୟଥା

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    କର {                                                                                           \
        ଯଦି (U_FAILURE(ec)) {                                                                       \
            ଫେରନ୍ତୁ result;                                                                         \
        }                                                                                          \
    } ଯେତେବେଳେ (ମିଥ୍ୟା)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    କର {                                                                                           \
        ଯଦି (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } ଅନ୍ୟଥା {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ଫେରନ୍ତୁ result;                                                                             \
    } ଯେତେବେଳେ (ମିଥ୍ୟା)

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

ଶୂନ୍ୟ UnicodeSet::applyPattern(ଅଚଳ UnicodeString &pattern,
                              ଅଚଳ ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ଅଚଳ ପ୍ରତୀକ_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    ଯଦି (U_FAILURE(ec)) ଫେରନ୍ତୁ;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *ଏହି = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    ଯଦି (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    ତାର୍କିକ isComplement = ମିଥ୍ୟା;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    ତାର୍କିକ preserveSyntaxInPattern = ମିଥ୍ୟା;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    ଯଦି (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/ମିଥ୍ୟା);
        lexer.advance();
        preserveSyntaxInPattern = ସତ୍ୟ;
    } ଅନ୍ୟଥା {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ଯଦି (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            ଯଦି (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = ସତ୍ୟ;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            ଯଦି (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } ଅନ୍ୟଥା {
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
    ଯଦି ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    ଯଦି (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    ଯଦି (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } ଅନ୍ୟଥା {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/ମିଥ୍ୟା);
    }
    ଫେରନ୍ତୁ result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    ତାର୍କିକ &containsSetOperation,
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
    ଯଦି (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ଯେତେବେଳେ (!lexer.atEnd()) {
        ଯଦି (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            ଫେରନ୍ତୁ result;
        } ଅନ୍ୟଥା ଯଦି (lexer.lookahead().isSetOperator(u'$')) {
            ଯଦି (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = ସତ୍ୟ;
                ଫେରନ୍ତୁ result;
            }
        }
        ଯଦି (lexer.lookahead().isSetOperator(u']')) {
            ଫେରନ୍ତୁ result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               ତାର୍କିକ &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ଯଦି (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = ସତ୍ୟ;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ଅନ୍ୟଥା {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

ଶୂନ୍ୟ UnicodeSet::parseSetOperations(Lexer &lexer,
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
    ଅଚଳ UnicodeSet ବାମ_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(ବାମ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    ପାଇଁ (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ଯଦି (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ଅଚଳ UnicodeSet ଡାହାଣ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ଡାହାଣ_Hand_Side);
        } ଅନ୍ୟଥା ଯଦି (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ଯଦି (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ଫେରନ୍ତୁ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ଅଚଳ UnicodeSet ଡାହାଣ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ଡାହାଣ_Hand_Side);
        } ଅନ୍ୟଥା {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ଫେରନ୍ତୁ;
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
    ଯଦି (lexer.lookahead().isStringLiteral()) {
        ଅଚଳ UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/ମିଥ୍ୟା);
        rebuiltPat.append(u'}');
        lexer.advance();
        ଫେରନ୍ତୁ UnicodeSet().add(element);
    }
    UChar32 first;
    ଯଦି (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ଅନ୍ୟଥା ଯଦି (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } ଅନ୍ୟଥା {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/ମିଥ୍ୟା);
    ଯଦି (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        ଫେରନ୍ତୁ UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ଯଦି (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ଫେରନ୍ତୁ UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    ଯଦି (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        ଯଦି (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ଅନ୍ୟଥା ଯଦି (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } ଅନ୍ୟଥା {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ଯଦି (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/ମିଥ୍ୟା);
    ଫେରନ୍ତୁ UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

ନାମାକାଶ {

UBool numeric_ମୂଲ୍ୟ_Filter(UChar32 ch, ଶୂନ୍ୟ* context) {
    ଫେରନ୍ତୁ u_get_Numeric_ମୂଲ୍ୟ(ch) == *static_cast<ଦ୍ୱିଗୁଣ*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, ଶୂନ୍ୟ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ଫେରନ୍ତୁ (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, ଶୂନ୍ୟ* context) {
    ସ୍ଥିର ଅଚଳ UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    ଫେରନ୍ତୁ uprv_memcmp(&v, &none, ଆକାର(v)) > 0 && uprv_memcmp(&v, version, ଆକାର(v)) <= 0;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, ଶୂନ୍ୟ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ଫେରନ୍ତୁ u_get_Int_Property_ମୂଲ୍ୟ(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, ଶୂନ୍ୟ* context) {
    ଫେରନ୍ତୁ uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, ଶୂନ୍ୟ* context) {
    ଫେରନ୍ତୁ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
ଶୂନ୍ୟ UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             ଶୂନ୍ୟ* context,
                             ଅଚଳ UnicodeSet* inclusions,
                             UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ;

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
    int32_t limitRange = inclusions->get_Range_ଗଣନା();

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        ପାଇଁ (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ଯଦି ((*filter)(ch, context)) {
                ଯଦି (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } ଅନ୍ୟଥା ଯଦି (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    ଯଦି (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    ଯଦି (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

ନାମାକାଶ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    ଫେରନ୍ତୁ *ଏହି; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_ମୂଲ୍ୟ(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(ec) || isFrozen()) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ଅଚଳ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } ଅନ୍ୟଥା ଯଦି (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ଅଚଳ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ଅନ୍ୟଥା ଯଦି (prop == UCHAR_IDENTIFIER_TYPE) {
        ଅଚଳ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ଅନ୍ୟଥା ଯଦି (0 <= prop && prop < UCHAR_ଦ୍ୱିମିକ_LIMIT) {
        ଯଦି (value == 0 || value == 1) {
            ଅଚଳ USet *set = u_get_ଦ୍ୱିମିକ_Property_Set(prop, &ec);
            ଯଦି (U_FAILURE(ec)) { ଫେରନ୍ତୁ *ଏହି; }
            copyFrom(*UnicodeSet::fromUSet(set), ସତ୍ୟ);
            ଯଦି (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } ଅନ୍ୟଥା {
            clear();
        }
    } ଅନ୍ୟଥା ଯଦି (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ଅଚଳ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ଅନ୍ୟଥା {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ଅଚଳ UnicodeString& prop,
                               ଅଚଳ UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(ec) || isFrozen()) ଫେରନ୍ତୁ *ଏହି;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ଯଦି( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ଯଦି (U_FAILURE(ec)) ଫେରନ୍ତୁ *ଏହି;

    UProperty p;
    int32_t v;
    UBool invert = ମିଥ୍ୟା;

    ଯଦି (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        ଯଦି (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        ଯଦି (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        ଯଦି ((p >= UCHAR_ଦ୍ୱିମିକ_START && p < UCHAR_ଦ୍ୱିମିକ_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_ମୂଲ୍ୟ_Enum(p, vname.data());
            ଯଦି (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                ଯଦି (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    ଅକ୍ଷର* end;
                    ଦ୍ୱିଗୁଣ val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ଯଦି (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ଅନ୍ୟଥା {
                    FAIL(ec);
                }
            }
        }

        ଅନ୍ୟଥା {

            ବାଛ (p) {
            ସ୍ଥିତି UCHAR_NUMERIC_ମୂଲ୍ୟ:
                {
                    ଅକ୍ଷର* end;
                    ଦ୍ୱିଗୁଣ val = uprv_strtod(vname.data(), &end);
                    ଯଦି (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_ମୂଲ୍ୟ_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ଫେରନ୍ତୁ *ଏହି;
                }
            ସ୍ଥିତି UCHAR_NAME:
                {
                    ଅଚଳ UChar32 ch = getCharacterByName(vname);
                    ଯଦି (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    ଫେରନ୍ତୁ *ଏହି;
                }
            ସ୍ଥିତି UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            ସ୍ଥିତି UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ଫେରନ୍ତୁ *ଏହି;
                }
            ସ୍ଥିତି UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_ମୂଲ୍ୟ_Enum(UCHAR_SCRIPT, vname.data());
                ଯଦି (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_ମୂଲ୍ୟ_Enum(p, vname.data());
                ଯଦି (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ଅନ୍ୟଥା {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_ମୂଲ୍ୟ_Enum(p, pname.data());
        ଯଦି (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_ମୂଲ୍ୟ_Enum(p, pname.data());
            ଯଦି (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                ଯଦି (p >= UCHAR_ଦ୍ୱିମିକ_START && p < UCHAR_ଦ୍ୱିମିକ_LIMIT) {
                    v = 1;
                } ଅନ୍ୟଥା ଯଦି (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_ମୂଲ୍ୟ, MAX_ମୂଲ୍ୟ);
                    ଫେରନ୍ତୁ *ଏହି;
                } ଅନ୍ୟଥା ଯଦି (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    ଫେରନ୍ତୁ *ଏହି;
                } ଅନ୍ୟଥା ଯଦି (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = ସତ୍ୟ;
                } ଅନ୍ୟଥା {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_ମୂଲ୍ୟ(p, v, ec);
    ଯଦି(invert) {
        complement().removeAllStrings();  // code point complement
    }

    ଯଦି (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(ଅଚଳ UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ଯଦି ((pos+5) > pattern.length()) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    // Look for an opening [:, [:^, \p, or \P
    ଫେରନ୍ତୁ isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
