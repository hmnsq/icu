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

ساكن Unicode_سيٽ *uni32Singleton;
ساكن icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
ساكن UBool U_CALLCONV uset_cleanup() {
    ميٽيو uni32Singleton;
    uni32Singleton = خالي_اشارو;
    uni32InitOnce.reset();
    أرجع سچ;
}

U_CDECL_پڇاڙي

U_NAMESPACE_BEGIN

استعمال U_سرو_ONLY_NAMESPACE::utf_String_ڪوڊ_Points;

نطاق {

// Cache some sets for other services -------------------------------------- ***
فراغ U_CALLCONV create_Uni32_سيٽ(UError_ڪوڊ &error_ڪوڊ) {
    U_ASSERT(uni32Singleton == خالي_اشارو);
    uni32Singleton = جديد Unicode_سيٽ(UnicodeString(u"[:age=3.2:]"), error_ڪوڊ);
    إذا(uni32Singleton==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    } وإلا {
        uni32Singleton->freeze();
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_سيٽ *
uniset_getUnicode32Instance(UError_ڪوڊ &error_ڪوڊ) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_سيٽ, error_ڪوڊ);
    أرجع uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

مضمن UBool
isPerlOpen(ثابت UnicodeString &نمونو, int32_t pos) {
    char16_t c;
    أرجع نمونو.charAt(pos)==u'\\' && ((c=نمونو.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

مضمن UBool
isNameOpen(ثابت UnicodeString &نمونو, int32_t pos) {
    أرجع نمونو.charAt(pos)==u'\\' && نمونو.charAt(pos+1)==u'N';
}

مضمن UBool
isPOSIXOpen(ثابت UnicodeString &نمونو, int32_t pos) {
    أرجع نمونو.charAt(pos)==u'[' && نمونو.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#وصف_ڪريو _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ثابت CharString& name) {
    لكل (ثابت UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_ڪوڊ ec = U_ZERO_ERROR;
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
Unicode_سيٽ::Unicode_سيٽ(ثابت UnicodeString& نمونو,
                       UError_ڪوڊ& حالت) {
    apply_نمونو(نمونو, حالت);
    _dbgct(هذا);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_سيٽ& Unicode_سيٽ::apply_نمونو(ثابت UnicodeString& نمونو,
                                     UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_نمونو_Ignore_Space(نمونو, pos, خالي_اشارو, حالت);
    إذا (U_FAILURE(حالت)) أرجع *هذا;

    int32_t i = pos.get_اشاريو();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(نمونو, i, سچ);
    إذا (i != نمونو.length()) {
        حالت = U_ILLEGAL_دليل_ERROR;
    }
    أرجع *هذا;
}

فراغ
Unicode_سيٽ::apply_نمونو_Ignore_Space(ثابت UnicodeString& نمونو,
                                    ParsePosition& pos,
                                    ثابت نشان_جدول* symbols,
                                    UError_ڪوڊ& حالت) {
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
    قاعدو_Character_Iterator chars(نمونو, symbols, pos);
    apply_نمونو(نمونو, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, خالي_اشارو, حالت);
    إذا (U_FAILURE(حالت)) أرجع;
    إذا (chars.اندريون_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        حالت = U_MALFORMED_سيٽ;
        أرجع;
    }
    سيٽ_نمونو(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_سيٽ::resembles_نمونو(ثابت UnicodeString& نمونو, int32_t pos) {
    أرجع ((pos+1) < نمونو.length() &&
            نمونو.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_نمونو(نمونو, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#وصف_ڪريو U_DEBUGGING_UNICODESET_PARSING 0

صنف Unicode_سيٽ::Lexer {
  عام:
    Lexer(ثابت UnicodeString &نمونو,
          ثابت ParsePosition &parsePosition,
          قاعدو_Character_Iterator &chars,
          uint32_t unicode_سيٽ_Options,
          ثابت نشان_جدول *ثابت symbols,
          Unicode_سيٽ &(Unicode_سيٽ::*case_گهيرو)(int32_t attribute))
        : نمونو_2(نمونو), parsePosition_(parsePosition), chars_(chars),
          unicode_سيٽ_Options_2(unicode_سيٽ_Options),
          charsOptions_(قاعدو_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_سيٽ_Options & USET_IGNORE_SPACE) != 0
                             ? قاعدو_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_گهيرو_2(case_گهيرو) {}

    صنف LexicalElement {
      عام:
        منطقي is_سيٽ_Operator(ثابت char16_t op) ثابت {
            أرجع U_SUCCESS(error_ڪوڊ_2) && category_ == سيٽ_OPERATOR && string_[0] == op;
        }

        منطقي isStringLiteral() ثابت {
            أرجع U_SUCCESS(error_ڪوڊ_2) && category_ == STRING_LITERAL;
        }

        منطقي isNamedElement() ثابت {
            أرجع U_SUCCESS(error_ڪوڊ_2) && category_ == NAMED_ELEMENT;
        }

        منطقي isBracketedElement() ثابت {
            أرجع U_SUCCESS(error_ڪوڊ_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ثابت {
            إذا (U_SUCCESS(error_ڪوڊ_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                أرجع string_;
            }
            أرجع std::nullopt;
        }

        std::optional<UChar32> ڪوڊ_نقطو() ثابت {
            إذا (U_SUCCESS(error_ڪوڊ_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                أرجع string_.char32At(0);
            }
            أرجع std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ثابت Unicode_سيٽ *سيٽ() ثابت {
            إذا (U_FAILURE(error_ڪوڊ_2)) {
                أرجع خالي_اشارو;
            }
            إذا (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                إذا (precomputed_سيٽ != خالي_اشارو) {
                    أرجع precomputed_سيٽ;
                } وإلا {
                    أرجع &سيٽ_2;
                }
            }
            أرجع خالي_اشارو;
        }

        ثابت UError_ڪوڊ& error_ڪوڊ() ثابت{
          أرجع error_ڪوڊ_2;
        }

#إذا U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ثابت {
            UnicodeString نتيجو;
            إذا (U_FAILURE(error_ڪوڊ_2)) {
                نتيجو.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_ڪوڊ_2)))
                    .append(u"), possibly ");
            }
            أرجع نتيجو.append(category_نالا[category_])
                .append(u" '")
                .append(ذريعو_متن)
                .append(u"'");
        }
#جيڪڏهن_پورو

      خاص:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        تعداد Category : std::uint8_t {
            سيٽ_OPERATOR,
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
            پڇاڙي_جو_متن,
        };
        ساكن constexpr std::array<std::u16string_view, پڇاڙي_جو_متن + 1> category_نالا{{
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
        LexicalElement(Category category, UnicodeString string, قاعدو_Character_Iterator::Pos after,
                       ثابت UError_ڪوڊ error_ڪوڊ, ثابت Unicode_سيٽ *precomputed_سيٽ_2, Unicode_سيٽ سيٽ,
                       std::u16string_view ذريعو_متن_2, منطقي separated,
                       std::optional<LexicalElement> ثابت &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_ڪوڊ_2(error_ڪوڊ),
              precomputed_سيٽ(precomputed_سيٽ_2), سيٽ_2(سيٽ), ذريعو_متن(ذريعو_متن_2) {
            إذا (U_FAILURE(error_ڪوڊ_2) || separated || !preceding.has_value()) {
                أرجع;
            }
            إذا (preceding->is_سيٽ_Operator(u'[') && category == LITERAL_ELEMENT &&
                *ڪوڊ_نقطو() == u':') {
                error_ڪوڊ_2 = U_MALFORMED_سيٽ;
            } وإلا إذا (preceding->category_ == ESCAPED_ELEMENT) {
                إذا (U_IS_LEAD(*preceding->ڪوڊ_نقطو()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*ڪوڊ_نقطو())) {
                    error_ڪوڊ_2 = U_MALFORMED_سيٽ;
                } وإلا إذا (preceding->ذريعو_متن.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *ڪوڊ_نقطو();
                    إذا (preceding->ذريعو_متن[1] == u'x') {
                        إذا ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_ڪوڊ_2 = U_MALFORMED_سيٽ;
                        }
                    } وإلا إذا (preceding->ذريعو_متن[1] >= u'0' &&
                               preceding->ذريعو_متن[1] <= u'7') {
                        إذا (cp >= u'0' && cp <= u'7') {
                            error_ڪوڊ_2 = U_MALFORMED_سيٽ;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        قاعدو_Character_Iterator::Pos after_;
        UError_ڪوڊ error_ڪوڊ_2;
        ثابت Unicode_سيٽ *precomputed_سيٽ;
        Unicode_سيٽ سيٽ_2;
        std::u16string_view ذريعو_متن;

        دوست صنف Lexer;
    };

    UnicodeString getPositionForDebugging() ثابت {
        أرجع نمونو_2.عارضي_Sub_String(0, parsePosition_.get_اشاريو()) + u"☞" +
               نمونو_2.عارضي_Sub_String(parsePosition_.get_اشاريو(), 60);
    }

    منطقي accept_سيٽ_Operator(char16_t op) {
        إذا (lookahead().is_سيٽ_Operator(op)) {
            advance();
            أرجع سچ;
        }
        أرجع خطأ;
    }

    ثابت LexicalElement &lookahead() {
        إذا (!ahead_.has_value()) {
            ثابت قاعدو_Character_Iterator::Pos before = getPos();
            ahead_.emplace(ايندڙ_Token(behind_));
            chars_.سيٽ_Pos(before);
        }
        أرجع *ahead_;
    }

    ثابت LexicalElement &lookahead2() {
        إذا (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ثابت قاعدو_Character_Iterator::Pos before = getPos();
            chars_.سيٽ_Pos(lookahead().after_);
            ahead2_.emplace(ايندڙ_Token(ahead_));
            chars_.سيٽ_Pos(before);
        }
        أرجع *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    قاعدو_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        أرجع chars_;
    }

    int32_t charsOptions() {
        أرجع charsOptions_;
    }

    منطقي at_پڇاڙي() ثابت {
        أرجع chars_.at_پڇاڙي();
    }

    فراغ advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.سيٽ_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  خاص:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    قاعدو_Character_Iterator::Pos getPos() ثابت {
        قاعدو_Character_Iterator::Pos نتيجو;
        chars_.getPos(نتيجو);
        أرجع نتيجو;
    }

    LexicalElement ايندڙ_Token(std::optional<LexicalElement> ثابت& preceding) {
        UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
        منطقي separated = خطأ;
        إذا (charsOptions_ & قاعدو_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            قاعدو_Character_Iterator::Pos pos;
            لكل (;;) {
                UBool اڻ_استعمال_ٿيل_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                  قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
                إذا (!نمونو_Props::isWhiteSpace(s)) {
                    chars_.سيٽ_Pos(pos);
                    اكسر;
                }
                إذا (s != u'\u200E' && s != u'\u200F') {
                    separated = سچ;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        إذا (chars_.at_پڇاڙي()) {
            أرجع LexicalElement(LexicalElement::پڇاڙي_جو_متن, {}, getPos(), error_ڪوڊ,
                                  /*precomputedSet=*/خالي_اشارو,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ثابت int32_t شروعات = parsePosition_.get_اشاريو();
        ثابت قاعدو_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool اڻ_استعمال_ٿيل_Escaped;
        ثابت UChar32 first =
            chars_.next(charsOptions_ & ~قاعدو_Character_Iterator::PARSE_ESCAPES, اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
        إذا (first == u'[' || first == u'\\') {
            ثابت قاعدو_Character_Iterator::Pos after_پهريون = getPos();
            // This could be a property-query or named-element.
            ثابت UChar32 ٻيو = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                                 قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                               اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
            إذا ((first == u'[' && ٻيو == u':') ||
                (first == u'\\' && (ٻيو == u'p' || ٻيو == u'P' || ٻيو == u'N'))) {
                إذا (ٻيو == u'N') {
                    UChar32 ثابت query_نتيجو = جاچ_Named_Element_Brackets(error_ڪوڊ);
                    أرجع LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_نتيجو), getPos(), error_ڪوڊ,
                        /*precomputedSet=*/خالي_اشارو,
                        /*set=*/{},
                        std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                        separated, preceding);
                } وإلا {
                    Unicode_سيٽ query_نتيجو = جاچ_Property_Query_After_شروعات(first, ٻيو, شروعات, error_ڪوڊ);
                    أرجع LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_ڪوڊ,
                        /*precomputedSet=*/خالي_اشارو, /*set=*/std::move(query_نتيجو),
                        std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.سيٽ_Pos(after_پهريون);
        }
        إذا (first == u'$' && symbols_ != خالي_اشارو) {
            خودڪار name_پڇاڙي = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            إذا (UnicodeString name = symbols_->parseReference(نمونو_2, name_پڇاڙي, نمونو_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_خالي_جايون()) {
                chars_.jumpahead(name_پڇاڙي.get_اشاريو() - (شروعات + 1));
                ثابت std::u16string_view ذريعو =
                    std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات);
                ثابت Unicode_سيٽ *precomputed_سيٽ_2 = symbols_->lookup_سيٽ(name);
                إذا (precomputed_سيٽ_2 != خالي_اشارو) {
                    أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_سيٽ_2, /*set=*/{}, ذريعو, /*separated=*/سچ,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ثابت UnicodeString *ثابت expression = symbols_->lookup(name);
                إذا (expression == خالي_اشارو) {
                    أرجع LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/خالي_اشارو,
                        /*set=*/{},
                        ذريعو,
                        /*separated=*/سچ,
                        /*preceding=*/std::nullopt);
                }
                أرجع evaluateVariable(*expression, ذريعو);
            }
        }
        بدل (first) {
        حالة u'[':
            أرجع LexicalElement(
                LexicalElement::سيٽ_OPERATOR, UnicodeString(u'['), getPos(), error_ڪوڊ,
                /*precomputedSet=*/خالي_اشارو,
                /*set=*/{},
                std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                separated, preceding);
        حالة u'\\': {
            // Now try to parse the escape.
            chars_.سيٽ_Pos(before);
            UChar32 ڪوڊ_نقطو = chars_.next(charsOptions_, اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
            أرجع LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(ڪوڊ_نقطو), getPos(), error_ڪوڊ,
                خالي_اشارو,
                /*set=*/{},
                std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                separated, preceding);
        }
        حالة u'&':
        حالة u'-':
        حالة u']':
        حالة u'^':
        حالة u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            أرجع LexicalElement(
                LexicalElement::سيٽ_OPERATOR, UnicodeString(first), getPos(), error_ڪوڊ,
                /*precomputedSet=*/خالي_اشارو,
                /*set=*/{},
                std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                separated, preceding);
        حالة u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t ڪوڊ_نقطو_ڳڻپ = 0;
            طالما (!chars_.at_پڇاڙي() && U_SUCCESS(error_ڪوڊ)) {
                ثابت قاعدو_Character_Iterator::Pos before_ايندڙ = getPos();
                next = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                     قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                   اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
                إذا (next == u'\\') {
                    ثابت UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                      قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                    اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
                    إذا (afterBackslash == u'N') {
                        next = جاچ_Named_Element_Brackets(error_ڪوڊ);
                        escaped = سچ;
                    } وإلا إذا (afterBackslash == u'p' || afterBackslash == u'P') {
                        أرجع LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_سيٽ,
                                              /*precomputedSet=*/خالي_اشارو,
                                              /*set=*/{},
                                              std::u16string_view(نمونو_2).substr(
                                                  شروعات, parsePosition_.get_اشاريو() - شروعات),
                                              separated, preceding);
                    } وإلا {
                        chars_.سيٽ_Pos(before_ايندڙ);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_ڪوڊ);
                    }
                } وإلا {
#إذا U_ICU_نسخو_MAJOR_انگ < 81
                    إذا (U_SUCCESS(error_ڪوڊ) && نمونو_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_ڪوڊ = UError_ڪوڊ::U_ILLEGAL_دليل_ERROR;
                    }
#وإلا
#غلطي Remove هذا transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#جيڪڏهن_پورو
                    escaped = خطأ;
                }
                إذا (!escaped && next == u'}') {
                    أرجع LexicalElement(
                        ڪوڊ_نقطو_ڳڻپ == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_ڪوڊ,
                        /*precomputedSet=*/خالي_اشارو,
                        /*set=*/{},
                        std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                        separated, preceding);
                }
                string.append(next);
                ڪوڊ_نقطو_ڳڻپ += 1;
            }
            أرجع LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_سيٽ,
                /*precomputedSet=*/خالي_اشارو,
                /*set=*/{},
                std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                separated, preceding);
        }
        افتراضي:
            أرجع LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_ڪوڊ, خالي_اشارو,
                /*set=*/{},
                std::u16string_view(نمونو_2).substr(شروعات, parsePosition_.get_اشاريو() - شروعات),
                separated, preceding);
        }
    }

    UChar32 جاچ_Named_Element_Brackets(UError_ڪوڊ &error_ڪوڊ) {
        UBool اڻ_استعمال_ٿيل_Escaped;
        ثابت UChar32 open = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                           قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                         اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
        إذا (open == u'{') {
            int32_t شروعات = parsePosition_.get_اشاريو();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            طالما (!chars_.at_پڇاڙي() && U_SUCCESS(error_ڪوڊ)) {
                UChar32 آخري = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                             قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                           اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
                إذا (آخري == u':') {
                    إذا (!hex.has_value()) {
                        hex.emplace();
                        إذا (شروعات == parsePosition_.get_اشاريو() - 1) {
                            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                            أرجع {};
                        }
                        لكل (char16_t digit : std::u16string_view(نمونو_2).substr(
                                 شروعات, parsePosition_.get_اشاريو() - 1 - شروعات)) {
                            uint8_t nibble;
                            إذا (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } وإلا {
                                digit = digit & ~0x20;
                                إذا (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } وإلا {
                                    error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                                    أرجع {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            إذا (hex > 0x10FFFF) {
                                error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                                أرجع {};
                            }
                        }
                    } وإلا إذا (!literal.has_value()) {
                        ثابت خودڪار literal_ڪوڊ_Points = utf_String_ڪوڊ_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(نمونو_2).substr(شروعات,
                                                                 parsePosition_.get_اشاريو() - 1 - شروعات));
                        خودڪار it = literal_ڪوڊ_Points.begin();
                        إذا (it == literal_ڪوڊ_Points.پڇاڙي() || !it->wellFormed() ||
                                (literal = it->ڪوڊ_نقطو(), ++it) != literal_ڪوڊ_Points.پڇاڙي()) {
                            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                            أرجع {};
                        }
                    } وإلا {
                        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                        أرجع {};
                    }
                    شروعات = parsePosition_.get_اشاريو();
                } وإلا إذا (آخري == u'}') {
                    ثابت std::u16string_view u16name = std::u16string_view(نمونو_2).substr(
                        شروعات, parsePosition_.get_اشاريو() - 1 - شروعات);
                    ثابت UChar32 نتيجو = getCharacterByName(CharString().appendInvariantChars(
                        u16name.معلومات(), static_cast<int32_t>(u16name.length()), error_ڪوڊ));
                    إذا (!U_SUCCESS(error_ڪوڊ)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                        أرجع {};
                    }
                    إذا (نتيجو < 0 || (hex.has_value() && نتيجو != hex) ||
                            (literal.has_value() && نتيجو != literal)) {
                        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                        أرجع {};
                    }
                    أرجع نتيجو;
                }
            }
        }
        إذا (U_SUCCESS(error_ڪوڊ)) {
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        }
        أرجع {};
    }

    LexicalElement evaluateVariable(ثابت UnicodeString &expression,
                                    ثابت std::u16string_view ذريعو) {
        UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
        ParsePosition اظهار_Position;
        قاعدو_Character_Iterator اظهار_Iterator(expression, symbols_, اظهار_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer اظهار_Lexer(expression, اظهار_Position, اظهار_Iterator, unicode_سيٽ_Options_2,
                              /*symbols=*/خالي_اشارو, case_گهيرو_2);
        خودڪار variableToken = اظهار_Lexer.lookahead();
        إذا (variableToken.is_سيٽ_Operator(u'[')) {
            UnicodeString rebuilt_نمونو;
            Unicode_سيٽ اظهار_قدر =
                parse_Unicode_سيٽ(اظهار_Lexer, rebuilt_نمونو, unicode_سيٽ_Options_2, case_گهيرو_2,
                                /*depth=*/0, error_ڪوڊ);
            اظهار_قدر.سيٽ_نمونو(rebuilt_نمونو);
            إذا (!اظهار_Lexer.at_پڇاڙي()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالي_اشارو,
                    /*set=*/{},
                    ذريعو,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            }
            أرجع LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_ڪوڊ,
                /*precomputedSet=*/خالي_اشارو,
                /*set=*/std::move(اظهار_قدر),
                ذريعو,
                /*separated=*/سچ,
                /*preceding=*/std::nullopt);
        } وإلا {
            اظهار_Lexer.advance();
            إذا (!اظهار_Lexer.at_پڇاڙي()) {
                أرجع LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/خالي_اشارو,
                    /*set=*/{},
                    ذريعو,
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
                    variableToken.error_ڪوڊ_2, variableToken.precomputed_سيٽ, std::move(variableToken.سيٽ_2), ذريعو,
                    /*separated=*/سچ,
                    /*preceding=*/std::nullopt);
            افتراضي:
                أرجع LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/خالي_اشارو,
                                      /*set=*/{}, ذريعو,
                                      /*separated=*/سچ,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_سيٽ جاچ_Property_Query_After_شروعات(UChar32 first, UChar32 ٻيو, int32_t query_شروعات, UError_ڪوڊ &error_ڪوڊ) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_اظهار_شروعات = parsePosition_.get_اشاريو();
        منطقي exteriorlyNegated = خطأ;
        منطقي interiorlyNegated = خطأ;
        UBool اڻ_استعمال_ٿيل_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ثابت UChar32 ٽيون = chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                                            قاعدو_Character_Iterator::SKIP_WHITESPACE),
                                          اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
        إذا (first == u'\\') {
            إذا (ٽيون != u'{') {
                error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                أرجع {};
            }
            exteriorlyNegated = ٻيو == u'P';
            query_اظهار_شروعات = parsePosition_.get_اشاريو();
        } وإلا {
            إذا (ٽيون == u'^') {
                exteriorlyNegated = سچ;
                query_اظهار_شروعات = parsePosition_.get_اشاريو();
            }
        }
        قاعدو_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                          قاعدو_Character_Iterator::SKIP_WHITESPACE),
                        اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);

        طالما (!chars_.at_پڇاڙي() && U_SUCCESS(error_ڪوڊ)) {
            ثابت قاعدو_Character_Iterator::Pos before_آخري = getPos();
            UChar32 آخري_Unescaped =
                chars_.next(charsOptions_ & ~(قاعدو_Character_Iterator::PARSE_ESCAPES |
                                              قاعدو_Character_Iterator::SKIP_WHITESPACE),
                            اڻ_استعمال_ٿيل_Escaped, error_ڪوڊ);
            إذا (penultimateUnescaped == u'\\') {
                إذا (آخري_Unescaped == 'N') {
                    جاچ_Named_Element_Brackets(error_ڪوڊ);
                    إذا (!U_SUCCESS(error_ڪوڊ)) {
                        أرجع {};
                    }
                } وإلا {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.سيٽ_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~قاعدو_Character_Iterator::SKIP_WHITESPACE, اڻ_استعمال_ٿيل_Escaped,
                                error_ڪوڊ);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                آخري_Unescaped = -1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخري_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_اشاريو() - 1;
            } وإلا إذا (!queryOperatorPosition.has_value() && آخري_Unescaped == u'≠') {
                إذا (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                    أرجع {};
                }
                interiorlyNegated = سچ;
                queryOperatorPosition = parsePosition_.get_اشاريو() - 1;
            } وإلا إذا ((first == u'[' && penultimateUnescaped == u':' && آخري_Unescaped == u']') ||
                       (first == u'\\' && آخري_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ثابت int32_t query_اظهار_حد_بندي =
                    first == u'[' ? parsePosition_.get_اشاريو() - 2 : parsePosition_.get_اشاريو() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                إذا (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        نمونو_2.عارضي_Sub_String_Between(*queryOperatorPosition + 1, query_اظهار_حد_بندي);
                    إذا (propertyPredicate.is_خالي_جايون()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                        أرجع {};
                    }
                }
                Unicode_سيٽ نتيجو;
                نتيجو.applyPropertyAlias(
                    نمونو_2.عارضي_Sub_String_Between(query_اظهار_شروعات,
                                                  queryOperatorPosition.value_or(query_اظهار_حد_بندي)),
                    propertyPredicate, error_ڪوڊ);
                إذا (exteriorlyNegated != interiorlyNegated) {
                    نتيجو.complement().remove_All_لڙيون();
                }
                نتيجو.سيٽ_نمونو(نمونو_2.عارضي_Sub_String_Between(query_شروعات, parsePosition_.get_اشاريو()));
                أرجع نتيجو;
            }
            beforePenultimate = before_آخري;
            penultimateUnescaped = آخري_Unescaped;
        }
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع {};
    }

    ثابت UnicodeString &نمونو_2;
    ثابت ParsePosition &parsePosition_;
    قاعدو_Character_Iterator &chars_;
    ثابت uint32_t unicode_سيٽ_Options_2;
    ثابت int32_t charsOptions_;
    ثابت نشان_جدول *ثابت symbols_;
    Unicode_سيٽ &(Unicode_سيٽ::* ثابت case_گهيرو_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

نطاق {

constexpr int32_t MAX_اونهائي = 100;

#إذا U_DEBUGGING_UNICODESET_PARSING

#وصف_ڪريو U_UNICODESET_RETURN_IF_ERROR(نتيجو, ec)                                                   \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        إذا (U_FAILURE(ec)) {                                                                       \
            إذا (اونهائي < 5) {                                                                       \
                لکو("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } وإلا إذا (اونهائي == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                لکو("--- [...]\n");                                                             \
            }                                                                                      \
            أرجع نتيجو;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#وصف_ڪريو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتيجو, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        لکو("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).منزل_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().منزل_UTF8_String(contextUTF8).c_str());                 \
        لکو("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        إذا (U_FAILURE(lexer.lookahead().error_ڪوڊ())) {                                            \
            (ec) = lexer.lookahead().error_ڪوڊ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سيٽ;                                                                \
        }                                                                                          \
        أرجع نتيجو;                                                                             \
    } طالما (خطأ)

#وإلا

#وصف_ڪريو U_UNICODESET_RETURN_IF_ERROR(نتيجو, ec)                                                   \
    افعل {                                                                                           \
        إذا (U_FAILURE(ec)) {                                                                       \
            أرجع نتيجو;                                                                         \
        }                                                                                          \
    } طالما (خطأ)
#وصف_ڪريو U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتيجو, expected, actual, lexer, ec)                  \
    افعل {                                                                                           \
        إذا (U_FAILURE(lexer.lookahead().error_ڪوڊ())) {                                            \
            (ec) = lexer.lookahead().error_ڪوڊ();                                                  \
        } وإلا {                                                                                   \
            (ec) = U_MALFORMED_سيٽ;                                                                \
        }                                                                                          \
        أرجع نتيجو;                                                                             \
    } طالما (خطأ)

#جيڪڏهن_پورو

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

فراغ Unicode_سيٽ::apply_نمونو(ثابت UnicodeString &نمونو,
                              ثابت ParsePosition &parsePosition,
                              قاعدو_Character_Iterator &chars,
                              ثابت نشان_جدول *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_سيٽ &(Unicode_سيٽ::*case_گهيرو)(int32_t attribute),
                              UError_ڪوڊ &ec) {
    إذا (U_FAILURE(ec)) أرجع;
    Lexer lexer(نمونو, parsePosition, chars, options, symbols, case_گهيرو);
    *هذا = parse_Unicode_سيٽ(lexer, rebuiltPat, options, case_گهيرو, /*depth=*/0, ec);
}

Unicode_سيٽ Unicode_سيٽ::parse_Unicode_سيٽ(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_سيٽ& (Unicode_سيٽ::*case_گهيرو)(int32_t attribute),
                                       int32_t اونهائي,
                                       UError_ڪوڊ &ec) {
    Unicode_سيٽ نتيجو;

    إذا (اونهائي > MAX_اونهائي) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_اونهائي)).c_str(),
                                             ("depth = " + std::to_string(اونهائي)).c_str(), lexer,
                                             ec);
    }

    منطقي isComplement = خطأ;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    منطقي preserve_Syntax_اندريون_نمونو = خطأ;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_نمونو;
    إذا (lexer.lookahead().سيٽ() != خالي_اشارو) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        نتيجو = *lexer.lookahead().سيٽ();
        نتيجو.منزل_نمونو_2(pretty_Printed_نمونو, /*escapeUnprintable=*/خطأ);
        lexer.advance();
        preserve_Syntax_اندريون_نمونو = سچ;
    } وإلا {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        إذا (lexer.accept_سيٽ_Operator(u'[')) {
            pretty_Printed_نمونو.append(u'[');
            إذا (lexer.accept_سيٽ_Operator(u'^')) {
                pretty_Printed_نمونو.append(u'^');
                isComplement = سچ;
            }
            نتيجو = parseContent(lexer, pretty_Printed_نمونو, options, case_گهيرو, اونهائي,
                                  /*containsSetOperation=*/preserve_Syntax_اندريون_نمونو, ec);
            U_UNICODESET_RETURN_IF_ERROR(نتيجو, ec);
            إذا (!lexer.accept_سيٽ_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتيجو, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_نمونو.append(u']');
        } وإلا {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(نتيجو, "property-query | [",
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
        (نتيجو.*case_گهيرو)(options);
    }
    إذا (isComplement) {
        نتيجو.complement().remove_All_لڙيون(); // code point complement
    }
    إذا (preserve_Syntax_اندريون_نمونو) {
        rebuiltPat.append(pretty_Printed_نمونو);
    } وإلا {
        نتيجو.generate_نمونو(rebuiltPat, /*escapeUnprintable=*/خطأ);
    }
    أرجع نتيجو;
}

Unicode_سيٽ Unicode_سيٽ::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سيٽ &(Unicode_سيٽ::*case_گهيرو)(int32_t attribute),
                                    int32_t اونهائي,
                                    منطقي &شامل_آهي_سيٽ_Operation,
                                    UError_ڪوڊ &ec) {
    Unicode_سيٽ نتيجو;
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
    إذا (lexer.accept_سيٽ_Operator(u'-')) {
        نتيجو.جوڙيو(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    طالما (!lexer.at_پڇاڙي()) {
        إذا (lexer.accept_سيٽ_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            نتيجو.جوڙيو(u'-');
            أرجع نتيجو;
        } وإلا إذا (lexer.lookahead().is_سيٽ_Operator(u'$')) {
            إذا (lexer.lookahead2().is_سيٽ_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                نتيجو.جوڙيو(U_ETHER);
                شامل_آهي_سيٽ_Operation = سچ;
                أرجع نتيجو;
            }
        }
        إذا (lexer.lookahead().is_سيٽ_Operator(u']')) {
            أرجع نتيجو;
        }
        // Also handles FinalMutation.
        نتيجو.parseMutation(lexer, rebuiltPat, options, case_گهيرو, اونهائي, شامل_آهي_سيٽ_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(نتيجو, ec);
    }
    أرجع نتيجو;
}

فراغ Unicode_سيٽ::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_سيٽ &(Unicode_سيٽ::*case_گهيرو)(int32_t attribute),
                               int32_t اونهائي,
                               منطقي &شامل_آهي_سيٽ_Operation,
                               UError_ڪوڊ &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    إذا (lexer.lookahead().is_سيٽ_Operator('[') || lexer.lookahead().سيٽ() != خالي_اشارو) {
        شامل_آهي_سيٽ_Operation = سچ;
        parse_سيٽ_Operations(lexer, rebuiltPat, options, case_گهيرو, اونهائي, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } وإلا {
        // Also handles DollarElements.
        جوڙيو_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

فراغ Unicode_سيٽ::parse_سيٽ_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_سيٽ &(Unicode_سيٽ::*case_گهيرو)(int32_t attribute),
                                    int32_t اونهائي,
                                    UError_ڪوڊ &ec) {
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
    ثابت Unicode_سيٽ کاٻو_Hand_Side =
        parse_Unicode_سيٽ(lexer, rebuiltPat, options, case_گهيرو, اونهائي + 1, ec);
    جوڙيو_All(کاٻو_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    لكل (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        إذا (lexer.accept_سيٽ_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ثابت Unicode_سيٽ ساڄو_Hand_Side =
                parse_Unicode_سيٽ(lexer, rebuiltPat, options, case_گهيرو, اونهائي + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(ساڄو_Hand_Side);
        } وإلا إذا (lexer.lookahead().is_سيٽ_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            إذا (lexer.lookahead2().is_سيٽ_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                أرجع;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ثابت Unicode_سيٽ ساڄو_Hand_Side =
                parse_Unicode_سيٽ(lexer, rebuiltPat, options, case_گهيرو, اونهائي + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(ساڄو_Hand_Side);
        } وإلا {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            أرجع;
        }
    }
}

Unicode_سيٽ Unicode_سيٽ::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_ڪوڊ &ec) {
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
        أرجع Unicode_سيٽ().جوڙيو(element);
    }
    UChar32 first;
    إذا (lexer.lookahead().is_سيٽ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } وإلا إذا (lexer.lookahead().ڪوڊ_نقطو().has_value()) {
        first = *lexer.lookahead().ڪوڊ_نقطو();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, first, /*escapeUnprintable=*/خطأ);
    إذا (!lexer.lookahead().is_سيٽ_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        أرجع Unicode_سيٽ(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    إذا (lexer.lookahead2().is_سيٽ_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        أرجع Unicode_سيٽ(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 آخري;
    إذا (lexer.lookahead().is_سيٽ_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        آخري = u'$';
        إذا (lexer.lookahead2().is_سيٽ_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } وإلا إذا (lexer.lookahead().ڪوڊ_نقطو().has_value()) {
        آخري = *lexer.lookahead().ڪوڊ_نقطو();
    } وإلا {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    إذا (آخري <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(آخري) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_منزل_Pat(rebuiltPat, آخري, /*escapeUnprintable=*/خطأ);
    أرجع Unicode_سيٽ(first, آخري);
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

UBool نسخو_Filter(UChar32 ch, فراغ* context) {
    ساكن ثابت UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* نسخو = static_cast<UVersionInfo*>(context);
    أرجع uprv_memcmp(&v, &none, حجم(v)) > 0 && uprv_memcmp(&v, نسخو, حجم(v)) <= 0;
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
    أرجع uscript_hasScript(ch, *static_cast<UScript_ڪوڊ*>(context));
}

UBool idTypeFilter(UChar32 ch, فراغ* context) {
    أرجع u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
فراغ Unicode_سيٽ::applyFilter(Unicode_سيٽ::Filter filter,
                             فراغ* context,
                             ثابت Unicode_سيٽ* inclusions,
                             UError_ڪوڊ &حالت) {
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

    UChar32 شروعات_Has_Property = -1;
    int32_t حد_بندي_Range = inclusions->get_Range_ڳڻپ();

    لكل (صحيح j=0; j<حد_بندي_Range; ++j) {
        // get current range
        UChar32 شروعات = inclusions->get_Range_شروعات(j);
        UChar32 پڇاڙي = inclusions->get_Range_پڇاڙي(j);

        // for all the code points in the range, process
        لكل (UChar32 ch = شروعات; ch <= پڇاڙي; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            إذا ((*filter)(ch, context)) {
                إذا (شروعات_Has_Property < 0) {
                    شروعات_Has_Property = ch;
                }
            } وإلا إذا (شروعات_Has_Property >= 0) {
                جوڙيو(شروعات_Has_Property, ch-1);
                شروعات_Has_Property = -1;
            }
        }
    }
    إذا (شروعات_Has_Property >= 0) {
        جوڙيو(شروعات_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    إذا (isBogus() && U_SUCCESS(حالت)) {
        // We likely ran out of memory. AHHH!
        حالت = U_يادگيري_ورهاست_ERROR;
    }
}

نطاق {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#وصف_ڪريو FAIL(ec) UPRV_بلاڪ_MACRO_BEGIN { \
    ec=U_ILLEGAL_دليل_ERROR; \
    أرجع *هذا; \
} UPRV_بلاڪ_MACRO_پڇاڙي

Unicode_سيٽ&
Unicode_سيٽ::apply_Int_Property_قدر(UProperty prop, int32_t value, UError_ڪوڊ& ec) U_LIFETIME_BOUND {
    إذا (U_FAILURE(ec) || isFrozen()) { أرجع *هذا; }
    إذا (prop == UCHAR_GENERAL_CATEGORY_نقاب) {
        ثابت Unicode_سيٽ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_نقاب_Filter, &value, inclusions, ec);
    } وإلا إذا (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ثابت Unicode_سيٽ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_ڪوڊ script = static_cast<UScript_ڪوڊ>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } وإلا إذا (prop == UCHAR_IDENTIFIER_TYPE) {
        ثابت Unicode_سيٽ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } وإلا إذا (0 <= prop && prop < UCHAR_ٻٽو_حد_بندي) {
        إذا (value == 0 || value == 1) {
            ثابت USet *سيٽ = u_get_ٻٽو_Property_سيٽ(prop, &ec);
            إذا (U_FAILURE(ec)) { أرجع *هذا; }
            copyFrom(*Unicode_سيٽ::fromUSet(سيٽ), سچ);
            إذا (value == 0) {
                complement().remove_All_لڙيون();  // code point complement
            }
        } وإلا {
            clear();
        }
    } وإلا إذا (UCHAR_INT_شروعات <= prop && prop < UCHAR_INT_حد_بندي) {
        ثابت Unicode_سيٽ* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } وإلا {
        ec = U_ILLEGAL_دليل_ERROR;
    }
    أرجع *هذا;
}

Unicode_سيٽ&
Unicode_سيٽ::applyPropertyAlias(ثابت UnicodeString& prop,
                               ثابت UnicodeString& value,
                               UError_ڪوڊ& ec) U_LIFETIME_BOUND {
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
        إذا (p == UCHAR_INVALID_ڪوڊ) FAIL(ec);

        // Treat gc as gcm
        إذا (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_نقاب;
        }

        إذا ((p >= UCHAR_ٻٽو_شروعات && p < UCHAR_ٻٽو_حد_بندي) ||
            (p >= UCHAR_INT_شروعات && p < UCHAR_INT_حد_بندي) ||
            (p >= UCHAR_نقاب_شروعات && p < UCHAR_نقاب_حد_بندي)) {
            v = u_get_Property_قدر_Enum(p, vname.معلومات());
            إذا (v == UCHAR_INVALID_ڪوڊ) {
                // Handle numeric CCC
                إذا (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    حرف* پڇاڙي;
                    مزدوج val = uprv_strtod(vname.معلومات(), &پڇاڙي);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    إذا (*پڇاڙي != 0 || !(0 <= val && val <= 255) ||
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
                    حرف* پڇاڙي;
                    مزدوج val = uprv_strtod(vname.معلومات(), &پڇاڙي);
                    إذا (*پڇاڙي != 0) {
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
                    جوڙيو(ch);
                    أرجع *هذا;
                }
            حالة UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            حالة UCHAR_AGE:
                {
                    UVersionInfo نسخو;
                    u_نسخو_From_String(نسخو, vname.معلومات());
                    applyFilter(نسخو_Filter, &نسخو,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    أرجع *هذا;
                }
            حالة UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_قدر_Enum(UCHAR_SCRIPT, vname.معلومات());
                إذا (v == UCHAR_INVALID_ڪوڊ) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                اكسر;
            حالة UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_قدر_Enum(p, vname.معلومات());
                إذا (v == UCHAR_INVALID_ڪوڊ) {
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
        v = u_get_Property_قدر_Enum(p, pname.معلومات());
        إذا (v == UCHAR_INVALID_ڪوڊ) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_قدر_Enum(p, pname.معلومات());
            إذا (v == UCHAR_INVALID_ڪوڊ) {
                p = u_getPropertyEnum(pname.معلومات());
                إذا (p >= UCHAR_ٻٽو_شروعات && p < UCHAR_ٻٽو_حد_بندي) {
                    v = 1;
                } وإلا إذا (0 == uprv_compare_Property_نالا(ANY, pname.معلومات())) {
                    سيٽ(MIN_قدر, MAX_قدر);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نالا(ASCII, pname.معلومات())) {
                    سيٽ(0, 0x7F);
                    أرجع *هذا;
                } وإلا إذا (0 == uprv_compare_Property_نالا(ASSIGNED, pname.معلومات())) {
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
        complement().remove_All_لڙيون();  // code point complement
    }

    إذا (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_يادگيري_ورهاست_ERROR;
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
UBool Unicode_سيٽ::resembles_Property_نمونو(ثابت UnicodeString& نمونو,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    إذا ((pos+5) > نمونو.length()) {
        أرجع خطأ;
    }

    // Look for an opening [:, [:^, \p, or \P
    أرجع isPOSIXOpen(نمونو, pos) || isPerlOpen(نمونو, pos) || isNameOpen(نمونو, pos);
}

U_NAMESPACE_پڇاڙي
