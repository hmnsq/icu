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
constexpr अक्षर ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr अक्षर ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr अक्षर ASSIGNED[] = "Assigned"; // [:^Cn:]

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
    परत सत्य;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

नामाकाश {

// Cache some sets for other services -------------------------------------- ***
शून्य U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = नवीन UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    जर(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    परत uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

ओळीत UBool
isPerlOpen(स्थिरांक UnicodeString &pattern, int32_t pos) {
    char16_t c;
    परत pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

ओळीत UBool
isNameOpen(स्थिरांक UnicodeString &pattern, int32_t pos) {
    परत pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

ओळीत UBool
isPOSIXOpen(स्थिरांक UnicodeString &pattern, int32_t pos) {
    परत pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
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
UChar32 getCharacterByName(स्थिरांक CharString& name) {
    साठी (स्थिरांक UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.data(), &ec);
        जर (U_SUCCESS(ec)) {
            परत ch;
        }
    }
    परत U_SENTINEL;
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
UnicodeSet::UnicodeSet(स्थिरांक UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(हे);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(स्थिरांक UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    जर (U_FAILURE(status)) परत *हे;

    int32_t i = pos.getIndex();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, सत्य);
    जर (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    परत *हे;
}

शून्य
UnicodeSet::applyPatternIgnoreSpace(स्थिरांक UnicodeString& pattern,
                                    ParsePosition& pos,
                                    स्थिरांक चिन्ह_Table* symbols,
                                    UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    जर (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        परत;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    जर (U_FAILURE(status)) परत;
    जर (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        परत;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(स्थिरांक UnicodeString& pattern, int32_t pos) {
    परत ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#define U_DEBUGGING_UNICODESET_PARSING 0

वर्ग UnicodeSet::Lexer {
  सार्वजनिक:
    Lexer(स्थिरांक UnicodeString &pattern,
          स्थिरांक ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          स्थिरांक चिन्ह_Table *स्थिरांक symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    वर्ग LexicalElement {
      सार्वजनिक:
        तार्किक isSetOperator(स्थिरांक char16_t op) स्थिरांक {
            परत U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        तार्किक isStringLiteral() स्थिरांक {
            परत U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        तार्किक isNamedElement() स्थिरांक {
            परत U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        तार्किक isBracketedElement() स्थिरांक {
            परत U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() स्थिरांक {
            जर (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                परत string_;
            }
            परत std::nullopt;
        }

        std::optional<UChar32> codePoint() स्थिरांक {
            जर (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                परत string_.char32At(0);
            }
            परत std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        स्थिरांक UnicodeSet *set() स्थिरांक {
            जर (U_FAILURE(errorCode_)) {
                परत nullptr;
            }
            जर (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                जर (precomputedSet_ != nullptr) {
                    परत precomputedSet_;
                } अन्यथा {
                    परत &set_;
                }
            }
            परत nullptr;
        }

        स्थिरांक UErrorCode& errorCode() स्थिरांक{
          परत errorCode_;
        }

#जर U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() स्थिरांक {
            UnicodeString result;
            जर (U_FAILURE(errorCode_)) {
                result.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            परत result.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#endif

      खाजगी:
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
                       स्थिरांक UErrorCode errorCode, स्थिरांक UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, तार्किक separated,
                       std::optional<LexicalElement> स्थिरांक &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            जर (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                परत;
            }
            जर (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } अन्यथा जर (preceding->category_ == ESCAPED_ELEMENT) {
                जर (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } अन्यथा जर (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    जर (preceding->sourceText_[1] == u'x') {
                        जर ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } अन्यथा जर (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        जर (cp >= u'0' && cp <= u'7') {
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
        स्थिरांक UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        मित्र वर्ग Lexer;
    };

    UnicodeString getPositionForDebugging() स्थिरांक {
        परत pattern_.tempSubString(0, parsePosition_.getIndex()) + u"☞" +
               pattern_.tempSubString(parsePosition_.getIndex(), 60);
    }

    तार्किक acceptSetOperator(char16_t op) {
        जर (lookahead().isSetOperator(op)) {
            advance();
            परत सत्य;
        }
        परत असत्य;
    }

    स्थिरांक LexicalElement &lookahead() {
        जर (!ahead_.has_value()) {
            स्थिरांक RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(nextToken(behind_));
            chars_.setPos(before);
        }
        परत *ahead_;
    }

    स्थिरांक LexicalElement &lookahead2() {
        जर (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            स्थिरांक RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(nextToken(ahead_));
            chars_.setPos(before);
        }
        परत *ahead2_;
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
        परत chars_;
    }

    int32_t charsOptions() {
        परत charsOptions_;
    }

    तार्किक atEnd() स्थिरांक {
        परत chars_.atEnd();
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

  खाजगी:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() स्थिरांक {
        RuleCharacterIterator::Pos result;
        chars_.getPos(result);
        परत result;
    }

    LexicalElement nextToken(std::optional<LexicalElement> स्थिरांक& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        तार्किक separated = असत्य;
        जर (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            साठी (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                जर (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    थांबा;
                }
                जर (s != u'\u200E' && s != u'\u200F') {
                    separated = सत्य;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        जर (chars_.atEnd()) {
            परत LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        स्थिरांक int32_t start = parsePosition_.getIndex();
        स्थिरांक RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        स्थिरांक UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        जर (first == u'[' || first == u'\\') {
            स्थिरांक RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            स्थिरांक UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            जर ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                जर (second == u'N') {
                    UChar32 स्थिरांक queryResult = scanNamedElementBrackets(errorCode);
                    परत LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(queryResult), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                } अन्यथा {
                    UnicodeSet queryResult = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    परत LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(queryResult),
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        जर (first == u'$' && symbols_ != nullptr) {
            स्वतः nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            जर (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.getIndex() - (start + 1));
                स्थिरांक std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start);
                स्थिरांक UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                जर (precomputedSet != nullptr) {
                    परत LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/सत्य,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                स्थिरांक UnicodeString *स्थिरांक expression = symbols_->lookup(name);
                जर (expression == nullptr) {
                    परत LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/सत्य,
                        /*preceding=*/std::nullopt);
                }
                परत evaluateVariable(*expression, source);
            }
        }
        निवड (first) {
        स्थिती u'[':
            परत LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        स्थिती u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            परत LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        स्थिती u'&':
        स्थिती u'-':
        स्थिती u']':
        स्थिती u'^':
        स्थिती u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            परत LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        स्थिती u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_मोजणी = 0;
            पर्यंत (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                स्थिरांक RuleCharacterIterator::Pos beforeNext = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                जर (next == u'\\') {
                    स्थिरांक UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    जर (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = सत्य;
                    } अन्यथा जर (afterBackslash == u'p' || afterBackslash == u'P') {
                        परत LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.getIndex() - start),
                                              separated, preceding);
                    } अन्यथा {
                        chars_.setPos(beforeNext);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } अन्यथा {
#जर U_ICU_VERSION_MAJOR_अंक < 81
                    जर (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#अन्यथा
#error Remove हे transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#endif
                    escaped = असत्य;
                }
                जर (!escaped && next == u'}') {
                    परत LexicalElement(
                        code_Point_मोजणी == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_मोजणी += 1;
            }
            परत LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
        पूर्वनिर्धारित:
            परत LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.getIndex() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        स्थिरांक UChar32 खोलो = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        जर (खोलो == u'{') {
            int32_t start = parsePosition_.getIndex();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            पर्यंत (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                जर (last == u':') {
                    जर (!hex.has_value()) {
                        hex.emplace();
                        जर (start == parsePosition_.getIndex() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            परत {};
                        }
                        साठी (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.getIndex() - 1 - start)) {
                            uint8_t nibble;
                            जर (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } अन्यथा {
                                digit = digit & ~0x20;
                                जर (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } अन्यथा {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    परत {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            जर (hex > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                परत {};
                            }
                        }
                    } अन्यथा जर (!literal.has_value()) {
                        स्थिरांक स्वतः literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.getIndex() - 1 - start));
                        स्वतः it = literalCodePoints.begin();
                        जर (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            परत {};
                        }
                    } अन्यथा {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        परत {};
                    }
                    start = parsePosition_.getIndex();
                } अन्यथा जर (last == u'}') {
                    स्थिरांक std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.getIndex() - 1 - start);
                    स्थिरांक UChar32 result = getCharacterByName(CharString().appendInvariantChars(
                        u16name.data(), static_cast<int32_t>(u16name.length()), errorCode));
                    जर (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        परत {};
                    }
                    जर (result < 0 || (hex.has_value() && result != hex) ||
                            (literal.has_value() && result != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        परत {};
                    }
                    परत result;
                }
            }
        }
        जर (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        परत {};
    }

    LexicalElement evaluateVariable(स्थिरांक UnicodeString &expression,
                                    स्थिरांक std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        स्वतः variableToken = expressionLexer.lookahead();
        जर (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_मूल्य =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_मूल्य.setPattern(rebuiltPattern);
            जर (!expressionLexer.atEnd()) {
                परत LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            }
            परत LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_मूल्य),
                source,
                /*separated=*/सत्य,
                /*preceding=*/std::nullopt);
        } अन्यथा {
            expressionLexer.advance();
            जर (!expressionLexer.atEnd()) {
                परत LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            }
            निवड (variableToken.category_) {
            स्थिती LexicalElement::LITERAL_ELEMENT:
            स्थिती LexicalElement::ESCAPED_ELEMENT:
            स्थिती LexicalElement::NAMED_ELEMENT:
            स्थिती LexicalElement::BRACKETED_ELEMENT:
            स्थिती LexicalElement::STRING_LITERAL:
            स्थिती LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                परत LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            पूर्वनिर्धारित:
                परत LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/nullptr,
                                      /*set=*/{}, source,
                                      /*separated=*/सत्य,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    UnicodeSet scanPropertyQueryAfterStart(UChar32 first, UChar32 second, int32_t queryStart, UErrorCode &errorCode) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.getIndex();
        तार्किक exteriorlyNegated = असत्य;
        तार्किक interiorlyNegated = असत्य;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        स्थिरांक UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        जर (first == u'\\') {
            जर (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                परत {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.getIndex();
        } अन्यथा {
            जर (third == u'^') {
                exteriorlyNegated = सत्य;
                queryExpressionStart = parsePosition_.getIndex();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        पर्यंत (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            स्थिरांक RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            जर (penultimateUnescaped == u'\\') {
                जर (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    जर (!U_SUCCESS(errorCode)) {
                        परत {};
                    }
                } अन्यथा {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.setPos(beforePenultimate);
                    chars_.next(charsOptions_ & ~RuleCharacterIterator::SKIP_WHITESPACE, unusedEscaped,
                                errorCode);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                lastUnescaped = -1;
            } अन्यथा जर (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } अन्यथा जर (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                जर (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    परत {};
                }
                interiorlyNegated = सत्य;
                queryOperatorPosition = parsePosition_.getIndex() - 1;
            } अन्यथा जर ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                स्थिरांक int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.getIndex() - 2 : parsePosition_.getIndex() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                जर (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.tempSubStringBetween(*queryOperatorPosition + 1, queryExpressionLimit);
                    जर (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        परत {};
                    }
                }
                UnicodeSet result;
                result.applyPropertyAlias(
                    pattern_.tempSubStringBetween(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                जर (exteriorlyNegated != interiorlyNegated) {
                    result.complement().removeAllStrings();
                }
                result.setPattern(pattern_.tempSubStringBetween(queryStart, parsePosition_.getIndex()));
                परत result;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत {};
    }

    स्थिरांक UnicodeString &pattern_;
    स्थिरांक ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    स्थिरांक uint32_t unicodeSetOptions_;
    स्थिरांक int32_t charsOptions_;
    स्थिरांक चिन्ह_Table *स्थिरांक symbols_;
    UnicodeSet &(UnicodeSet::* स्थिरांक caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

नामाकाश {

constexpr int32_t MAX_DEPTH = 100;

#जर U_DEBUGGING_UNICODESET_PARSING

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    करा {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        जर (U_FAILURE(ec)) {                                                                       \
            जर (depth < 5) {                                                                       \
                म_लिखो("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } अन्यथा जर (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                म_लिखो("--- [...]\n");                                                             \
            }                                                                                      \
            परत result;                                                                         \
        }                                                                                          \
    } पर्यंत (असत्य)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    करा {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        म_लिखो("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        म_लिखो("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        जर (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        परत result;                                                                             \
    } पर्यंत (असत्य)

#अन्यथा

#define U_UNICODESET_RETURN_IF_ERROR(result, ec)                                                   \
    करा {                                                                                           \
        जर (U_FAILURE(ec)) {                                                                       \
            परत result;                                                                         \
        }                                                                                          \
    } पर्यंत (असत्य)
#define U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, expected, actual, lexer, ec)                  \
    करा {                                                                                           \
        जर (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        परत result;                                                                             \
    } पर्यंत (असत्य)

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

शून्य UnicodeSet::applyPattern(स्थिरांक UnicodeString &pattern,
                              स्थिरांक ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              स्थिरांक चिन्ह_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    जर (U_FAILURE(ec)) परत;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *हे = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet result;

    जर (depth > MAX_DEPTH) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_DEPTH)).c_str(),
                                             ("depth = " + std::to_string(depth)).c_str(), lexer,
                                             ec);
    }

    तार्किक isComplement = असत्य;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    तार्किक preserveSyntaxInPattern = असत्य;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString prettyPrintedPattern;
    जर (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        result = *lexer.lookahead().set();
        result._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/असत्य);
        lexer.advance();
        preserveSyntaxInPattern = सत्य;
    } अन्यथा {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        जर (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            जर (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = सत्य;
            }
            result = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(result, ec);
            जर (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(result, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } अन्यथा {
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
    जर ((options & USET_CASE_MASK) != 0) {
        (result.*caseClosure)(options);
    }
    जर (isComplement) {
        result.complement().removeAllStrings(); // code point complement
    }
    जर (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } अन्यथा {
        result._generatePattern(rebuiltPat, /*escapeUnprintable=*/असत्य);
    }
    परत result;
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
    जर (lexer.acceptSetOperator(u'-')) {
        result.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    पर्यंत (!lexer.atEnd()) {
        जर (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            result.add(u'-');
            परत result;
        } अन्यथा जर (lexer.lookahead().isSetOperator(u'$')) {
            जर (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                result.add(U_ETHER);
                containsSetOperation = सत्य;
                परत result;
            }
        }
        जर (lexer.lookahead().isSetOperator(u']')) {
            परत result;
        }
        // Also handles FinalMutation.
        result.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(result, ec);
    }
    परत result;
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
    जर (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = सत्य;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } अन्यथा {
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
    स्थिरांक UnicodeSet डावा_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(डावा_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    साठी (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        जर (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            स्थिरांक UnicodeSet उजवा_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(उजवा_Hand_Side);
        } अन्यथा जर (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            जर (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                परत;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            स्थिरांक UnicodeSet उजवा_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(उजवा_Hand_Side);
        } अन्यथा {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            परत;
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
    जर (lexer.lookahead().isStringLiteral()) {
        स्थिरांक UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/असत्य);
        rebuiltPat.append(u'}');
        lexer.advance();
        परत UnicodeSet().add(element);
    }
    UChar32 first;
    जर (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } अन्यथा जर (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/असत्य);
    जर (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        परत UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    जर (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        परत UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    जर (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        जर (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } अन्यथा जर (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    जर (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/असत्य);
    परत UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

नामाकाश {

UBool numeric_मूल्य_Filter(UChar32 ch, शून्य* context) {
    परत u_get_Numeric_मूल्य(ch) == *static_cast<द्विगुण*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, शून्य* context) {
    int32_t value = *static_cast<int32_t*>(context);
    परत (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, शून्य* context) {
    स्थिर स्थिरांक UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    परत uprv_memcmp(&v, &none, आकार(v)) > 0 && uprv_memcmp(&v, version, आकार(v)) <= 0;
}

प्रकारव्याख्या संरचना {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, शून्य* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    परत u_get_Int_Property_मूल्य(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, शून्य* context) {
    परत uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, शून्य* context) {
    परत u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
शून्य UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             शून्य* context,
                             स्थिरांक UnicodeSet* inclusions,
                             UErrorCode &status) {
    जर (U_FAILURE(status)) परत;

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
    int32_t limitRange = inclusions->get_Range_मोजणी();

    साठी (पूर्णांक j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        साठी (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            जर ((*filter)(ch, context)) {
                जर (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } अन्यथा जर (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    जर (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    जर (isBogus() && U_SUCCESS(status)) {
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
    परत *हे; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_मूल्य(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    जर (U_FAILURE(ec) || isFrozen()) { परत *हे; }
    जर (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        स्थिरांक UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } अन्यथा जर (prop == UCHAR_SCRIPT_EXTENSIONS) {
        स्थिरांक UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } अन्यथा जर (prop == UCHAR_IDENTIFIER_TYPE) {
        स्थिरांक UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } अन्यथा जर (0 <= prop && prop < UCHAR_द्विमान_LIMIT) {
        जर (value == 0 || value == 1) {
            स्थिरांक USet *set = u_get_द्विमान_Property_Set(prop, &ec);
            जर (U_FAILURE(ec)) { परत *हे; }
            copyFrom(*UnicodeSet::fromUSet(set), सत्य);
            जर (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } अन्यथा {
            clear();
        }
    } अन्यथा जर (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        स्थिरांक UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } अन्यथा {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    परत *हे;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(स्थिरांक UnicodeString& prop,
                               स्थिरांक UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    जर (U_FAILURE(ec) || isFrozen()) परत *हे;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    जर( !uprv_isInvariantUString(prop.getBuffer(), prop.length()) ||
        !uprv_isInvariantUString(value.getBuffer(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    जर (U_FAILURE(ec)) परत *हे;

    UProperty p;
    int32_t v;
    UBool invert = असत्य;

    जर (value.length() > 0) {
        p = u_getPropertyEnum(pname.data());
        जर (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        जर (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        जर ((p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_मूल्य_Enum(p, vname.data());
            जर (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                जर (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    अक्षर* end;
                    द्विगुण val = uprv_strtod(vname.data(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    जर (*end != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } अन्यथा {
                    FAIL(ec);
                }
            }
        }

        अन्यथा {

            निवड (p) {
            स्थिती UCHAR_NUMERIC_मूल्य:
                {
                    अक्षर* end;
                    द्विगुण val = uprv_strtod(vname.data(), &end);
                    जर (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_मूल्य_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    परत *हे;
                }
            स्थिती UCHAR_NAME:
                {
                    स्थिरांक UChar32 ch = getCharacterByName(vname);
                    जर (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    add(ch);
                    परत *हे;
                }
            स्थिती UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            स्थिती UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.data());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    परत *हे;
                }
            स्थिती UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, vname.data());
                जर (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                थांबा;
            स्थिती UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_मूल्य_Enum(p, vname.data());
                जर (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                थांबा;
            पूर्वनिर्धारित:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    अन्यथा {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_MASK;
        v = u_get_Property_मूल्य_Enum(p, pname.data());
        जर (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_मूल्य_Enum(p, pname.data());
            जर (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.data());
                जर (p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_LIMIT) {
                    v = 1;
                } अन्यथा जर (0 == uprv_comparePropertyNames(ANY, pname.data())) {
                    set(MIN_मूल्य, MAX_मूल्य);
                    परत *हे;
                } अन्यथा जर (0 == uprv_comparePropertyNames(ASCII, pname.data())) {
                    set(0, 0x7F);
                    परत *हे;
                } अन्यथा जर (0 == uprv_comparePropertyNames(ASSIGNED, pname.data())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = सत्य;
                } अन्यथा {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_मूल्य(p, v, ec);
    जर(invert) {
        complement().removeAllStrings();  // code point complement
    }

    जर (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    परत *हे;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(स्थिरांक UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    जर ((pos+5) > pattern.length()) {
        परत असत्य;
    }

    // Look for an opening [:, [:^, \p, or \P
    परत isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
