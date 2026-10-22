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

#include <array>
#include <optional>

#include "unicode/utypes.h"
#include "unicode/uniset.h"
#include "unicode/parsepos.h"
#include "unicode/uchar.h"
#include "unicode/uscript.h"
#include "unicode/symtable.h"
#include "unicode/uset.h"
#include "unicode/locid.h"
#include "unicode/brkiter.h"
#include "unicode/utfiterator.h"
#include "uset_imp.h"
#include "ruleiter.h"
#include "cmemory.h"
#include "ucln_cmn.h"
#include "util.h"
#include "uvector.h"
#include "uprops.h"
#include "patternprops.h"
#include "propname.h"
#include "normalizer2impl.h"
#include "uinvchar.h"
#include "uprops.h"
#include "charstr.h"
#include "cstring.h"
#include "mutex.h"
#include "umutex.h"
#include "uassert.h"
#include "hash.h"

U_NAMESPACE_USE

namespace {

// Special property set IDs
constexpr var_na ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr var_na ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr var_na ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
static UBool U_CALLCONV uset_cleanup();

static UnicodeSet *uni32Singleton;
static icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
static UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    ត្រឡប់ true;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

namespace {

// Cache some sets for other services -------------------------------------- ***
void U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = new UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    ប្រសិនបើ(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } បើមិនដូច្នេះ {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    ត្រឡប់ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

anta_hstha UBool
isPerlOpen(const UnicodeString &pattern, int32_t pos) {
    char16_t c;
    ត្រឡប់ pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

anta_hstha UBool
isNameOpen(const UnicodeString &pattern, int32_t pos) {
    ត្រឡប់ pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

anta_hstha UBool
isPOSIXOpen(const UnicodeString &pattern, int32_t pos) {
    ត្រឡប់ pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(const CharString& name) {
    សម្រាប់ (const UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        ប្រសិនបើ (U_SUCCESS(ec)) {
            ត្រឡប់ ch;
        }
    }
    ត្រឡប់ U_SENTINEL;
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
UnicodeSet::UnicodeSet(const UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(this);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(const UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    ប្រសិនបើ (U_FAILURE(status)) ត្រឡប់ *this;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, true);
    ប្រសិនបើ (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ត្រឡប់ *this;
}

void
UnicodeSet::applyPatternIgnoreSpace(const UnicodeString& pattern,
                                    ParsePosition& pos,
                                    const SymbolTable* symbols,
                                    UErrorCode& status) {
    ប្រសិនបើ (U_FAILURE(status)) {
        ត្រឡប់;
    }
    ប្រសិនបើ (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ត្រឡប់;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    ប្រសិនបើ (U_FAILURE(status)) ត្រឡប់;
    ប្រសិនបើ (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ត្រឡប់;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(const UnicodeString& pattern, int32_t pos) {
    ត្រឡប់ ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

class UnicodeSet::Lexer {
  public:
    Lexer(const UnicodeString &pattern,
          const ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          const SymbolTable *const symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    class LexicalElement {
      public:
        bool isSetOperator(const char16_t op) const {
            ត្រឡប់ U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        bool isStringLiteral() const {
            ត្រឡប់ U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        bool isNamedElement() const {
            ត្រឡប់ U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        bool isBracketedElement() const {
            ត្រឡប់ U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() const {
            ប្រសិនបើ (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ត្រឡប់ string_;
            }
            ត្រឡប់ std::nullopt;
        }

        std::optional<UChar32> codePoint() const {
            ប្រសិនបើ (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ត្រឡប់ string_.char32At(0);
            }
            ត្រឡប់ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        const UnicodeSet *set() const {
            ប្រសិនបើ (U_FAILURE(errorCode_)) {
                ត្រឡប់ nullptr;
            }
            ប្រសិនបើ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ប្រសិនបើ (precomputedSet_ != nullptr) {
                    ត្រឡប់ precomputedSet_;
                } បើមិនដូច្នេះ {
                    ត្រឡប់ &set_;
                }
            }
            ត្រឡប់ nullptr;
        }

        const UErrorCode& errorCode() const{
          ត្រឡប់ errorCode_;
        }

#ប្រសិនបើ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() const {
            UnicodeString result;
            ប្រសិនបើ (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            ត្រឡប់ result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      private:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ga_nanaa Category : std::uint8_t {
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
        static constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       const UErrorCode errorCode, const UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, bool separated,
                       std::optional<LexicalElement> const &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            ប្រសិនបើ (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                ត្រឡប់;
            }
            ប្រសិនបើ (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } បើមិនដូច្នេះ ប្រសិនបើ (preceding->category_ == ESCAPED_ELEMENT) {
                ប្រសិនបើ (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } បើមិនដូច្នេះ ប្រសិនបើ (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    ប្រសិនបើ (preceding->sourceText_[1] == u'x') {
                        ប្រសិនបើ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } បើមិនដូច្នេះ ប្រសិនបើ (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        ប្រសិនបើ (cp >= u'0' && cp <= u'7') {
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
        const UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend class Lexer;
    };

    UnicodeString getPositionForDebugging() const {
        ត្រឡប់ pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    bool acceptSetOperator(char16_t op) {
        ប្រសិនបើ (lookahead().isSetOperator(op)) {
            advance();
            ត្រឡប់ true;
        }
        ត្រឡប់ false;
    }

    const LexicalElement &lookahead() {
        ប្រសិនបើ (!ahead_.has_value()) {
            const RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        ត្រឡប់ *ahead_;
    }

    const LexicalElement &lookahead2() {
        ប្រសិនបើ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            const RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        ត្រឡប់ *ahead2_;
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
        ត្រឡប់ chars_;
    }

    int32_t charsOptions() {
        ត្រឡប់ charsOptions_;
    }

    bool atEnd() const {
        ត្រឡប់ chars_.atEnd();
    }

    void advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  private:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() const {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        ត្រឡប់ result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> const& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        bool separated = false;
        ប្រសិនបើ (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            សម្រាប់ (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                ប្រសិនបើ (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    bha_nga;
                }
                ប្រសិនបើ (s != u'\u200E' && s != u'\u200F') {
                    separated = true;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ប្រសិនបើ (chars_.atEnd()) {
            ត្រឡប់ LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        const int32_t start = parsePosition_.getIndex();
        const RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        const UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        ប្រសិនបើ (first == u'[' || first == u'\\') {
            const RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            const UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            ប្រសិនបើ ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                ប្រសិនបើ (second == u'N') {
                    UChar32 const queryResult = scanNamedElementBrackets(errorCode);
                    ត្រឡប់ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } បើមិនដូច្នេះ {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    ត្រឡប់ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        ប្រសិនបើ (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ប្រសិនបើ (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                const std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                const UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                ប្រសិនបើ (precomputedSet != nullptr) {
                    ត្រឡប់ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/true,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                const UnicodeString *const expression = symbols_->lookup(name);
                ប្រសិនបើ (expression == nullptr) {
                    ត្រឡប់ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/true,
                        /*preceding=*/std::nullopt);
                }
                ត្រឡប់ evaluateVariable(*expression, source);
            }
        }
        switch (first) {
        pak_sa u'[':
            ត្រឡប់ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        pak_sa u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            ត្រឡប់ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        pak_sa u'&':
        pak_sa u'-':
        pak_sa u']':
        pak_sa u'^':
        pak_sa u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ត្រឡប់ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        pak_sa u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t codePointCount = 0;
            ខណៈពេល (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                const RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                ប្រសិនបើ (next == u'\\') {
                    const UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    ប្រសិនបើ (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = true;
                    } បើមិនដូច្នេះ ប្រសិនបើ (afterBackslash == u'p' || afterBackslash == u'P') {
                        ត្រឡប់ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } បើមិនដូច្នេះ {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } បើមិនដូច្នេះ {
#ប្រសិនបើ U_ICU_VERSION_MAJOR_NUM < 81
                    ប្រសិនបើ (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#បើមិនដូច្នេះ
#error Remove this transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = false;
                }
                ប្រសិនបើ (!escaped && next == u'}') {
                    ត្រឡប់ LexicalElement(
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
            ត្រឡប់ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        default:
            ត្រឡប់ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        const UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        ប្រសិនបើ (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ខណៈពេល (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                ប្រសិនបើ (last == u':') {
                    ប្រសិនបើ (!hex.has_value()) {
                        hex.emplace();
                        ប្រសិនបើ (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ត្រឡប់ {};
                        }
                        សម្រាប់ (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            ប្រសិនបើ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } បើមិនដូច្នេះ {
                                digit = digit & ~0x20;
                                ប្រសិនបើ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } បើមិនដូច្នេះ {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    ត្រឡប់ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ប្រសិនបើ (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                ត្រឡប់ {};
                            }
                        }
                    } បើមិនដូច្នេះ ប្រសិនបើ (!literal.has_value()) {
                        const auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        ប្រសិនបើ (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ត្រឡប់ {};
                        }
                    } បើមិនដូច្នេះ {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ត្រឡប់ {};
                    }
                    start = parsePosition_.getIndex();
                } បើមិនដូច្នេះ ប្រសិនបើ (last == u'}') {
                    const std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    const UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    ប្រសិនបើ (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ត្រឡប់ {};
                    }
                    ប្រសិនបើ (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ត្រឡប់ {};
                    }
                    ត្រឡប់ result;
                }
            }
        }
        ប្រសិនបើ (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ត្រឡប់ {};
    }

    LexicalElement evaluateVariable(const UnicodeString &expression,
                                    const std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        ប្រសិនបើ (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expressionValue =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expressionValue.setPattern(rebuiltPattern);
            ប្រសិនបើ (!expressionLexer.atEnd()) {
                ត្រឡប់ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            }
            ត្រឡប់ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expressionValue),
                source,
                /*separated=*/true,
                /*preceding=*/std::nullopt);
        } បើមិនដូច្នេះ {
            expressionLexer.advance();
            ប្រសិនបើ (!expressionLexer.atEnd()) {
                ត្រឡប់ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            }
            switch (variableToken.category_) {
            pak_sa LexicalElement::LITERAL_ELEMENT:
            pak_sa LexicalElement::ESCAPED_ELEMENT:
            pak_sa LexicalElement::NAMED_ELEMENT:
            pak_sa LexicalElement::BRACKETED_ELEMENT:
            pak_sa LexicalElement::STRING_LITERAL:
            pak_sa LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ត្រឡប់ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            default:
                ត្រឡប់ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/true,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        bool exteriorlyNegated = false;
        bool interiorlyNegated = false;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        const UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        ប្រសិនបើ (first == u'\\') {
            ប្រសិនបើ (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ត្រឡប់ {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } បើមិនដូច្នេះ {
            ប្រសិនបើ (third == u'^') {
                exteriorlyNegated = true;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        ខណៈពេល (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            const RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            ប្រសិនបើ (penultimateUnescaped == u'\\') {
                ប្រសិនបើ (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    ប្រសិនបើ (!U_SUCCESS(errorCode)) {
                        ត្រឡប់ {};
                    }
                } បើមិនដូច្នេះ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } បើមិនដូច្នេះ ប្រសិនបើ (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                ប្រសិនបើ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ត្រឡប់ {};
                }
                interiorlyNegated = true;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } បើមិនដូច្នេះ ប្រសិនបើ ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                const int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ប្រសិនបើ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    ប្រសិនបើ (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ត្រឡប់ {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                ប្រសិនបើ (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                ត្រឡប់ result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ {};
    }

    const UnicodeString &pattern_;
    const ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    const uint32_t unicodeSetOptions_;
    const int32_t charsOptions_;
    const SymbolTable *const symbols_;
    UnicodeSet &(UnicodeSet::* const caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

namespace {

constexpr int32_t MAX_DEPTH = 100;

#ប្រសិនបើ U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    do {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ប្រសិនបើ (U_FAILURE(ec)) {                                                                       \
            ប្រសិនបើ (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } បើមិនដូច្នេះ ប្រសិនបើ (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            ត្រឡប់ result;                                                                         \
        }                                                                                          \
    } ខណៈពេល (false)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    do {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ប្រសិនបើ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } បើមិនដូច្នេះ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ត្រឡប់ result;                                                                             \
    } ខណៈពេល (false)

#បើមិនដូច្នេះ

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    do {                                                                                           \
        ប្រសិនបើ (U_FAILURE(ec)) {                                                                       \
            ត្រឡប់ result;                                                                         \
        }                                                                                          \
    } ខណៈពេល (false)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    do {                                                                                           \
        ប្រសិនបើ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } បើមិនដូច្នេះ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ត្រឡប់ result;                                                                             \
    } ខណៈពេល (false)

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

void UnicodeSet::applyPattern(const UnicodeString &pattern,
                              const ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              const SymbolTable *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    ប្រសិនបើ (U_FAILURE(ec)) ត្រឡប់;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *this = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    ប្រសិនបើ (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    bool isComplement = false;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    bool preserveSyntaxInPattern = false;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    ប្រសិនបើ (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/false);
        lexer.advance();
        preserveSyntaxInPattern = true;
    } បើមិនដូច្នេះ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ប្រសិនបើ (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            ប្រសិនបើ (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = true;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            ប្រសិនបើ (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } បើមិនដូច្នេះ {
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
    ប្រសិនបើ ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    ប្រសិនបើ (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    ប្រសិនបើ (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } បើមិនដូច្នេះ {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/false);
    }
    ត្រឡប់ result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    bool &containsSetOperation,
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
    ប្រសិនបើ (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ខណៈពេល (!lexer.atEnd()) {
        ប្រសិនបើ (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            ត្រឡប់ result;
        } បើមិនដូច្នេះ ប្រសិនបើ (lexer.lookahead().isSetOperator(u'$')) {
            ប្រសិនបើ (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = true;
                ត្រឡប់ result;
            }
        }
        ប្រសិនបើ (lexer.lookahead().isSetOperator(u']')) {
            ត្រឡប់ result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    ត្រឡប់ result;
}

void UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               bool &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ប្រសិនបើ (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = true;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } បើមិនដូច្នេះ {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

void UnicodeSet::parseSetOperations(Lexer &lexer,
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
    const UnicodeSet leftHandSide =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(leftHandSide);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    សម្រាប់ (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ប្រសិនបើ (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            const UnicodeSet rightHandSide =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(rightHandSide);
        } បើមិនដូច្នេះ ប្រសិនបើ (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ប្រសិនបើ (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ត្រឡប់;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            const UnicodeSet rightHandSide =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(rightHandSide);
        } បើមិនដូច្នេះ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ត្រឡប់;
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
    ប្រសិនបើ (lexer.lookahead().isStringLiteral()) {
        const UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/false);
        rebuiltPat.append(u'}');
        lexer.advance();
        ត្រឡប់ UnicodeSet().add(element);
    }
    UChar32 first;
    ប្រសិនបើ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } បើមិនដូច្នេះ ប្រសិនបើ (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } បើមិនដូច្នេះ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/false);
    ប្រសិនបើ (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        ត្រឡប់ UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ប្រសិនបើ (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ត្រឡប់ UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    ប្រសិនបើ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        ប្រសិនបើ (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } បើមិនដូច្នេះ ប្រសិនបើ (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } បើមិនដូច្នេះ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ប្រសិនបើ (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/false);
    ត្រឡប់ UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

namespace {

UBool numericValueFilter(UChar32 ch, void* context) {
    ត្រឡប់ u_getNumericValue(ch) == *static_cast<dviguu_na*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, void* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ត្រឡប់ (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, void* context) {
    static const UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    ត្រឡប់ uprv_memcmp(&v, &none, parimaa_na(v)) > 0 && uprv_memcmp(&v, version, parimaa_na(v)) <= 0;
}

typedef sa_nracanaa {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, void* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ត្រឡប់ u_getIntPropertyValue(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, void* context) {
    ត្រឡប់ uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, void* context) {
    ត្រឡប់ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
void UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             void* context,
                             const UnicodeSet* inclusions,
                             UErrorCode &status) {
    ប្រសិនបើ (U_FAILURE(status)) ត្រឡប់;

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

    សម្រាប់ (ចំនួនគត់ j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        សម្រាប់ (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ប្រសិនបើ ((*filter)(ch, context)) {
                ប្រសិនបើ (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    ប្រសិនបើ (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    ប្រសិនបើ (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

namespace {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    ត្រឡប់ *this; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::applyIntPropertyValue(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(ec) || isFrozen()) { ត្រឡប់ *this; }
    ប្រសិនបើ (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        const UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } បើមិនដូច្នេះ ប្រសិនបើ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        const UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } បើមិនដូច្នេះ ប្រសិនបើ (prop == UCHAR_IDENTIFIER_TYPE) {
        const UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } បើមិនដូច្នេះ ប្រសិនបើ (0 <= prop && prop < UCHAR_BINARY_LIMIT) {
        ប្រសិនបើ (value == 0 || value == 1) {
            const USet *set = u_getBinaryPropertySet(prop, &ec);
            ប្រសិនបើ (U_FAILURE(ec)) { ត្រឡប់ *this; }
            copyFrom(*UnicodeSet::fromUSet(set), true);
            ប្រសិនបើ (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } បើមិនដូច្នេះ {
            clear();
        }
    } បើមិនដូច្នេះ ប្រសិនបើ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        const UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } បើមិនដូច្នេះ {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ត្រឡប់ *this;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(const UnicodeString& prop,
                               const UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(ec) || isFrozen()) ត្រឡប់ *this;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ប្រសិនបើ( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ប្រសិនបើ (U_FAILURE(ec)) ត្រឡប់ *this;

    UProperty p;
    int32_t v;
    UBool invert = false;

    ប្រសិនបើ (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        ប្រសិនបើ (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        ប្រសិនបើ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        ប្រសិនបើ ((p >= UCHAR_BINARY_START && p < UCHAR_BINARY_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_getPropertyValueEnum(p, vname.data());
            ប្រសិនបើ (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                ប្រសិនបើ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    var_na* end;
                    dviguu_na val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ប្រសិនបើ (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } បើមិនដូច្នេះ {
                    FAIL(ec);
                }
            }
        }

        បើមិនដូច្នេះ {

            switch (p) {
            pak_sa UCHAR_NUMERIC_VALUE:
                {
                    var_na* end;
                    dviguu_na val = uprv_strtod(vname.data(), &end);
                    ប្រសិនបើ (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numericValueFilter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ត្រឡប់ *this;
                }
            pak_sa UCHAR_NAME:
                {
                    const UChar32 ch = getCharacterByName(vname);
                    ប្រសិនបើ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    ត្រឡប់ *this;
                }
            pak_sa UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            pak_sa UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ត្រឡប់ *this;
                }
            pak_sa UCHAR_SCRIPT_EXTENSIONS:
                v = u_getPropertyValueEnum(UCHAR_SCRIPT, vname.data());
                ប្រសិនបើ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                bha_nga;
            pak_sa UCHAR_IDENTIFIER_TYPE:
                v = u_getPropertyValueEnum(p, vname.data());
                ប្រសិនបើ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                bha_nga;
            default:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    បើមិនដូច្នេះ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_getPropertyValueEnum(p, pname.data());
        ប្រសិនបើ (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_getPropertyValueEnum(p, pname.data());
            ប្រសិនបើ (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                ប្រសិនបើ (p >= UCHAR_BINARY_START && p < UCHAR_BINARY_LIMIT) {
                    v = 1;
                } បើមិនដូច្នេះ ប្រសិនបើ (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_VALUE, MAX_VALUE);
                    ត្រឡប់ *this;
                } បើមិនដូច្នេះ ប្រសិនបើ (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    ត្រឡប់ *this;
                } បើមិនដូច្នេះ ប្រសិនបើ (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = true;
                } បើមិនដូច្នេះ {
                    FAIL(ec);
                }
            }
        }
    }

    applyIntPropertyValue(p, v, ec);
    ប្រសិនបើ(invert) {
        complement().removeAllStrings();  // code point complement
    }

    ប្រសិនបើ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    ត្រឡប់ *this;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(const UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ប្រសិនបើ ((pos+5) > pattern.length()) {
        ត្រឡប់ false;
    }

    // Look for an opening [:, [:^, \p, or \P
    ត្រឡប់ isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
