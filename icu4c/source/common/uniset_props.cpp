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

ساكن Unicode_مجموعه *uni32Singleton;
ساكن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساكن UBool U_CALLCONV uset_cleanup() {
    حذف uni32Singleton;
    uni32Singleton = اشاره_تهی;
    uni32InitOnce.reset();
    أرجع درست;
}

U_CDECL_انتها

U_NAMESPACE_BEGIN

استفاده U_سرآیند_ONLY_NAMESPACE::utf_String_کد_Points;

نطاق {

// Cache some sets for other services -------------------------------------- ***
فراغ U_CALLCONV create_Uni32_مجموعه(UError_کد &error_کد) {
    U_ASSERT(uni32Singleton == اشاره_تهی);
    uni32Singleton = جديد Unicode_مجموعه(UnicodeString(u"[:age=3.2:]"), error_کد);
    إذا(uni32Singleton==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    } وإلا {
        uni32Singleton->freeze();
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_مجموعه *
uniset_getUnicode32Instance(UError_کد &error_کد) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_مجموعه, error_کد);
    أرجع uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

مضمن UBool
isPerlOpen(ثابت UnicodeString &الگو, int32_t pos) {
    char16_t c;
    أرجع الگو.charAt(pos)==u'\\' && ((c=الگو.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

مضمن UBool
isNameOpen(ثابت UnicodeString &الگو, int32_t pos) {
    أرجع الگو.charAt(pos)==u'\\' && الگو.charAt(pos+1)==u'N';
}

مضمن UBool
isPOSIXOpen(ثابت UnicodeString &الگو, int32_t pos) {
    أرجع الگو.charAt(pos)==u'[' && الگو.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#تعریف_کن _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ثابت CharString& name) {
    لكل (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_کد ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.داده(), &ec);
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
Unicode_مجموعه::Unicode_مجموعه(ثابت UnicodeString& الگو,
                       UError_کد& وضعیت) {
    apply_الگو(الگو, وضعیت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_مجموعه& Unicode_مجموعه::apply_الگو(ثابت UnicodeString& الگو,
                                     UError_کد& وضعیت) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_الگو_Ignore_Space(الگو, pos, اشاره_تهی, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع *هذا;

    int32_t i = pos.get_شاخص();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(الگو, i, درست);
    إذا (i != الگو.length()) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع *هذا;
}

فراغ
Unicode_مجموعه::apply_الگو_Ignore_Space(ثابت UnicodeString& الگو,
                                    ParsePosition& pos,
                                    ثابت نماد_جدول* symbols,
                                    UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (isFrozen()) {
        وضعیت = U_NO_WRITE_PERMISSION;
        أرجع;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    قاعده_Character_Iterator chars(الگو, symbols, pos);
    apply_الگو(الگو, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, اشاره_تهی, وضعیت);
    إذا (U_FAILURE(وضعیت)) أرجع;
    إذا (chars.درون_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        وضعیت = U_MALFORMED_مجموعه;
        أرجع;
    }
    مجموعه_الگو(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_مجموعه::resembles_الگو(ثابت UnicodeString& الگو, int32_t pos) {
    أرجع ((pos+1) < الگو.length() &&
            الگو.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_الگو(الگو, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#تعریف_کن U_DEBUGGING_UNICODESET_PARSING 0

صنف Unicode_مجموعه::Lexer {
  عام:
    Lexer(ثابت UnicodeString &الگو,
          ثابت ParsePosition &parsePosition,
          قاعده_Character_Iterator &chars,
          uint32_t unicode_مجموعه_Options,
          ثابت نماد_جدول *ثابت symbols,
          Unicode_مجموعه &(Unicode_مجموعه::*case_بستگی)(int32_t attribute))
        : الگو_2(الگو), parsePosition_(parsePosition), chars_(chars),
          unicode_مجموعه_Options_2(unicode_مجموعه_Options),
          charsOptions_(قاعده_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_مجموعه_Options & USET_IGNORE_SPACE) != 0
                             ? قاعده_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_بستگی_2(case_بستگی) {}

    صنف LexicalElement {
      عام:
        منطقي is_مجموعه_Operator(ثابت char16_t op) ثابت {
            أرجع U_SUCCESS(error_کد_2) && category_ == مجموعه_OPERATOR && string_[0] == op;
        }

        منطقي isStringLiteral() ثابت {
            أرجع U_SUCCESS(error_کد_2) && category_ == STRING_LITERAL;
        }

        منطقي isNamedElement() ثابت {
            أرجع U_SUCCESS(error_کد_2) && category_ == NAMED_ELEMENT;
        }

        منطقي isBracketedElement() ثابت {
            أرجع U_SUCCESS(error_کد_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            إذا (U_SUCCESS(error_کد_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                أرجع string_;
            }
            أرجع std::nullopt;
        }

        std::optional<UChar32> کد_نقطه() ثابت {
            إذا (U_SUCCESS(error_کد_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                أرجع string_.char32At(0);
            }
            أرجع std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت Unicode_مجموعه *مجموعه() ثابت {
            إذا (U_FAILURE(error_کد_2)) {
                أرجع اشاره_تهی;
            }
            إذا (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                إذا (precomputed_مجموعه != اشاره_تهی) {
                    أرجع precomputed_مجموعه;
                } وإلا {
                    أرجع &مجموعه_2;
                }
            }
            أرجع اشاره_تهی;
        }

        ثابت UError_کد& error_کد() ثابت{
          أرجع error_کد_2;
        }

#إذا U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString نتیجه;
            إذا (U_FAILURE(error_کد_2)) {
                نتیجه.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_کد_2)))
                    .append(u"), possibly ");
            }
            أرجع نتیجه.append(category_نامها[category_])
                .append(u" '")
                .append(منبع_متن)
                .append(u"'");
        }
#پایان_اگر

      خاص:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        تعداد Category : std::uint8_t {
            مجموعه_OPERATOR,
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
            انتها_از_متن,
        };
        ساكن constexpr std::array<std::u16string_view, انتها_از_متن + 1> category_نامها{{
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
                       ثابت UError_کد error_کد, ثابت Unicode_مجموعه *precomputed_مجموعه_2, Unicode_مجموعه مجموعه,
                       std::u16string_view منبع_متن_2, منطقي separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_کد_2(error_کد),
              precomputed_مجموعه(precomputed_مجموعه_2), مجموعه_2(مجموعه), منبع_متن(منبع_متن_2) {
            إذا (U_FAILURE(error_کد_2) || separated || !preceding.has_value()) {
                أرجع;
            }
            إذا (preceding->is_مجموعه_Operator(u'[') && category == LITERAL_ELEMENT &&
                *کد_نقطه() == u':') {
                error_کد_2 = U_MALFORMED_مجموعه;
            } وإلا إذا (preceding->category_ == ESCAPED_ELEMENT) {
                إذا (U_IS_LEAD(*preceding->کد_نقطه()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*کد_نقطه())) {
                    error_کد_2 = U_MALFORMED_مجموعه;
                } وإلا إذا (preceding->منبع_متن.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *کد_نقطه();
                    إذا (preceding->منبع_متن[1] == u'x') {
                        إذا ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_کد_2 = U_MALFORMED_مجموعه;
                        }
                    } وإلا إذا (preceding->منبع_متن[1] >= u'0' &&
                               preceding->منبع_متن[1] <= u'7') {
                        إذا (cp >= u'0' && cp <= u'7') {
                            error_کد_2 = U_MALFORMED_مجموعه;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        قاعده_Character_Iterator::Pos after_;
        UError_کد error_کد_2;
        ثابت Unicode_مجموعه *precomputed_مجموعه;
        Unicode_مجموعه مجموعه_2;
        std::u16string_view منبع_متن;

        دوست صنف Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        أرجع الگو_2.موقت_Sub_String(0, parsePosition_.get_شاخص()) + u"☞" +
               الگو_2.موقت_Sub_String(parsePosition_.get_شاخص(), 60);
    }

    منطقي accept_مجموعه_Operator(char16_t op) {
        إذا (lookahead().is_مجموعه_Operator(op)) {
            advance();
            أرجع درست;
        }
        أرجع خطأ;
    }

    ثابت LexicalElement &lookahead() {
        إذا (!ahead_.has_value()) {
            ثابت قاعده_Character_Iterator::Pos before = getPos();
            ahead_.emplace(بعدی_Token(behind_));
            chars_.مجموعه_Pos(before);
        }
        أرجع *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        إذا (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت قاعده_Character_Iterator::Pos before = getPos();
            chars_.مجموعه_Pos(lookahead().after_);
            ahead2_.emplace(بعدی_Token(ahead_));
            chars_.مجموعه_Pos(before);
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

    منطقي at_انتها() ثابت {
        أرجع chars_.at_انتها();
    }

    فراغ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.مجموعه_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خاص:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    قاعده_Character_Iterator::Pos getPos() ثابت {
        قاعده_Character_Iterator::Pos نتیجه;
        chars_.getPos(نتیجه);
        أرجع نتیجه;
    }

    LexicalElement بعدی_Token(std::optional<LexicalElement> ثابت& preceding) {
        UError_کد error_کد = U_ZERO_ERROR;
        منطقي separated = خطأ;
        إذا (charsOptions_ & قاعده_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            قاعده_Character_Iterator::Pos pos;
            لكل (;;) {
                UBool استفادهنشده_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                  قاعده_Character_Iterator::SKIP_WHITESPACE),
                                استفادهنشده_Escaped, error_کد);
                إذا (!الگو_Props::isWhiteSpace(s)) {
                    chars_.مجموعه_Pos(pos);
                    اكسر;
                }
                إذا (s != u'\u200E' && s != u'\u200F') {
                    separated = درست;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        إذا (chars_.at_انتها()) {
            أرجع LexicalElement(LexicalElement::انتها_از_متن, {}, getPos(), error_کد,
                                  /*precomputedSet=*/اشاره_تهی,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t شروع = parsePosition_.get_شاخص();
        ثابت قاعده_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool استفادهنشده_Escaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~قاعده_Character_Iterator::PARSE_ESCAPES, استفادهنشده_Escaped, error_کد);
        إذا (first == u'[' || first == u'\\') {
            ثابت قاعده_Character_Iterator::Pos after_اولی = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 دومی = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                                 قاعده_Character_Iterator::SKIP_WHITESPACE),
                                               استفادهنشده_Escaped, error_کد);
            إذا ((first == u'[' && دومی == u':') ||
                (first == u'\\' && (دومی == u'p' || دومی == u'P' || دومی == u'N'))) {
                إذا (دومی == u'N') {
                    UChar32 ثابت query_نتیجه = پیمایش_Named_Element_Brackets(error_کد);
                    أرجع LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_نتیجه), getPos(), error_کد,
                        /*precomputedSet=*/اشاره_تهی,
                        /*set=*/{},
                        std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                        separated, preceding);
                } وإلا {
                    Unicode_مجموعه query_نتیجه = پیمایش_Property_Query_After_شروع(first, دومی, شروع, error_کد);
                    أرجع LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_کد,
                        /*precomputedSet=*/اشاره_تهی, /*set=*/std::move(query_نتیجه),
                        std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.مجموعه_Pos(after_اولی);
        }
        إذا (first == u'$' && symbols_ != اشاره_تهی) {
            خودکار name_انتها = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            إذا (UnicodeString name = symbols_->parseReference(الگو_2, name_انتها, الگو_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_خالی()) {
                chars_.jumpahead(name_انتها.get_شاخص() - (شروع + 1));
                ثابت std::u16string_view منبع =
                    std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع);
                ثابت Unicode_مجموعه *precomputed_مجموعه_2 = symbols_->lookup_مجموعه(name);
                إذا (precomputed_مجموعه_2 != اشاره_تهی) {
                    أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_مجموعه_2, /*set=*/{}, منبع, /*separated=*/درست,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                إذا (expression == اشاره_تهی) {
                    أرجع LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/اشاره_تهی,
                        /*set=*/{},
                        منبع,
                        /*separated=*/درست,
                        /*preceding=*/std::nullopt);
                }
                أرجع evaluateVariable(*expression, منبع);
            }
        }
        بدل (first) {
        حالة u'[':
            أرجع LexicalElement(
                LexicalElement::مجموعه_OPERATOR, UnicodeString(u'['), getPos(), error_کد,
                /*precomputedSet=*/اشاره_تهی,
                /*set=*/{},
                std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                separated, preceding);
        حالة u'\\': {
            // Now try to parse the escape.
            chars_.مجموعه_Pos(before);
            UChar32 کد_نقطه = chars_.next(charsOptions_, استفادهنشده_Escaped, error_کد);
            أرجع LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(کد_نقطه), getPos(), error_کد,
                اشاره_تهی,
                /*set=*/{},
                std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                separated, preceding);
        }
        حالة u'&':
        حالة u'-':
        حالة u']':
        حالة u'^':
        حالة u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            أرجع LexicalElement(
                LexicalElement::مجموعه_OPERATOR, UnicodeString(first), getPos(), error_کد,
                /*precomputedSet=*/اشاره_تهی,
                /*set=*/{},
                std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                separated, preceding);
        حالة u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t کد_نقطه_شمار = 0;
            طالما (!chars_.at_انتها() && U_SUCCESS(error_کد)) {
                ثابت قاعده_Character_Iterator::Pos before_بعدی = getPos();
                next = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                     قاعده_Character_Iterator::SKIP_WHITESPACE),
                                   استفادهنشده_Escaped, error_کد);
                إذا (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                      قاعده_Character_Iterator::SKIP_WHITESPACE),
                                    استفادهنشده_Escaped, error_کد);
                    إذا (afterBackslash == u'N') {
                        next = پیمایش_Named_Element_Brackets(error_کد);
                        escaped = درست;
                    } وإلا إذا (afterBackslash == u'p' || afterBackslash == u'P') {
                        أرجع LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_مجموعه,
                                              /*precomputedSet=*/اشاره_تهی,
                                              /*set=*/{},
                                              std::u16string_view(الگو_2).substr(
                                                  شروع, parsePosition_.get_شاخص() - شروع),
                                              separated, preceding);
                    } وإلا {
                        chars_.مجموعه_Pos(before_بعدی);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_کد);
                    }
                } وإلا {
#إذا U_ICU_نسخه_MAJOR_رقم < 81
                    إذا (U_SUCCESS(error_کد) && الگو_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_کد = UError_کد::U_ILLEGAL_آرگومان_ERROR;
                    }
#وإلا
#خطا Remove هذا transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#پایان_اگر
                    escaped = خطأ;
                }
                إذا (!escaped && next == u'}') {
                    أرجع LexicalElement(
                        کد_نقطه_شمار == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_کد,
                        /*precomputedSet=*/اشاره_تهی,
                        /*set=*/{},
                        std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                        separated, preceding);
                }
                string.append(next);
                کد_نقطه_شمار += 1;
            }
            أرجع LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_مجموعه,
                /*precomputedSet=*/اشاره_تهی,
                /*set=*/{},
                std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                separated, preceding);
        }
        افتراضي:
            أرجع LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_کد, اشاره_تهی,
                /*set=*/{},
                std::u16string_view(الگو_2).substr(شروع, parsePosition_.get_شاخص() - شروع),
                separated, preceding);
        }
    }

    UChar32 پیمایش_Named_Element_Brackets(UError_کد &error_کد) {
        UBool استفادهنشده_Escaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                           قاعده_Character_Iterator::SKIP_WHITESPACE),
                                         استفادهنشده_Escaped, error_کد);
        إذا (open == u'{') {
            int32_t شروع = parsePosition_.get_شاخص();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            طالما (!chars_.at_انتها() && U_SUCCESS(error_کد)) {
                UChar32 آخرین = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                             قاعده_Character_Iterator::SKIP_WHITESPACE),
                                           استفادهنشده_Escaped, error_کد);
                إذا (آخرین == u':') {
                    إذا (!hex.has_value()) {
                        hex.emplace();
                        إذا (شروع == parsePosition_.get_شاخص() - 1) {
                            error_کد = U_ILLEGAL_آرگومان_ERROR;
                            أرجع {};
                        }
                        لكل (char16_t digit : std::u16string_view(الگو_2).substr(
                                 شروع, parsePosition_.get_شاخص() - 1 - شروع)) {
                            uint8_t nibble;
                            إذا (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } وإلا {
                                digit = digit & ~0x20;
                                إذا (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } وإلا {
                                    error_کد = U_ILLEGAL_آرگومان_ERROR;
                                    أرجع {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            إذا (hex > 0x10FFFF) {
                                error_کد = U_ILLEGAL_آرگومان_ERROR;
                                أرجع {};
                            }
                        }
                    } وإلا إذا (!literal.has_value()) {
                        ثابت خودکار literal_کد_Points = utf_String_کد_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(الگو_2).substr(شروع,
                                                                 parsePosition_.get_شاخص() - 1 - شروع));
                        خودکار it = literal_کد_Points.begin();
                        إذا (it == literal_کد_Points.انتها() || !it->wellFormed() ||
                                (literal = it->کد_نقطه(), ++it) != literal_کد_Points.انتها()) {
                            error_کد = U_ILLEGAL_آرگومان_ERROR;
                            أرجع {};
                        }
                    } وإلا {
                        error_کد = U_ILLEGAL_آرگومان_ERROR;
                        أرجع {};
                    }
                    شروع = parsePosition_.get_شاخص();
                } وإلا إذا (آخرین == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(الگو_2).substr(
                        شروع, parsePosition_.get_شاخص() - 1 - شروع);
                    ثابت UChar32 نتیجه = getCharacterByName(CharString().appendInvariantChars(
                        u16name.داده(), static_cast<int32_t>(u16name.length()), error_کد));
                    إذا (!U_SUCCESS(error_کد)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_کد = U_ILLEGAL_آرگومان_ERROR;
                        أرجع {};
                    }
                    إذا (نتیجه < 0 || (hex.has_value() && نتیجه != hex) ||
                            (literal.has_value() && نتیجه != literal)) {
                        error_کد = U_ILLEGAL_آرگومان_ERROR;
                        أرجع {};
                    }
                    أرجع نتیجه;
                }
            }
        }
        إذا (U_SUCCESS(error_کد)) {
            error_کد = U_ILLEGAL_آرگومان_ERROR;
        }
        أرجع {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view منبع) {
        UError_کد error_کد = U_ZERO_ERROR;
        ParsePosition عبارت_Position;
        قاعده_Character_Iterator عبارت_Iterator(expression, symbols_, عبارت_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer عبارت_Lexer(expression, عبارت_Position, عبارت_Iterator, unicode_مجموعه_Options_2,
                              /*symbols=*/اشاره_تهی, case_بستگی_2);
        خودکار variableToken = عبارت_Lexer.lookahead();
        إذا (variableToken.is_مجموعه_Operator(u'[')) {
            UnicodeString rebuilt_الگو;
            Unicode_مجموعه عبارت_مقدار =
                parse_Unicode_مجموعه(عبارت_Lexer, rebuilt_الگو, unicode_مجموعه_Options_2, case_بستگی_2,
                                /*depth=*/0, error_کد);
            عبارت_مقدار.مجموعه_الگو(rebuilt_الگو);
            إذا (!عبارت_Lexer.at_انتها()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/اشاره_تهی,
                    /*set=*/{},
                    منبع,
                    /*separated=*/درست,
                    /*preceding=*/std::nullopt);
            }
            أرجع LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_کد,
                /*precomputedSet=*/اشاره_تهی,
                /*set=*/std::move(عبارت_مقدار),
                منبع,
                /*separated=*/درست,
                /*preceding=*/std::nullopt);
        } وإلا {
            عبارت_Lexer.advance();
            إذا (!عبارت_Lexer.at_انتها()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/اشاره_تهی,
                    /*set=*/{},
                    منبع,
                    /*separated=*/درست,
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
                    variableToken.error_کد_2, variableToken.precomputed_مجموعه, std::move(variableToken.مجموعه_2), منبع,
                    /*separated=*/درست,
                    /*preceding=*/std::nullopt);
            افتراضي:
                أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/اشاره_تهی,
                                      /*set=*/{}, منبع,
                                      /*separated=*/درست,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_مجموعه پیمایش_Property_Query_After_شروع(UChar32 first, UChar32 دومی, int32_t query_شروع, UError_کد &error_کد) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_عبارت_شروع = parsePosition_.get_شاخص();
        منطقي exteriorlyNegated = خطأ;
        منطقي interiorlyNegated = خطأ;
        UBool استفادهنشده_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 سوم = chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                                            قاعده_Character_Iterator::SKIP_WHITESPACE),
                                          استفادهنشده_Escaped, error_کد);
        إذا (first == u'\\') {
            إذا (سوم != u'{') {
                error_کد = U_ILLEGAL_آرگومان_ERROR;
                أرجع {};
            }
            exteriorlyNegated = دومی == u'P';
            query_عبارت_شروع = parsePosition_.get_شاخص();
        } وإلا {
            إذا (سوم == u'^') {
                exteriorlyNegated = درست;
                query_عبارت_شروع = parsePosition_.get_شاخص();
            }
        }
        قاعده_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                          قاعده_Character_Iterator::SKIP_WHITESPACE),
                        استفادهنشده_Escaped, error_کد);

        طالما (!chars_.at_انتها() && U_SUCCESS(error_کد)) {
            ثابت قاعده_Character_Iterator::Pos before_آخرین = getPos();
            UChar32 آخرین_Unescaped =
                chars_.next(charsOptions_ & ~(قاعده_Character_Iterator::PARSE_ESCAPES |
                                              قاعده_Character_Iterator::SKIP_WHITESPACE),
                            استفادهنشده_Escaped, error_کد);
            إذا (penultimateUnescaped == u'\\') {
                إذا (آخرین_Unescaped == 'N') {
                    پیمایش_Named_Element_Brackets(error_کد);
                    إذا (!U_SUCCESS(error_کد)) {
                        أرجع {};
                    }
                } وإلا {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.مجموعه_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~قاعده_Character_Iterator::SKIP_WHITESPACE, استفادهنشده_Escaped,
                                error_کد);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                آخرین_Unescaped = -1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخرین_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_شاخص() - 1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخرین_Unescaped == u'≠') {
                إذا (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_کد = U_ILLEGAL_آرگومان_ERROR;
                    أرجع {};
                }
                interiorlyNegated = درست;
                queryOperatorPosition = parsePosition_.get_شاخص() - 1;
            } وإلا إذا ((first == u'[' && penultimateUnescaped == u':' && آخرین_Unescaped == u']') ||
                       (first == u'\\' && آخرین_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ثابت int32_t query_عبارت_حد =
                    first == u'[' ? parsePosition_.get_شاخص() - 2 : parsePosition_.get_شاخص() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                إذا (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        الگو_2.موقت_Sub_String_Between(*queryOperatorPosition + 1, query_عبارت_حد);
                    إذا (propertyPredicate.is_خالی()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_کد = U_ILLEGAL_آرگومان_ERROR;
                        أرجع {};
                    }
                }
                Unicode_مجموعه نتیجه;
                نتیجه.applyPropertyAlias(
                    الگو_2.موقت_Sub_String_Between(query_عبارت_شروع,
                                                  queryOperatorPosition.value_or(query_عبارت_حد)),
                    propertyPredicate, error_کد);
                إذا (exteriorlyNegated != interiorlyNegated) {
                    نتیجه.complement().remove_All_رشتهها();
                }
                نتیجه.مجموعه_الگو(الگو_2.موقت_Sub_String_Between(query_شروع, parsePosition_.get_شاخص()));
                أرجع نتیجه;
            }
            beforePenultimate = before_آخرین;
            penultimateUnescaped = آخرین_Unescaped;
        }
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع {};
    }

    ثابت UnicodeString &الگو_2;
    ثابت ParsePosition &parsePosition_;
    قاعده_Character_Iterator &chars_;
    ثابت uint32_t unicode_مجموعه_Options_2;
    ثابت int32_t charsOptions_;
    ثابت نماد_جدول *ثابت symbols_;
    Unicode_مجموعه &(Unicode_مجموعه::* ثابت case_بستگی_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نطاق {

constexpr int32_t MAX_عمق = 100;

#إذا U_DEBUGGING_UNICODESET_PARSING

#تعریف_کن U_UNICODESET_RETURN_IF_ERROR(نتیجه, ec)                                                   \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        إذا (U_FAILURE(ec)) {                                                                       \
            إذا (عمق < 5) {                                                                       \
                چاپ_کن("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } وإلا إذا (عمق == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                چاپ_کن("--- [...]\n");                                                             \
            }                                                                                      \
            أرجع نتیجه;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کن U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجه, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        چاپ_کن("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).مقصد_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().مقصد_UTF8_String(contextUTF8).c_str());                 \
        چاپ_کن("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        إذا (U_FAILURE(lexer.lookahead().error_کد())) {                                            \
            (ec) = lexer.lookahead().error_کد();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_مجموعه;                                                                \
        }                                                                                          \
        أرجع نتیجه;                                                                             \
    } طالما (خطأ)

#وإلا

#تعریف_کن U_UNICODESET_RETURN_IF_ERROR(نتیجه, ec)                                                   \
    افعل {                                                                                           \
        إذا (U_FAILURE(ec)) {                                                                       \
            أرجع نتیجه;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#تعریف_کن U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجه, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        إذا (U_FAILURE(lexer.lookahead().error_کد())) {                                            \
            (ec) = lexer.lookahead().error_کد();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_مجموعه;                                                                \
        }                                                                                          \
        أرجع نتیجه;                                                                             \
    } طالما (خطأ)

#پایان_اگر

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

فراغ Unicode_مجموعه::apply_الگو(ثابت UnicodeString &الگو,
                              ثابت ParsePosition &parsePosition,
                              قاعده_Character_Iterator &chars,
                              ثابت نماد_جدول *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_مجموعه &(Unicode_مجموعه::*case_بستگی)(int32_t attribute),
                              UError_کد &ec) {
    إذا (U_FAILURE(ec)) أرجع;
    Lexer lexer(الگو, parsePosition, chars, options, symbols, case_بستگی);
    *هذا = parse_Unicode_مجموعه(lexer, rebuiltPat, options, case_بستگی, /*depth=*/0, ec);
}

Unicode_مجموعه Unicode_مجموعه::parse_Unicode_مجموعه(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_مجموعه& (Unicode_مجموعه::*case_بستگی)(int32_t attribute),
                                       int32_t عمق,
                                       UError_کد &ec) {
    Unicode_مجموعه نتیجه;

    إذا (عمق > MAX_عمق) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_عمق)).c_str(),
                                             ("depth = " + std::to_string(عمق)).c_str(), lexer,
                                             ec);
    }

    منطقي isComplement = خطأ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقي preserve_Syntax_درون_الگو = خطأ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_الگو;
    إذا (lexer.lookahead().مجموعه() != اشاره_تهی) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        نتیجه = *lexer.lookahead().مجموعه();
        نتیجه.مقصد_الگو_2(pretty_Printed_الگو, /*escapeUnprintable=*/خطأ);
        lexer.advance();
        preserve_Syntax_درون_الگو = درست;
    } وإلا {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        إذا (lexer.accept_مجموعه_Operator(u'[')) {
            pretty_Printed_الگو.append(u'[');
            إذا (lexer.accept_مجموعه_Operator(u'^')) {
                pretty_Printed_الگو.append(u'^');
                isComplement = درست;
            }
            نتیجه = parseContent(lexer, pretty_Printed_الگو, options, case_بستگی, عمق,
                                  /*containsSetOperation=*/preserve_Syntax_درون_الگو, ec);
            U_UNICODESET_RETURN_IF_ERROR(نتیجه, ec);
            إذا (!lexer.accept_مجموعه_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجه, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_الگو.append(u']');
        } وإلا {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتیجه, "property-query | [",
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
        (نتیجه.*case_بستگی)(options);
    }
    إذا (isComplement) {
        نتیجه.complement().remove_All_رشتهها(); // code point complement
    }
    إذا (preserve_Syntax_درون_الگو) {
        rebuiltPat.append(pretty_Printed_الگو);
    } وإلا {
        نتیجه.generate_الگو(rebuiltPat, /*escapeUnprintable=*/خطأ);
    }
    أرجع نتیجه;
}

Unicode_مجموعه Unicode_مجموعه::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_مجموعه &(Unicode_مجموعه::*case_بستگی)(int32_t attribute),
                                    int32_t عمق,
                                    منطقي &شامل_است_مجموعه_Operation,
                                    UError_کد &ec) {
    Unicode_مجموعه نتیجه;
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
    إذا (lexer.accept_مجموعه_Operator(u'-')) {
        نتیجه.جمع_کن(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    طالما (!lexer.at_انتها()) {
        إذا (lexer.accept_مجموعه_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            نتیجه.جمع_کن(u'-');
            أرجع نتیجه;
        } وإلا إذا (lexer.lookahead().is_مجموعه_Operator(u'$')) {
            إذا (lexer.lookahead2().is_مجموعه_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                نتیجه.جمع_کن(U_ETHER);
                شامل_است_مجموعه_Operation = درست;
                أرجع نتیجه;
            }
        }
        إذا (lexer.lookahead().is_مجموعه_Operator(u']')) {
            أرجع نتیجه;
        }
        // Also handles FinalMutation.
        نتیجه.parseMutation(lexer, rebuiltPat, options, case_بستگی, عمق, شامل_است_مجموعه_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(نتیجه, ec);
    }
    أرجع نتیجه;
}

فراغ Unicode_مجموعه::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_مجموعه &(Unicode_مجموعه::*case_بستگی)(int32_t attribute),
                               int32_t عمق,
                               منطقي &شامل_است_مجموعه_Operation,
                               UError_کد &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    إذا (lexer.lookahead().is_مجموعه_Operator('[') || lexer.lookahead().مجموعه() != اشاره_تهی) {
        شامل_است_مجموعه_Operation = درست;
        parse_مجموعه_Operations(lexer, rebuiltPat, options, case_بستگی, عمق, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } وإلا {
        // Also handles DollarElements.
        جمع_کن_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

فراغ Unicode_مجموعه::parse_مجموعه_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_مجموعه &(Unicode_مجموعه::*case_بستگی)(int32_t attribute),
                                    int32_t عمق,
                                    UError_کد &ec) {
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
    ثابت Unicode_مجموعه چپ_Hand_Side =
        parse_Unicode_مجموعه(lexer, rebuiltPat, options, case_بستگی, عمق + 1, ec);
    جمع_کن_All(چپ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    لكل (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        إذا (lexer.accept_مجموعه_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت Unicode_مجموعه راست_Hand_Side =
                parse_Unicode_مجموعه(lexer, rebuiltPat, options, case_بستگی, عمق + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(راست_Hand_Side);
        } وإلا إذا (lexer.lookahead().is_مجموعه_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            إذا (lexer.lookahead2().is_مجموعه_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                أرجع;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت Unicode_مجموعه راست_Hand_Side =
                parse_Unicode_مجموعه(lexer, rebuiltPat, options, case_بستگی, عمق + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(راست_Hand_Side);
        } وإلا {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            أرجع;
        }
    }
}

Unicode_مجموعه Unicode_مجموعه::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_کد &ec) {
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
        append_مقصد_Pat(rebuiltPat, element, /*escapeUnprintable=*/خطأ);
        rebuiltPat.append(u'}');
        lexer.advance();
        أرجع Unicode_مجموعه().جمع_کن(element);
    }
    UChar32 first;
    إذا (lexer.lookahead().is_مجموعه_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } وإلا إذا (lexer.lookahead().کد_نقطه().has_value()) {
        first = *lexer.lookahead().کد_نقطه();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_مقصد_Pat(rebuiltPat, first, /*escapeUnprintable=*/خطأ);
    إذا (!lexer.lookahead().is_مجموعه_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        أرجع Unicode_مجموعه(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    إذا (lexer.lookahead2().is_مجموعه_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        أرجع Unicode_مجموعه(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 آخرین;
    إذا (lexer.lookahead().is_مجموعه_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        آخرین = u'$';
        إذا (lexer.lookahead2().is_مجموعه_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } وإلا إذا (lexer.lookahead().کد_نقطه().has_value()) {
        آخرین = *lexer.lookahead().کد_نقطه();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    إذا (آخرین <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(آخرین) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_مقصد_Pat(rebuiltPat, آخرین, /*escapeUnprintable=*/خطأ);
    أرجع Unicode_مجموعه(first, آخرین);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

نطاق {

UBool numeric_مقدار_Filter(UChar32 ch, فراغ* context) {
    أرجع u_get_Numeric_مقدار(ch) == *static_cast<مزدوج*>(context);
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
    أرجع u_get_Int_Property_مقدار(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, فراغ* context) {
    أرجع uscript_hasScript(ch, *static_cast<UScript_کد*>(context));
}

UBool idTypeFilter(UChar32 ch, فراغ* context) {
    أرجع u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
فراغ Unicode_مجموعه::applyFilter(Unicode_مجموعه::Filter filter,
                             فراغ* context,
                             ثابت Unicode_مجموعه* inclusions,
                             UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) أرجع;

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
    int32_t حد_Range = inclusions->get_Range_شمار();

    لكل (صحيح j=0; j<حد_Range; ++j) {
        // get current range
        UChar32 شروع = inclusions->get_Range_شروع(j);
        UChar32 انتها = inclusions->get_Range_انتها(j);

        // for all the code points in the range, process
        لكل (UChar32 ch = شروع; ch <= انتها; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            إذا ((*filter)(ch, context)) {
                إذا (شروع_Has_Property < 0) {
                    شروع_Has_Property = ch;
                }
            } وإلا إذا (شروع_Has_Property >= 0) {
                جمع_کن(شروع_Has_Property, ch-1);
                شروع_Has_Property = -1;
            }
        }
    }
    إذا (شروع_Has_Property >= 0) {
        جمع_کن(شروع_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    إذا (isBogus() && U_SUCCESS(وضعیت)) {
        // We likely ran out of memory. AHHH!
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
}

نطاق {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#تعریف_کن FAIL(ec) UPRV_بلوک_MACRO_BEGIN { \
    ec=U_ILLEGAL_آرگومان_ERROR; \
    أرجع *هذا; \
} UPRV_بلوک_MACRO_انتها

Unicode_مجموعه&
Unicode_مجموعه::apply_Int_Property_مقدار(UProperty prop, int32_t value, UError_کد& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) { أرجع *هذا; }
    إذا (prop == UCHAR_GENERAL_CATEGORY_ماسک) {
        ثابت Unicode_مجموعه* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_ماسک_Filter, &value, inclusions, ec);
    } وإلا إذا (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت Unicode_مجموعه* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_کد script = static_cast<UScript_کد>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } وإلا إذا (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت Unicode_مجموعه* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } وإلا إذا (0 <= prop && prop < UCHAR_دودویی_حد) {
        إذا (value == 0 || value == 1) {
            ثابت USet *مجموعه = u_get_دودویی_Property_مجموعه(prop, &ec);
            إذا (U_FAILURE(ec)) { أرجع *هذا; }
            copyFrom(*Unicode_مجموعه::fromUSet(مجموعه), درست);
            إذا (value == 0) {
                complement().remove_All_رشتهها();  // code point complement
            }
        } وإلا {
            clear();
        }
    } وإلا إذا (UCHAR_INT_شروع <= prop && prop < UCHAR_INT_حد) {
        ثابت Unicode_مجموعه* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } وإلا {
        ec = U_ILLEGAL_آرگومان_ERROR;
    }
    أرجع *هذا;
}

Unicode_مجموعه&
Unicode_مجموعه::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UError_کد& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) أرجع *هذا;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    إذا( !uprv_isInvariantUString(prop.get_میانگیر(), prop.length()) ||
        !uprv_isInvariantUString(value.get_میانگیر(), value.length())
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
        p = u_getPropertyEnum(pname.داده());
        إذا (p == UCHAR_INVALID_کد) FAIL(ec);

        // Treat gc as gcm
        إذا (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_ماسک;
        }

        إذا ((p >= UCHAR_دودویی_شروع && p < UCHAR_دودویی_حد) ||
            (p >= UCHAR_INT_شروع && p < UCHAR_INT_حد) ||
            (p >= UCHAR_ماسک_شروع && p < UCHAR_ماسک_حد)) {
            v = u_get_Property_مقدار_Enum(p, vname.داده());
            إذا (v == UCHAR_INVALID_کد) {
                // Handle numeric CCC
                إذا (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* انتها;
                    مزدوج val = uprv_strtod(vname.داده(), &انتها);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    إذا (*انتها != 0 || !(0 <= val && val <= 255) ||
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
            حالة UCHAR_NUMERIC_مقدار:
                {
                    حرف* انتها;
                    مزدوج val = uprv_strtod(vname.داده(), &انتها);
                    إذا (*انتها != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_مقدار_Filter, &val,
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
                    جمع_کن(ch);
                    أرجع *هذا;
                }
            حالة UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالة UCHAR_AGE:
                {
                    UVersionInfo نسخه;
                    u_نسخه_From_String(نسخه, vname.داده());
                    applyFilter(نسخه_Filter, &نسخه,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    أرجع *هذا;
                }
            حالة UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_مقدار_Enum(UCHAR_SCRIPT, vname.داده());
                إذا (v == UCHAR_INVALID_کد) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            حالة UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_مقدار_Enum(p, vname.داده());
                إذا (v == UCHAR_INVALID_کد) {
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
        v = u_get_Property_مقدار_Enum(p, pname.داده());
        إذا (v == UCHAR_INVALID_کد) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_مقدار_Enum(p, pname.داده());
            إذا (v == UCHAR_INVALID_کد) {
                p = u_getPropertyEnum(pname.داده());
                إذا (p >= UCHAR_دودویی_شروع && p < UCHAR_دودویی_حد) {
                    v = 1;
                } وإلا إذا (0 == uprv_compare_Property_نامها(ANY, pname.داده())) {
                    مجموعه(MIN_مقدار, MAX_مقدار);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نامها(ASCII, pname.داده())) {
                    مجموعه(0, 0x7F);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نامها(ASSIGNED, pname.داده())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_ماسک;
                    v = U_GC_CN_ماسک;
                    invert = درست;
                } وإلا {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_مقدار(p, v, ec);
    إذا(invert) {
        complement().remove_All_رشتهها();  // code point complement
    }

    إذا (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_حافظه_تخصیص_ERROR;
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
UBool Unicode_مجموعه::resembles_Property_الگو(ثابت UnicodeString& الگو,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    إذا ((pos+5) > الگو.length()) {
        أرجع خطأ;
    }

    // Look for an opening [:, [:^, \p, or \P
    أرجع isPOSIXOpen(الگو, pos) || isPerlOpen(الگو, pos) || isNameOpen(الگو, pos);
}

U_NAMESPACE_انتها
