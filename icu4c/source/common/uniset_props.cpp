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

#समावेश <array>
#समावेश <optional>

#समावेश "unicode/utypes.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/symtable.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/utfiterator.h"
#समावेश "uset_imp.h"
#समावेश "ruleiter.h"
#समावेश "cmemory.h"
#समावेश "ucln_cmn.h"
#समावेश "util.h"
#समावेश "uvector.h"
#समावेश "uprops.h"
#समावेश "patternprops.h"
#समावेश "propname.h"
#समावेश "normalizer2impl.h"
#समावेश "uinvchar.h"
#समावेश "uprops.h"
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "mutex.h"
#समावेश "umutex.h"
#समावेश "uassert.h"
#समावेश "hash.h"

U_NAMESPACE_USE

namespace {

// Special property set IDs
constexpr अक्षर ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr अक्षर ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr अक्षर ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
अटल UBool U_CALLCONV uset_cleanup();

अटल UnicodeSet *uni32Singleton;
अटल icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
अटल UBool U_CALLCONV uset_cleanup() {
    मिटाओ uni32Singleton;
    uni32Singleton = nullptr;
    uni32InitOnce.reset();
    वापस true;
}

U_CDECL_END

U_NAMESPACE_BEGIN

using U_HEADER_ONLY_NAMESPACE::utfStringCodePoints;

namespace {

// Cache some sets for other services -------------------------------------- ***
व्योम U_CALLCONV createUni32Set(UErrorCode &errorCode) {
    U_ASSERT(uni32Singleton == nullptr);
    uni32Singleton = नया UnicodeSet(UnicodeString(u"[:age=3.2:]"), errorCode);
    अगर(uni32Singleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        uni32Singleton->freeze();
    }
    ucln_common_registerCleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC UnicodeSet *
uniset_getUnicode32Instance(UErrorCode &errorCode) {
    umtx_initOnce(uni32InitOnce, &createUni32Set, errorCode);
    वापस uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

अंतरभूत UBool
isPerlOpen(स्थिर UnicodeString &pattern, int32_t pos) {
    char16_t c;
    वापस pattern.charAt(pos)==u'\\' && ((c=pattern.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

अंतरभूत UBool
isNameOpen(स्थिर UnicodeString &pattern, int32_t pos) {
    वापस pattern.charAt(pos)==u'\\' && pattern.charAt(pos+1)==u'N';
}

अंतरभूत UBool
isPOSIXOpen(स्थिर UnicodeString &pattern, int32_t pos) {
    वापस pattern.charAt(pos)==u'[' && pattern.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#घोषणा _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(स्थिर CharString& name) {
    क्रम (स्थिर UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UErrorCode ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.तथ्य(), &ec);
        अगर (U_SUCCESS(ec)) {
            वापस ch;
        }
    }
    वापस U_SENTINEL;
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
UnicodeSet::UnicodeSet(स्थिर UnicodeString& pattern,
                       UErrorCode& status) {
    applyPattern(pattern, status);
    _dbgct(यह);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

UnicodeSet& UnicodeSet::applyPattern(स्थिर UnicodeString& pattern,
                                     UErrorCode& status) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    applyPatternIgnoreSpace(pattern, pos, nullptr, status);
    अगर (U_FAILURE(status)) वापस *यह;

    int32_t i = pos.get_सूचकांक();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(pattern, i, true);
    अगर (i != pattern.length()) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस *यह;
}

व्योम
UnicodeSet::applyPatternIgnoreSpace(स्थिर UnicodeString& pattern,
                                    ParsePosition& pos,
                                    स्थिर प्रतीक_Table* symbols,
                                    UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (isFrozen()) {
        status = U_NO_WRITE_PERMISSION;
        वापस;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    RuleCharacterIterator chars(pattern, symbols, pos);
    applyPattern(pattern, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, nullptr, status);
    अगर (U_FAILURE(status)) वापस;
    अगर (chars.inVariable()) {
        // syntaxError(chars, "Extra chars in variable value");
        status = U_MALFORMED_SET;
        वापस;
    }
    setPattern(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool UnicodeSet::resemblesPattern(स्थिर UnicodeString& pattern, int32_t pos) {
    वापस ((pos+1) < pattern.length() &&
            pattern.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resemblesPropertyPattern(pattern, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#घोषणा U_DEBUGGING_UNICODESET_PARSING 0

श्रेणी UnicodeSet::Lexer {
  खुला:
    Lexer(स्थिर UnicodeString &pattern,
          स्थिर ParsePosition &parsePosition,
          RuleCharacterIterator &chars,
          uint32_t unicodeSetOptions,
          स्थिर प्रतीक_Table *स्थिर symbols,
          UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute))
        : pattern_(pattern), parsePosition_(parsePosition), chars_(chars),
          unicodeSetOptions_(unicodeSetOptions),
          charsOptions_(RuleCharacterIterator::PARSE_ESCAPES |
                        ((unicodeSetOptions & USET_IGNORE_SPACE) != 0
                             ? RuleCharacterIterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          caseClosure_(caseClosure) {}

    श्रेणी LexicalElement {
      खुला:
        bool isSetOperator(स्थिर char16_t op) स्थिर {
            वापस U_SUCCESS(errorCode_) && category_ == SET_OPERATOR && string_[0] == op;
        }

        bool isStringLiteral() स्थिर {
            वापस U_SUCCESS(errorCode_) && category_ == STRING_LITERAL;
        }

        bool isNamedElement() स्थिर {
            वापस U_SUCCESS(errorCode_) && category_ == NAMED_ELEMENT;
        }

        bool isBracketedElement() स्थिर {
            वापस U_SUCCESS(errorCode_) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() स्थिर {
            अगर (U_SUCCESS(errorCode_) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                वापस string_;
            }
            वापस std::nullopt;
        }

        std::optional<UChar32> codePoint() स्थिर {
            अगर (U_SUCCESS(errorCode_) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                वापस string_.char32At(0);
            }
            वापस std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        स्थिर UnicodeSet *set() स्थिर {
            अगर (U_FAILURE(errorCode_)) {
                वापस nullptr;
            }
            अगर (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                अगर (precomputedSet_ != nullptr) {
                    वापस precomputedSet_;
                } अन्यथा {
                    वापस &set_;
                }
            }
            वापस nullptr;
        }

        स्थिर UErrorCode& errorCode() स्थिर{
          वापस errorCode_;
        }

#अगर U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() स्थिर {
            UnicodeString परिणाम;
            अगर (U_FAILURE(errorCode_)) {
                परिणाम.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(errorCode_)))
                    .append(u"), possibly ");
            }
            वापस परिणाम.append(category_names_[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#पूर्ण_अगर

      निजी:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        क्रमागत Category : std::uint8_t {
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
        अटल constexpr std::array<std::u16string_view, END_OF_TEXT + 1> category_names_{{
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
                       स्थिर UErrorCode errorCode, स्थिर UnicodeSet *precomputedSet, UnicodeSet set,
                       std::u16string_view sourceText, bool separated,
                       std::optional<LexicalElement> स्थिर &preceding)
            : category_(category), string_(std::move(string)), after_(after), errorCode_(errorCode),
              precomputedSet_(precomputedSet), set_(set), sourceText_(sourceText) {
            अगर (U_FAILURE(errorCode_) || separated || !preceding.has_value()) {
                वापस;
            }
            अगर (preceding->isSetOperator(u'[') && category == LITERAL_ELEMENT &&
                *codePoint() == u':') {
                errorCode_ = U_MALFORMED_SET;
            } अन्यथा अगर (preceding->category_ == ESCAPED_ELEMENT) {
                अगर (U_IS_LEAD(*preceding->codePoint()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*codePoint())) {
                    errorCode_ = U_MALFORMED_SET;
                } अन्यथा अगर (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *codePoint();
                    अगर (preceding->sourceText_[1] == u'x') {
                        अगर ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            errorCode_ = U_MALFORMED_SET;
                        }
                    } अन्यथा अगर (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        अगर (cp >= u'0' && cp <= u'7') {
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
        स्थिर UnicodeSet *precomputedSet_;
        UnicodeSet set_;
        std::u16string_view sourceText_;

        मित्र श्रेणी Lexer;
    };

    UnicodeString getPositionForDebugging() स्थिर {
        वापस pattern_.अस्थायी_Sub_String(0, parsePosition_.get_सूचकांक()) + u"☞" +
               pattern_.अस्थायी_Sub_String(parsePosition_.get_सूचकांक(), 60);
    }

    bool acceptSetOperator(char16_t op) {
        अगर (lookahead().isSetOperator(op)) {
            advance();
            वापस true;
        }
        वापस false;
    }

    स्थिर LexicalElement &lookahead() {
        अगर (!ahead_.has_value()) {
            स्थिर RuleCharacterIterator::Pos before = getPos();
            ahead_.emplace(अग्र_Token(behind_));
            chars_.setPos(before);
        }
        वापस *ahead_;
    }

    स्थिर LexicalElement &lookahead2() {
        अगर (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            स्थिर RuleCharacterIterator::Pos before = getPos();
            chars_.setPos(lookahead().after_);
            ahead2_.emplace(अग्र_Token(ahead_));
            chars_.setPos(before);
        }
        वापस *ahead2_;
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
        वापस chars_;
    }

    int32_t charsOptions() {
        वापस charsOptions_;
    }

    bool atEnd() स्थिर {
        वापस chars_.atEnd();
    }

    व्योम advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.setPos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  निजी:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    RuleCharacterIterator::Pos getPos() स्थिर {
        RuleCharacterIterator::Pos परिणाम;
        chars_.getPos(परिणाम);
        वापस परिणाम;
    }

    LexicalElement अग्र_Token(std::optional<LexicalElement> स्थिर& preceding) {
        UErrorCode errorCode = U_ZERO_ERROR;
        bool separated = false;
        अगर (charsOptions_ & RuleCharacterIterator::SKIP_WHITESPACE) {
            UChar32 s;
            RuleCharacterIterator::Pos pos;
            क्रम (;;) {
                UBool unusedEscaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                  RuleCharacterIterator::SKIP_WHITESPACE),
                                unusedEscaped, errorCode);
                अगर (!PatternProps::isWhiteSpace(s)) {
                    chars_.setPos(pos);
                    अवरोध;
                }
                अगर (s != u'\u200E' && s != u'\u200F') {
                    separated = true;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        अगर (chars_.atEnd()) {
            वापस LexicalElement(LexicalElement::END_OF_TEXT, {}, getPos(), errorCode,
                                  /*precomputedSet=*/nullptr,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        स्थिर int32_t start = parsePosition_.get_सूचकांक();
        स्थिर RuleCharacterIterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool unusedEscaped;
        स्थिर UChar32 first =
            chars_.next(charsOptions_ & ~RuleCharacterIterator::PARSE_ESCAPES, unusedEscaped, errorCode);
        अगर (first == u'[' || first == u'\\') {
            स्थिर RuleCharacterIterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            स्थिर UChar32 second = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                                 RuleCharacterIterator::SKIP_WHITESPACE),
                                               unusedEscaped, errorCode);
            अगर ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                अगर (second == u'N') {
                    UChar32 स्थिर query_परिणाम = scanNamedElementBrackets(errorCode);
                    वापस LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_परिणाम), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                } अन्यथा {
                    UnicodeSet query_परिणाम = scanPropertyQueryAfterStart(first, second, start, errorCode);
                    वापस LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), errorCode,
                        /*precomputedSet=*/nullptr, /*set=*/std::move(query_परिणाम),
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.setPos(afterFirst);
        }
        अगर (first == u'$' && symbols_ != nullptr) {
            स्वतः nameEnd = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            अगर (UnicodeString name = symbols_->parseReference(pattern_, nameEnd, pattern_.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.isEmpty()) {
                chars_.jumpahead(nameEnd.get_सूचकांक() - (start + 1));
                स्थिर std::u16string_view source =
                    std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start);
                स्थिर UnicodeSet *precomputedSet = symbols_->lookupSet(name);
                अगर (precomputedSet != nullptr) {
                    वापस LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputedSet, /*set=*/{}, source, /*separated=*/true,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                स्थिर UnicodeString *स्थिर expression = symbols_->lookup(name);
                अगर (expression == nullptr) {
                    वापस LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        source,
                        /*separated=*/true,
                        /*preceding=*/std::nullopt);
                }
                वापस evaluateVariable(*expression, source);
            }
        }
        चयन (first) {
        हाल u'[':
            वापस LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(u'['), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        हाल u'\\': {
            // Now try to parse the escape.
            chars_.setPos(before);
            UChar32 codePoint = chars_.next(charsOptions_, unusedEscaped, errorCode);
            वापस LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(codePoint), getPos(), errorCode,
                nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
        हाल u'&':
        हाल u'-':
        हाल u']':
        हाल u'^':
        हाल u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            वापस LexicalElement(
                LexicalElement::SET_OPERATOR, UnicodeString(first), getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        हाल u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t code_Point_गिनती = 0;
            जबतक (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                स्थिर RuleCharacterIterator::Pos before_अग्र = getPos();
                next = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                     RuleCharacterIterator::SKIP_WHITESPACE),
                                   unusedEscaped, errorCode);
                अगर (next == u'\\') {
                    स्थिर UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                      RuleCharacterIterator::SKIP_WHITESPACE),
                                    unusedEscaped, errorCode);
                    अगर (afterBackslash == u'N') {
                        next = scanNamedElementBrackets(errorCode);
                        escaped = true;
                    } अन्यथा अगर (afterBackslash == u'p' || afterBackslash == u'P') {
                        वापस LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_SET,
                                              /*precomputedSet=*/nullptr,
                                              /*set=*/{},
                                              std::u16string_view(pattern_).substr(
                                                  start, parsePosition_.get_सूचकांक() - start),
                                              separated, preceding);
                    } अन्यथा {
                        chars_.setPos(before_अग्र);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, errorCode);
                    }
                } अन्यथा {
#अगर U_ICU_VERSION_MAJOR_अंक < 81
                    अगर (U_SUCCESS(errorCode) && PatternProps::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        errorCode = UErrorCode::U_ILLEGAL_ARGUMENT_ERROR;
                    }
#अन्यथा
#त्रुटि Remove यह transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#पूर्ण_अगर
                    escaped = false;
                }
                अगर (!escaped && next == u'}') {
                    वापस LexicalElement(
                        code_Point_गिनती == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), errorCode,
                        /*precomputedSet=*/nullptr,
                        /*set=*/{},
                        std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                }
                string.append(next);
                code_Point_गिनती += 1;
            }
            वापस LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_SET,
                /*precomputedSet=*/nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
        शेष:
            वापस LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), errorCode, nullptr,
                /*set=*/{},
                std::u16string_view(pattern_).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
    }

    UChar32 scanNamedElementBrackets(UErrorCode &errorCode) {
        UBool unusedEscaped;
        स्थिर UChar32 खोलो = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                           RuleCharacterIterator::SKIP_WHITESPACE),
                                         unusedEscaped, errorCode);
        अगर (खोलो == u'{') {
            int32_t start = parsePosition_.get_सूचकांक();
            std::optional<UChar32> षष्ठ;
            std::optional<UChar32> literal;
            जबतक (!chars_.atEnd() && U_SUCCESS(errorCode)) {
                UChar32 last = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                             RuleCharacterIterator::SKIP_WHITESPACE),
                                           unusedEscaped, errorCode);
                अगर (last == u':') {
                    अगर (!षष्ठ.has_value()) {
                        षष्ठ.emplace();
                        अगर (start == parsePosition_.get_सूचकांक() - 1) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            वापस {};
                        }
                        क्रम (char16_t digit : std::u16string_view(pattern_).substr(
                                 start, parsePosition_.get_सूचकांक() - 1 - start)) {
                            uint8_t nibble;
                            अगर (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } अन्यथा {
                                digit = digit & ~0x20;
                                अगर (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } अन्यथा {
                                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                    वापस {};
                                }
                            }
                            *षष्ठ = (*षष्ठ << 4) + nibble;
                            अगर (षष्ठ > 0x10FFFF) {
                                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                                वापस {};
                            }
                        }
                    } अन्यथा अगर (!literal.has_value()) {
                        स्थिर स्वतः literalCodePoints = utfStringCodePoints<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(pattern_).substr(start,
                                                                 parsePosition_.get_सूचकांक() - 1 - start));
                        स्वतः it = literalCodePoints.begin();
                        अगर (it == literalCodePoints.end() || !it->wellFormed() ||
                                (literal = it->codePoint(), ++it) != literalCodePoints.end()) {
                            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                            वापस {};
                        }
                    } अन्यथा {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        वापस {};
                    }
                    start = parsePosition_.get_सूचकांक();
                } अन्यथा अगर (last == u'}') {
                    स्थिर std::u16string_view u16name = std::u16string_view(pattern_).substr(
                        start, parsePosition_.get_सूचकांक() - 1 - start);
                    स्थिर UChar32 परिणाम = getCharacterByName(CharString().appendInvariantChars(
                        u16name.तथ्य(), static_cast<int32_t>(u16name.length()), errorCode));
                    अगर (!U_SUCCESS(errorCode)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        वापस {};
                    }
                    अगर (परिणाम < 0 || (षष्ठ.has_value() && परिणाम != षष्ठ) ||
                            (literal.has_value() && परिणाम != literal)) {
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        वापस {};
                    }
                    वापस परिणाम;
                }
            }
        }
        अगर (U_SUCCESS(errorCode)) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        }
        वापस {};
    }

    LexicalElement evaluateVariable(स्थिर UnicodeString &expression,
                                    स्थिर std::u16string_view source) {
        UErrorCode errorCode = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        RuleCharacterIterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicodeSetOptions_,
                              /*symbols=*/nullptr, caseClosure_);
        स्वतः variableToken = expressionLexer.lookahead();
        अगर (variableToken.isSetOperator(u'[')) {
            UnicodeString rebuiltPattern;
            UnicodeSet expression_मूल्य =
                parseUnicodeSet(expressionLexer, rebuiltPattern, unicodeSetOptions_, caseClosure_,
                                /*depth=*/0, errorCode);
            expression_मूल्य.setPattern(rebuiltPattern);
            अगर (!expressionLexer.atEnd()) {
                वापस LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            }
            वापस LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), errorCode,
                /*precomputedSet=*/nullptr,
                /*set=*/std::move(expression_मूल्य),
                source,
                /*separated=*/true,
                /*preceding=*/std::nullopt);
        } अन्यथा {
            expressionLexer.advance();
            अगर (!expressionLexer.atEnd()) {
                वापस LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/nullptr,
                    /*set=*/{},
                    source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            }
            चयन (variableToken.category_) {
            हाल LexicalElement::LITERAL_ELEMENT:
            हाल LexicalElement::ESCAPED_ELEMENT:
            हाल LexicalElement::NAMED_ELEMENT:
            हाल LexicalElement::BRACKETED_ELEMENT:
            हाल LexicalElement::STRING_LITERAL:
            हाल LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                वापस LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.errorCode_, variableToken.precomputedSet_, std::move(variableToken.set_), source,
                    /*separated=*/true,
                    /*preceding=*/std::nullopt);
            शेष:
                वापस LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
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
        int32_t queryExpressionStart = parsePosition_.get_सूचकांक();
        bool exteriorlyNegated = false;
        bool interiorlyNegated = false;
        UBool unusedEscaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        स्थिर UChar32 third = chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                                            RuleCharacterIterator::SKIP_WHITESPACE),
                                          unusedEscaped, errorCode);
        अगर (first == u'\\') {
            अगर (third != u'{') {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                वापस {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.get_सूचकांक();
        } अन्यथा {
            अगर (third == u'^') {
                exteriorlyNegated = true;
                queryExpressionStart = parsePosition_.get_सूचकांक();
            }
        }
        RuleCharacterIterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                          RuleCharacterIterator::SKIP_WHITESPACE),
                        unusedEscaped, errorCode);

        जबतक (!chars_.atEnd() && U_SUCCESS(errorCode)) {
            स्थिर RuleCharacterIterator::Pos beforeLast = getPos();
            UChar32 lastUnescaped =
                chars_.next(charsOptions_ & ~(RuleCharacterIterator::PARSE_ESCAPES |
                                              RuleCharacterIterator::SKIP_WHITESPACE),
                            unusedEscaped, errorCode);
            अगर (penultimateUnescaped == u'\\') {
                अगर (lastUnescaped == 'N') {
                    scanNamedElementBrackets(errorCode);
                    अगर (!U_SUCCESS(errorCode)) {
                        वापस {};
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
            } अन्यथा अगर (!queryOperatorPosition.has_value() && lastUnescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_सूचकांक() - 1;
            } अन्यथा अगर (!queryOperatorPosition.has_value() && lastUnescaped == u'≠') {
                अगर (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    वापस {};
                }
                interiorlyNegated = true;
                queryOperatorPosition = parsePosition_.get_सूचकांक() - 1;
            } अन्यथा अगर ((first == u'[' && penultimateUnescaped == u':' && lastUnescaped == u']') ||
                       (first == u'\\' && lastUnescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                स्थिर int32_t queryExpressionLimit =
                    first == u'[' ? parsePosition_.get_सूचकांक() - 2 : parsePosition_.get_सूचकांक() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                अगर (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        pattern_.अस्थायी_Sub_String_Between(*queryOperatorPosition + 1, queryExpressionLimit);
                    अगर (propertyPredicate.isEmpty()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                        वापस {};
                    }
                }
                UnicodeSet परिणाम;
                परिणाम.applyPropertyAlias(
                    pattern_.अस्थायी_Sub_String_Between(queryExpressionStart,
                                                  queryOperatorPosition.value_or(queryExpressionLimit)),
                    propertyPredicate, errorCode);
                अगर (exteriorlyNegated != interiorlyNegated) {
                    परिणाम.complement().removeAllStrings();
                }
                परिणाम.setPattern(pattern_.अस्थायी_Sub_String_Between(queryStart, parsePosition_.get_सूचकांक()));
                वापस परिणाम;
            }
            beforePenultimate = beforeLast;
            penultimateUnescaped = lastUnescaped;
        }
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस {};
    }

    स्थिर UnicodeString &pattern_;
    स्थिर ParsePosition &parsePosition_;
    RuleCharacterIterator &chars_;
    स्थिर uint32_t unicodeSetOptions_;
    स्थिर int32_t charsOptions_;
    स्थिर प्रतीक_Table *स्थिर symbols_;
    UnicodeSet &(UnicodeSet::* स्थिर caseClosure_)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

namespace {

constexpr int32_t MAX_DEPTH = 100;

#अगर U_DEBUGGING_UNICODESET_PARSING

#घोषणा U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec)                                                   \
    करो {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        अगर (U_FAILURE(ec)) {                                                                       \
            अगर (depth < 5) {                                                                       \
                म_लिखो("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } अन्यथा अगर (depth == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                म_लिखो("--- [...]\n");                                                             \
            }                                                                                      \
            वापस परिणाम;                                                                         \
        }                                                                                          \
    } जबतक (false)
#घोषणा U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, expected, actual, lexer, ec)                  \
    करो {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        म_लिखो("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).toUTF8String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().toUTF8String(contextUTF8).c_str());                 \
        म_लिखो("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        अगर (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        वापस परिणाम;                                                                             \
    } जबतक (false)

#अन्यथा

#घोषणा U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec)                                                   \
    करो {                                                                                           \
        अगर (U_FAILURE(ec)) {                                                                       \
            वापस परिणाम;                                                                         \
        }                                                                                          \
    } जबतक (false)
#घोषणा U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, expected, actual, lexer, ec)                  \
    करो {                                                                                           \
        अगर (U_FAILURE(lexer.lookahead().errorCode())) {                                            \
            (ec) = lexer.lookahead().errorCode();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_SET;                                                                \
        }                                                                                          \
        वापस परिणाम;                                                                             \
    } जबतक (false)

#पूर्ण_अगर

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

व्योम UnicodeSet::applyPattern(स्थिर UnicodeString &pattern,
                              स्थिर ParsePosition &parsePosition,
                              RuleCharacterIterator &chars,
                              स्थिर प्रतीक_Table *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                              UErrorCode &ec) {
    अगर (U_FAILURE(ec)) वापस;
    Lexer lexer(pattern, parsePosition, chars, options, symbols, caseClosure);
    *यह = parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, /*depth=*/0, ec);
}

UnicodeSet UnicodeSet::parseUnicodeSet(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       UnicodeSet& (UnicodeSet::*caseClosure)(int32_t attribute),
                                       int32_t depth,
                                       UErrorCode &ec) {
    UnicodeSet परिणाम;

    अगर (depth > MAX_DEPTH) {
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
    अगर (lexer.lookahead().set() != nullptr) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        परिणाम = *lexer.lookahead().set();
        परिणाम._toPattern(prettyPrintedPattern, /*escapeUnprintable=*/false);
        lexer.advance();
        preserveSyntaxInPattern = true;
    } अन्यथा {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        अगर (lexer.acceptSetOperator(u'[')) {
            prettyPrintedPattern.append(u'[');
            अगर (lexer.acceptSetOperator(u'^')) {
                prettyPrintedPattern.append(u'^');
                isComplement = true;
            }
            परिणाम = parseContent(lexer, prettyPrintedPattern, options, caseClosure, depth,
                                  /*containsSetOperation=*/preserveSyntaxInPattern, ec);
            U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec);
            अगर (!lexer.acceptSetOperator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            prettyPrintedPattern.append(u']');
        } अन्यथा {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    अगर ((options & USET_CASE_MASK) != 0) {
        (परिणाम.*caseClosure)(options);
    }
    अगर (isComplement) {
        परिणाम.complement().removeAllStrings(); // code point complement
    }
    अगर (preserveSyntaxInPattern) {
        rebuiltPat.append(prettyPrintedPattern);
    } अन्यथा {
        परिणाम._generatePattern(rebuiltPat, /*escapeUnprintable=*/false);
    }
    वापस परिणाम;
}

UnicodeSet UnicodeSet::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    UnicodeSet &(UnicodeSet::*caseClosure)(int32_t attribute),
                                    int32_t depth,
                                    bool &containsSetOperation,
                                    UErrorCode &ec) {
    UnicodeSet परिणाम;
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
    अगर (lexer.acceptSetOperator(u'-')) {
        परिणाम.add(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    जबतक (!lexer.atEnd()) {
        अगर (lexer.acceptSetOperator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            परिणाम.add(u'-');
            वापस परिणाम;
        } अन्यथा अगर (lexer.lookahead().isSetOperator(u'$')) {
            अगर (lexer.lookahead2().isSetOperator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                परिणाम.add(U_ETHER);
                containsSetOperation = true;
                वापस परिणाम;
            }
        }
        अगर (lexer.lookahead().isSetOperator(u']')) {
            वापस परिणाम;
        }
        // Also handles FinalMutation.
        परिणाम.parseMutation(lexer, rebuiltPat, options, caseClosure, depth, containsSetOperation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec);
    }
    वापस परिणाम;
}

व्योम UnicodeSet::parseMutation(Lexer &lexer,
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
    अगर (lexer.lookahead().isSetOperator('[') || lexer.lookahead().set() != nullptr) {
        containsSetOperation = true;
        parseSetOperations(lexer, rebuiltPat, options, caseClosure, depth, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } अन्यथा {
        // Also handles DollarElements.
        addAll(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

व्योम UnicodeSet::parseSetOperations(Lexer &lexer,
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
    स्थिर UnicodeSet बायाँ_Hand_Side =
        parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
    addAll(बायाँ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    क्रम (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        अगर (lexer.acceptSetOperator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            स्थिर UnicodeSet दायाँ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(दायाँ_Hand_Side);
        } अन्यथा अगर (lexer.lookahead().isSetOperator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            अगर (lexer.lookahead2().isSetOperator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                वापस;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            स्थिर UnicodeSet दायाँ_Hand_Side =
                parseUnicodeSet(lexer, rebuiltPat, options, caseClosure, depth + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(दायाँ_Hand_Side);
        } अन्यथा {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            वापस;
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
    अगर (lexer.lookahead().isStringLiteral()) {
        स्थिर UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        _appendToPat(rebuiltPat, element, /*escapeUnprintable=*/false);
        rebuiltPat.append(u'}');
        lexer.advance();
        वापस UnicodeSet().add(element);
    }
    UChar32 first;
    अगर (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } अन्यथा अगर (lexer.lookahead().codePoint().has_value()) {
        first = *lexer.lookahead().codePoint();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, first, /*escapeUnprintable=*/false);
    अगर (!lexer.lookahead().isSetOperator(u'-')) {
        // No operator,
        // Elements ::= Element
        वापस UnicodeSet(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    अगर (lexer.lookahead2().isSetOperator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        वापस UnicodeSet(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 last;
    अगर (lexer.lookahead().isSetOperator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        last = u'$';
        अगर (lexer.lookahead2().isSetOperator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } अन्यथा अगर (lexer.lookahead().codePoint().has_value()) {
        last = *lexer.lookahead().codePoint();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    अगर (last <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(last) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    _appendToPat(rebuiltPat, last, /*escapeUnprintable=*/false);
    वापस UnicodeSet(first, last);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

namespace {

UBool numeric_मूल्य_Filter(UChar32 ch, व्योम* context) {
    वापस u_get_Numeric_मूल्य(ch) == *static_cast<द्विगुन*>(context);
}

UBool generalCategoryMaskFilter(UChar32 ch, व्योम* context) {
    int32_t value = *static_cast<int32_t*>(context);
    वापस (U_GET_GC_MASK((UChar32) ch) & value) != 0;
}

UBool versionFilter(UChar32 ch, व्योम* context) {
    अटल स्थिर UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* version = static_cast<UVersionInfo*>(context);
    वापस uprv_memcmp(&v, &none, माप(v)) > 0 && uprv_memcmp(&v, version, माप(v)) <= 0;
}

प्रकार काष्ठा {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, व्योम* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    वापस u_get_Int_Property_मूल्य(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, व्योम* context) {
    वापस uscript_hasScript(ch, *static_cast<UScriptCode*>(context));
}

UBool idTypeFilter(UChar32 ch, व्योम* context) {
    वापस u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
व्योम UnicodeSet::applyFilter(UnicodeSet::Filter filter,
                             व्योम* context,
                             स्थिर UnicodeSet* inclusions,
                             UErrorCode &status) {
    अगर (U_FAILURE(status)) वापस;

    // Logically, walk through all Unicode characters, noting the start
    // and end of each range for which filter.contain(c) is
    // true.  Add each range to a set.
    //
    // To improve performance, use an inclusions set which
    // encodes information about character ranges that are known
    // to have identical properties.
    // inclusions contains the first characters of
    // same-value ranges for the given property.

    सफा();

    UChar32 startHasProperty = -1;
    int32_t limitRange = inclusions->get_Range_गिनती();

    क्रम (पूर्णांक j=0; j<limitRange; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 end = inclusions->getRangeEnd(j);

        // for all the code points in the range, process
        क्रम (UChar32 ch = start; ch <= end; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            अगर ((*filter)(ch, context)) {
                अगर (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } अन्यथा अगर (startHasProperty >= 0) {
                add(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    अगर (startHasProperty >= 0) {
        add(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    अगर (isBogus() && U_SUCCESS(status)) {
        // We likely ran out of memory. AHHH!
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}

namespace {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#घोषणा FAIL(ec) UPRV_BLOCK_MACRO_BEGIN { \
    ec=U_ILLEGAL_ARGUMENT_ERROR; \
    वापस *यह; \
} UPRV_BLOCK_MACRO_END

UnicodeSet&
UnicodeSet::apply_Int_Property_मूल्य(UProperty prop, int32_t value, UErrorCode& ec) U_LIFETIME_BOUND {
    अगर (U_FAILURE(ec) || isFrozen()) { वापस *यह; }
    अगर (prop == UCHAR_GENERAL_CATEGORY_MASK) {
        स्थिर UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(generalCategoryMaskFilter, &value, inclusions, ec);
    } अन्यथा अगर (prop == UCHAR_SCRIPT_EXTENSIONS) {
        स्थिर UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScriptCode script = static_cast<UScriptCode>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } अन्यथा अगर (prop == UCHAR_IDENTIFIER_TYPE) {
        स्थिर UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } अन्यथा अगर (0 <= prop && prop < UCHAR_द्विआधारी_LIMIT) {
        अगर (value == 0 || value == 1) {
            स्थिर USet *set = u_get_द्विआधारी_Property_Set(prop, &ec);
            अगर (U_FAILURE(ec)) { वापस *यह; }
            copyFrom(*UnicodeSet::fromUSet(set), true);
            अगर (value == 0) {
                complement().removeAllStrings();  // code point complement
            }
        } अन्यथा {
            सफा();
        }
    } अन्यथा अगर (UCHAR_INT_START <= prop && prop < UCHAR_INT_LIMIT) {
        स्थिर UnicodeSet* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } अन्यथा {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
    }
    वापस *यह;
}

UnicodeSet&
UnicodeSet::applyPropertyAlias(स्थिर UnicodeString& prop,
                               स्थिर UnicodeString& value,
                               UErrorCode& ec) U_LIFETIME_BOUND {
    अगर (U_FAILURE(ec) || isFrozen()) वापस *यह;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    अगर( !uprv_isInvariantUString(prop.get_बफर(), prop.length()) ||
        !uprv_isInvariantUString(value.get_बफर(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    अगर (U_FAILURE(ec)) वापस *यह;

    UProperty p;
    int32_t v;
    UBool invert = false;

    अगर (value.length() > 0) {
        p = u_getPropertyEnum(pname.तथ्य());
        अगर (p == UCHAR_INVALID_CODE) FAIL(ec);

        // Treat gc as gcm
        अगर (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_MASK;
        }

        अगर ((p >= UCHAR_द्विआधारी_START && p < UCHAR_द्विआधारी_LIMIT) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_LIMIT) ||
            (p >= UCHAR_MASK_START && p < UCHAR_MASK_LIMIT)) {
            v = u_get_Property_मूल्य_Enum(p, vname.तथ्य());
            अगर (v == UCHAR_INVALID_CODE) {
                // Handle numeric CCC
                अगर (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    अक्षर* end;
                    द्विगुन val = uprv_strtod(vname.तथ्य(), &end);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    अगर (*end != 0 || !(0 <= val && val <= 255) ||
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

            चयन (p) {
            हाल UCHAR_NUMERIC_मूल्य:
                {
                    अक्षर* end;
                    द्विगुन val = uprv_strtod(vname.तथ्य(), &end);
                    अगर (*end != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_मूल्य_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    वापस *यह;
                }
            हाल UCHAR_NAME:
                {
                    स्थिर UChar32 ch = getCharacterByName(vname);
                    अगर (ch < 0) {
                        FAIL(ec);
                    }
                    सफा();
                    add(ch);
                    वापस *यह;
                }
            हाल UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            हाल UCHAR_AGE:
                {
                    UVersionInfo version;
                    u_versionFromString(version, vname.तथ्य());
                    applyFilter(versionFilter, &version,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    वापस *यह;
                }
            हाल UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_मूल्य_Enum(UCHAR_SCRIPT, vname.तथ्य());
                अगर (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                अवरोध;
            हाल UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_मूल्य_Enum(p, vname.तथ्य());
                अगर (v == UCHAR_INVALID_CODE) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                अवरोध;
            शेष:
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
        v = u_get_Property_मूल्य_Enum(p, pname.तथ्य());
        अगर (v == UCHAR_INVALID_CODE) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_मूल्य_Enum(p, pname.तथ्य());
            अगर (v == UCHAR_INVALID_CODE) {
                p = u_getPropertyEnum(pname.तथ्य());
                अगर (p >= UCHAR_द्विआधारी_START && p < UCHAR_द्विआधारी_LIMIT) {
                    v = 1;
                } अन्यथा अगर (0 == uprv_comparePropertyNames(ANY, pname.तथ्य())) {
                    set(MIN_मूल्य, MAX_मूल्य);
                    वापस *यह;
                } अन्यथा अगर (0 == uprv_comparePropertyNames(ASCII, pname.तथ्य())) {
                    set(0, 0x7F);
                    वापस *यह;
                } अन्यथा अगर (0 == uprv_comparePropertyNames(ASSIGNED, pname.तथ्य())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_MASK;
                    v = U_GC_CN_MASK;
                    invert = true;
                } अन्यथा {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_मूल्य(p, v, ec);
    अगर(invert) {
        complement().removeAllStrings();  // code point complement
    }

    अगर (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_MEMORY_ALLOCATION_ERROR;
    }
    वापस *यह;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool UnicodeSet::resemblesPropertyPattern(स्थिर UnicodeString& pattern,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    अगर ((pos+5) > pattern.length()) {
        वापस false;
    }

    // Look for an opening [:, [:^, \p, or \P
    वापस isPOSIXOpen(pattern, pos) || isPerlOpen(pattern, pos) || isNameOpen(pattern, pos);
}

U_NAMESPACE_END
