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

#અન્તર્ભાવ <array>
#અન્તર્ભાવ <optional>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/parsepos.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "unicode/symtable.h"
#અન્તર્ભાવ "unicode/uset.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/utfiterator.h"
#અન્તર્ભાવ "uset_imp.h"
#અન્તર્ભાવ "ruleiter.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "util.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "uprops.h"
#અન્તર્ભાવ "patternprops.h"
#અન્તર્ભાવ "propname.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "uprops.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "hash.h"

U_NAMESPACE_USE

નામાકાશ {

// Special property set IDs
constexpr અક્ષર ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr અક્ષર ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr અક્ષર ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV uset_cleanup();

સ્થિર UnicodeSet *uni32Singleton;
સ્થિર icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
સ્થિર UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    પાછા સાચું;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

નામાકાશ {

// Cache some sets for other services -------------------------------------- ***
શૂન્ય U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = નવું UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    જો(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    પાછા uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

અંતઃરેખ UBool
isPerlOpen(અચલ UnicodeString &pattern, int32_t pos) {
    char16_t c;
    પાછા pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

અંતઃરેખ UBool
isNameOpen(અચલ UnicodeString &pattern, int32_t pos) {
    પાછા pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

અંતઃરેખ UBool
isPOSIXOpen(અચલ UnicodeString &pattern, int32_t pos) {
    પાછા pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(અચલ CharString& name) {
    માટે (અચલ UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        જો (U_SUCCESS(ec)) {
            પાછા ch;
        }
    }
    પાછા U_SENTINEL;
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
UnicodeSet::UnicodeSet(અચલ UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(આ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(અચલ UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    જો (U_FAILURE(status)) પાછા *આ;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, સાચું);
    જો (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    પાછા *આ;
}

શૂન્ય
UnicodeSet::applyPatternIgnoreSpace(અચલ UnicodeString& pattern,
                                    ParsePosition& pos,
                                    અચલ પ્રતીક_Table* symbols,
                                    UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    જો (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        પાછા;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    જો (U_FAILURE(status)) પાછા;
    જો (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        પાછા;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(અચલ UnicodeString& pattern, int32_t pos) {
    પાછા ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

વર્ગ UnicodeSet::Lexer {
  જાહેર:
    Lexer(અચલ UnicodeString &pattern,
          અચલ ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          અચલ પ્રતીક_Table *અચલ symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    વર્ગ LexicalElement {
      જાહેર:
        તાર્કિક isSetOperator(અચલ char16_t op) અચલ {
            પાછા U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        તાર્કિક isStringLiteral() અચલ {
            પાછા U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        તાર્કિક isNamedElement() અચલ {
            પાછા U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        તાર્કિક isBracketedElement() અચલ {
            પાછા U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() અચલ {
            જો (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                પાછા string_;
            }
            પાછા std::nullopt;
        }

        std::optional<UChar32> codePoint() અચલ {
            જો (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                પાછા string_.char32At(0);
            }
            પાછા std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        અચલ UnicodeSet *set() અચલ {
            જો (U_FAILURE(errorCode_)) {
                પાછા nullptr;
            }
            જો (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                જો (precomputedSet_ != nullptr) {
                    પાછા precomputedSet_;
                } નહીંતર {
                    પાછા &set_;
                }
            }
            પાછા nullptr;
        }

        અચલ UErrorCode& errorCode() અચલ{
          પાછા errorCode_;
        }

#જો U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() અચલ {
            UnicodeString result;
            જો (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            પાછા result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      ખાનગી:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        ગણના Category : std::uint8_t {
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
        સ્થિર constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       અચલ UErrorCode errorCode, અચલ UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, તાર્કિક separated,
                       std::optional<LexicalElement> અચલ &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            જો (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                પાછા;
            }
            જો (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } નહીંતર જો (preceding->category_ == ESCAPED_ELEMENT) {
                જો (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } નહીંતર જો (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    જો (preceding->sourceText_[1] == u'x') {
                        જો ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } નહીંતર જો (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        જો (cp >= u'0' && cp <= u'7') {
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
        અચલ UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend વર્ગ Lexer;
    };

    UnicodeString getPositionForDebugging() અચલ {
        પાછા pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    તાર્કિક acceptSetOperator(char16_t op) {
        જો (lookahead().isSetOperator(op)) {
            advance();
            પાછા સાચું;
        }
        પાછા ખોટું;
    }

    અચલ LexicalElement &lookahead() {
        જો (!ahead_.has_value()) {
            અચલ RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        પાછા *ahead_;
    }

    અચલ LexicalElement &lookahead2() {
        જો (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            અચલ RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        પાછા *ahead2_;
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
        પાછા chars_;
    }

    int32_t charsOptions() {
        પાછા charsOptions_;
    }

    તાર્કિક atEnd() અચલ {
        પાછા chars_.atEnd();
    }

    શૂન્ય advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  ખાનગી:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() અચલ {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        પાછા result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> અચલ& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        તાર્કિક separated = ખોટું;
        જો (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            માટે (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                જો (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    તોડો;
                }
                જો (s != u'\u200E' && s != u'\u200F') {
                    separated = સાચું;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        જો (chars_.atEnd()) {
            પાછા LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        અચલ int32_t start = parsePosition_.getIndex();
        અચલ RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        અચલ UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        જો (first == u'[' || first == u'\\') {
            અચલ RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            અચલ UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            જો ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                જો (second == u'N') {
                    UChar32 અચલ queryResult = scanNamedElementBrackets(errorCode);
                    પાછા LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } નહીંતર {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    પાછા LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        જો (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            જો (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                અચલ std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                અચલ UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                જો (precomputedSet != nullptr) {
                    પાછા LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/સાચું,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                અચલ UnicodeString *અચલ expression = symbols_->lookup(name);
                જો (expression == nullptr) {
                    પાછા LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/સાચું,
                        /*preceding=*/std::nullopt);
                }
                પાછા evaluateVariable(*expression, source);
            }
        }
        પસંદગી (first) {
        સ્થિતિ u'[':
            પાછા LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        સ્થિતિ u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            પાછા LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        સ્થિતિ u'&':
        સ્થિતિ u'-':
        સ્થિતિ u']':
        સ્થિતિ u'^':
        સ્થિતિ u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            પાછા LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        સ્થિતિ u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_ગણતરી = 0;
            જ્યારે (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                અચલ RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                જો (next == u'\\') {
                    અચલ UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    જો (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = સાચું;
                    } નહીંતર જો (afterBackslash == u'p' || afterBackslash == u'P') {
                        પાછા LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } નહીંતર {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } નહીંતર {
#જો U_ICU_VERSION_MAJOR_અંક < 81
                    જો (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#નહીંતર
#error Remove આ transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = ખોટું;
                }
                જો (!escaped && next == u'}') {
                    પાછા LexicalElement(
                        code_Point_ગણતરી == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_ગણતરી += 1;
            }
            પાછા LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        મૂળભૂત:
            પાછા LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        અચલ UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        જો (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            જ્યારે (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                જો (last == u':') {
                    જો (!hex.has_value()) {
                        hex.emplace();
                        જો (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            પાછા {};
                        }
                        માટે (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            જો (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } નહીંતર {
                                digit = digit & ~0x20;
                                જો (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } નહીંતર {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    પાછા {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            જો (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                પાછા {};
                            }
                        }
                    } નહીંતર જો (!literal.has_value()) {
                        અચલ auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        જો (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            પાછા {};
                        }
                    } નહીંતર {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        પાછા {};
                    }
                    start = parsePosition_.getIndex();
                } નહીંતર જો (last == u'}') {
                    અચલ std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    અચલ UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    જો (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        પાછા {};
                    }
                    જો (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        પાછા {};
                    }
                    પાછા result;
                }
            }
        }
        જો (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        પાછા {};
    }

    LexicalElement evaluateVariable(અચલ UnicodeString &expression,
                                    અચલ std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        જો (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_કિંમત =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_કિંમત.setPattern(rebuiltPattern);
            જો (!expressionLexer.atEnd()) {
                પાછા LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/સાચું,
                    /*preceding=*/std::nullopt);
            }
            પાછા LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_કિંમત),
                source,
                /*separated=*/સાચું,
                /*preceding=*/std::nullopt);
        } નહીંતર {
            expressionLexer.advance();
            જો (!expressionLexer.atEnd()) {
                પાછા LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/સાચું,
                    /*preceding=*/std::nullopt);
            }
            પસંદગી (variableToken.category_) {
            સ્થિતિ LexicalElement::LITERAL_ELEMENT:
            સ્થિતિ LexicalElement::ESCAPED_ELEMENT:
            સ્થિતિ LexicalElement::NAMED_ELEMENT:
            સ્થિતિ LexicalElement::BRACKETED_ELEMENT:
            સ્થિતિ LexicalElement::STRING_LITERAL:
            સ્થિતિ LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                પાછા LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/સાચું,
                    /*preceding=*/std::nullopt);
            મૂળભૂત:
                પાછા LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/સાચું,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        તાર્કિક exteriorlyNegated = ખોટું;
        તાર્કિક interiorlyNegated = ખોટું;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        અચલ UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        જો (first == u'\\') {
            જો (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                પાછા {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } નહીંતર {
            જો (third == u'^') {
                exteriorlyNegated = સાચું;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        જ્યારે (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            અચલ RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            જો (penultimateUnescaped == u'\\') {
                જો (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    જો (!U_SUCCESS(errorCode)) {
                        પાછા {};
                    }
                } નહીંતર {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } નહીંતર જો (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } નહીંતર જો (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                જો (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    પાછા {};
                }
                interiorlyNegated = સાચું;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } નહીંતર જો ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                અચલ int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                જો (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    જો (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        પાછા {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                જો (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                પાછા result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા {};
    }

    અચલ UnicodeString &pattern_;
    અચલ ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    અચલ uint32_t unicodeSetOptions_;
    અચલ int32_t charsOptions_;
    અચલ પ્રતીક_Table *અચલ symbols_;
    UnicodeSet &(UnicodeSet::* અચલ caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

નામાકાશ {

constexpr int32_t MAX_DEPTH = 100;

#જો U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    કરો {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        જો (U_FAILURE(ec)) {                                                                       \
            જો (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } નહીંતર જો (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            પાછા result;                                                                         \
        }                                                                                          \
    } જ્યારે (ખોટું)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    કરો {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        જો (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } નહીંતર {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        પાછા result;                                                                             \
    } જ્યારે (ખોટું)

#નહીંતર

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    કરો {                                                                                           \
        જો (U_FAILURE(ec)) {                                                                       \
            પાછા result;                                                                         \
        }                                                                                          \
    } જ્યારે (ખોટું)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    કરો {                                                                                           \
        જો (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } નહીંતર {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        પાછા result;                                                                             \
    } જ્યારે (ખોટું)

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

શૂન્ય UnicodeSet::applyPattern(અચલ UnicodeString &pattern,
                              અચલ ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              અચલ પ્રતીક_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    જો (U_FAILURE(ec)) પાછા;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *આ = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    જો (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    તાર્કિક isComplement = ખોટું;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    તાર્કિક preserveSyntaxInPattern = ખોટું;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    જો (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/ખોટું);
        lexer.advance();
        preserveSyntaxInPattern = સાચું;
    } નહીંતર {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        જો (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            જો (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = સાચું;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            જો (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } નહીંતર {
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
    જો ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    જો (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    જો (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } નહીંતર {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/ખોટું);
    }
    પાછા result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    તાર્કિક &containsSetOperation,
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
    જો (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    જ્યારે (!lexer.atEnd()) {
        જો (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            પાછા result;
        } નહીંતર જો (lexer.lookahead().isSetOperator(u'$')) {
            જો (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = સાચું;
                પાછા result;
            }
        }
        જો (lexer.lookahead().isSetOperator(u']')) {
            પાછા result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    પાછા result;
}

શૂન્ય UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               તાર્કિક &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    જો (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = સાચું;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } નહીંતર {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

શૂન્ય UnicodeSet::parseSetOperations(Lexer &lexer,
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
    અચલ UnicodeSet ડાબું_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(ડાબું_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    માટે (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        જો (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            અચલ UnicodeSet જમણું_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(જમણું_Hand_Side);
        } નહીંતર જો (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            જો (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                પાછા;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            અચલ UnicodeSet જમણું_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(જમણું_Hand_Side);
        } નહીંતર {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            પાછા;
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
    જો (lexer.lookahead().isStringLiteral()) {
        અચલ UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/ખોટું);
        rebuiltPat.append(u'}');
        lexer.advance();
        પાછા UnicodeSet().add(element);
    }
    UChar32 first;
    જો (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } નહીંતર જો (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } નહીંતર {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/ખોટું);
    જો (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        પાછા UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    જો (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        પાછા UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    જો (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        જો (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } નહીંતર જો (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } નહીંતર {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    જો (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/ખોટું);
    પાછા UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

નામાકાશ {

UBool numeric_કિંમત_Filter(UChar32 ch, શૂન્ય* context) {
    પાછા u_get_Numeric_કિંમત(ch) == *static_cast<દ્વિગુણ*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, શૂન્ય* context) {
    int32_t value = *static_cast<int32_t*>(context);
    પાછા (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, શૂન્ય* context) {
    સ્થિર અચલ UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    પાછા uprv_memcmp(&v, &none, કદ(v)) > 0 && uprv_memcmp(&v, version, કદ(v)) <= 0;
}

પ્રકારવ્યાખ્યા સંરચના {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, શૂન્ય* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    પાછા u_get_Int_Property_કિંમત(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, શૂન્ય* context) {
    પાછા uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, શૂન્ય* context) {
    પાછા u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
શૂન્ય UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             શૂન્ય* context,
                             અચલ UnicodeSet* inclusions,
                             UErrorCode &status) {
    જો (U_FAILURE(status)) પાછા;

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
    int32_t limitRange = inclusions->get_Range_ગણતરી();

    માટે (પૂર્ણાંક j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        માટે (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            જો ((*filter)(ch, context)) {
                જો (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } નહીંતર જો (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    જો (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    જો (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

નામાકાશ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    પાછા *આ; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_કિંમત(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    જો (U_FAILURE(ec) || isFrozen()) { પાછા *આ; }
    જો (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        અચલ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } નહીંતર જો (prop == UCHAR_SCRIPT_EXTENSIONS) {
        અચલ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } નહીંતર જો (prop == UCHAR_IDENTIFIER_TYPE) {
        અચલ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } નહીંતર જો (0 <= prop && prop < UCHAR_દ્વિઅંકી_LIMIT) {
        જો (value == 0 || value == 1) {
            અચલ USet *set = u_get_દ્વિઅંકી_Property_Set(prop, &ec);
            જો (U_FAILURE(ec)) { પાછા *આ; }
            copyFrom(*UnicodeSet::fromUSet(set), સાચું);
            જો (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } નહીંતર {
            clear();
        }
    } નહીંતર જો (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        અચલ UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } નહીંતર {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    પાછા *આ;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(અચલ UnicodeString& prop,
                               અચલ UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    જો (U_FAILURE(ec) || isFrozen()) પાછા *આ;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    જો( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    જો (U_FAILURE(ec)) પાછા *આ;

    UProperty p;
    int32_t v;
    UBool invert = ખોટું;

    જો (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        જો (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        જો (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        જો ((p >= UCHAR_દ્વિઅંકી_START && p < UCHAR_દ્વિઅંકી_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_કિંમત_Enum(p, vname.data());
            જો (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                જો (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    અક્ષર* end;
                    દ્વિગુણ val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    જો (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } નહીંતર {
                    FAIL(ec);
                }
            }
        }

        નહીંતર {

            પસંદગી (p) {
            સ્થિતિ UCHAR_NUMERIC_કિંમત:
                {
                    અક્ષર* end;
                    દ્વિગુણ val = uprv_strtod(vname.data(), &end);
                    જો (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_કિંમત_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    પાછા *આ;
                }
            સ્થિતિ UCHAR_NAME:
                {
                    અચલ UChar32 ch = getCharacterByName(vname);
                    જો (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    પાછા *આ;
                }
            સ્થિતિ UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            સ્થિતિ UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    પાછા *આ;
                }
            સ્થિતિ UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_કિંમત_Enum(UCHAR_SCRIPT, vname.data());
                જો (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                તોડો;
            સ્થિતિ UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_કિંમત_Enum(p, vname.data());
                જો (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                તોડો;
            મૂળભૂત:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    નહીંતર {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_કિંમત_Enum(p, pname.data());
        જો (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_કિંમત_Enum(p, pname.data());
            જો (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                જો (p >= UCHAR_દ્વિઅંકી_START && p < UCHAR_દ્વિઅંકી_LIMIT) {
                    v = 1;
                } નહીંતર જો (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_કિંમત, MAX_કિંમત);
                    પાછા *આ;
                } નહીંતર જો (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    પાછા *આ;
                } નહીંતર જો (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = સાચું;
                } નહીંતર {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_કિંમત(p, v, ec);
    જો(invert) {
        complement().removeAllStrings();  // code point complement
    }

    જો (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા *આ;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(અચલ UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    જો ((pos+5) > pattern.length()) {
        પાછા ખોટું;
    }

    // Look for an opening [:, [:^, \p, or \P
    પાછા isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
