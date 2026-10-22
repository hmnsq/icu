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

#അന്തര്ഭാവ <array>
#അന്തര്ഭാവ <optional>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/parsepos.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "unicode/symtable.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/utfiterator.h"
#അന്തര്ഭാവ "uset_imp.h"
#അന്തര്ഭാവ "ruleiter.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "util.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "uprops.h"
#അന്തര്ഭാവ "patternprops.h"
#അന്തര്ഭാവ "propname.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "uprops.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "hash.h"

U_NAMESPACE_USE

നാമാകാശ {

// Special property set IDs
constexpr അക്ഷരം ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr അക്ഷരം ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr അക്ഷരം ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV uset_cleanup();

സ്ഥിതം UnicodeSet *uni32Singleton;
സ്ഥിതം icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
സ്ഥിതം UBool U_CALLCONV uset_cleanup() {
    delete uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    തിരികെ സത്യം;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

നാമാകാശ {

// Cache some sets for other services -------------------------------------- ***
ശൂന്യം U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = പുതിയ UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    എങ്കിൽ(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    തിരികെ uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

വരിയിൽ UBool
isPerlOpen(സ്ഥിരം UnicodeString &pattern, int32_t pos) {
    char16_t c;
    തിരികെ pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

വരിയിൽ UBool
isNameOpen(സ്ഥിരം UnicodeString &pattern, int32_t pos) {
    തിരികെ pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

വരിയിൽ UBool
isPOSIXOpen(സ്ഥിരം UnicodeString &pattern, int32_t pos) {
    തിരികെ pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(സ്ഥിരം CharString& name) {
    വേണ്ടി (സ്ഥിരം UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        എങ്കിൽ (U_SUCCESS(ec)) {
            തിരികെ ch;
        }
    }
    തിരികെ U_SENTINEL;
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
UnicodeSet::UnicodeSet(സ്ഥിരം UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(ഇത്);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(സ്ഥിരം UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ *ഇത്;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, സത്യം);
    എങ്കിൽ (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    തിരികെ *ഇത്;
}

ശൂന്യം
UnicodeSet::applyPatternIgnoreSpace(സ്ഥിരം UnicodeString& pattern,
                                    ParsePosition& pos,
                                    സ്ഥിരം ചിഹ്നം_Table* symbols,
                                    UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        തിരികെ;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    എങ്കിൽ (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        തിരികെ;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(സ്ഥിരം UnicodeString& pattern, int32_t pos) {
    തിരികെ ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

വർഗ്ഗം UnicodeSet::Lexer {
  പൊതു:
    Lexer(സ്ഥിരം UnicodeString &pattern,
          സ്ഥിരം ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          സ്ഥിരം ചിഹ്നം_Table *സ്ഥിരം symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    വർഗ്ഗം LexicalElement {
      പൊതു:
        തർക്കം isSetOperator(സ്ഥിരം char16_t op) സ്ഥിരം {
            തിരികെ U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        തർക്കം isStringLiteral() സ്ഥിരം {
            തിരികെ U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        തർക്കം isNamedElement() സ്ഥിരം {
            തിരികെ U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        തർക്കം isBracketedElement() സ്ഥിരം {
            തിരികെ U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() സ്ഥിരം {
            എങ്കിൽ (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                തിരികെ string_;
            }
            തിരികെ std::nullopt;
        }

        std::optional<UChar32> codePoint() സ്ഥിരം {
            എങ്കിൽ (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                തിരികെ string_.char32At(0);
            }
            തിരികെ std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        സ്ഥിരം UnicodeSet *set() സ്ഥിരം {
            എങ്കിൽ (U_FAILURE(errorCode_)) {
                തിരികെ nullptr;
            }
            എങ്കിൽ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                എങ്കിൽ (precomputedSet_ != nullptr) {
                    തിരികെ precomputedSet_;
                } അല്ലെങ്കിൽ {
                    തിരികെ &set_;
                }
            }
            തിരികെ nullptr;
        }

        സ്ഥിരം UErrorCode& errorCode() സ്ഥിരം{
          തിരികെ errorCode_;
        }

#എങ്കിൽ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() സ്ഥിരം {
            UnicodeString result;
            എങ്കിൽ (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            തിരികെ result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      സ്വകാര്യ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        എണ്ണൽ Category : std::uint8_t {
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
        സ്ഥിതം constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       സ്ഥിരം UErrorCode errorCode, സ്ഥിരം UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, തർക്കം separated,
                       std::optional<LexicalElement> സ്ഥിരം &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            എങ്കിൽ (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                തിരികെ;
            }
            എങ്കിൽ (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } അല്ലെങ്കിൽ എങ്കിൽ (preceding->category_ == ESCAPED_ELEMENT) {
                എങ്കിൽ (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } അല്ലെങ്കിൽ എങ്കിൽ (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    എങ്കിൽ (preceding->sourceText_[1] == u'x') {
                        എങ്കിൽ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } അല്ലെങ്കിൽ എങ്കിൽ (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        എങ്കിൽ (cp >= u'0' && cp <= u'7') {
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
        സ്ഥിരം UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        friend വർഗ്ഗം Lexer;
    };

    UnicodeString getPositionForDebugging() സ്ഥിരം {
        തിരികെ pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    തർക്കം acceptSetOperator(char16_t op) {
        എങ്കിൽ (lookahead().isSetOperator(op)) {
            advance();
            തിരികെ സത്യം;
        }
        തിരികെ അസത്യം;
    }

    സ്ഥിരം LexicalElement &lookahead() {
        എങ്കിൽ (!ahead_.has_value()) {
            സ്ഥിരം RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        തിരികെ *ahead_;
    }

    സ്ഥിരം LexicalElement &lookahead2() {
        എങ്കിൽ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            സ്ഥിരം RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        തിരികെ *ahead2_;
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
        തിരികെ chars_;
    }

    int32_t charsOptions() {
        തിരികെ charsOptions_;
    }

    തർക്കം atEnd() സ്ഥിരം {
        തിരികെ chars_.atEnd();
    }

    ശൂന്യം advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  സ്വകാര്യ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() സ്ഥിരം {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        തിരികെ result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> സ്ഥിരം& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        തർക്കം separated = അസത്യം;
        എങ്കിൽ (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            വേണ്ടി (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                എങ്കിൽ (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    നിർത്തുക;
                }
                എങ്കിൽ (s != u'\u200E' && s != u'\u200F') {
                    separated = സത്യം;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        എങ്കിൽ (chars_.atEnd()) {
            തിരികെ LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        സ്ഥിരം int32_t start = parsePosition_.getIndex();
        സ്ഥിരം RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        സ്ഥിരം UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        എങ്കിൽ (first == u'[' || first == u'\\') {
            സ്ഥിരം RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            സ്ഥിരം UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            എങ്കിൽ ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                എങ്കിൽ (second == u'N') {
                    UChar32 സ്ഥിരം queryResult = scanNamedElementBrackets(errorCode);
                    തിരികെ LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } അല്ലെങ്കിൽ {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    തിരികെ LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        എങ്കിൽ (first == u'$' && symbols_ != nullptr) {
            auto nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            എങ്കിൽ (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                സ്ഥിരം std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                സ്ഥിരം UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                എങ്കിൽ (precomputedSet != nullptr) {
                    തിരികെ LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/സത്യം,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                സ്ഥിരം UnicodeString *സ്ഥിരം expression = symbols_->lookup(name);
                എങ്കിൽ (expression == nullptr) {
                    തിരികെ LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/സത്യം,
                        /*preceding=*/std::nullopt);
                }
                തിരികെ evaluateVariable(*expression, source);
            }
        }
        തിരഞ്ഞെടുപ്പ് (first) {
        അവസ്ഥ u'[':
            തിരികെ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        അവസ്ഥ u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            തിരികെ LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        അവസ്ഥ u'&':
        അവസ്ഥ u'-':
        അവസ്ഥ u']':
        അവസ്ഥ u'^':
        അവസ്ഥ u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            തിരികെ LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        അവസ്ഥ u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_എണ്ണം = 0;
            ആകുമ്പോൾ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                സ്ഥിരം RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                എങ്കിൽ (next == u'\\') {
                    സ്ഥിരം UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    എങ്കിൽ (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = സത്യം;
                    } അല്ലെങ്കിൽ എങ്കിൽ (afterBackslash == u'p' || afterBackslash == u'P') {
                        തിരികെ LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } അല്ലെങ്കിൽ {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } അല്ലെങ്കിൽ {
#എങ്കിൽ U_ICU_VERSION_MAJOR_അക്കം < 81
                    എങ്കിൽ (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#അല്ലെങ്കിൽ
#error Remove ഇത് transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = അസത്യം;
                }
                എങ്കിൽ (!escaped && next == u'}') {
                    തിരികെ LexicalElement(
                        code_Point_എണ്ണം == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_എണ്ണം += 1;
            }
            തിരികെ LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        സ്ഥിരസ്ഥിതി:
            തിരികെ LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        സ്ഥിരം UChar32 open = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        എങ്കിൽ (open == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            ആകുമ്പോൾ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                എങ്കിൽ (last == u':') {
                    എങ്കിൽ (!hex.has_value()) {
                        hex.emplace();
                        എങ്കിൽ (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            തിരികെ {};
                        }
                        വേണ്ടി (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            എങ്കിൽ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } അല്ലെങ്കിൽ {
                                digit = digit & ~0x20;
                                എങ്കിൽ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } അല്ലെങ്കിൽ {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    തിരികെ {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            എങ്കിൽ (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                തിരികെ {};
                            }
                        }
                    } അല്ലെങ്കിൽ എങ്കിൽ (!literal.has_value()) {
                        സ്ഥിരം auto literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        auto it = literalCodePoints.begin();
                        എങ്കിൽ (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            തിരികെ {};
                        }
                    } അല്ലെങ്കിൽ {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        തിരികെ {};
                    }
                    start = parsePosition_.getIndex();
                } അല്ലെങ്കിൽ എങ്കിൽ (last == u'}') {
                    സ്ഥിരം std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    സ്ഥിരം UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    എങ്കിൽ (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        തിരികെ {};
                    }
                    എങ്കിൽ (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        തിരികെ {};
                    }
                    തിരികെ result;
                }
            }
        }
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        തിരികെ {};
    }

    LexicalElement evaluateVariable(സ്ഥിരം UnicodeString &expression,
                                    സ്ഥിരം std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        auto variableToken = expressionLexer.lookahead();
        എങ്കിൽ (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_മൂല്യം =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_മൂല്യം.setPattern(rebuiltPattern);
            എങ്കിൽ (!expressionLexer.atEnd()) {
                തിരികെ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/സത്യം,
                    /*preceding=*/std::nullopt);
            }
            തിരികെ LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_മൂല്യം),
                source,
                /*separated=*/സത്യം,
                /*preceding=*/std::nullopt);
        } അല്ലെങ്കിൽ {
            expressionLexer.advance();
            എങ്കിൽ (!expressionLexer.atEnd()) {
                തിരികെ LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/സത്യം,
                    /*preceding=*/std::nullopt);
            }
            തിരഞ്ഞെടുപ്പ് (variableToken.category_) {
            അവസ്ഥ LexicalElement::LITERAL_ELEMENT:
            അവസ്ഥ LexicalElement::ESCAPED_ELEMENT:
            അവസ്ഥ LexicalElement::NAMED_ELEMENT:
            അവസ്ഥ LexicalElement::BRACKETED_ELEMENT:
            അവസ്ഥ LexicalElement::STRING_LITERAL:
            അവസ്ഥ LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                തിരികെ LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/സത്യം,
                    /*preceding=*/std::nullopt);
            സ്ഥിരസ്ഥിതി:
                തിരികെ LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/സത്യം,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        തർക്കം exteriorlyNegated = അസത്യം;
        തർക്കം interiorlyNegated = അസത്യം;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        സ്ഥിരം UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        എങ്കിൽ (first == u'\\') {
            എങ്കിൽ (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (third == u'^') {
                exteriorlyNegated = സത്യം;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        ആകുമ്പോൾ (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            സ്ഥിരം RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            എങ്കിൽ (penultimateUnescaped == u'\\') {
                എങ്കിൽ (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    എങ്കിൽ (!U_SUCCESS(errorCode)) {
                        തിരികെ {};
                    }
                } അല്ലെങ്കിൽ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } അല്ലെങ്കിൽ എങ്കിൽ (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                എങ്കിൽ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    തിരികെ {};
                }
                interiorlyNegated = സത്യം;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } അല്ലെങ്കിൽ എങ്കിൽ ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                സ്ഥിരം int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                എങ്കിൽ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    എങ്കിൽ (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        തിരികെ {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                എങ്കിൽ (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                തിരികെ result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ {};
    }

    സ്ഥിരം UnicodeString &pattern_;
    സ്ഥിരം ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    സ്ഥിരം uint32_t unicodeSetOptions_;
    സ്ഥിരം int32_t charsOptions_;
    സ്ഥിരം ചിഹ്നം_Table *സ്ഥിരം symbols_;
    UnicodeSet &(UnicodeSet::* സ്ഥിരം caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

നാമാകാശ {

constexpr int32_t MAX_DEPTH = 100;

#എങ്കിൽ U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ചെയ്യുക {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        എങ്കിൽ (U_FAILURE(ec)) {                                                                       \
            എങ്കിൽ (depth < 5) {                                                                       \
                printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } അല്ലെങ്കിൽ എങ്കിൽ (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                printf("--- [...]\n");                                                             \
            }                                                                                      \
            തിരികെ result;                                                                         \
        }                                                                                          \
    } ആകുമ്പോൾ (അസത്യം)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ചെയ്യുക {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        printf("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        printf("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        എങ്കിൽ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } അല്ലെങ്കിൽ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        തിരികെ result;                                                                             \
    } ആകുമ്പോൾ (അസത്യം)

#അല്ലെങ്കിൽ

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    ചെയ്യുക {                                                                                           \
        എങ്കിൽ (U_FAILURE(ec)) {                                                                       \
            തിരികെ result;                                                                         \
        }                                                                                          \
    } ആകുമ്പോൾ (അസത്യം)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    ചെയ്യുക {                                                                                           \
        എങ്കിൽ (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } അല്ലെങ്കിൽ {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        തിരികെ result;                                                                             \
    } ആകുമ്പോൾ (അസത്യം)

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

ശൂന്യം UnicodeSet::applyPattern(സ്ഥിരം UnicodeString &pattern,
                              സ്ഥിരം ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              സ്ഥിരം ചിഹ്നം_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    എങ്കിൽ (U_FAILURE(ec)) തിരികെ;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *ഇത് = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    എങ്കിൽ (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    തർക്കം isComplement = അസത്യം;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    തർക്കം preserveSyntaxInPattern = അസത്യം;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    എങ്കിൽ (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/അസത്യം);
        lexer.advance();
        preserveSyntaxInPattern = സത്യം;
    } അല്ലെങ്കിൽ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        എങ്കിൽ (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            എങ്കിൽ (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = സത്യം;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            എങ്കിൽ (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } അല്ലെങ്കിൽ {
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
    എങ്കിൽ ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    എങ്കിൽ (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    എങ്കിൽ (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } അല്ലെങ്കിൽ {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/അസത്യം);
    }
    തിരികെ result;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    തർക്കം &containsSetOperation,
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
    എങ്കിൽ (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    ആകുമ്പോൾ (!lexer.atEnd()) {
        എങ്കിൽ (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            തിരികെ result;
        } അല്ലെങ്കിൽ എങ്കിൽ (lexer.lookahead().isSetOperator(u'$')) {
            എങ്കിൽ (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = സത്യം;
                തിരികെ result;
            }
        }
        എങ്കിൽ (lexer.lookahead().isSetOperator(u']')) {
            തിരികെ result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    തിരികെ result;
}

ശൂന്യം UnicodeSet::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                               int32_t depth,
                               തർക്കം &containsSetOperation,
                               UErrorCode &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    എങ്കിൽ (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = സത്യം;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } അല്ലെങ്കിൽ {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

ശൂന്യം UnicodeSet::parseSetOperations(Lexer &lexer,
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
    സ്ഥിരം UnicodeSet ഇടത്_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(ഇടത്_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    വേണ്ടി (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        എങ്കിൽ (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            സ്ഥിരം UnicodeSet വലത്_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(വലത്_Hand_Side);
        } അല്ലെങ്കിൽ എങ്കിൽ (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            എങ്കിൽ (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                തിരികെ;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            സ്ഥിരം UnicodeSet വലത്_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(വലത്_Hand_Side);
        } അല്ലെങ്കിൽ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            തിരികെ;
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
    എങ്കിൽ (lexer.lookahead().isStringLiteral()) {
        സ്ഥിരം UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/അസത്യം);
        rebuiltPat.append(u'}');
        lexer.advance();
        തിരികെ UnicodeSet().add(element);
    }
    UChar32 first;
    എങ്കിൽ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } അല്ലെങ്കിൽ എങ്കിൽ (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } അല്ലെങ്കിൽ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/അസത്യം);
    എങ്കിൽ (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        തിരികെ UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    എങ്കിൽ (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        തിരികെ UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    എങ്കിൽ (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        എങ്കിൽ (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } അല്ലെങ്കിൽ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    എങ്കിൽ (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/അസത്യം);
    തിരികെ UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

നാമാകാശ {

UBool numeric_മൂല്യം_Filter(UChar32 ch, ശൂന്യം* context) {
    തിരികെ u_get_Numeric_മൂല്യം(ch) == *static_cast<ഇരട്ട*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, ശൂന്യം* context) {
    int32_t value = *static_cast<int32_t*>(context);
    തിരികെ (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, ശൂന്യം* context) {
    സ്ഥിതം സ്ഥിരം UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    തിരികെ uprv_memcmp(&v, &none, വലുപ്പം(v)) > 0 && uprv_memcmp(&v, version, വലുപ്പം(v)) <= 0;
}

തരനിർവചനം ഘടന {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, ശൂന്യം* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    തിരികെ u_get_Int_Property_മൂല്യം(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, ശൂന്യം* context) {
    തിരികെ uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, ശൂന്യം* context) {
    തിരികെ u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
ശൂന്യം UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             ശൂന്യം* context,
                             സ്ഥിരം UnicodeSet* inclusions,
                             UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;

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
    int32_t limitRange = inclusions->get_Range_എണ്ണം();

    വേണ്ടി (പൂർണ്ണസംഖ്യ j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        വേണ്ടി (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            എങ്കിൽ ((*filter)(ch, context)) {
                എങ്കിൽ (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    എങ്കിൽ (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    എങ്കിൽ (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

നാമാകാശ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#define FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    തിരികെ *ഇത്; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_മൂല്യം(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(ec) || isFrozen()) { തിരികെ *ഇത്; }
    എങ്കിൽ (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        സ്ഥിരം UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } അല്ലെങ്കിൽ എങ്കിൽ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        സ്ഥിരം UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } അല്ലെങ്കിൽ എങ്കിൽ (prop == UCHAR_IDENTIFIER_TYPE) {
        സ്ഥിരം UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } അല്ലെങ്കിൽ എങ്കിൽ (0 <= prop && prop < UCHAR_ദ്വിമാന_LIMIT) {
        എങ്കിൽ (value == 0 || value == 1) {
            സ്ഥിരം USet *set = u_get_ദ്വിമാന_Property_Set(prop, &ec);
            എങ്കിൽ (U_FAILURE(ec)) { തിരികെ *ഇത്; }
            copyFrom(*UnicodeSet::fromUSet(set), സത്യം);
            എങ്കിൽ (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } അല്ലെങ്കിൽ {
            clear();
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        സ്ഥിരം UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } അല്ലെങ്കിൽ {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    തിരികെ *ഇത്;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(സ്ഥിരം UnicodeString& prop,
                               സ്ഥിരം UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(ec) || isFrozen()) തിരികെ *ഇത്;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    എങ്കിൽ( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    എങ്കിൽ (U_FAILURE(ec)) തിരികെ *ഇത്;

    UProperty p;
    int32_t v;
    UBool invert = അസത്യം;

    എങ്കിൽ (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        എങ്കിൽ (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        എങ്കിൽ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        എങ്കിൽ ((p >= UCHAR_ദ്വിമാന_START && p < UCHAR_ദ്വിമാന_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_മൂല്യം_Enum(p, vname.data());
            എങ്കിൽ (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                എങ്കിൽ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    അക്ഷരം* end;
                    ഇരട്ട val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    എങ്കിൽ (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } അല്ലെങ്കിൽ {
                    FAIL(ec);
                }
            }
        }

        അല്ലെങ്കിൽ {

            തിരഞ്ഞെടുപ്പ് (p) {
            അവസ്ഥ UCHAR_NUMERIC_മൂല്യം:
                {
                    അക്ഷരം* end;
                    ഇരട്ട val = uprv_strtod(vname.data(), &end);
                    എങ്കിൽ (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_മൂല്യം_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    തിരികെ *ഇത്;
                }
            അവസ്ഥ UCHAR_NAME:
                {
                    സ്ഥിരം UChar32 ch = getCharacterByName(vname);
                    എങ്കിൽ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    തിരികെ *ഇത്;
                }
            അവസ്ഥ UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            അവസ്ഥ UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    തിരികെ *ഇത്;
                }
            അവസ്ഥ UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_മൂല്യം_Enum(UCHAR_SCRIPT, vname.data());
                എങ്കിൽ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                നിർത്തുക;
            അവസ്ഥ UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_മൂല്യം_Enum(p, vname.data());
                എങ്കിൽ (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    അല്ലെങ്കിൽ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_മൂല്യം_Enum(p, pname.data());
        എങ്കിൽ (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_മൂല്യം_Enum(p, pname.data());
            എങ്കിൽ (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                എങ്കിൽ (p >= UCHAR_ദ്വിമാന_START && p < UCHAR_ദ്വിമാന_LIMIT) {
                    v = 1;
                } അല്ലെങ്കിൽ എങ്കിൽ (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_മൂല്യം, MAX_മൂല്യം);
                    തിരികെ *ഇത്;
                } അല്ലെങ്കിൽ എങ്കിൽ (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    തിരികെ *ഇത്;
                } അല്ലെങ്കിൽ എങ്കിൽ (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = സത്യം;
                } അല്ലെങ്കിൽ {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_മൂല്യം(p, v, ec);
    എങ്കിൽ(invert) {
        complement().removeAllStrings();  // code point complement
    }

    എങ്കിൽ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ *ഇത്;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(സ്ഥിരം UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    എങ്കിൽ ((pos+5) > pattern.length()) {
        തിരികെ അസത്യം;
    }

    // Look for an opening [:, [:^, \p, or \P
    തിരികെ isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
