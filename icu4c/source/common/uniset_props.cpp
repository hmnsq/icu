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

#شامل <array>
#شامل <optional>

#شامل "unicode/utypes.h"
#شامل "unicode/uniset.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uscript.h"
#شامل "unicode/symtable.h"
#شامل "unicode/uset.h"
#شامل "unicode/locid.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/utfiterator.h"
#شامل "uset_imp.h"
#شامل "ruleiter.h"
#شامل "cmemory.h"
#شامل "ucln_cmn.h"
#شامل "util.h"
#شامل "uvector.h"
#شامل "uprops.h"
#شامل "patternprops.h"
#شامل "propname.h"
#شامل "normalizer2impl.h"
#شامل "uinvchar.h"
#شامل "uprops.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "umutex.h"
#شامل "uassert.h"
#شامل "hash.h"

U_NAMESPACE_USE

نامخلا {

// Special property set IDs
constexpr حرف ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr حرف ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr حرف ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
ساکن UBool U_CALLCONV uset_cleanup();

ساکن Unicode_سیٹ *uni32Singleton;
ساکن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساکن UBool U_CALLCONV uset_cleanup() {
    حذف uni32Singleton;
    uni32Singleton = خالی_اشاریہ;
    uni32InitOnce.reset();
    واپس سچ;
}

U_CDECL_انتہا

U_NAMESPACE_BEGIN

استعمال U_سرخی_ONLY_NAMESPACE::utf_String_رمز_Points;

نامخلا {

// Cache some sets for other services -------------------------------------- ***
خالی U_CALLCONV create_Uni32_سیٹ(UError_رمز &error_رمز) {
    U_ASSERT(uni32Singleton == خالی_اشاریہ);
    uni32Singleton = نیا Unicode_سیٹ(UnicodeString(u"[:age=3.2:]"), error_رمز);
    اگر(uni32Singleton==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        uni32Singleton->freeze();
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_سیٹ *
uniset_getUnicode32Instance(UError_رمز &error_رمز) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_سیٹ, error_رمز);
    واپس uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

اندرونی UBool
isPerlOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    char16_t c;
    واپس نمونہ.charAt(pos)==u'\\' && ((c=نمونہ.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

اندرونی UBool
isNameOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    واپس نمونہ.charAt(pos)==u'\\' && نمونہ.charAt(pos+1)==u'N';
}

اندرونی UBool
isPOSIXOpen(ثابت UnicodeString &نمونہ, int32_t pos) {
    واپس نمونہ.charAt(pos)==u'[' && نمونہ.charAt(pos+1)==u':';
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
    برائے (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_رمز ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.ڈیٹا(), &ec);
        اگر (U_SUCCESS(ec)) {
            واپس ch;
        }
    }
    واپس U_SENTINEL;
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
                       UError_رمز& حالت_2) {
    apply_نمونہ(نمونہ, حالت_2);
    _dbgct(یہ);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سیٹ& Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString& نمونہ,
                                     UError_رمز& حالت_2) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_نمونہ_Ignore_Space(نمونہ, pos, خالی_اشاریہ, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس *یہ;

    int32_t i = pos.get_اشاریہ();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(نمونہ, i, سچ);
    اگر (i != نمونہ.length()) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
    }
    واپس *یہ;
}

خالی
Unicode_سیٹ::apply_نمونہ_Ignore_Space(ثابت UnicodeString& نمونہ,
                                    ParsePosition& pos,
                                    ثابت علامت_جدول* symbols,
                                    UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (isFrozen()) {
        حالت_2 = U_NO_WRITE_PERMISSION;
        واپس;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعدہ_Character_Iterator chars(نمونہ, symbols, pos);
    apply_نمونہ(نمونہ, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, خالی_اشاریہ, حالت_2);
    اگر (U_FAILURE(حالت_2)) واپس;
    اگر (chars.اندر_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت_2 = U_MALFORMED_سیٹ;
        واپس;
    }
    سیٹ_نمونہ(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_سیٹ::resembles_نمونہ(ثابت UnicodeString& نمونہ, int32_t pos) {
    واپس ((pos+1) < نمونہ.length() &&
            نمونہ.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_نمونہ(نمونہ, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#تعریف_کرو U_DEBUGGING_UNICODESET_PARSING 0

جماعت Unicode_سیٹ::Lexer {
  عوامی:
    Lexer(ثابت UnicodeString &نمونہ,
          ثابت ParsePosition &parsePosition,
          قاعدہ_Character_Iterator &chars,
          uint32_t unicode_سیٹ_Options,
          ثابت علامت_جدول *ثابت symbols,
          Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute))
        : نمونہ_2(نمونہ), parsePosition_(parsePosition), chars_(chars),
          unicode_سیٹ_Options_2(unicode_سیٹ_Options),
          charsOptions_(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_سیٹ_Options & USET_IGNORE_SPACE) != 0
                             ? قاعدہ_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_احاطہ_2(case_احاطہ) {}

    جماعت LexicalElement {
      عوامی:
        منطقی is_سیٹ_Operator(ثابت char16_t op) ثابت {
            واپس U_SUCCESS(error_رمز_2) && category_ == سیٹ_OPERATOR && string_[0] == op;
        }

        منطقی isStringLiteral() ثابت {
            واپس U_SUCCESS(error_رمز_2) && category_ == STRING_LITERAL;
        }

        منطقی isNamedElement() ثابت {
            واپس U_SUCCESS(error_رمز_2) && category_ == NAMED_ELEMENT;
        }

        منطقی isBracketedElement() ثابت {
            واپس U_SUCCESS(error_رمز_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            اگر (U_SUCCESS(error_رمز_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                واپس string_;
            }
            واپس std::nullopt;
        }

        std::optional<UChar32> رمز_نقطہ() ثابت {
            اگر (U_SUCCESS(error_رمز_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                واپس string_.char32At(0);
            }
            واپس std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت Unicode_سیٹ *سیٹ() ثابت {
            اگر (U_FAILURE(error_رمز_2)) {
                واپس خالی_اشاریہ;
            }
            اگر (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                اگر (precomputed_سیٹ != خالی_اشاریہ) {
                    واپس precomputed_سیٹ;
                } ورنہ {
                    واپس &سیٹ_2;
                }
            }
            واپس خالی_اشاریہ;
        }

        ثابت UError_رمز& error_رمز() ثابت{
          واپس error_رمز_2;
        }

#اگر U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString نتیجہ;
            اگر (U_FAILURE(error_رمز_2)) {
                نتیجہ.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_رمز_2)))
                    .append(u"), possibly ");
            }
            واپس نتیجہ.append(category_نام[category_])
                .append(u" '")
                .append(ماخذ_متن)
                .append(u"'");
        }
#اگر_ختم

      نجی:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        شمار Category : std::uint8_t {
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
            انتہا_کا_متن,
        };
        ساکن constexpr std::array<std::u16string_view, انتہا_کا_متن + 1> category_نام{{
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
                       ثابت UError_رمز error_رمز, ثابت Unicode_سیٹ *precomputed_سیٹ_2, Unicode_سیٹ سیٹ,
                       std::u16string_view ماخذ_متن_2, منطقی separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_رمز_2(error_رمز),
              precomputed_سیٹ(precomputed_سیٹ_2), سیٹ_2(سیٹ), ماخذ_متن(ماخذ_متن_2) {
            اگر (U_FAILURE(error_رمز_2) || separated || !preceding.has_value()) {
                واپس;
            }
            اگر (preceding->is_سیٹ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *رمز_نقطہ() == u':') {
                error_رمز_2 = U_MALFORMED_سیٹ;
            } ورنہ اگر (preceding->category_ == ESCAPED_ELEMENT) {
                اگر (U_IS_LEAD(*preceding->رمز_نقطہ()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*رمز_نقطہ())) {
                    error_رمز_2 = U_MALFORMED_سیٹ;
                } ورنہ اگر (preceding->ماخذ_متن.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *رمز_نقطہ();
                    اگر (preceding->ماخذ_متن[1] == u'x') {
                        اگر ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_رمز_2 = U_MALFORMED_سیٹ;
                        }
                    } ورنہ اگر (preceding->ماخذ_متن[1] >= u'0' &&
                               preceding->ماخذ_متن[1] <= u'7') {
                        اگر (cp >= u'0' && cp <= u'7') {
                            error_رمز_2 = U_MALFORMED_سیٹ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        قاعدہ_Character_Iterator::Pos after_;
        UError_رمز error_رمز_2;
        ثابت Unicode_سیٹ *precomputed_سیٹ;
        Unicode_سیٹ سیٹ_2;
        std::u16string_view ماخذ_متن;

        دوست جماعت Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        واپس نمونہ_2.عارضی_Sub_String(0, parsePosition_.get_اشاریہ()) + u"☞" +
               نمونہ_2.عارضی_Sub_String(parsePosition_.get_اشاریہ(), 60);
    }

    منطقی accept_سیٹ_Operator(char16_t op) {
        اگر (lookahead().is_سیٹ_Operator(op)) {
            advance();
            واپس سچ;
        }
        واپس جھوٹ;
    }

    ثابت LexicalElement &lookahead() {
        اگر (!ahead_.has_value()) {
            ثابت قاعدہ_Character_Iterator::Pos before = getPos();
            ahead_.emplace(اگلا_Token(behind_));
            chars_.سیٹ_Pos(before);
        }
        واپس *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        اگر (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت قاعدہ_Character_Iterator::Pos before = getPos();
            chars_.سیٹ_Pos(lookahead().after_);
            ahead2_.emplace(اگلا_Token(ahead_));
            chars_.سیٹ_Pos(before);
        }
        واپس *ahead2_;
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
        واپس chars_;
    }

    int32_t charsOptions() {
        واپس charsOptions_;
    }

    منطقی at_انتہا() ثابت {
        واپس chars_.at_انتہا();
    }

    خالی advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.سیٹ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  نجی:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    قاعدہ_Character_Iterator::Pos getPos() ثابت {
        قاعدہ_Character_Iterator::Pos نتیجہ;
        chars_.getPos(نتیجہ);
        واپس نتیجہ;
    }

    LexicalElement اگلا_Token(std::optional<LexicalElement> ثابت& preceding) {
        UError_رمز error_رمز = U_ZERO_ERROR;
        منطقی separated = جھوٹ;
        اگر (charsOptions_ & قاعدہ_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            قاعدہ_Character_Iterator::Pos pos;
            برائے (;;) {
                UBool غیر_مستعمل_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                  قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                غیر_مستعمل_Escaped, error_رمز);
                اگر (!نمونہ_Props::isWhiteSpace(s)) {
                    chars_.سیٹ_Pos(pos);
                    توڑو;
                }
                اگر (s != u'\u200E' && s != u'\u200F') {
                    separated = سچ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        اگر (chars_.at_انتہا()) {
            واپس LexicalElement(LexicalElement::انتہا_کا_متن, {}, getPos(), error_رمز,
                                  /*precomputedSet=*/خالی_اشاریہ,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t آغاز = parsePosition_.get_اشاریہ();
        ثابت قاعدہ_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool غیر_مستعمل_Escaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~قاعدہ_Character_Iterator::PARSE_ESCAPES, غیر_مستعمل_Escaped, error_رمز);
        اگر (first == u'[' || first == u'\\') {
            ثابت قاعدہ_Character_Iterator::Pos after_پہلا = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 دوسرا = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                                 قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                               غیر_مستعمل_Escaped, error_رمز);
            اگر ((first == u'[' && دوسرا == u':') ||
                (first == u'\\' && (دوسرا == u'p' || دوسرا == u'P' || دوسرا == u'N'))) {
                اگر (دوسرا == u'N') {
                    UChar32 ثابت query_نتیجہ = جائزہ_Named_Element_Brackets(error_رمز);
                    واپس LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_نتیجہ), getPos(), error_رمز,
                        /*precomputedSet=*/خالی_اشاریہ,
                        /*set=*/{},
                        std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                        separated, preceding);
                } ورنہ {
                    Unicode_سیٹ query_نتیجہ = جائزہ_Property_Query_After_آغاز(first, دوسرا, آغاز, error_رمز);
                    واپس LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_رمز,
                        /*precomputedSet=*/خالی_اشاریہ, /*set=*/std::move(query_نتیجہ),
                        std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.سیٹ_Pos(after_پہلا);
        }
        اگر (first == u'$' && symbols_ != خالی_اشاریہ) {
            خودکار name_انتہا = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            اگر (UnicodeString name = symbols_->parseReference(نمونہ_2, name_انتہا, نمونہ_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_خالی_خانے()) {
                chars_.jumpahead(name_انتہا.get_اشاریہ() - (آغاز + 1));
                ثابت std::u16string_view ماخذ =
                    std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز);
                ثابت Unicode_سیٹ *precomputed_سیٹ_2 = symbols_->lookup_سیٹ(name);
                اگر (precomputed_سیٹ_2 != خالی_اشاریہ) {
                    واپس LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_سیٹ_2, /*set=*/{}, ماخذ, /*separated=*/سچ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                اگر (expression == خالی_اشاریہ) {
                    واپس LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/خالی_اشاریہ,
                        /*set=*/{},
                        ماخذ,
                        /*separated=*/سچ,
                        /*preceding=*/std::nullopt);
                }
                واپس evaluateVariable(*expression, ماخذ);
            }
        }
        انتخاب (first) {
        حالت u'[':
            واپس LexicalElement(
                LexicalElement::سیٹ_OPERATOR, UnicodeString(u'['), getPos(), error_رمز,
                /*precomputedSet=*/خالی_اشاریہ,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                separated, preceding);
        حالت u'\\': {
            // Now try to parse the escape.
            chars_.سیٹ_Pos(before);
            UChar32 رمز_نقطہ = chars_.next(charsOptions_, غیر_مستعمل_Escaped, error_رمز);
            واپس LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(رمز_نقطہ), getPos(), error_رمز,
                خالی_اشاریہ,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                separated, preceding);
        }
        حالت u'&':
        حالت u'-':
        حالت u']':
        حالت u'^':
        حالت u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            واپس LexicalElement(
                LexicalElement::سیٹ_OPERATOR, UnicodeString(first), getPos(), error_رمز,
                /*precomputedSet=*/خالی_اشاریہ,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                separated, preceding);
        حالت u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t رمز_نقطہ_گنتی = 0;
            جب (!chars_.at_انتہا() && U_SUCCESS(error_رمز)) {
                ثابت قاعدہ_Character_Iterator::Pos before_اگلا = getPos();
                next = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                     قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                   غیر_مستعمل_Escaped, error_رمز);
                اگر (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                      قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                    غیر_مستعمل_Escaped, error_رمز);
                    اگر (afterBackslash == u'N') {
                        next = جائزہ_Named_Element_Brackets(error_رمز);
                        escaped = سچ;
                    } ورنہ اگر (afterBackslash == u'p' || afterBackslash == u'P') {
                        واپس LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_سیٹ,
                                              /*precomputedSet=*/خالی_اشاریہ,
                                              /*set=*/{},
                                              std::u16string_view(نمونہ_2).substr(
                                                  آغاز, parsePosition_.get_اشاریہ() - آغاز),
                                              separated, preceding);
                    } ورنہ {
                        chars_.سیٹ_Pos(before_اگلا);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_رمز);
                    }
                } ورنہ {
#اگر U_ICU_نسخہ_MAJOR_عدد < 81
                    اگر (U_SUCCESS(error_رمز) && نمونہ_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_رمز = UError_رمز::U_ILLEGAL_دلیل_ERROR;
                    }
#ورنہ
#غلطی Remove یہ transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#اگر_ختم
                    escaped = جھوٹ;
                }
                اگر (!escaped && next == u'}') {
                    واپس LexicalElement(
                        رمز_نقطہ_گنتی == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_رمز,
                        /*precomputedSet=*/خالی_اشاریہ,
                        /*set=*/{},
                        std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                        separated, preceding);
                }
                string.append(next);
                رمز_نقطہ_گنتی += 1;
            }
            واپس LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_سیٹ,
                /*precomputedSet=*/خالی_اشاریہ,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                separated, preceding);
        }
        طےشدہ:
            واپس LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_رمز, خالی_اشاریہ,
                /*set=*/{},
                std::u16string_view(نمونہ_2).substr(آغاز, parsePosition_.get_اشاریہ() - آغاز),
                separated, preceding);
        }
    }

    UChar32 جائزہ_Named_Element_Brackets(UError_رمز &error_رمز) {
        UBool غیر_مستعمل_Escaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                           قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                         غیر_مستعمل_Escaped, error_رمز);
        اگر (open == u'{') {
            int32_t آغاز = parsePosition_.get_اشاریہ();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            جب (!chars_.at_انتہا() && U_SUCCESS(error_رمز)) {
                UChar32 آخری = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                             قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                           غیر_مستعمل_Escaped, error_رمز);
                اگر (آخری == u':') {
                    اگر (!hex.has_value()) {
                        hex.emplace();
                        اگر (آغاز == parsePosition_.get_اشاریہ() - 1) {
                            error_رمز = U_ILLEGAL_دلیل_ERROR;
                            واپس {};
                        }
                        برائے (char16_t digit : std::u16string_view(نمونہ_2).substr(
                                 آغاز, parsePosition_.get_اشاریہ() - 1 - آغاز)) {
                            uint8_t nibble;
                            اگر (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } ورنہ {
                                digit = digit & ~0x20;
                                اگر (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } ورنہ {
                                    error_رمز = U_ILLEGAL_دلیل_ERROR;
                                    واپس {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            اگر (hex > 0x10FFFF) {
                                error_رمز = U_ILLEGAL_دلیل_ERROR;
                                واپس {};
                            }
                        }
                    } ورنہ اگر (!literal.has_value()) {
                        ثابت خودکار literal_رمز_Points = utf_String_رمز_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(نمونہ_2).substr(آغاز,
                                                                 parsePosition_.get_اشاریہ() - 1 - آغاز));
                        خودکار it = literal_رمز_Points.begin();
                        اگر (it == literal_رمز_Points.انتہا() || !it->wellFormed() ||
                                (literal = it->رمز_نقطہ(), ++it) != literal_رمز_Points.انتہا()) {
                            error_رمز = U_ILLEGAL_دلیل_ERROR;
                            واپس {};
                        }
                    } ورنہ {
                        error_رمز = U_ILLEGAL_دلیل_ERROR;
                        واپس {};
                    }
                    آغاز = parsePosition_.get_اشاریہ();
                } ورنہ اگر (آخری == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(نمونہ_2).substr(
                        آغاز, parsePosition_.get_اشاریہ() - 1 - آغاز);
                    ثابت UChar32 نتیجہ = getCharacterByName(CharString().appendInvariantChars(
                        u16name.ڈیٹا(), static_cast<int32_t>(u16name.length()), error_رمز));
                    اگر (!U_SUCCESS(error_رمز)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_رمز = U_ILLEGAL_دلیل_ERROR;
                        واپس {};
                    }
                    اگر (نتیجہ < 0 || (hex.has_value() && نتیجہ != hex) ||
                            (literal.has_value() && نتیجہ != literal)) {
                        error_رمز = U_ILLEGAL_دلیل_ERROR;
                        واپس {};
                    }
                    واپس نتیجہ;
                }
            }
        }
        اگر (U_SUCCESS(error_رمز)) {
            error_رمز = U_ILLEGAL_دلیل_ERROR;
        }
        واپس {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view ماخذ) {
        UError_رمز error_رمز = U_ZERO_ERROR;
        ParsePosition عبارت_Position;
        قاعدہ_Character_Iterator عبارت_Iterator(expression, symbols_, عبارت_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer عبارت_Lexer(expression, عبارت_Position, عبارت_Iterator, unicode_سیٹ_Options_2,
                              /*symbols=*/خالی_اشاریہ, case_احاطہ_2);
        خودکار variableToken = عبارت_Lexer.lookahead();
        اگر (variableToken.is_سیٹ_Operator(u'[')) {
            UnicodeString rebuilt_نمونہ;
            Unicode_سیٹ عبارت_قدر =
                parse_Unicode_سیٹ(عبارت_Lexer, rebuilt_نمونہ, unicode_سیٹ_Options_2, case_احاطہ_2,
                                /*depth=*/0, error_رمز);
            عبارت_قدر.سیٹ_نمونہ(rebuilt_نمونہ);
            اگر (!عبارت_Lexer.at_انتہا()) {
                واپس LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالی_اشاریہ,
                    /*set=*/{},
                    ماخذ,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            }
            واپس LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_رمز,
                /*precomputedSet=*/خالی_اشاریہ,
                /*set=*/std::move(عبارت_قدر),
                ماخذ,
                /*separated=*/سچ,
                /*preceding=*/std::nullopt);
        } ورنہ {
            عبارت_Lexer.advance();
            اگر (!عبارت_Lexer.at_انتہا()) {
                واپس LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالی_اشاریہ,
                    /*set=*/{},
                    ماخذ,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            }
            انتخاب (variableToken.category_) {
            حالت LexicalElement::LITERAL_ELEMENT:
            حالت LexicalElement::ESCAPED_ELEMENT:
            حالت LexicalElement::NAMED_ELEMENT:
            حالت LexicalElement::BRACKETED_ELEMENT:
            حالت LexicalElement::STRING_LITERAL:
            حالت LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                واپس LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_رمز_2, variableToken.precomputed_سیٹ, std::move(variableToken.سیٹ_2), ماخذ,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            طےشدہ:
                واپس LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/خالی_اشاریہ,
                                      /*set=*/{}, ماخذ,
                                      /*separated=*/سچ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_سیٹ جائزہ_Property_Query_After_آغاز(UChar32 first, UChar32 دوسرا, int32_t query_آغاز, UError_رمز &error_رمز) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_عبارت_آغاز = parsePosition_.get_اشاریہ();
        منطقی exteriorlyNegated = جھوٹ;
        منطقی interiorlyNegated = جھوٹ;
        UBool غیر_مستعمل_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 تیسرا = chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                                            قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                                          غیر_مستعمل_Escaped, error_رمز);
        اگر (first == u'\\') {
            اگر (تیسرا != u'{') {
                error_رمز = U_ILLEGAL_دلیل_ERROR;
                واپس {};
            }
            exteriorlyNegated = دوسرا == u'P';
            query_عبارت_آغاز = parsePosition_.get_اشاریہ();
        } ورنہ {
            اگر (تیسرا == u'^') {
                exteriorlyNegated = سچ;
                query_عبارت_آغاز = parsePosition_.get_اشاریہ();
            }
        }
        قاعدہ_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                          قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                        غیر_مستعمل_Escaped, error_رمز);

        جب (!chars_.at_انتہا() && U_SUCCESS(error_رمز)) {
            ثابت قاعدہ_Character_Iterator::Pos before_آخری = getPos();
            UChar32 آخری_Unescaped =
                chars_.next(charsOptions_ & ~(قاعدہ_Character_Iterator::PARSE_ESCAPES |
                                              قاعدہ_Character_Iterator::SKIP_WHITESPACE),
                            غیر_مستعمل_Escaped, error_رمز);
            اگر (penultimateUnescaped == u'\\') {
                اگر (آخری_Unescaped == 'N') {
                    جائزہ_Named_Element_Brackets(error_رمز);
                    اگر (!U_SUCCESS(error_رمز)) {
                        واپس {};
                    }
                } ورنہ {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.سیٹ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~قاعدہ_Character_Iterator::SKIP_WHITESPACE, غیر_مستعمل_Escaped,
                                error_رمز);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                آخری_Unescaped = -1;
            } ورنہ اگر (!queryOperatorPosition.has_value() && آخری_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_اشاریہ() - 1;
            } ورنہ اگر (!queryOperatorPosition.has_value() && آخری_Unescaped == u'≠') {
                اگر (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_رمز = U_ILLEGAL_دلیل_ERROR;
                    واپس {};
                }
                interiorlyNegated = سچ;
                queryOperatorPosition = parsePosition_.get_اشاریہ() - 1;
            } ورنہ اگر ((first == u'[' && penultimateUnescaped == u':' && آخری_Unescaped == u']') ||
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
                اگر (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        نمونہ_2.عارضی_Sub_String_Between(*queryOperatorPosition + 1, query_عبارت_حد_بندی);
                    اگر (propertyPredicate.is_خالی_خانے()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_رمز = U_ILLEGAL_دلیل_ERROR;
                        واپس {};
                    }
                }
                Unicode_سیٹ نتیجہ;
                نتیجہ.applyPropertyAlias(
                    نمونہ_2.عارضی_Sub_String_Between(query_عبارت_آغاز,
                                                  queryOperatorPosition.value_or(query_عبارت_حد_بندی)),
                    propertyPredicate, error_رمز);
                اگر (exteriorlyNegated != interiorlyNegated) {
                    نتیجہ.complement().remove_All_لڑیاں();
                }
                نتیجہ.سیٹ_نمونہ(نمونہ_2.عارضی_Sub_String_Between(query_آغاز, parsePosition_.get_اشاریہ()));
                واپس نتیجہ;
            }
            beforePenultimate = before_آخری;
            penultimateUnescaped = آخری_Unescaped;
        }
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس {};
    }

    ثابت UnicodeString &نمونہ_2;
    ثابت ParsePosition &parsePosition_;
    قاعدہ_Character_Iterator &chars_;
    ثابت uint32_t unicode_سیٹ_Options_2;
    ثابت int32_t charsOptions_;
    ثابت علامت_جدول *ثابت symbols_;
    Unicode_سیٹ &(Unicode_سیٹ::* ثابت case_احاطہ_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نامخلا {

constexpr int32_t MAX_گہرائی = 100;

#اگر U_DEBUGGING_UNICODESET_PARSING

#تعریف_کرو U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec)                                                   \
    کرو {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        اگر (U_FAILURE(ec)) {                                                                       \
            اگر (گہرائی < 5) {                                                                       \
                لکھو("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } ورنہ اگر (گہرائی == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                لکھو("--- [...]\n");                                                             \
            }                                                                                      \
            واپس نتیجہ;                                                                         \
        }                                                                                          \
    } جب (جھوٹ)
#تعریف_کرو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, expected, actual, lexer, ec)                  \
    کرو {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        لکھو("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).مقصد_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().مقصد_UTF8_String(contextUTF8).c_str());                 \
        لکھو("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        اگر (U_FAILURE(lexer.lookahead().error_رمز())) {                                            \
            (ec) = lexer.lookahead().error_رمز();                                                  \
        } ورنہ {                                                                                   \
            (ec) = U_MALFORMED_سیٹ;                                                                \
        }                                                                                          \
        واپس نتیجہ;                                                                             \
    } جب (جھوٹ)

#ورنہ

#تعریف_کرو U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec)                                                   \
    کرو {                                                                                           \
        اگر (U_FAILURE(ec)) {                                                                       \
            واپس نتیجہ;                                                                         \
        }                                                                                          \
    } جب (جھوٹ)
#تعریف_کرو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, expected, actual, lexer, ec)                  \
    کرو {                                                                                           \
        اگر (U_FAILURE(lexer.lookahead().error_رمز())) {                                            \
            (ec) = lexer.lookahead().error_رمز();                                                  \
        } ورنہ {                                                                                   \
            (ec) = U_MALFORMED_سیٹ;                                                                \
        }                                                                                          \
        واپس نتیجہ;                                                                             \
    } جب (جھوٹ)

#اگر_ختم

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

خالی Unicode_سیٹ::apply_نمونہ(ثابت UnicodeString &نمونہ,
                              ثابت ParsePosition &parsePosition,
                              قاعدہ_Character_Iterator &chars,
                              ثابت علامت_جدول *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                              UError_رمز &ec) {
    اگر (U_FAILURE(ec)) واپس;
    Lexer lexer(نمونہ, parsePosition, chars, options, symbols, case_احاطہ);
    *یہ = parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_احاطہ, /*depth=*/0, ec);
}

Unicode_سیٹ Unicode_سیٹ::parse_Unicode_سیٹ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_سیٹ& (Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                                       int32_t گہرائی,
                                       UError_رمز &ec) {
    Unicode_سیٹ نتیجہ;

    اگر (گہرائی > MAX_گہرائی) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_گہرائی)).c_str(),
                                             ("depth = " + std::to_string(گہرائی)).c_str(), lexer,
                                             ec);
    }

    منطقی isComplement = جھوٹ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقی preserve_Syntax_اندر_نمونہ = جھوٹ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_نمونہ;
    اگر (lexer.lookahead().سیٹ() != خالی_اشاریہ) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        نتیجہ = *lexer.lookahead().سیٹ();
        نتیجہ.مقصد_نمونہ_2(pretty_Printed_نمونہ, /*escapeUnprintable=*/جھوٹ);
        lexer.advance();
        preserve_Syntax_اندر_نمونہ = سچ;
    } ورنہ {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        اگر (lexer.accept_سیٹ_Operator(u'[')) {
            pretty_Printed_نمونہ.append(u'[');
            اگر (lexer.accept_سیٹ_Operator(u'^')) {
                pretty_Printed_نمونہ.append(u'^');
                isComplement = سچ;
            }
            نتیجہ = parseContent(lexer, pretty_Printed_نمونہ, options, case_احاطہ, گہرائی,
                                  /*containsSetOperation=*/preserve_Syntax_اندر_نمونہ, ec);
            U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec);
            اگر (!lexer.accept_سیٹ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجہ, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_نمونہ.append(u']');
        } ورنہ {
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
    اگر ((options & USET_CASE_نقاب) != 0) {
        (نتیجہ.*case_احاطہ)(options);
    }
    اگر (isComplement) {
        نتیجہ.complement().remove_All_لڑیاں(); // code point complement
    }
    اگر (preserve_Syntax_اندر_نمونہ) {
        rebuiltPat.append(pretty_Printed_نمونہ);
    } ورنہ {
        نتیجہ.generate_نمونہ(rebuiltPat, /*escapeUnprintable=*/جھوٹ);
    }
    واپس نتیجہ;
}

Unicode_سیٹ Unicode_سیٹ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                                    int32_t گہرائی,
                                    منطقی &شامل_ہے_سیٹ_Operation,
                                    UError_رمز &ec) {
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
    اگر (lexer.accept_سیٹ_Operator(u'-')) {
        نتیجہ.جوڑو(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    جب (!lexer.at_انتہا()) {
        اگر (lexer.accept_سیٹ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            نتیجہ.جوڑو(u'-');
            واپس نتیجہ;
        } ورنہ اگر (lexer.lookahead().is_سیٹ_Operator(u'$')) {
            اگر (lexer.lookahead2().is_سیٹ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                نتیجہ.جوڑو(U_ETHER);
                شامل_ہے_سیٹ_Operation = سچ;
                واپس نتیجہ;
            }
        }
        اگر (lexer.lookahead().is_سیٹ_Operator(u']')) {
            واپس نتیجہ;
        }
        // Also handles FinalMutation.
        نتیجہ.parseMutation(lexer, rebuiltPat, options, case_احاطہ, گہرائی, شامل_ہے_سیٹ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(نتیجہ, ec);
    }
    واپس نتیجہ;
}

خالی Unicode_سیٹ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                               int32_t گہرائی,
                               منطقی &شامل_ہے_سیٹ_Operation,
                               UError_رمز &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    اگر (lexer.lookahead().is_سیٹ_Operator('[') || lexer.lookahead().سیٹ() != خالی_اشاریہ) {
        شامل_ہے_سیٹ_Operation = سچ;
        parse_سیٹ_Operations(lexer, rebuiltPat, options, case_احاطہ, گہرائی, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } ورنہ {
        // Also handles DollarElements.
        جوڑو_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

خالی Unicode_سیٹ::parse_سیٹ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                                    int32_t گہرائی,
                                    UError_رمز &ec) {
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
        parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_احاطہ, گہرائی + 1, ec);
    جوڑو_All(بایاں_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    برائے (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        اگر (lexer.accept_سیٹ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت Unicode_سیٹ دایاں_Hand_Side =
                parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_احاطہ, گہرائی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(دایاں_Hand_Side);
        } ورنہ اگر (lexer.lookahead().is_سیٹ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            اگر (lexer.lookahead2().is_سیٹ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                واپس;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت Unicode_سیٹ دایاں_Hand_Side =
                parse_Unicode_سیٹ(lexer, rebuiltPat, options, case_احاطہ, گہرائی + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(دایاں_Hand_Side);
        } ورنہ {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            واپس;
        }
    }
}

Unicode_سیٹ Unicode_سیٹ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_رمز &ec) {
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
    اگر (lexer.lookahead().isStringLiteral()) {
        ثابت UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_مقصد_Pat(rebuiltPat, element, /*escapeUnprintable=*/جھوٹ);
        rebuiltPat.append(u'}');
        lexer.advance();
        واپس Unicode_سیٹ().جوڑو(element);
    }
    UChar32 first;
    اگر (lexer.lookahead().is_سیٹ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } ورنہ اگر (lexer.lookahead().رمز_نقطہ().has_value()) {
        first = *lexer.lookahead().رمز_نقطہ();
    } ورنہ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_مقصد_Pat(rebuiltPat, first, /*escapeUnprintable=*/جھوٹ);
    اگر (!lexer.lookahead().is_سیٹ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        واپس Unicode_سیٹ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    اگر (lexer.lookahead2().is_سیٹ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        واپس Unicode_سیٹ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 آخری;
    اگر (lexer.lookahead().is_سیٹ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        آخری = u'$';
        اگر (lexer.lookahead2().is_سیٹ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } ورنہ اگر (lexer.lookahead().رمز_نقطہ().has_value()) {
        آخری = *lexer.lookahead().رمز_نقطہ();
    } ورنہ {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    اگر (آخری <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(آخری) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_مقصد_Pat(rebuiltPat, آخری, /*escapeUnprintable=*/جھوٹ);
    واپس Unicode_سیٹ(first, آخری);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

نامخلا {

UBool numeric_قدر_Filter(UChar32 ch, خالی* context) {
    واپس u_get_Numeric_قدر(ch) == *static_cast<دوہرا*>(context);
}

UBool general_Category_نقاب_Filter(UChar32 ch, خالی* context) {
    int32_t value = *static_cast<int32_t*>(context);
    واپس (U_GET_GC_نقاب((UChar32) ch) & value) != 0;
}

UBool نسخہ_Filter(UChar32 ch, خالی* context) {
    ساکن ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* نسخہ = static_cast<UVersionInfo*>(context);
    واپس uprv_memcmp(&v, &none, حجم(v)) > 0 && uprv_memcmp(&v, نسخہ, حجم(v)) <= 0;
}

قسمتعریف ساخت {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, خالی* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    واپس u_get_Int_Property_قدر(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, خالی* context) {
    واپس uscript_hasScript(ch, *static_cast<UScript_رمز*>(context));
}

UBool idTypeFilter(UChar32 ch, خالی* context) {
    واپس u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
خالی Unicode_سیٹ::applyFilter(Unicode_سیٹ::Filter filter,
                             خالی* context,
                             ثابت Unicode_سیٹ* inclusions,
                             UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) واپس;

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

    UChar32 آغاز_Has_Property = -1;
    int32_t حد_بندی_Range = inclusions->get_Range_گنتی();

    برائے (صحیح j=0; j<حد_بندی_Range; ++j) {
        // get current range
        UChar32 آغاز = inclusions->get_Range_آغاز(j);
        UChar32 انتہا = inclusions->get_Range_انتہا(j);

        // for all the code points in the range, process
        برائے (UChar32 ch = آغاز; ch <= انتہا; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            اگر ((*filter)(ch, context)) {
                اگر (آغاز_Has_Property < 0) {
                    آغاز_Has_Property = ch;
                }
            } ورنہ اگر (آغاز_Has_Property >= 0) {
                جوڑو(آغاز_Has_Property, ch-1);
                آغاز_Has_Property = -1;
            }
        }
    }
    اگر (آغاز_Has_Property >= 0) {
        جوڑو(آغاز_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    اگر (isBogus() && U_SUCCESS(حالت_2)) {
        // We likely ran out of memory. AHHH!
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
}

نامخلا {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#تعریف_کرو FAIL(ec) UPRV_بلاک_MACRO_BEGIN { \
    ec=U_ILLEGAL_دلیل_ERROR; \
    واپس *یہ; \
} UPRV_بلاک_MACRO_انتہا

Unicode_سیٹ&
Unicode_سیٹ::apply_Int_Property_قدر(UProperty prop, int32_t value, UError_رمز& ec) U_LIFETIME_BOUND {
    اگر (U_FAILURE(ec) || isFrozen()) { واپس *یہ; }
    اگر (prop == UCHAR_GENERAL_CATEGORY_نقاب) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_نقاب_Filter, &value, inclusions, ec);
    } ورنہ اگر (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_رمز script = static_cast<UScript_رمز>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } ورنہ اگر (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } ورنہ اگر (0 <= prop && prop < UCHAR_ثنائی_حد_بندی) {
        اگر (value == 0 || value == 1) {
            ثابت USet *سیٹ = u_get_ثنائی_Property_سیٹ(prop, &ec);
            اگر (U_FAILURE(ec)) { واپس *یہ; }
            copyFrom(*Unicode_سیٹ::fromUSet(سیٹ), سچ);
            اگر (value == 0) {
                complement().remove_All_لڑیاں();  // code point complement
            }
        } ورنہ {
            clear();
        }
    } ورنہ اگر (UCHAR_INT_آغاز <= prop && prop < UCHAR_INT_حد_بندی) {
        ثابت Unicode_سیٹ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } ورنہ {
        ec = U_ILLEGAL_دلیل_ERROR;
    }
    واپس *یہ;
}

Unicode_سیٹ&
Unicode_سیٹ::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UError_رمز& ec) U_LIFETIME_BOUND {
    اگر (U_FAILURE(ec) || isFrozen()) واپس *یہ;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    اگر( !uprv_isInvariantUString(prop.get_بفر(), prop.length()) ||
        !uprv_isInvariantUString(value.get_بفر(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    اگر (U_FAILURE(ec)) واپس *یہ;

    UProperty p;
    int32_t v;
    UBool invert = جھوٹ;

    اگر (value.length() > 0) {
        p = u_getPropertyEnum(pname.ڈیٹا());
        اگر (p == UCHAR_INVALID_رمز) FAIL(ec);

        // Treat gc as gcm
        اگر (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_نقاب;
        }

        اگر ((p >= UCHAR_ثنائی_آغاز && p < UCHAR_ثنائی_حد_بندی) ||
            (p >= UCHAR_INT_آغاز && p < UCHAR_INT_حد_بندی) ||
            (p >= UCHAR_نقاب_آغاز && p < UCHAR_نقاب_حد_بندی)) {
            v = u_get_Property_قدر_Enum(p, vname.ڈیٹا());
            اگر (v == UCHAR_INVALID_رمز) {
                // Handle numeric CCC
                اگر (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* انتہا;
                    دوہرا val = uprv_strtod(vname.ڈیٹا(), &انتہا);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    اگر (*انتہا != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } ورنہ {
                    FAIL(ec);
                }
            }
        }

        ورنہ {

            انتخاب (p) {
            حالت UCHAR_NUMERIC_قدر:
                {
                    حرف* انتہا;
                    دوہرا val = uprv_strtod(vname.ڈیٹا(), &انتہا);
                    اگر (*انتہا != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_قدر_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    واپس *یہ;
                }
            حالت UCHAR_NAME:
                {
                    ثابت UChar32 ch = getCharacterByName(vname);
                    اگر (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    جوڑو(ch);
                    واپس *یہ;
                }
            حالت UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالت UCHAR_AGE:
                {
                    UVersionInfo نسخہ;
                    u_نسخہ_From_String(نسخہ, vname.ڈیٹا());
                    applyFilter(نسخہ_Filter, &نسخہ,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    واپس *یہ;
                }
            حالت UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_قدر_Enum(UCHAR_SCRIPT, vname.ڈیٹا());
                اگر (v == UCHAR_INVALID_رمز) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                توڑو;
            حالت UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_قدر_Enum(p, vname.ڈیٹا());
                اگر (v == UCHAR_INVALID_رمز) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                توڑو;
            طےشدہ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    ورنہ {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_نقاب;
        v = u_get_Property_قدر_Enum(p, pname.ڈیٹا());
        اگر (v == UCHAR_INVALID_رمز) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_قدر_Enum(p, pname.ڈیٹا());
            اگر (v == UCHAR_INVALID_رمز) {
                p = u_getPropertyEnum(pname.ڈیٹا());
                اگر (p >= UCHAR_ثنائی_آغاز && p < UCHAR_ثنائی_حد_بندی) {
                    v = 1;
                } ورنہ اگر (0 == uprv_compare_Property_نام(ANY, pname.ڈیٹا())) {
                    سیٹ(MIN_قدر, MAX_قدر);
                    واپس *یہ;
                } ورنہ اگر (0 == uprv_compare_Property_نام(ASCII, pname.ڈیٹا())) {
                    سیٹ(0, 0x7F);
                    واپس *یہ;
                } ورنہ اگر (0 == uprv_compare_Property_نام(ASSIGNED, pname.ڈیٹا())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_نقاب;
                    v = U_GC_CN_نقاب;
                    invert = سچ;
                } ورنہ {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_قدر(p, v, ec);
    اگر(invert) {
        complement().remove_All_لڑیاں();  // code point complement
    }

    اگر (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_حافظہ_تخصیص_ERROR;
    }
    واپس *یہ;
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
    اگر ((pos+5) > نمونہ.length()) {
        واپس جھوٹ;
    }

    // Look for an opening [:, [:^, \p, or \P
    واپس isPOSIXOpen(نمونہ, pos) || isPerlOpen(نمونہ, pos) || isNameOpen(نمونہ, pos);
}

U_NAMESPACE_انتہا
