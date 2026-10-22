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

ساكن Unicode_سیٹ *uni32Singleton;
ساكن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساكن UBool U_CALLCONV uset_cleanup() {
    مٹاؤ uni32Singleton;
    uni32Singleton = خالی_سوچک;
    uni32InitOnce.reset();
    أرجع سچ;
}

U_CDECL_سرا

U_NAMESPACE_BEGIN

ورتوں U_سرخی_ONLY_NAMESPACE::utf_String_کوڈ_Points;

نطاق {

// Cache some sets for other services -------------------------------------- ***
فراغ U_CALLCONV create_Uni32_سیٹ(UError_کوڈ &error_کوڈ) {
    U_ASSERT(uni32Singleton == خالی_سوچک);
    uni32Singleton = جديد Unicode_سیٹ(UnicodeString(u"[:age=3.2:]"), error_کوڈ);
    إذا(uni32Singleton==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    } وإلا {
        uni32Singleton->freeze();
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_سیٹ *
uniset_getUnicode32Instance(UError_کوڈ &error_کوڈ) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_سیٹ, error_کوڈ);
    أرجع uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

مضمن UBool
isPerlOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    char16_t c;
    أرجع نمونہ.charAt(pos)==u'\\' && ((c=نمونہ.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

مضمن UBool
isNameOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    أرجع نمونہ.charAt(pos)==u'\\' && نمونہ.charAt(pos+1)==u'N';
}

مضمن UBool
isPOSIXOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    أرجع نمونہ.charAt(pos)==u'[' && نمونہ.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#تعریف_کرو _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ثابت CharString& name) {
    لكل (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_کوڈ ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.انکڑے(), &ec);
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
Unicode_سیٹ::Unicode_سیٹ(ثابت UnicodeString& نمونہ,
                       UError_کوڈ& حالت) {
    apply_نمونہ(نمونہ, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     UError_کوڈ& حالت) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_نمونہ_Ignore_Space(نمونہ, pos, خالی_سوچک, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_اشاریہ();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(نمونہ, i, سچ);
    إذا (i != نمونہ.length()) {
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

فراغ
Unicode_سیٹ::apply_نمونہ_Ignore_Space(ثابت UnicodeString& نمونہ,
                                    ParsePosition& pos,
                                    ثابت علامت_جدول* symbols,
                                    UError_کوڈ& حالت) {
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
    قاعدہ_Character_Iterator chars(نمونہ, symbols, pos);
    apply_نمونہ(نمونہ, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, خالی_سوچک, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (chars.وچ_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سیٹ;
        أرجع;
    }
    سیٹ_نمونہ(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_سیٹ::resembles_نمونہ(ثابت UnicodeString& نمونہ, int32_t pos) {
    أرجع ((pos+1) < نمونہ.length() &&
            نمونہ.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_نمونہ(نمونہ, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#تعریف_کرو U_DEBUGGING_UNICODESET_PARSING 0

صنف Unicode_سیٹ::Lexer {
  عام:
    Lexer(ثابت UnicodeString &نمونہ,
          ثابت ParsePosition &parsePosition,
          قاعدہ_Character_Iterator &chars,
          uint32_t unicode_سیٹ_Options,
          ثابت علامت_جدول *ثابت symbols,
          Unicode_سیٹ &(Unicode_سیٹ::*case_گھیرا)(int32_t attribute))
        : نمونہ_2(نمونہ), parsePosition_(parsePosition), chars_(chars),
          unicode_سیٹ_Options_2(unicode_سیٹ_Options),
          charsOptions_(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_سیٹ_Options & USET_IGNORE_SPACE) != 0
                             ? قاعدہ_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_گھیرا_2(case_گھیرا) {}

    صنف LexicalElement {
      عام:
        منطقي is_سیٹ_Operator(ثابت char16_t op) ثابت {
            أرجع U_SUCCESS(error_کوڈ_2) && category_ == سیٹ_OPERATOR && string_[0] == op;
        }

        منطقي isStringLiteral() ثابت {
            أرجع U_SUCCESS(error_کوڈ_2) && category_ == STRING_LITERAL;
        }

        منطقي isNamedElement() ثابت {
            أرجع U_SUCCESS(error_کوڈ_2) && category_ == NAMED_ELEMENT;
        }

        منطقي isBracketedElement() ثابت {
            أرجع U_SUCCESS(error_کوڈ_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            إذا (U_SUCCESS(error_کوڈ_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                أرجع string_;
            }
            أرجع std::nullopt;
        }

        std::optional<UChar32> کوڈ_نقطہ() ثابت {
            إذا (U_SUCCESS(error_کوڈ_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                أرجع string_.char32At(0);
            }
            أرجع std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت Unicode_سیٹ *سیٹ() ثابت {
            إذا (U_FAILURE(error_کوڈ_2)) {
                أرجع خالی_سوچک;
            }
            إذا (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                إذا (precomputed_سیٹ != خالی_سوچک) {
                    أرجع precomputed_سیٹ;
                } وإلا {
                    أرجع &سیٹ_2;
                }
            }
            أرجع خالی_سوچک;
        }

        ثابت UError_کوڈ& error_کوڈ() ثابت{
          أرجع error_کوڈ_2;
        }

#إذا U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString نتیجہ;
            إذا (U_FAILURE(error_کوڈ_2)) {
                نتیجہ.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_کوڈ_2)))
                    .append(u"), possibly ");
            }
            أرجع نتیجہ.append(category_ناں[category_])
                .append(u" '")
                .append(سروت_متن)
                .append(u"'");
        }
#جے_انت

      خاص:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        تعداد Category : std::uint8_t {
            سیٹ_OPERATOR,
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
            سرا_دا_متن,
        };
        ساكن constexpr std::array<std::u16string_view, سرا_دا_متن + 1> category_ناں{{
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
        LexicalElement(Category category, UnicodeString string, قاعدہ_Character_Iterator::Pos after,
                       ثابت UError_کوڈ error_کوڈ, ثابت Unicode_سیٹ *precomputed_سیٹ_2, Unicode_سیٹ سیٹ,
                       std::u16string_view سروت_متن_2, منطقي separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_کوڈ_2(error_کوڈ),
              precomputed_سیٹ(precomputed_سیٹ_2), سیٹ_2(سیٹ), سروت_متن(سروت_متن_2) {
            إذا (U_FAILURE(error_کوڈ_2) || separated || !preceding.has_value()) {
                أرجع;
            }
            إذا (preceding->is_سیٹ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *کوڈ_نقطہ() == u':') {
                error_کوڈ_2 = U_MALFORMED_سیٹ;
            } وإلا إذا (preceding->category_ == ESCAPED_ELEMENT) {
                إذا (U_IS_LEAD(*preceding->کوڈ_نقطہ()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*کوڈ_نقطہ())) {
                    error_کوڈ_2 = U_MALFORMED_سیٹ;
                } وإلا إذا (preceding->سروت_متن.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *کوڈ_نقطہ();
                    إذا (preceding->سروت_متن[1] == u'x') {
                        إذا ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_کوڈ_2 = U_MALFORMED_سیٹ;
                        }
                    } وإلا إذا (preceding->سروت_متن[1] >= u'0' &&
                               preceding->سروت_متن[1] <= u'7') {
                        إذا (cp >= u'0' && cp <= u'7') {
                            error_کوڈ_2 = U_MALFORMED_سیٹ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        قاعدہ_Character_Iterator::Pos after_;
        UError_کوڈ error_کوڈ_2;
        ثابت Unicode_سیٹ *precomputed_سیٹ;
        Unicode_سیٹ سیٹ_2;
        std::u16string_view سروت_متن;

        یار صنف Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        أرجع نمونہ_2.عارضی_Sub_String(0, parsePosition_.get_اشاریہ()) + u"☞" +
               نمونہ_2.عارضی_Sub_String(parsePosition_.get_اشاریہ(), 60);
    }

    منطقي accept_سیٹ_Operator(char16_t op) {
        إذا (lookahead().is_سیٹ_Operator(op)) {
            advance();
            أرجع سچ;
        }
        أرجع خطأ;
    }

    ثابت LexicalElement &lookahead() {
        إذا (!ahead_.has_value()) {
            ثابت قاعدہ_Character_Iterator::Pos before = getPos();
            ahead_.emplace(اگلا_Token(behind_));
            chars_.سیٹ_Pos(before);
        }
        أرجع *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        إذا (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت قاعدہ_Character_Iterator::Pos before = getPos();
            chars_.سیٹ_Pos(lookahead().after_);
            ahead2_.emplace(اگلا_Token(ahead_));
            chars_.سیٹ_Pos(before);
        }
        أرجع *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    قاعدہ_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        أرجع chars_;
    }

    int32_t charsOptions() {
        أرجع charsOptions_;
    }

    منطقي at_سرا() ثابت {
        أرجع chars_.at_سرا();
    }

    فراغ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.سیٹ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خاص:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    قاعدہ_Character_Iterator::Pos getPos() ثابت {
        قاعدہ_Character_Iterator::Pos نتیجہ;
        chars_.getPos(نتیجہ);
        أرجع نتیجہ;
    }

    LexicalElement اگلا_Token(std::optional<LexicalElement> ثابت& preceding) {
        UError_کوڈ error_کوڈ = U_ZERO_ERROR;
        منطقي separated = خطأ;
        إذا (charsOptions_ & قاعدہ_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            قاعدہ_Character_Iterator::Pos pos;
            لكل (;;) {
                UBool بن_ورتیا_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                  قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                بن_ورتیا_Escaped, error_کوڈ);
                إذا (!نمونہ_Props::isWhiteSpace(s)) {
                    chars_.سیٹ_Pos(pos);
                    اكسر;
                }
                إذا (s != u'\u200E' && s != u'\u200F') {
                    separated = سچ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        إذا (chars_.at_سرا()) {
            أرجع LexicalElement(LexicalElement::سرا_دا_متن, {}, getPos(), error_کوڈ,
                                  /*precomputedSet=*/خالی_سوچک,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t شروع = parsePosition_.get_اشاریہ();
        ثابت قاعدہ_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool بن_ورتیا_Escaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~قاعدہ_Character_Iterator::PARSE_ESCAPES, بن_ورتیا_Escaped, error_کوڈ);
        إذا (first == u'[' || first == u'\\') {
            ثابت قاعدہ_Character_Iterator::Pos after_پہلا = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 دوجا = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                                 قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                               بن_ورتیا_Escaped, error_کوڈ);
            إذا ((first == u'[' && دوجا == u':') ||
                (first == u'\\' && (دوجا == u'p' || دوجا == u'P' || دوجا == u'N'))) {
                إذا (دوجا == u'N') {
                    UChar32 ثابت query_نتیجہ = جانچ_Named_Element_Brackets(error_کوڈ);
                    أرجع LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_نتیجہ), getPos(), error_کوڈ,
                        /*precomputedSet=*/خالی_سوچک,
                        /*set=*/{},
                        std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                        separated, preceding);
                } وإلا {
                    Unicode_سیٹ query_نتیجہ = جانچ_Property_Query_After_شروع(first, دوجا, شروع, error_کوڈ);
                    أرجع LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_کوڈ,
                        /*precomputedSet=*/خالی_سوچک, /*set=*/std::move(query_نتیجہ),
                        std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.سیٹ_Pos(after_پہلا);
        }
        إذا (first == u'$' && symbols_ != خالی_سوچک) {
            خودکار name_سرا = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            إذا (UnicodeString name = symbols_->parseReference(نمونہ_2, name_سرا, نمونہ_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_خالی_تھاواں()) {
                chars_.jumpahead(name_سرا.get_اشاریہ() - (شروع + 1));
                ثابت std::u16string_view سروت =
                    std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع);
                ثابت Unicode_سیٹ *precomputed_سیٹ_2 = symbols_->lookup_سیٹ(name);
                إذا (precomputed_سیٹ_2 != خالی_سوچک) {
                    أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_سیٹ_2, /*set=*/{}, سروت, /*separated=*/سچ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                إذا (expression == خالی_سوچک) {
                    أرجع LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/خالی_سوچک,
                        /*set=*/{},
                        سروت,
                        /*separated=*/سچ,
                        /*preceding=*/std::nullopt);
                }
                أرجع evaluateVariable(*expression, سروت);
            }
        }
        بدل (first) {
        حالة u'[':
            أرجع LexicalElement(
                LexicalElement::سیٹ_OPERATOR, UnicodeString(u'['), getPos(), error_کوڈ,
                /*precomputedSet=*/خالی_سوچک,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                separated, preceding);
        حالة u'\\': {
            // Now try to parse the escape.
            chars_.سیٹ_Pos(before);
            UChar32 کوڈ_نقطہ = chars_.next(charsOptions_, بن_ورتیا_Escaped, error_کوڈ);
            أرجع LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(کوڈ_نقطہ), getPos(), error_کوڈ,
                خالی_سوچک,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                separated, preceding);
        }
        حالة u'&':
        حالة u'-':
        حالة u']':
        حالة u'^':
        حالة u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            أرجع LexicalElement(
                LexicalElement::سیٹ_OPERATOR, UnicodeString(first), getPos(), error_کوڈ,
                /*precomputedSet=*/خالی_سوچک,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                separated, preceding);
        حالة u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t کوڈ_نقطہ_گنتی = 0;
            طالما (!chars_.at_سرا() && U_SUCCESS(error_کوڈ)) {
                ثابت قاعدہ_Character_Iterator::Pos before_اگلا = getPos();
                next = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                     قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                   بن_ورتیا_Escaped, error_کوڈ);
                إذا (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                      قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                    بن_ورتیا_Escaped, error_کوڈ);
                    إذا (afterBackslash == u'N') {
                        next = جانچ_Named_Element_Brackets(error_کوڈ);
                        escaped = سچ;
                    } وإلا إذا (afterBackslash == u'p' || afterBackslash == u'P') {
                        أرجع LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_سیٹ,
                                              /*precomputedSet=*/خالی_سوچک,
                                              /*set=*/{},
                                              std::u16string_view(نمونہ_2).substr(
                                                  شروع, parsePosition_.get_اشاریہ() - شروع),
                                              separated, preceding);
                    } وإلا {
                        chars_.سیٹ_Pos(before_اگلا);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_کوڈ);
                    }
                } وإلا {
#إذا U_ICU_نسخہ_MAJOR_عدد < 81
                    إذا (U_SUCCESS(error_کوڈ) && نمونہ_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_کوڈ = UError_کوڈ::U_ILLEGAL_دلیل_ERROR;
                    }
#وإلا
#غلطی Remove هذا transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#جے_انت
                    escaped = خطأ;
                }
                إذا (!escaped && next == u'}') {
                    أرجع LexicalElement(
                        کوڈ_نقطہ_گنتی == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_کوڈ,
                        /*precomputedSet=*/خالی_سوچک,
                        /*set=*/{},
                        std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                        separated, preceding);
                }
                string.append(next);
                کوڈ_نقطہ_گنتی += 1;
            }
            أرجع LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_سیٹ,
                /*precomputedSet=*/خالی_سوچک,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                separated, preceding);
        }
        افتراضي:
            أرجع LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_کوڈ, خالی_سوچک,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(شروع, parsePosition_.get_اشاریہ() - شروع),
                separated, preceding);
        }
    }

    UChar32 جانچ_Named_Element_Brackets(UError_کوڈ &error_کوڈ) {
        UBool بن_ورتیا_Escaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                           قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                         بن_ورتیا_Escaped, error_کوڈ);
        إذا (open == u'{') {
            int32_t شروع = parsePosition_.get_اشاریہ();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            طالما (!chars_.at_سرا() && U_SUCCESS(error_کوڈ)) {
                UChar32 آخری = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                             قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                           بن_ورتیا_Escaped, error_کوڈ);
                إذا (آخری == u':') {
                    إذا (!hex.has_value()) {
                        hex.emplace();
                        إذا (شروع == parsePosition_.get_اشاریہ() - 1) {
                            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                            أرجع {};
                        }
                        لكل (char16_t digit : std::u16string_view(نمونہ_2).substr(
                                 شروع, parsePosition_.get_اشاریہ() - 1 - شروع)) {
                            uint8_t nibble;
                            إذا (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } وإلا {
                                digit = digit & ~0x20;
                                إذا (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } وإلا {
                                    error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                                    أرجع {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            إذا (hex > 0x10FFFF) {
                                error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                                أرجع {};
                            }
                        }
                    } وإلا إذا (!literal.has_value()) {
                        ثابت خودکار literal_کوڈ_Points = utf_String_کوڈ_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(نمونہ_2).substr(شروع,
                                                                 parsePosition_.get_اشاریہ() - 1 - شروع));
                        خودکار it = literal_کوڈ_Points.begin();
                        إذا (it == literal_کوڈ_Points.سرا() || !it->wellFormed() ||
                                (literal = it->کوڈ_نقطہ(), ++it) != literal_کوڈ_Points.سرا()) {
                            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                            أرجع {};
                        }
                    } وإلا {
                        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    شروع = parsePosition_.get_اشاریہ();
                } وإلا إذا (آخری == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(نمونہ_2).substr(
                        شروع, parsePosition_.get_اشاریہ() - 1 - شروع);
                    ثابت UChar32 نتیجہ = getCharacterByName(CharString().appendInvariantChars(
                        u16name.انکڑے(), static_cast<int32_t>(u16name.length()), error_کوڈ));
                    إذا (!U_SUCCESS(error_کوڈ)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    إذا (نتیجہ < 0 || (hex.has_value() && نتیجہ != hex) ||
                            (literal.has_value() && نتیجہ != literal)) {
                        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                    أرجع نتیجہ;
                }
            }
        }
        إذا (U_SUCCESS(error_کوڈ)) {
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view سروت) {
        UError_کوڈ error_کوڈ = U_ZERO_ERROR;
        ParsePosition عبارت_Position;
        قاعدہ_Character_Iterator عبارت_Iterator(expression, symbols_, عبارت_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer عبارت_Lexer(expression, عبارت_Position, عبارت_Iterator, unicode_سیٹ_Options_2,
                              /*symbols=*/خالی_سوچک, case_گھیرا_2);
        خودکار variableToken = عبارت_Lexer.lookahead();
        إذا (variableToken.is_سیٹ_Operator(u'[')) {
            UnicodeString rebuilt_نمونہ;
            Unicode_سیٹ عبارت_قدر =
                parse_Unicode_سیٹ(عبارت_Lexer, rebuilt_نمونہ, unicode_سیٹ_Options_2, case_گھیرا_2,
                                /*depth=*/0, error_کوڈ);
            عبارت_قدر.سیٹ_نمونہ(rebuilt_نمونہ);
            إذا (!عبارت_Lexer.at_سرا()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالی_سوچک,
                    /*set=*/{},
                    سروت,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            }
            أرجع LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_کوڈ,
                /*precomputedSet=*/خالی_سوچک,
                /*set=*/std::move(عبارت_قدر),
                سروت,
                /*separated=*/سچ,
                /*preceding=*/std::nullopt);
        } وإلا {
            عبارت_Lexer.advance();
            إذا (!عبارت_Lexer.at_سرا()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالی_سوچک,
                    /*set=*/{},
                    سروت,
                    /*separated=*/سچ,
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
                    variableToken.error_کوڈ_2, variableToken.precomputed_سیٹ, std::move(variableToken.سیٹ_2), سروت,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            افتراضي:
                أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/خالی_سوچک,
                                      /*set=*/{}, سروت,
                                      /*separated=*/سچ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_سیٹ جانچ_Property_Query_After_شروع(UChar32 first, UChar32 دوجا, int32_t query_شروع, UError_کوڈ &error_کوڈ) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_عبارت_شروع = parsePosition_.get_اشاریہ();
        منطقي exteriorlyNegated = خطأ;
        منطقي interiorlyNegated = خطأ;
        UBool بن_ورتیا_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 تیجا = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                            قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                          بن_ورتیا_Escaped, error_کوڈ);
        إذا (first == u'\\') {
            إذا (تیجا != u'{') {
                error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                أرجع {};
            }
            exteriorlyNegated = دوجا == u'P';
            query_عبارت_شروع = parsePosition_.get_اشاریہ();
        } وإلا {
            إذا (تیجا == u'^') {
                exteriorlyNegated = سچ;
                query_عبارت_شروع = parsePosition_.get_اشاریہ();
            }
        }
        قاعدہ_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                          قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                        بن_ورتیا_Escaped, error_کوڈ);

        طالما (!chars_.at_سرا() && U_SUCCESS(error_کوڈ)) {
            ثابت قاعدہ_Character_Iterator::Pos before_آخری = getPos();
            UChar32 آخری_Unescaped =
                chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                              قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                            بن_ورتیا_Escaped, error_کوڈ);
            إذا (penultimateUnescaped == u'\\') {
                إذا (آخری_Unescaped == 'N') {
                    جانچ_Named_Element_Brackets(error_کوڈ);
                    إذا (!U_SUCCESS(error_کوڈ)) {
                        أرجع {};
                    }
                } وإلا {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.سیٹ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~قاعدہ_Character_Iterator::SKIP_WHITESPACE, بن_ورتیا_Escaped,
                                error_کوڈ);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                آخری_Unescaped = -1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخری_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_اشاریہ() - 1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخری_Unescaped == u'≠') {
                إذا (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                    أرجع {};
                }
                interiorlyNegated = سچ;
                queryOperatorPosition = parsePosition_.get_اشاریہ() - 1;
            } وإلا إذا ((first == u'[' && penultimateUnescaped == u':' && آخری_Unescaped == u']') ||
                       (first == u'\\' && آخری_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ثابت int32_t query_عبارت_حد_بندی =
                    first == u'[' ? parsePosition_.get_اشاریہ() - 2 : parsePosition_.get_اشاریہ() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                إذا (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        نمونہ_2.عارضی_Sub_String_Between(*queryOperatorPosition + 1, query_عبارت_حد_بندی);
                    إذا (propertyPredicate.is_خالی_تھاواں()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                        أرجع {};
                    }
                }
                Unicode_سیٹ نتیجہ;
                نتیجہ.applyPropertyAlias(
                    نمونہ_2.عارضی_Sub_String_Between(query_عبارت_شروع,
                                                  queryOperatorPosition.value_or(query_عبارت_حد_بندی)),
                    propertyPredicate, error_کوڈ);
                إذا (exteriorlyNegated != interiorlyNegated) {
                    نتیجہ.complement().remove_All_لڑیاں();
                }
                نتیجہ.سیٹ_نمونہ(نمونہ_2.عارضی_Sub_String_Between(query_شروع, parsePosition_.get_اشاریہ()));
                أرجع نتیجہ;
            }
            beforePenultimate = before_آخری;
            penultimateUnescaped = آخری_Unescaped;
        }
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع {};
    }

    ثابت UnicodeString &نمونہ_2;
    ثابت ParsePosition &parsePosition_;
    قاعدہ_Character_Iterator &chars_;
    ثابت uint32_t unicode_سیٹ_Options_2;
    ثابت int32_t charsOptions_;
    ثابت علامت_جدول *ثابت symbols_;
    Unicode_سیٹ &(Unicode_سیٹ::* ثابت case_گھیرا_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نطاق {

constexpr int32_t MAX_ڈونگھائی = 100;

#إذا U_DEBUGGING_UNICODESET_PARSING

#تعریف_کرو U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec)                                                   \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        إذا (U_FAILURE(ec)) {                                                                       \
            إذا (ڈونگھائی < 5) {                                                                       \
                لکھو("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } وإلا إذا (ڈونگھائی == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                لکھو("--- [...]\n");                                                             \
            }                                                                                      \
            أرجع نتیجہ;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کرو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        لکھو("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).منزل_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().منزل_UTF8_String(contextUTF8).c_str());                 \
        لکھو("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        إذا (U_FAILURE(lexer.lookahead().error_کوڈ())) {                                            \
            (ec) = lexer.lookahead().error_کوڈ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سیٹ;                                                                \
        }                                                                                          \
        أرجع نتیجہ;                                                                             \
    } طالما (خطأ)

#وإلا

#تعریف_کرو U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec)                                                   \
    افعل {                                                                                           \
        إذا (U_FAILURE(ec)) {                                                                       \
            أرجع نتیجہ;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کرو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        إذا (U_FAILURE(lexer.lookahead().error_کوڈ())) {                                            \
            (ec) = lexer.lookahead().error_کوڈ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سیٹ;                                                                \
        }                                                                                          \
        أرجع نتیجہ;                                                                             \
    } طالما (خطأ)

#جے_انت

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

فراغ Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString &نمونہ,
                              ثابت ParsePosition &parsePosition,
                              قاعدہ_Character_Iterator &chars,
                              ثابت علامت_جدول *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_سیٹ &(Unicode_سیٹ::*case_گھیرا)(int32_t attribute),
                              UError_کوڈ &ec) {
    إذا (U_FAILURE(ec)) أرجع;
    Lexer lexer(نمونہ, parsePosition, chars, options, symbols, case_گھیرا);
    *هذا = parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_گھیرا, /*depth=*/0, ec);
}

Unicode_سیٹ Unicode_سیٹ::parse_Unicode_سیٹ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_سیٹ& (Unicode_سیٹ::*case_گھیرا)(int32_t attribute),
                                       int32_t ڈونگھائی,
                                       UError_کوڈ &ec) {
    Unicode_سیٹ نتیجہ;

    إذا (ڈونگھائی > MAX_ڈونگھائی) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_ڈونگھائی)).c_str(),
                                             ("depth = " + std::to_string(ڈونگھائی)).c_str(), lexer,
                                             ec);
    }

    منطقي isComplement = خطأ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقي preserve_Syntax_وچ_نمونہ = خطأ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_نمونہ;
    إذا (lexer.lookahead().سیٹ() != خالی_سوچک) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        نتیجہ = *lexer.lookahead().سیٹ();
        نتیجہ.منزل_نمونہ_2(pretty_Printed_نمونہ, /*escapeUnprintable=*/خطأ);
        lexer.advance();
        preserve_Syntax_وچ_نمونہ = سچ;
    } وإلا {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        إذا (lexer.accept_سیٹ_Operator(u'[')) {
            pretty_Printed_نمونہ.append(u'[');
            إذا (lexer.accept_سیٹ_Operator(u'^')) {
                pretty_Printed_نمونہ.append(u'^');
                isComplement = سچ;
            }
            نتیجہ = parseContent(lexer, pretty_Printed_نمونہ, options, case_گھیرا, ڈونگھائی,
                                  /*containsSetOperation=*/preserve_Syntax_وچ_نمونہ, ec);
            U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec);
            إذا (!lexer.accept_سیٹ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_نمونہ.append(u']');
        } وإلا {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    إذا ((options & USET_CASE_نقاب) != 0) {
        (نتیجہ.*case_گھیرا)(options);
    }
    إذا (isComplement) {
        نتیجہ.complement().remove_All_لڑیاں(); // code point complement
    }
    إذا (preserve_Syntax_وچ_نمونہ) {
        rebuiltPat.append(pretty_Printed_نمونہ);
    } وإلا {
        نتیجہ.generate_نمونہ(rebuiltPat, /*escapeUnprintable=*/خطأ);
    }
    أرجع نتیجہ;
}

Unicode_سیٹ Unicode_سیٹ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سیٹ &(Unicode_سیٹ::*case_گھیرا)(int32_t attribute),
                                    int32_t ڈونگھائی,
                                    منطقي &شامل_اے_سیٹ_Operation,
                                    UError_کوڈ &ec) {
    Unicode_سیٹ نتیجہ;
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
    إذا (lexer.accept_سیٹ_Operator(u'-')) {
        نتیجہ.جوڑو(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    طالما (!lexer.at_سرا()) {
        إذا (lexer.accept_سیٹ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            نتیجہ.جوڑو(u'-');
            أرجع نتیجہ;
        } وإلا إذا (lexer.lookahead().is_سیٹ_Operator(u'$')) {
            إذا (lexer.lookahead2().is_سیٹ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                نتیجہ.جوڑو(U_ETHER);
                شامل_اے_سیٹ_Operation = سچ;
                أرجع نتیجہ;
            }
        }
        إذا (lexer.lookahead().is_سیٹ_Operator(u']')) {
            أرجع نتیجہ;
        }
        // Also handles FinalMutation.
        نتیجہ.parseMutation(lexer, rebuiltPat, options, case_گھیرا, ڈونگھائی, شامل_اے_سیٹ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec);
    }
    أرجع نتیجہ;
}

فراغ Unicode_سیٹ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_سیٹ &(Unicode_سیٹ::*case_گھیرا)(int32_t attribute),
                               int32_t ڈونگھائی,
                               منطقي &شامل_اے_سیٹ_Operation,
                               UError_کوڈ &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    إذا (lexer.lookahead().is_سیٹ_Operator('[') || lexer.lookahead().سیٹ() != خالی_سوچک) {
        شامل_اے_سیٹ_Operation = سچ;
        parse_سیٹ_Operations(lexer, rebuiltPat, options, case_گھیرا, ڈونگھائی, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } وإلا {
        // Also handles DollarElements.
        جوڑو_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

فراغ Unicode_سیٹ::parse_سیٹ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سیٹ &(Unicode_سیٹ::*case_گھیرا)(int32_t attribute),
                                    int32_t ڈونگھائی,
                                    UError_کوڈ &ec) {
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
    ثابت Unicode_سیٹ بایاں_Hand_Side =
        parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_گھیرا, ڈونگھائی + 1, ec);
    جوڑو_All(بایاں_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    لكل (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        إذا (lexer.accept_سیٹ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت Unicode_سیٹ دایاں_Hand_Side =
                parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_گھیرا, ڈونگھائی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(دایاں_Hand_Side);
        } وإلا إذا (lexer.lookahead().is_سیٹ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            إذا (lexer.lookahead2().is_سیٹ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                أرجع;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت Unicode_سیٹ دایاں_Hand_Side =
                parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_گھیرا, ڈونگھائی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(دایاں_Hand_Side);
        } وإلا {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            أرجع;
        }
    }
}

Unicode_سیٹ Unicode_سیٹ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_کوڈ &ec) {
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
        أرجع Unicode_سیٹ().جوڑو(element);
    }
    UChar32 first;
    إذا (lexer.lookahead().is_سیٹ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } وإلا إذا (lexer.lookahead().کوڈ_نقطہ().has_value()) {
        first = *lexer.lookahead().کوڈ_نقطہ();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, first, /*escapeUnprintable=*/خطأ);
    إذا (!lexer.lookahead().is_سیٹ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        أرجع Unicode_سیٹ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    إذا (lexer.lookahead2().is_سیٹ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        أرجع Unicode_سیٹ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 آخری;
    إذا (lexer.lookahead().is_سیٹ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        آخری = u'$';
        إذا (lexer.lookahead2().is_سیٹ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } وإلا إذا (lexer.lookahead().کوڈ_نقطہ().has_value()) {
        آخری = *lexer.lookahead().کوڈ_نقطہ();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    إذا (آخری <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(آخری) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, آخری, /*escapeUnprintable=*/خطأ);
    أرجع Unicode_سیٹ(first, آخری);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

نطاق {

UBool numeric_قدر_Filter(UChar32 ch, فراغ* context) {
    أرجع u_get_Numeric_قدر(ch) == *static_cast<مزدوج*>(context);
}

UBool general_Category_نقاب_Filter(UChar32 ch, فراغ* context) {
    int32_t value = *static_cast<int32_t*>(context);
    أرجع (U_GET_GC_نقاب((UChar32) ch) & value) != 0;
}

UBool نسخہ_Filter(UChar32 ch, فراغ* context) {
    ساكن ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* نسخہ = static_cast<UVersionInfo*>(context);
    أرجع uprv_memcmp(&v, &none, حجم(v)) > 0 && uprv_memcmp(&v, نسخہ, حجم(v)) <= 0;
}

تعريفنوع بنية {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, فراغ* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    أرجع u_get_Int_Property_قدر(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, فراغ* context) {
    أرجع uscript_hasScript(ch, *static_cast<UScript_کوڈ*>(context));
}

UBool idTypeFilter(UChar32 ch, فراغ* context) {
    أرجع u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
فراغ Unicode_سیٹ::applyFilter(Unicode_سیٹ::Filter filter,
                             فراغ* context,
                             ثابت Unicode_سیٹ* inclusions,
                             UError_کوڈ &حالت) {
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

    UChar32 شروع_Has_Property = -1;
    int32_t حد_بندی_Range = inclusions->get_Range_گنتی();

    لكل (صحيح j=0; j<حد_بندی_Range; ++j) {
        // get current range
        UChar32 شروع = inclusions->get_Range_شروع(j);
        UChar32 سرا = inclusions->get_Range_سرا(j);

        // for all the code points in the range, process
        لكل (UChar32 ch = شروع; ch <= سرا; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            إذا ((*filter)(ch, context)) {
                إذا (شروع_Has_Property < 0) {
                    شروع_Has_Property = ch;
                }
            } وإلا إذا (شروع_Has_Property >= 0) {
                جوڑو(شروع_Has_Property, ch-1);
                شروع_Has_Property = -1;
            }
        }
    }
    إذا (شروع_Has_Property >= 0) {
        جوڑو(شروع_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    إذا (isBogus() && U_SUCCESS(حالت)) {
        // We likely ran out of memory. AHHH!
        حالت = U_حافظہ_ونڈ_ERROR;
    }
}

نطاق {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#تعریف_کرو FAIL(ec) UPRV_بلاک_MACRO_BEGIN { \
    ec=U_ILLEGAL_دلیل_ERROR; \
    أرجع *هذا; \
} UPRV_بلاک_MACRO_سرا

Unicode_سیٹ&
Unicode_سیٹ::apply_Int_Property_قدر(UProperty prop, int32_t value, UError_کوڈ& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) { أرجع *هذا; }
    إذا (prop == UCHAR_GENERAL_CATEGORY_نقاب) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_نقاب_Filter, &value, inclusions, ec);
    } وإلا إذا (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_کوڈ script = static_cast<UScript_کوڈ>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } وإلا إذا (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } وإلا إذا (0 <= prop && prop < UCHAR_ثنائی_حد_بندی) {
        إذا (value == 0 || value == 1) {
            ثابت USet *سیٹ = u_get_ثنائی_Property_سیٹ(prop, &ec);
            إذا (U_FAILURE(ec)) { أرجع *هذا; }
            copyFrom(*Unicode_سیٹ::fromUSet(سیٹ), سچ);
            إذا (value == 0) {
                complement().remove_All_لڑیاں();  // code point complement
            }
        } وإلا {
            clear();
        }
    } وإلا إذا (UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد_بندی) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } وإلا {
        ec = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سیٹ&
Unicode_سیٹ::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UError_کوڈ& ec) U_LIFETIME_BOUND {
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
        p = u_getPropertyEnum(pname.انکڑے());
        إذا (p == UCHAR_INVALID_کوڈ) FAIL(ec);

        // Treat gc as gcm
        إذا (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_نقاب;
        }

        إذا ((p >= UCHAR_ثنائی_شروع && p < UCHAR_ثنائی_حد_بندی) ||
            (p >= UCHAR_INT_شروع && p < UCHAR_INT_حد_بندی) ||
            (p >= UCHAR_نقاب_شروع && p < UCHAR_نقاب_حد_بندی)) {
            v = u_get_Property_قدر_Enum(p, vname.انکڑے());
            إذا (v == UCHAR_INVALID_کوڈ) {
                // Handle numeric CCC
                إذا (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* سرا;
                    مزدوج val = uprv_strtod(vname.انکڑے(), &سرا);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    إذا (*سرا != 0 || !(0 <= val && val <= 255) ||
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
            حالة UCHAR_NUMERIC_قدر:
                {
                    حرف* سرا;
                    مزدوج val = uprv_strtod(vname.انکڑے(), &سرا);
                    إذا (*سرا != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_قدر_Filter, &val,
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
                    جوڑو(ch);
                    أرجع *هذا;
                }
            حالة UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالة UCHAR_AGE:
                {
                    UVersionInfo نسخہ;
                    u_نسخہ_From_String(نسخہ, vname.انکڑے());
                    applyFilter(نسخہ_Filter, &نسخہ,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    أرجع *هذا;
                }
            حالة UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_قدر_Enum(UCHAR_SCRIPT, vname.انکڑے());
                إذا (v == UCHAR_INVALID_کوڈ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            حالة UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_قدر_Enum(p, vname.انکڑے());
                إذا (v == UCHAR_INVALID_کوڈ) {
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
        p = UCHAR_GENERAL_CATEGORY_نقاب;
        v = u_get_Property_قدر_Enum(p, pname.انکڑے());
        إذا (v == UCHAR_INVALID_کوڈ) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_قدر_Enum(p, pname.انکڑے());
            إذا (v == UCHAR_INVALID_کوڈ) {
                p = u_getPropertyEnum(pname.انکڑے());
                إذا (p >= UCHAR_ثنائی_شروع && p < UCHAR_ثنائی_حد_بندی) {
                    v = 1;
                } وإلا إذا (0 == uprv_compare_Property_ناں(ANY, pname.انکڑے())) {
                    سیٹ(MIN_قدر, MAX_قدر);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_ناں(ASCII, pname.انکڑے())) {
                    سیٹ(0, 0x7F);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_ناں(ASSIGNED, pname.انکڑے())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_نقاب;
                    v = U_GC_CN_نقاب;
                    invert = سچ;
                } وإلا {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_قدر(p, v, ec);
    إذا(invert) {
        complement().remove_All_لڑیاں();  // code point complement
    }

    إذا (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_حافظہ_ونڈ_ERROR;
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
UBool Unicode_سیٹ::resembles_Property_نمونہ(ثابت UnicodeString& نمونہ,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    إذا ((pos+5) > نمونہ.length()) {
        أرجع خطأ;
    }

    // Look for an opening [:, [:^, \p, or \P
    أرجع isPOSIXOpen(نمونہ, pos) || isPerlOpen(نمونہ, pos) || isNameOpen(نمونہ, pos);
}

U_NAMESPACE_سرا
