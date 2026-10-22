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

#অন্তর্ভাব <array>
#অন্তর্ভাব <optional>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "unicode/symtable.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/utfiterator.h"
#অন্তর্ভাব "uset_imp.h"
#অন্তর্ভাব "ruleiter.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "util.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "uprops.h"
#অন্তর্ভাব "patternprops.h"
#অন্তর্ভাব "propname.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "uprops.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "hash.h"

U_NAMESPACE_USE

নামাকাশ {

// Special property set IDs
constexpr অক্ষর ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr অক্ষর ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr অক্ষর ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
স্থির UBool U_CALLCONV uset_cleanup();

স্থির UnicodeSet *uni32Singleton;
স্থির icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
স্থির UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    ফেরত সত্য;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

নামাকাশ {

// Cache some sets for other services -------------------------------------- ***
শূন্য U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = নতুন UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    যদি(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    ফেরত uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

অন্তঃরেখ UBool
isPerlOpen(ধ্রুবক UnicodeString &pattern, int32_t pos) {
    char16_t c;
    ফেরত pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

অন্তঃরেখ UBool
isNameOpen(ধ্রুবক UnicodeString &pattern, int32_t pos) {
    ফেরত pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

অন্তঃরেখ UBool
isPOSIXOpen(ধ্রুবক UnicodeString &pattern, int32_t pos) {
    ফেরত pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(ধ্রুবক CharString& name) {
    জন্য (ধ্রুবক UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.তথ্য(), &ec);
        যদি (U_SUCCESS(ec)) {
            ফেরত ch;
        }
    }
    ফেরত U_SENTINEL;
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
UnicodeSet::UnicodeSet(ধ্রুবক UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(এই);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(ধ্রুবক UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    যদি (U_FAILURE(status)) ফেরত *এই;

    int32_t i = pos.get_সূচক();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, সত্য);
    যদি (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত *এই;
}

শূন্য
UnicodeSet::applyPatternIgnoreSpace(ধ্রুবক UnicodeString& pattern,
                                    ParsePosition& pos,
                                    ধ্রুবক প্রতীক_Table* symbols,
                                    UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        ফেরত;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    যদি (U_FAILURE(status)) ফেরত;
    যদি (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        ফেরত;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(ধ্রুবক UnicodeString& pattern, int32_t pos) {
    ফেরত ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

শ্রেণী UnicodeSet::Lexer {
  সর্বজনীন:
    Lexer(ধ্রুবক UnicodeString &pattern,
          ধ্রুবক ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          ধ্রুবক প্রতীক_Table *ধ্রুবক symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    শ্রেণী LexicalElement {
      সর্বজনীন:
        যৌক্তিক isSetOperator(ধ্রুবক char16_t op) ধ্রুবক {
            ফেরত U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        যৌক্তিক isStringLiteral() ধ্রুবক {
            ফেরত U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        যৌক্তিক isNamedElement() ধ্রুবক {
            ফেরত U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        যৌক্তিক isBracketedElement() ধ্রুবক {
            ফেরত U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ধ্রুবক {
            যদি (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                ফেরত string_;
            }
            ফেরত std::nullopt;
        }

        std::optional<UChar32> codePoint() ধ্রুবক {
            যদি (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                ফেরত string_.char32At(0);
            }
            ফেরত std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ধ্রুবক UnicodeSet *set() ধ্রুবক {
            যদি (U_FAILURE(errorCode_)) {
                ফেরত nullptr;
            }
            যদি (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                যদি (precomputedSet_ != nullptr) {
                    ফেরত precomputedSet_;
                } অন্যথা {
                    ফেরত &set_;
                }
            }
            ফেরত nullptr;
        }

        ধ্রুবক UErrorCode& errorCode() ধ্রুবক{
          ফেরত errorCode_;
        }

#যদি U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ধ্রুবক {
            UnicodeString ফলাফল;
            যদি (U_FAILURE(errorCode_)) {
                ফলাফল.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            ফেরত ফলাফল.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      ব্যক্তিগত:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        গণনা Category : std::uint8_t {
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
        স্থির constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       ধ্রুবক UErrorCode errorCode, ধ্রুবক UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, যৌক্তিক separated,
                       std::optional<LexicalElement> ধ্রুবক &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            যদি (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                ফেরত;
            }
            যদি (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } অন্যথা যদি (preceding->category_ == ESCAPED_ELEMENT) {
                যদি (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } অন্যথা যদি (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    যদি (preceding->sourceText_[1] == u'x') {
                        যদি ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } অন্যথা যদি (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        যদি (cp >= u'0' && cp <= u'7') {
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
        ধ্রুবক UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend শ্রেণী Lexer;
    };

    UnicodeString getPositionForDebugging() ধ্রুবক {
        ফেরত pattern_.অস্থায়ী_Sub_String(0, parsePosition_.get_সূচক()) + u"☞" +
               pattern_.অস্থায়ী_Sub_String(parsePosition_.get_সূচক(), 60);
    }

    যৌক্তিক acceptSetOperator(char16_t op) {
        যদি (lookahead().isSetOperator(op)) {
            advance();
            ফেরত সত্য;
        }
        ফেরত মিথ্যা;
    }

    ধ্রুবক LexicalElement &lookahead() {
        যদি (!ahead_.has_value()) {
            ধ্রুবক RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(পরবর্তী_Token(behind_));
            chars_.setPos(before);
        }
        ফেরত *ahead_;
    }

    ধ্রুবক LexicalElement &lookahead2() {
        যদি (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ধ্রুবক RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(পরবর্তী_Token(ahead_));
            chars_.setPos(before);
        }
        ফেরত *ahead2_;
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
        ফেরত chars_;
    }

    int32_t charsOptions() {
        ফেরত charsOptions_;
    }

    যৌক্তিক atEnd() ধ্রুবক {
        ফেরত chars_.atEnd();
    }

    শূন্য advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ব্যক্তিগত:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() ধ্রুবক {
        RuleCharacterIterator::Pos ফলাফল;
        chars_.getPos(ফলাফল);
        ফেরত ফলাফল;
    }

    LexicalElement পরবর্তী_Token(std::optional<LexicalElement> ধ্রুবক& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        যৌক্তিক separated = মিথ্যা;
        যদি (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            জন্য (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                যদি (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    বিরতি;
                }
                যদি (s != u'\u200E' && s != u'\u200F') {
                    separated = সত্য;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        যদি (chars_.atEnd()) {
            ফেরত LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ধ্রুবক int32_t start = parsePosition_.get_সূচক();
        ধ্রুবক RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        ধ্রুবক UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        যদি (first == u'[' || first == u'\\') {
            ধ্রুবক RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ধ্রুবক UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            যদি ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                যদি (second == u'N') {
                    UChar32 ধ্রুবক query_ফলাফল = scanNamedElementBrackets(errorCode);
                    ফেরত LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_ফলাফল), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                        separated, preceding);
                } অন্যথা {
                    UnicodeSet query_ফলাফল = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    ফেরত LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(query_ফলাফল),
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        যদি (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            যদি (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.get_সূচক() - (start + 1));
                ধ্রুবক std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start);
                ধ্রুবক UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                যদি (precomputedSet != nullptr) {
                    ফেরত LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/সত্য,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ধ্রুবক UnicodeString *ধ্রুবক expression = symbols_->lookup(name);
                যদি (expression == nullptr) {
                    ফেরত LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/সত্য,
                        /*preceding=*/std::nullopt);
                }
                ফেরত evaluateVariable(*expression, source);
            }
        }
        নির্বাচন (first) {
        ক্ষেত্র u'[':
            ফেরত LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                separated, preceding);
        ক্ষেত্র u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            ফেরত LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                separated, preceding);
        }
        ক্ষেত্র u'&':
        ক্ষেত্র u'-':
        ক্ষেত্র u']':
        ক্ষেত্র u'^':
        ক্ষেত্র u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            ফেরত LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                separated, preceding);
        ক্ষেত্র u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_গুনতি = 0;
            যখন (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                ধ্রুবক RuleCharacterIterator::Pos before_পরবর্তী = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                যদি (next == u'\\') {
                    ধ্রুবক UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    যদি (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = সত্য;
                    } অন্যথা যদি (afterBackslash == u'p' || afterBackslash == u'P') {
                        ফেরত LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.get_সূচক() - start),
                                              separated, preceding);
                    } অন্যথা {
                        chars_.setPos(before_পরবর্তী);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } অন্যথা {
#যদি U_ICU_VERSION_MAJOR_অঙ্ক < 81
                    যদি (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#অন্যথা
#error Remove এই transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = মিথ্যা;
                }
                যদি (!escaped && next == u'}') {
                    ফেরত LexicalElement(
                        code_Point_গুনতি == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_গুনতি += 1;
            }
            ফেরত LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                separated, preceding);
        }
        পূর্বনির্ধারিত:
            ফেরত LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_সূচক() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        ধ্রুবক UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        যদি (open == u'{') {
            int32_t start = parsePosition_.get_সূচক();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            যখন (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                যদি (last == u':') {
                    যদি (!hex.has_value()) {
                        hex.emplace();
                        যদি (start == parsePosition_.get_সূচক() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ফেরত {};
                        }
                        জন্য (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.get_সূচক() - 1 - start)) {
                            uint8_t nibble;
                            যদি (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } অন্যথা {
                                digit = digit & ~0x20;
                                যদি (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } অন্যথা {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    ফেরত {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            যদি (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                ফেরত {};
                            }
                        }
                    } অন্যথা যদি (!literal.has_value()) {
                        ধ্রুবক auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.get_সূচক() - 1 - start));
                        auto it = literalCodePoints.begin();
                        যদি (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            ফেরত {};
                        }
                    } অন্যথা {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ফেরত {};
                    }
                    start = parsePosition_.get_সূচক();
                } অন্যথা যদি (last == u'}') {
                    ধ্রুবক std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.get_সূচক() - 1 - start);
                    ধ্রুবক UChar32 ফলাফল = getCharacterByName(CharString().appendInvariantChars(
                        u16name.তথ্য(), static_cast<int32_t>(u16name.length()), errorCode));
                    যদি (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ফেরত {};
                    }
                    যদি (ফলাফল < 0 || (hex.has_value() && ফলাফল != hex) ||
                            (literal.has_value() && ফলাফল != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ফেরত {};
                    }
                    ফেরত ফলাফল;
                }
            }
        }
        যদি (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ফেরত {};
    }

    LexicalElement evaluateVariable(ধ্রুবক UnicodeString &expression,
                                    ধ্রুবক std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        যদি (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_মান =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_মান.setPattern(rebuiltPattern);
            যদি (!expressionLexer.atEnd()) {
                ফেরত LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/সত্য,
                    /*preceding=*/std::nullopt);
            }
            ফেরত LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_মান),
                source,
                /*separated=*/সত্য,
                /*preceding=*/std::nullopt);
        } অন্যথা {
            expressionLexer.advance();
            যদি (!expressionLexer.atEnd()) {
                ফেরত LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/সত্য,
                    /*preceding=*/std::nullopt);
            }
            নির্বাচন (variableToken.category_) {
            ক্ষেত্র LexicalElement::LITERAL_ELEMENT:
            ক্ষেত্র LexicalElement::ESCAPED_ELEMENT:
            ক্ষেত্র LexicalElement::NAMED_ELEMENT:
            ক্ষেত্র LexicalElement::BRACKETED_ELEMENT:
            ক্ষেত্র LexicalElement::STRING_LITERAL:
            ক্ষেত্র LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                ফেরত LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/সত্য,
                    /*preceding=*/std::nullopt);
            পূর্বনির্ধারিত:
                ফেরত LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/সত্য,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.get_সূচক();
        যৌক্তিক exteriorlyNegated = মিথ্যা;
        যৌক্তিক interiorlyNegated = মিথ্যা;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ধ্রুবক UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        যদি (first == u'\\') {
            যদি (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.get_সূচক();
        } অন্যথা {
            যদি (third == u'^') {
                exteriorlyNegated = সত্য;
                queryExpressionStart = parsePosition_.get_সূচক();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        যখন (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            ধ্রুবক RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            যদি (penultimateUnescaped == u'\\') {
                যদি (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    যদি (!U_SUCCESS(errorCode)) {
                        ফেরত {};
                    }
                } অন্যথা {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } অন্যথা যদি (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_সূচক() - 1;
            } অন্যথা যদি (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                যদি (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ফেরত {};
                }
                interiorlyNegated = সত্য;
                queryOperatorPosition = parsePosition_.get_সূচক() - 1;
            } অন্যথা যদি ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ধ্রুবক int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.get_সূচক() - 2 : parsePosition_.get_সূচক() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                যদি (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.অস্থায়ী_Sub_String_Between(*queryOperatorPosition + 1, queryExpressionLimit);
                    যদি (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        ফেরত {};
                    }
                }
                UnicodeSet ফলাফল;
                ফলাফল.applyPropertyAlias(
                    pattern_.অস্থায়ী_Sub_String_Between(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                যদি (exteriorlyNegated != interiorlyNegated) {
                    ফলাফল.complement().removeAllStrings();
                }
                ফলাফল.setPattern(pattern_.অস্থায়ী_Sub_String_Between(queryStart, parsePosition_.get_সূচক()));
                ফেরত ফলাফল;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত {};
    }

    ধ্রুবক UnicodeString &pattern_;
    ধ্রুবক ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    ধ্রুবক uint32_t unicodeSetOptions_;
    ধ্রুবক int32_t charsOptions_;
    ধ্রুবক প্রতীক_Table *ধ্রুবক symbols_;
    UnicodeSet &(UnicodeSet::* ধ্রুবক caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

নামাকাশ {

constexpr int32_t MAX_DEPTH = 100;

#যদি U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(ফলাফল, ec)                                                   \
    করো {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        যদি (U_FAILURE(ec)) {                                                                       \
            যদি (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } অন্যথা যদি (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            ফেরত ফলাফল;                                                                         \
        }                                                                                          \
    } যখন (মিথ্যা)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(ফলাফল, expected, actual, lexer, ec)                  \
    করো {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        যদি (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } অন্যথা {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ফেরত ফলাফল;                                                                             \
    } যখন (মিথ্যা)

#অন্যথা

#define U_UNICODESET_RETURN_IF_ERROR(ফলাফল, ec)                                                   \
    করো {                                                                                           \
        যদি (U_FAILURE(ec)) {                                                                       \
            ফেরত ফলাফল;                                                                         \
        }                                                                                          \
    } যখন (মিথ্যা)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(ফলাফল, expected, actual, lexer, ec)                  \
    করো {                                                                                           \
        যদি (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } অন্যথা {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        ফেরত ফলাফল;                                                                             \
    } যখন (মিথ্যা)

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

শূন্য UnicodeSet::applyPattern(ধ্রুবক UnicodeString &pattern,
                              ধ্রুবক ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              ধ্রুবক প্রতীক_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    যদি (U_FAILURE(ec)) ফেরত;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *এই = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet ফলাফল;

    যদি (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    যৌক্তিক isComplement = মিথ্যা;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    যৌক্তিক preserveSyntaxInPattern = মিথ্যা;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    যদি (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        ফলাফল = *lexer.lookahead().set();
        ফলাফল._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/মিথ্যা);
        lexer.advance();
        preserveSyntaxInPattern = সত্য;
    } অন্যথা {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        যদি (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            যদি (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = সত্য;
            }
            ফলাফল = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(ফলাফল, ec);
            যদি (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(ফলাফল, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } অন্যথা {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(ফলাফল, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    যদি ((options & USET_CASE_MASK) != 0) {
        (ফলাফল.*caseClosure)(options);
    }
    যদি (isComplement) {
        ফলাফল.complement().removeAllStrings(); // code point complement
    }
    যদি (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } অন্যথা {
        ফলাফল._generatePattern(rebuiltPat, /*escapeUnprintable=*/মিথ্যা);
    }
    ফেরত ফলাফল;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    যৌক্তিক &containsSetOperation,
                                    UErrorCode &ec) {
    UnicodeSet ফলাফল;
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
    যদি (lexer.acceptSetOperator(u'-')) {
        ফলাফল.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    যখন (!lexer.atEnd()) {
        যদি (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            ফলাফল.add(u'-');
            ফেরত ফলাফল;
        } অন্যথা যদি (lexer.lookahead().isSetOperator(u'$')) {
            যদি (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                ফলাফল.add(U_ETHER);
                containsSetOperation = সত্য;
                ফেরত ফলাফল;
            }
        }
        যদি (lexer.lookahead().isSetOperator(u']')) {
            ফেরত ফলাফল;
        }
        // Also handles FinalMutation.
        ফলাফল.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(ফলাফল, ec);
    }
    ফেরত ফলাফল;
}

শূন্য UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               যৌক্তিক &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    যদি (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = সত্য;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } অন্যথা {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

শূন্য UnicodeSet::parseSetOperations(Lexer &lexer,
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
    ধ্রুবক UnicodeSet বাম_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(বাম_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    জন্য (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        যদি (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ধ্রুবক UnicodeSet ডান_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ডান_Hand_Side);
        } অন্যথা যদি (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            যদি (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                ফেরত;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ধ্রুবক UnicodeSet ডান_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ডান_Hand_Side);
        } অন্যথা {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            ফেরত;
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
    যদি (lexer.lookahead().isStringLiteral()) {
        ধ্রুবক UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/মিথ্যা);
        rebuiltPat.append(u'}');
        lexer.advance();
        ফেরত UnicodeSet().add(element);
    }
    UChar32 first;
    যদি (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } অন্যথা যদি (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } অন্যথা {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/মিথ্যা);
    যদি (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        ফেরত UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    যদি (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        ফেরত UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    যদি (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        যদি (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } অন্যথা যদি (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } অন্যথা {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    যদি (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/মিথ্যা);
    ফেরত UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

নামাকাশ {

UBool numeric_মান_Filter(UChar32 ch, শূন্য* context) {
    ফেরত u_get_Numeric_মান(ch) == *static_cast<দ্বিগুণ*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, শূন্য* context) {
    int32_t value = *static_cast<int32_t*>(context);
    ফেরত (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, শূন্য* context) {
    স্থির ধ্রুবক UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    ফেরত uprv_memcmp(&v, &none, আকার(v)) > 0 && uprv_memcmp(&v, version, আকার(v)) <= 0;
}

প্রকারলেখ কাঠামো {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, শূন্য* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    ফেরত u_get_Int_Property_মান(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, শূন্য* context) {
    ফেরত uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, শূন্য* context) {
    ফেরত u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
শূন্য UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             শূন্য* context,
                             ধ্রুবক UnicodeSet* inclusions,
                             UErrorCode &status) {
    যদি (U_FAILURE(status)) ফেরত;

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
    int32_t limitRange = inclusions->get_Range_গুনতি();

    জন্য (পূর্ণসংখ্যা j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        জন্য (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            যদি ((*filter)(ch, context)) {
                যদি (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } অন্যথা যদি (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    যদি (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    যদি (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

নামাকাশ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    ফেরত *এই; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_মান(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    যদি (U_FAILURE(ec) || isFrozen()) { ফেরত *এই; }
    যদি (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        ধ্রুবক UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } অন্যথা যদি (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ধ্রুবক UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } অন্যথা যদি (prop == UCHAR_IDENTIFIER_TYPE) {
        ধ্রুবক UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } অন্যথা যদি (0 <= prop && prop < UCHAR_দ্বিমিক_LIMIT) {
        যদি (value == 0 || value == 1) {
            ধ্রুবক USet *set = u_get_দ্বিমিক_Property_Set(prop, &ec);
            যদি (U_FAILURE(ec)) { ফেরত *এই; }
            copyFrom(*UnicodeSet::fromUSet(set), সত্য);
            যদি (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } অন্যথা {
            clear();
        }
    } অন্যথা যদি (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        ধ্রুবক UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } অন্যথা {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত *এই;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(ধ্রুবক UnicodeString& prop,
                               ধ্রুবক UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    যদি (U_FAILURE(ec) || isFrozen()) ফেরত *এই;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    যদি( !uprv_isInvariantUString(prop.get_বাফার(), prop.length()) ||
        !uprv_isInvariantUString(value.get_বাফার(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    যদি (U_FAILURE(ec)) ফেরত *এই;

    UProperty p;
    int32_t v;
    UBool invert = মিথ্যা;

    যদি (value.length() > 0) {
        p = u_getPropertyEnum(pname.তথ্য());
        যদি (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        যদি (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        যদি ((p >= UCHAR_দ্বিমিক_START && p < UCHAR_দ্বিমিক_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_মান_Enum(p, vname.তথ্য());
            যদি (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                যদি (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    অক্ষর* end;
                    দ্বিগুণ val = uprv_strtod(vname.তথ্য(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    যদি (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } অন্যথা {
                    FAIL(ec);
                }
            }
        }

        অন্যথা {

            নির্বাচন (p) {
            ক্ষেত্র UCHAR_NUMERIC_মান:
                {
                    অক্ষর* end;
                    দ্বিগুণ val = uprv_strtod(vname.তথ্য(), &end);
                    যদি (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_মান_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ফেরত *এই;
                }
            ক্ষেত্র UCHAR_NAME:
                {
                    ধ্রুবক UChar32 ch = getCharacterByName(vname);
                    যদি (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    ফেরত *এই;
                }
            ক্ষেত্র UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            ক্ষেত্র UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.তথ্য());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    ফেরত *এই;
                }
            ক্ষেত্র UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_মান_Enum(UCHAR_SCRIPT, vname.তথ্য());
                যদি (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                বিরতি;
            ক্ষেত্র UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_মান_Enum(p, vname.তথ্য());
                যদি (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                বিরতি;
            পূর্বনির্ধারিত:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    অন্যথা {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_মান_Enum(p, pname.তথ্য());
        যদি (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_মান_Enum(p, pname.তথ্য());
            যদি (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.তথ্য());
                যদি (p >= UCHAR_দ্বিমিক_START && p < UCHAR_দ্বিমিক_LIMIT) {
                    v = 1;
                } অন্যথা যদি (0 == uprv_comparePropertyNames(ANY, pname.তথ্য())) {
                    set(MIN_মান, MAX_মান);
                    ফেরত *এই;
                } অন্যথা যদি (0 == uprv_comparePropertyNames(ASCII, pname.তথ্য())) {
                    set(0, 0x7F);
                    ফেরত *এই;
                } অন্যথা যদি (0 == uprv_comparePropertyNames(ASSIGNED, pname.তথ্য())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = সত্য;
                } অন্যথা {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_মান(p, v, ec);
    যদি(invert) {
        complement().removeAllStrings();  // code point complement
    }

    যদি (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত *এই;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(ধ্রুবক UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    যদি ((pos+5) > pattern.length()) {
        ফেরত মিথ্যা;
    }

    // Look for an opening [:, [:^, \p, or \P
    ফেরত isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
