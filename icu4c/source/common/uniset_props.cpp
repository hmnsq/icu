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

ساكن Unicode_سېټ *uni32Singleton;
ساكن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساكن UBool U_CALLCONV uset_cleanup() {
    ړنګول uni32Singleton;
    uni32Singleton = تش_اشاره;
    uni32InitOnce.reset();
    أرجع ریښتیا;
}

U_CDECL_پای_ټکی

U_NAMESPACE_BEGIN

کارول U_سرلیک_ONLY_NAMESPACE::utf_String_کوډ_Points;

نطاق {

// Cache some sets for other services -------------------------------------- ***
فراغ U_CALLCONV create_Uni32_سېټ(UError_کوډ &error_کوډ) {
    U_ASSERT(uni32Singleton == تش_اشاره);
    uni32Singleton = جديد Unicode_سېټ(UnicodeString(u"[:age=3.2:]"), error_کوډ);
    إذا(uni32Singleton==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    } وإلا {
        uni32Singleton->freeze();
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_سېټ *
uniset_getUnicode32Instance(UError_کوډ &error_کوډ) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_سېټ, error_کوډ);
    أرجع uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

مضمن UBool
isPerlOpen(ثابت UnicodeString &نمونه, int32_t pos) {
    char16_t c;
    أرجع نمونه.charAt(pos)==u'\\' && ((c=نمونه.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

مضمن UBool
isNameOpen(ثابت UnicodeString &نمونه, int32_t pos) {
    أرجع نمونه.charAt(pos)==u'\\' && نمونه.charAt(pos+1)==u'N';
}

مضمن UBool
isPOSIXOpen(ثابت UnicodeString &نمونه, int32_t pos) {
    أرجع نمونه.charAt(pos)==u'[' && نمونه.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#تعریف_کړه _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ثابت CharString& name) {
    لكل (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_کوډ ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.معلومات(), &ec);
        إذا (U_SUCCESS(ec)) {
            أرجع ch;
        }
    }
    أرجع U_SENTINEL;
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
Unicode_سېټ::Unicode_سېټ(ثابت UnicodeString& نمونه,
                       UError_کوډ& حالت) {
    apply_نمونه(نمونه, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سېټ& Unicode_سېټ::apply_نمونه(ثابت UnicodeString& نمونه,
                                     UError_کوډ& حالت) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_نمونه_Ignore_Space(نمونه, pos, تش_اشاره, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_شاخص();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(نمونه, i, ریښتیا);
    إذا (i != نمونه.length()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

فراغ
Unicode_سېټ::apply_نمونه_Ignore_Space(ثابت UnicodeString& نمونه,
                                    ParsePosition& pos,
                                    ثابت نښه_جدول* symbols,
                                    UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (isFrozen()) {
        حالت = U_NO_WRITE_PERMISSION;
        أرجع;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعده_Character_Iterator chars(نمونه, symbols, pos);
    apply_نمونه(نمونه, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, تش_اشاره, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (chars.دننه_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سېټ;
        أرجع;
    }
    سېټ_نمونه(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_سېټ::resembles_نمونه(ثابت UnicodeString& نمونه, int32_t pos) {
    أرجع ((pos+1) < نمونه.length() &&
            نمونه.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_نمونه(نمونه, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#تعریف_کړه U_DEBUGGING_UNICODESET_PARSING 0

صنف Unicode_سېټ::Lexer {
  عام:
    Lexer(ثابت UnicodeString &نمونه,
          ثابت ParsePosition &parsePosition,
          قاعده_Character_Iterator &chars,
          uint32_t unicode_سېټ_Options,
          ثابت نښه_جدول *ثابت symbols,
          Unicode_سېټ &(Unicode_سېټ::*case_تړون)(int32_t attribute))
        : نمونه_2(نمونه), parsePosition_(parsePosition), chars_(chars),
          unicode_سېټ_Options_2(unicode_سېټ_Options),
          charsOptions_(قاعده_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_سېټ_Options & USET_IGNORE_SPACE) != 0
                             ? قاعده_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_تړون_2(case_تړون) {}

    صنف LexicalElement {
      عام:
        منطقي is_سېټ_Operator(ثابت char16_t op) ثابت {
            أرجع U_SUCCESS(error_کوډ_2) && category_ == سېټ_OPERATOR && string_[0] == op;
        }

        منطقي isStringLiteral() ثابت {
            أرجع U_SUCCESS(error_کوډ_2) && category_ == STRING_LITERAL;
        }

        منطقي isNamedElement() ثابت {
            أرجع U_SUCCESS(error_کوډ_2) && category_ == NAMED_ELEMENT;
        }

        منطقي isBracketedElement() ثابت {
            أرجع U_SUCCESS(error_کوډ_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            إذا (U_SUCCESS(error_کوډ_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                أرجع string_;
            }
            أرجع std::nullopt;
        }

        std::optional<UChar32> کوډ_ټکی() ثابت {
            إذا (U_SUCCESS(error_کوډ_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                أرجع string_.char32At(0);
            }
            أرجع std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت Unicode_سېټ *سېټ() ثابت {
            إذا (U_FAILURE(error_کوډ_2)) {
                أرجع تش_اشاره;
            }
            إذا (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                إذا (precomputed_سېټ != تش_اشاره) {
                    أرجع precomputed_سېټ;
                } وإلا {
                    أرجع &سېټ_2;
                }
            }
            أرجع تش_اشاره;
        }

        ثابت UError_کوډ& error_کوډ() ثابت{
          أرجع error_کوډ_2;
        }

#إذا U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString پایله;
            إذا (U_FAILURE(error_کوډ_2)) {
                پایله.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_کوډ_2)))
                    .append(u"), possibly ");
            }
            أرجع پایله.append(category_نومونه[category_])
                .append(u" '")
                .append(سرچینه_متن)
                .append(u"'");
        }
#د_که_پای

      خاص:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        تعداد Category : std::uint8_t {
            سېټ_OPERATOR,
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
            پای_ټکی_د_متن,
        };
        ساكن constexpr std::array<std::u16string_view, پای_ټکی_د_متن + 1> category_نومونه{{
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
        LexicalElement(Category category, UnicodeString string, قاعده_Character_Iterator::Pos after,
                       ثابت UError_کوډ error_کوډ, ثابت Unicode_سېټ *precomputed_سېټ_2, Unicode_سېټ سېټ,
                       std::u16string_view سرچینه_متن_2, منطقي separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_کوډ_2(error_کوډ),
              precomputed_سېټ(precomputed_سېټ_2), سېټ_2(سېټ), سرچینه_متن(سرچینه_متن_2) {
            إذا (U_FAILURE(error_کوډ_2) || separated || !preceding.has_value()) {
                أرجع;
            }
            إذا (preceding->is_سېټ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *کوډ_ټکی() == u':') {
                error_کوډ_2 = U_MALFORMED_سېټ;
            } وإلا إذا (preceding->category_ == ESCAPED_ELEMENT) {
                إذا (U_IS_LEAD(*preceding->کوډ_ټکی()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*کوډ_ټکی())) {
                    error_کوډ_2 = U_MALFORMED_سېټ;
                } وإلا إذا (preceding->سرچینه_متن.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *کوډ_ټکی();
                    إذا (preceding->سرچینه_متن[1] == u'x') {
                        إذا ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_کوډ_2 = U_MALFORMED_سېټ;
                        }
                    } وإلا إذا (preceding->سرچینه_متن[1] >= u'0' &&
                               preceding->سرچینه_متن[1] <= u'7') {
                        إذا (cp >= u'0' && cp <= u'7') {
                            error_کوډ_2 = U_MALFORMED_سېټ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        قاعده_Character_Iterator::Pos after_;
        UError_کوډ error_کوډ_2;
        ثابت Unicode_سېټ *precomputed_سېټ;
        Unicode_سېټ سېټ_2;
        std::u16string_view سرچینه_متن;

        ملګری صنف Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        أرجع نمونه_2.لنډمهاله_Sub_String(0, parsePosition_.get_شاخص()) + u"☞" +
               نمونه_2.لنډمهاله_Sub_String(parsePosition_.get_شاخص(), 60);
    }

    منطقي accept_سېټ_Operator(char16_t op) {
        إذا (lookahead().is_سېټ_Operator(op)) {
            advance();
            أرجع ریښتیا;
        }
        أرجع خطأ;
    }

    ثابت LexicalElement &lookahead() {
        إذا (!ahead_.has_value()) {
            ثابت قاعده_Character_Iterator::Pos before = getPos();
            ahead_.emplace(بل_Token(behind_));
            chars_.سېټ_Pos(before);
        }
        أرجع *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        إذا (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت قاعده_Character_Iterator::Pos before = getPos();
            chars_.سېټ_Pos(lookahead().after_);
            ahead2_.emplace(بل_Token(ahead_));
            chars_.سېټ_Pos(before);
        }
        أرجع *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    قاعده_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        أرجع chars_;
    }

    int32_t charsOptions() {
        أرجع charsOptions_;
    }

    منطقي at_پای_ټکی() ثابت {
        أرجع chars_.at_پای_ټکی();
    }

    فراغ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.سېټ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خاص:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    قاعده_Character_Iterator::Pos getPos() ثابت {
        قاعده_Character_Iterator::Pos پایله;
        chars_.getPos(پایله);
        أرجع پایله;
    }

    LexicalElement بل_Token(std::optional<LexicalElement> ثابت& preceding) {
        UError_کوډ error_کوډ = U_ZERO_ERROR;
        منطقي separated = خطأ;
        إذا (charsOptions_ & قاعده_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            قاعده_Character_Iterator::Pos pos;
            لكل (;;) {
                UBool ناکارول_شوی_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                  قاعده_Character_Iterator::SKIP_WHITESPACE),
                                ناکارول_شوی_Escaped, error_کوډ);
                إذا (!نمونه_Props::isWhiteSpace(s)) {
                    chars_.سېټ_Pos(pos);
                    اكسر;
                }
                إذا (s != u'\u200E' && s != u'\u200F') {
                    separated = ریښتیا;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        إذا (chars_.at_پای_ټکی()) {
            أرجع LexicalElement(LexicalElement::پای_ټکی_د_متن, {}, getPos(), error_کوډ,
                                  /*precomputedSet=*/تش_اشاره,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t پیل = parsePosition_.get_شاخص();
        ثابت قاعده_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool ناکارول_شوی_Escaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~قاعده_Character_Iterator::PARSE_ESCAPES, ناکارول_شوی_Escaped, error_کوډ);
        إذا (first == u'[' || first == u'\\') {
            ثابت قاعده_Character_Iterator::Pos after_لومړی = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 دوهم = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                                 قاعده_Character_Iterator::SKIP_WHITESPACE),
                                               ناکارول_شوی_Escaped, error_کوډ);
            إذا ((first == u'[' && دوهم == u':') ||
                (first == u'\\' && (دوهم == u'p' || دوهم == u'P' || دوهم == u'N'))) {
                إذا (دوهم == u'N') {
                    UChar32 ثابت query_پایله = لټون_Named_Element_Brackets(error_کوډ);
                    أرجع LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_پایله), getPos(), error_کوډ,
                        /*precomputedSet=*/تش_اشاره,
                        /*set=*/{},
                        std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                        separated, preceding);
                } وإلا {
                    Unicode_سېټ query_پایله = لټون_Property_Query_After_پیل(first, دوهم, پیل, error_کوډ);
                    أرجع LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_کوډ,
                        /*precomputedSet=*/تش_اشاره, /*set=*/std::move(query_پایله),
                        std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.سېټ_Pos(after_لومړی);
        }
        إذا (first == u'$' && symbols_ != تش_اشاره) {
            خپلکاری name_پای_ټکی = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            إذا (UnicodeString name = symbols_->parseReference(نمونه_2, name_پای_ټکی, نمونه_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_تشه()) {
                chars_.jumpahead(name_پای_ټکی.get_شاخص() - (پیل + 1));
                ثابت std::u16string_view سرچینه =
                    std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل);
                ثابت Unicode_سېټ *precomputed_سېټ_2 = symbols_->lookup_سېټ(name);
                إذا (precomputed_سېټ_2 != تش_اشاره) {
                    أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_سېټ_2, /*set=*/{}, سرچینه, /*separated=*/ریښتیا,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                إذا (expression == تش_اشاره) {
                    أرجع LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/تش_اشاره,
                        /*set=*/{},
                        سرچینه,
                        /*separated=*/ریښتیا,
                        /*preceding=*/std::nullopt);
                }
                أرجع evaluateVariable(*expression, سرچینه);
            }
        }
        بدل (first) {
        حالة u'[':
            أرجع LexicalElement(
                LexicalElement::سېټ_OPERATOR, UnicodeString(u'['), getPos(), error_کوډ,
                /*precomputedSet=*/تش_اشاره,
                /*set=*/{},
                std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                separated, preceding);
        حالة u'\\': {
            // Now try to parse the escape.
            chars_.سېټ_Pos(before);
            UChar32 کوډ_ټکی = chars_.next(charsOptions_, ناکارول_شوی_Escaped, error_کوډ);
            أرجع LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(کوډ_ټکی), getPos(), error_کوډ,
                تش_اشاره,
                /*set=*/{},
                std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                separated, preceding);
        }
        حالة u'&':
        حالة u'-':
        حالة u']':
        حالة u'^':
        حالة u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            أرجع LexicalElement(
                LexicalElement::سېټ_OPERATOR, UnicodeString(first), getPos(), error_کوډ,
                /*precomputedSet=*/تش_اشاره,
                /*set=*/{},
                std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                separated, preceding);
        حالة u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t کوډ_ټکی_شمېر = 0;
            طالما (!chars_.at_پای_ټکی() && U_SUCCESS(error_کوډ)) {
                ثابت قاعده_Character_Iterator::Pos before_بل = getPos();
                next = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                     قاعده_Character_Iterator::SKIP_WHITESPACE),
                                   ناکارول_شوی_Escaped, error_کوډ);
                إذا (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                      قاعده_Character_Iterator::SKIP_WHITESPACE),
                                    ناکارول_شوی_Escaped, error_کوډ);
                    إذا (afterBackslash == u'N') {
                        next = لټون_Named_Element_Brackets(error_کوډ);
                        escaped = ریښتیا;
                    } وإلا إذا (afterBackslash == u'p' || afterBackslash == u'P') {
                        أرجع LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_سېټ,
                                              /*precomputedSet=*/تش_اشاره,
                                              /*set=*/{},
                                              std::u16string_view(نمونه_2).substr(
                                                  پیل, parsePosition_.get_شاخص() - پیل),
                                              separated, preceding);
                    } وإلا {
                        chars_.سېټ_Pos(before_بل);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_کوډ);
                    }
                } وإلا {
#إذا U_ICU_نسخه_MAJOR_عدد < 81
                    إذا (U_SUCCESS(error_کوډ) && نمونه_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_کوډ = UError_کوډ::U_ILLEGAL_دلیل_ERROR;
                    }
#وإلا
#تېروتنه Remove هذا transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#د_که_پای
                    escaped = خطأ;
                }
                إذا (!escaped && next == u'}') {
                    أرجع LexicalElement(
                        کوډ_ټکی_شمېر == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_کوډ,
                        /*precomputedSet=*/تش_اشاره,
                        /*set=*/{},
                        std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                        separated, preceding);
                }
                string.append(next);
                کوډ_ټکی_شمېر += 1;
            }
            أرجع LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_سېټ,
                /*precomputedSet=*/تش_اشاره,
                /*set=*/{},
                std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                separated, preceding);
        }
        افتراضي:
            أرجع LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_کوډ, تش_اشاره,
                /*set=*/{},
                std::u16string_view(نمونه_2).substr(پیل, parsePosition_.get_شاخص() - پیل),
                separated, preceding);
        }
    }

    UChar32 لټون_Named_Element_Brackets(UError_کوډ &error_کوډ) {
        UBool ناکارول_شوی_Escaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                           قاعده_Character_Iterator::SKIP_WHITESPACE),
                                         ناکارول_شوی_Escaped, error_کوډ);
        إذا (open == u'{') {
            int32_t پیل = parsePosition_.get_شاخص();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            طالما (!chars_.at_پای_ټکی() && U_SUCCESS(error_کوډ)) {
                UChar32 وروستی = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                             قاعده_Character_Iterator::SKIP_WHITESPACE),
                                           ناکارول_شوی_Escaped, error_کوډ);
                إذا (وروستی == u':') {
                    إذا (!hex.has_value()) {
                        hex.emplace();
                        إذا (پیل == parsePosition_.get_شاخص() - 1) {
                            error_کوډ = U_ILLEGAL_دلیل_ERROR;
                            أرجع {};
                        }
                        لكل (char16_t digit : std::u16string_view(نمونه_2).substr(
                                 پیل, parsePosition_.get_شاخص() - 1 - پیل)) {
                            uint8_t nibble;
                            إذا (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } وإلا {
                                digit = digit & ~0x20;
                                إذا (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } وإلا {
                                    error_کوډ = U_ILLEGAL_دلیل_ERROR;
                                    أرجع {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            إذا (hex > 0x10FFFF) {
                                error_کوډ = U_ILLEGAL_دلیل_ERROR;
                                أرجع {};
                            }
                        }
                    } وإلا إذا (!literal.has_value()) {
                        ثابت خپلکاری literal_کوډ_Points = utf_String_کوډ_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(نمونه_2).substr(پیل,
                                                                 parsePosition_.get_شاخص() - 1 - پیل));
                        خپلکاری it = literal_کوډ_Points.begin();
                        إذا (it == literal_کوډ_Points.پای_ټکی() || !it->wellFormed() ||
                                (literal = it->کوډ_ټکی(), ++it) != literal_کوډ_Points.پای_ټکی()) {
                            error_کوډ = U_ILLEGAL_دلیل_ERROR;
                            أرجع {};
                        }
                    } وإلا {
                        error_کوډ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    پیل = parsePosition_.get_شاخص();
                } وإلا إذا (وروستی == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(نمونه_2).substr(
                        پیل, parsePosition_.get_شاخص() - 1 - پیل);
                    ثابت UChar32 پایله = getCharacterByName(CharString().appendInvariantChars(
                        u16name.معلومات(), static_cast<int32_t>(u16name.length()), error_کوډ));
                    إذا (!U_SUCCESS(error_کوډ)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_کوډ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    إذا (پایله < 0 || (hex.has_value() && پایله != hex) ||
                            (literal.has_value() && پایله != literal)) {
                        error_کوډ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    أرجع پایله;
                }
            }
        }
        إذا (U_SUCCESS(error_کوډ)) {
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view سرچینه) {
        UError_کوډ error_کوډ = U_ZERO_ERROR;
        ParsePosition افاده_Position;
        قاعده_Character_Iterator افاده_Iterator(expression, symbols_, افاده_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer افاده_Lexer(expression, افاده_Position, افاده_Iterator, unicode_سېټ_Options_2,
                              /*symbols=*/تش_اشاره, case_تړون_2);
        خپلکاری variableToken = افاده_Lexer.lookahead();
        إذا (variableToken.is_سېټ_Operator(u'[')) {
            UnicodeString rebuilt_نمونه;
            Unicode_سېټ افاده_ارزښت =
                parse_Unicode_سېټ(افاده_Lexer, rebuilt_نمونه, unicode_سېټ_Options_2, case_تړون_2,
                                /*depth=*/0, error_کوډ);
            افاده_ارزښت.سېټ_نمونه(rebuilt_نمونه);
            إذا (!افاده_Lexer.at_پای_ټکی()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/تش_اشاره,
                    /*set=*/{},
                    سرچینه,
                    /*separated=*/ریښتیا,
                    /*preceding=*/std::nullopt);
            }
            أرجع LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_کوډ,
                /*precomputedSet=*/تش_اشاره,
                /*set=*/std::move(افاده_ارزښت),
                سرچینه,
                /*separated=*/ریښتیا,
                /*preceding=*/std::nullopt);
        } وإلا {
            افاده_Lexer.advance();
            إذا (!افاده_Lexer.at_پای_ټکی()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/تش_اشاره,
                    /*set=*/{},
                    سرچینه,
                    /*separated=*/ریښتیا,
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
                أرجع LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_کوډ_2, variableToken.precomputed_سېټ, std::move(variableToken.سېټ_2), سرچینه,
                    /*separated=*/ریښتیا,
                    /*preceding=*/std::nullopt);
            افتراضي:
                أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/تش_اشاره,
                                      /*set=*/{}, سرچینه,
                                      /*separated=*/ریښتیا,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_سېټ لټون_Property_Query_After_پیل(UChar32 first, UChar32 دوهم, int32_t query_پیل, UError_کوډ &error_کوډ) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_افاده_پیل = parsePosition_.get_شاخص();
        منطقي exteriorlyNegated = خطأ;
        منطقي interiorlyNegated = خطأ;
        UBool ناکارول_شوی_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 دریم = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                            قاعده_Character_Iterator::SKIP_WHITESPACE),
                                          ناکارول_شوی_Escaped, error_کوډ);
        إذا (first == u'\\') {
            إذا (دریم != u'{') {
                error_کوډ = U_ILLEGAL_دلیل_ERROR;
                أرجع {};
            }
            exteriorlyNegated = دوهم == u'P';
            query_افاده_پیل = parsePosition_.get_شاخص();
        } وإلا {
            إذا (دریم == u'^') {
                exteriorlyNegated = ریښتیا;
                query_افاده_پیل = parsePosition_.get_شاخص();
            }
        }
        قاعده_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                          قاعده_Character_Iterator::SKIP_WHITESPACE),
                        ناکارول_شوی_Escaped, error_کوډ);

        طالما (!chars_.at_پای_ټکی() && U_SUCCESS(error_کوډ)) {
            ثابت قاعده_Character_Iterator::Pos before_وروستی = getPos();
            UChar32 وروستی_Unescaped =
                chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                              قاعده_Character_Iterator::SKIP_WHITESPACE),
                            ناکارول_شوی_Escaped, error_کوډ);
            إذا (penultimateUnescaped == u'\\') {
                إذا (وروستی_Unescaped == 'N') {
                    لټون_Named_Element_Brackets(error_کوډ);
                    إذا (!U_SUCCESS(error_کوډ)) {
                        أرجع {};
                    }
                } وإلا {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.سېټ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~قاعده_Character_Iterator::SKIP_WHITESPACE, ناکارول_شوی_Escaped,
                                error_کوډ);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                وروستی_Unescaped = -1;
            } وإلا إذا (!queryOperatorPosition.has_value() && وروستی_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_شاخص() - 1;
            } وإلا إذا (!queryOperatorPosition.has_value() && وروستی_Unescaped == u'≠') {
                إذا (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_کوډ = U_ILLEGAL_دلیل_ERROR;
                    أرجع {};
                }
                interiorlyNegated = ریښتیا;
                queryOperatorPosition = parsePosition_.get_شاخص() - 1;
            } وإلا إذا ((first == u'[' && penultimateUnescaped == u':' && وروستی_Unescaped == u']') ||
                       (first == u'\\' && وروستی_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ثابت int32_t query_افاده_حد =
                    first == u'[' ? parsePosition_.get_شاخص() - 2 : parsePosition_.get_شاخص() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                إذا (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        نمونه_2.لنډمهاله_Sub_String_Between(*queryOperatorPosition + 1, query_افاده_حد);
                    إذا (propertyPredicate.is_تشه()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_کوډ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                }
                Unicode_سېټ پایله;
                پایله.applyPropertyAlias(
                    نمونه_2.لنډمهاله_Sub_String_Between(query_افاده_پیل,
                                                  queryOperatorPosition.value_or(query_افاده_حد)),
                    propertyPredicate, error_کوډ);
                إذا (exteriorlyNegated != interiorlyNegated) {
                    پایله.complement().remove_All_مزي_ګانې();
                }
                پایله.سېټ_نمونه(نمونه_2.لنډمهاله_Sub_String_Between(query_پیل, parsePosition_.get_شاخص()));
                أرجع پایله;
            }
            beforePenultimate = before_وروستی;
            penultimateUnescaped = وروستی_Unescaped;
        }
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع {};
    }

    ثابت UnicodeString &نمونه_2;
    ثابت ParsePosition &parsePosition_;
    قاعده_Character_Iterator &chars_;
    ثابت uint32_t unicode_سېټ_Options_2;
    ثابت int32_t charsOptions_;
    ثابت نښه_جدول *ثابت symbols_;
    Unicode_سېټ &(Unicode_سېټ::* ثابت case_تړون_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نطاق {

constexpr int32_t MAX_ژوروالی = 100;

#إذا U_DEBUGGING_UNICODESET_PARSING

#تعریف_کړه U_UNICODESET_RETURN_IF_ERROR(پایله, ec)                                                   \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        إذا (U_FAILURE(ec)) {                                                                       \
            إذا (ژوروالی < 5) {                                                                       \
                چاپ_کړه("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } وإلا إذا (ژوروالی == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                چاپ_کړه("--- [...]\n");                                                             \
            }                                                                                      \
            أرجع پایله;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کړه U_UNICODESET_RETURN_WITH_PARSE_ERROR(پایله, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        چاپ_کړه("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).منزل_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().منزل_UTF8_String(contextUTF8).c_str());                 \
        چاپ_کړه("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        إذا (U_FAILURE(lexer.lookahead().error_کوډ())) {                                            \
            (ec) = lexer.lookahead().error_کوډ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سېټ;                                                                \
        }                                                                                          \
        أرجع پایله;                                                                             \
    } طالما (خطأ)

#وإلا

#تعریف_کړه U_UNICODESET_RETURN_IF_ERROR(پایله, ec)                                                   \
    افعل {                                                                                           \
        إذا (U_FAILURE(ec)) {                                                                       \
            أرجع پایله;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کړه U_UNICODESET_RETURN_WITH_PARSE_ERROR(پایله, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        إذا (U_FAILURE(lexer.lookahead().error_کوډ())) {                                            \
            (ec) = lexer.lookahead().error_کوډ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سېټ;                                                                \
        }                                                                                          \
        أرجع پایله;                                                                             \
    } طالما (خطأ)

#د_که_پای

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

فراغ Unicode_سېټ::apply_نمونه(ثابت UnicodeString &نمونه,
                              ثابت ParsePosition &parsePosition,
                              قاعده_Character_Iterator &chars,
                              ثابت نښه_جدول *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_سېټ &(Unicode_سېټ::*case_تړون)(int32_t attribute),
                              UError_کوډ &ec) {
    إذا (U_FAILURE(ec)) أرجع;
    Lexer lexer(نمونه, parsePosition, chars, options, symbols, case_تړون);
    *هذا = parse_Unicode_سېټ(lexer, rebuiltPat, options, case_تړون, /*depth=*/0, ec);
}

Unicode_سېټ Unicode_سېټ::parse_Unicode_سېټ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_سېټ& (Unicode_سېټ::*case_تړون)(int32_t attribute),
                                       int32_t ژوروالی,
                                       UError_کوډ &ec) {
    Unicode_سېټ پایله;

    إذا (ژوروالی > MAX_ژوروالی) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_ژوروالی)).c_str(),
                                             ("depth = " + std::to_string(ژوروالی)).c_str(), lexer,
                                             ec);
    }

    منطقي isComplement = خطأ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقي preserve_Syntax_دننه_نمونه = خطأ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_نمونه;
    إذا (lexer.lookahead().سېټ() != تش_اشاره) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        پایله = *lexer.lookahead().سېټ();
        پایله.منزل_نمونه_2(pretty_Printed_نمونه, /*escapeUnprintable=*/خطأ);
        lexer.advance();
        preserve_Syntax_دننه_نمونه = ریښتیا;
    } وإلا {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        إذا (lexer.accept_سېټ_Operator(u'[')) {
            pretty_Printed_نمونه.append(u'[');
            إذا (lexer.accept_سېټ_Operator(u'^')) {
                pretty_Printed_نمونه.append(u'^');
                isComplement = ریښتیا;
            }
            پایله = parseContent(lexer, pretty_Printed_نمونه, options, case_تړون, ژوروالی,
                                  /*containsSetOperation=*/preserve_Syntax_دننه_نمونه, ec);
            U_UNICODESET_RETURN_IF_ERROR(پایله, ec);
            إذا (!lexer.accept_سېټ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(پایله, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_نمونه.append(u']');
        } وإلا {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(پایله, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    إذا ((options & USET_CASE_ماسک) != 0) {
        (پایله.*case_تړون)(options);
    }
    إذا (isComplement) {
        پایله.complement().remove_All_مزي_ګانې(); // code point complement
    }
    إذا (preserve_Syntax_دننه_نمونه) {
        rebuiltPat.append(pretty_Printed_نمونه);
    } وإلا {
        پایله.generate_نمونه(rebuiltPat, /*escapeUnprintable=*/خطأ);
    }
    أرجع پایله;
}

Unicode_سېټ Unicode_سېټ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سېټ &(Unicode_سېټ::*case_تړون)(int32_t attribute),
                                    int32_t ژوروالی,
                                    منطقي &لري_سېټ_Operation,
                                    UError_کوډ &ec) {
    Unicode_سېټ پایله;
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
    إذا (lexer.accept_سېټ_Operator(u'-')) {
        پایله.جمع_کړه(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    طالما (!lexer.at_پای_ټکی()) {
        إذا (lexer.accept_سېټ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            پایله.جمع_کړه(u'-');
            أرجع پایله;
        } وإلا إذا (lexer.lookahead().is_سېټ_Operator(u'$')) {
            إذا (lexer.lookahead2().is_سېټ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                پایله.جمع_کړه(U_ETHER);
                لري_سېټ_Operation = ریښتیا;
                أرجع پایله;
            }
        }
        إذا (lexer.lookahead().is_سېټ_Operator(u']')) {
            أرجع پایله;
        }
        // Also handles FinalMutation.
        پایله.parseMutation(lexer, rebuiltPat, options, case_تړون, ژوروالی, لري_سېټ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(پایله, ec);
    }
    أرجع پایله;
}

فراغ Unicode_سېټ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_سېټ &(Unicode_سېټ::*case_تړون)(int32_t attribute),
                               int32_t ژوروالی,
                               منطقي &لري_سېټ_Operation,
                               UError_کوډ &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    إذا (lexer.lookahead().is_سېټ_Operator('[') || lexer.lookahead().سېټ() != تش_اشاره) {
        لري_سېټ_Operation = ریښتیا;
        parse_سېټ_Operations(lexer, rebuiltPat, options, case_تړون, ژوروالی, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } وإلا {
        // Also handles DollarElements.
        جمع_کړه_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

فراغ Unicode_سېټ::parse_سېټ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سېټ &(Unicode_سېټ::*case_تړون)(int32_t attribute),
                                    int32_t ژوروالی,
                                    UError_کوډ &ec) {
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
    ثابت Unicode_سېټ کیڼ_Hand_Side =
        parse_Unicode_سېټ(lexer, rebuiltPat, options, case_تړون, ژوروالی + 1, ec);
    جمع_کړه_All(کیڼ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    لكل (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        إذا (lexer.accept_سېټ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت Unicode_سېټ ښی_Hand_Side =
                parse_Unicode_سېټ(lexer, rebuiltPat, options, case_تړون, ژوروالی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ښی_Hand_Side);
        } وإلا إذا (lexer.lookahead().is_سېټ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            إذا (lexer.lookahead2().is_سېټ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                أرجع;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت Unicode_سېټ ښی_Hand_Side =
                parse_Unicode_سېټ(lexer, rebuiltPat, options, case_تړون, ژوروالی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ښی_Hand_Side);
        } وإلا {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            أرجع;
        }
    }
}

Unicode_سېټ Unicode_سېټ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_کوډ &ec) {
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
    إذا (lexer.lookahead().isStringLiteral()) {
        ثابت UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_منزل_Pat(rebuiltPat, element, /*escapeUnprintable=*/خطأ);
        rebuiltPat.append(u'}');
        lexer.advance();
        أرجع Unicode_سېټ().جمع_کړه(element);
    }
    UChar32 first;
    إذا (lexer.lookahead().is_سېټ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } وإلا إذا (lexer.lookahead().کوډ_ټکی().has_value()) {
        first = *lexer.lookahead().کوډ_ټکی();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, first, /*escapeUnprintable=*/خطأ);
    إذا (!lexer.lookahead().is_سېټ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        أرجع Unicode_سېټ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    إذا (lexer.lookahead2().is_سېټ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        أرجع Unicode_سېټ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 وروستی;
    إذا (lexer.lookahead().is_سېټ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        وروستی = u'$';
        إذا (lexer.lookahead2().is_سېټ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } وإلا إذا (lexer.lookahead().کوډ_ټکی().has_value()) {
        وروستی = *lexer.lookahead().کوډ_ټکی();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    إذا (وروستی <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(وروستی) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, وروستی, /*escapeUnprintable=*/خطأ);
    أرجع Unicode_سېټ(first, وروستی);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

نطاق {

UBool numeric_ارزښت_Filter(UChar32 ch, فراغ* context) {
    أرجع u_get_Numeric_ارزښت(ch) == *static_cast<مزدوج*>(context);
}

UBool general_Category_ماسک_Filter(UChar32 ch, فراغ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    أرجع (U_GET_GC_ماسک((UChar32) ch) & value) != 0;
}

UBool نسخه_Filter(UChar32 ch, فراغ* context) {
    ساكن ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* نسخه = static_cast<UVersionInfo*>(context);
    أرجع uprv_memcmp(&v, &none, حجم(v)) > 0 && uprv_memcmp(&v, نسخه, حجم(v)) <= 0;
}

تعريفنوع بنية {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, فراغ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    أرجع u_get_Int_Property_ارزښت(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, فراغ* context) {
    أرجع uscript_hasScript(ch, *static_cast<UScript_کوډ*>(context));
}

UBool idTypeFilter(UChar32 ch, فراغ* context) {
    أرجع u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
فراغ Unicode_سېټ::applyFilter(Unicode_سېټ::Filter filter,
                             فراغ* context,
                             ثابت Unicode_سېټ* inclusions,
                             UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) أرجع;

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

    UChar32 پیل_Has_Property = -1;
    int32_t حد_Range = inclusions->get_Range_شمېر();

    لكل (صحيح j=0; j<حد_Range; ++j) {
        // get current range
        UChar32 پیل = inclusions->get_Range_پیل(j);
        UChar32 پای_ټکی = inclusions->get_Range_پای_ټکی(j);

        // for all the code points in the range, process
        لكل (UChar32 ch = پیل; ch <= پای_ټکی; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            إذا ((*filter)(ch, context)) {
                إذا (پیل_Has_Property < 0) {
                    پیل_Has_Property = ch;
                }
            } وإلا إذا (پیل_Has_Property >= 0) {
                جمع_کړه(پیل_Has_Property, ch-1);
                پیل_Has_Property = -1;
            }
        }
    }
    إذا (پیل_Has_Property >= 0) {
        جمع_کړه(پیل_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    إذا (isBogus() && U_SUCCESS(حالت)) {
        // We likely ran out of memory. AHHH!
        حالت = U_حافظه_ویش_ERROR;
    }
}

نطاق {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#تعریف_کړه FAIL(ec) UPRV_بلاک_MACRO_BEGIN { \
    ec=U_ILLEGAL_دلیل_ERROR; \
    أرجع *هذا; \
} UPRV_بلاک_MACRO_پای_ټکی

Unicode_سېټ&
Unicode_سېټ::apply_Int_Property_ارزښت(UProperty prop, int32_t value, UError_کوډ& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) { أرجع *هذا; }
    إذا (prop == UCHAR_GENERAL_CATEGORY_ماسک) {
        ثابت Unicode_سېټ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_ماسک_Filter, &value, inclusions, ec);
    } وإلا إذا (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت Unicode_سېټ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_کوډ script = static_cast<UScript_کوډ>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } وإلا إذا (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت Unicode_سېټ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } وإلا إذا (0 <= prop && prop < UCHAR_دوګونی_حد) {
        إذا (value == 0 || value == 1) {
            ثابت USet *سېټ = u_get_دوګونی_Property_سېټ(prop, &ec);
            إذا (U_FAILURE(ec)) { أرجع *هذا; }
            copyFrom(*Unicode_سېټ::fromUSet(سېټ), ریښتیا);
            إذا (value == 0) {
                complement().remove_All_مزي_ګانې();  // code point complement
            }
        } وإلا {
            clear();
        }
    } وإلا إذا (UCHAR_INT_پیل <= prop && prop < UCHAR_INT_حد) {
        ثابت Unicode_سېټ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } وإلا {
        ec = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سېټ&
Unicode_سېټ::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UError_کوډ& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) أرجع *هذا;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    إذا( !uprv_isInvariantUString(prop.get_بفر(), prop.length()) ||
        !uprv_isInvariantUString(value.get_بفر(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    إذا (U_FAILURE(ec)) أرجع *هذا;

    UProperty p;
    int32_t v;
    UBool invert = خطأ;

    إذا (value.length() > 0) {
        p = u_getPropertyEnum(pname.معلومات());
        إذا (p == UCHAR_INVALID_کوډ) FAIL(ec);

        // Treat gc as gcm
        إذا (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_ماسک;
        }

        إذا ((p >= UCHAR_دوګونی_پیل && p < UCHAR_دوګونی_حد) ||
            (p >= UCHAR_INT_پیل && p < UCHAR_INT_حد) ||
            (p >= UCHAR_ماسک_پیل && p < UCHAR_ماسک_حد)) {
            v = u_get_Property_ارزښت_Enum(p, vname.معلومات());
            إذا (v == UCHAR_INVALID_کوډ) {
                // Handle numeric CCC
                إذا (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* پای_ټکی;
                    مزدوج val = uprv_strtod(vname.معلومات(), &پای_ټکی);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    إذا (*پای_ټکی != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } وإلا {
                    FAIL(ec);
                }
            }
        }

        وإلا {

            بدل (p) {
            حالة UCHAR_NUMERIC_ارزښت:
                {
                    حرف* پای_ټکی;
                    مزدوج val = uprv_strtod(vname.معلومات(), &پای_ټکی);
                    إذا (*پای_ټکی != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_ارزښت_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    أرجع *هذا;
                }
            حالة UCHAR_NAME:
                {
                    ثابت UChar32 ch = getCharacterByName(vname);
                    إذا (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    جمع_کړه(ch);
                    أرجع *هذا;
                }
            حالة UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالة UCHAR_AGE:
                {
                    UVersionInfo نسخه;
                    u_نسخه_From_String(نسخه, vname.معلومات());
                    applyFilter(نسخه_Filter, &نسخه,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    أرجع *هذا;
                }
            حالة UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_ارزښت_Enum(UCHAR_SCRIPT, vname.معلومات());
                إذا (v == UCHAR_INVALID_کوډ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            حالة UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_ارزښت_Enum(p, vname.معلومات());
                إذا (v == UCHAR_INVALID_کوډ) {
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

    وإلا {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_ماسک;
        v = u_get_Property_ارزښت_Enum(p, pname.معلومات());
        إذا (v == UCHAR_INVALID_کوډ) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_ارزښت_Enum(p, pname.معلومات());
            إذا (v == UCHAR_INVALID_کوډ) {
                p = u_getPropertyEnum(pname.معلومات());
                إذا (p >= UCHAR_دوګونی_پیل && p < UCHAR_دوګونی_حد) {
                    v = 1;
                } وإلا إذا (0 == uprv_compare_Property_نومونه(ANY, pname.معلومات())) {
                    سېټ(MIN_ارزښت, MAX_ارزښت);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نومونه(ASCII, pname.معلومات())) {
                    سېټ(0, 0x7F);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نومونه(ASSIGNED, pname.معلومات())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_ماسک;
                    v = U_GC_CN_ماسک;
                    invert = ریښتیا;
                } وإلا {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_ارزښت(p, v, ec);
    إذا(invert) {
        complement().remove_All_مزي_ګانې();  // code point complement
    }

    إذا (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_حافظه_ویش_ERROR;
    }
    أرجع *هذا;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool Unicode_سېټ::resembles_Property_نمونه(ثابت UnicodeString& نمونه,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    إذا ((pos+5) > نمونه.length()) {
        أرجع خطأ;
    }

    // Look for an opening [:, [:^, \p, or \P
    أرجع isPOSIXOpen(نمونه, pos) || isPerlOpen(نمونه, pos) || isNameOpen(نمونه, pos);
}

U_NAMESPACE_پای_ټکی
