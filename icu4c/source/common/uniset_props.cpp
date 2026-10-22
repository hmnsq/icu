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

#అన్తర్భావ <array>
#అన్తర్భావ <optional>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/symtable.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/utfiterator.h"
#అన్తర్భావ "uset_imp.h"
#అన్తర్భావ "ruleiter.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "util.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "uprops.h"
#అన్తర్భావ "patternprops.h"
#అన్తర్భావ "propname.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "uprops.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "hash.h"

U_NAMESPACE_USE

నామాకాశ {

// Special property set IDs
constexpr అక్షరం ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr అక్షరం ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr అక్షరం ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
స్థిర UBool U_CALLCONV uset_cleanup();

స్థిర Unicode_సమితి *uni32Singleton;
స్థిర icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
స్థిర UBool U_CALLCONV uset_cleanup() {
    తొలగించు uni32Singleton;
    uni32Singleton = శూన్యసూచిక;
    uni32InitOnce.reset();
    తిరిగి నిజం;
}

U_CDECL_చివర

U_NAMESPACE_BEGIN

ఉపయోగించి U_శీర్షిక_ONLY_NAMESPACE::utf_String_సంకేతం_Points;

నామాకాశ {

// Cache some sets for other services -------------------------------------- ***
శూన్య U_CALLCONV create_Uni32_సమితి(UError_సంకేతం &error_సంకేతం) {
    U_ASSERT(uni32Singleton == శూన్యసూచిక);
    uni32Singleton = కొత్త Unicode_సమితి(UnicodeString(u"[:age=3.2:]"), error_సంకేతం);
    ఒకవేళ(uni32Singleton==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        uni32Singleton->freeze();
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_సమితి *
uniset_getUnicode32Instance(UError_సంకేతం &error_సంకేతం) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_సమితి, error_సంకేతం);
    తిరిగి uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

అంతఃవరుస UBool
isPerlOpen(స్థిరరాశి UnicodeString &నమూనా, int32_t pos) {
    char16_t c;
    తిరిగి నమూనా.charAt(pos)==u'\\' && ((c=నమూనా.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

అంతఃవరుస UBool
isNameOpen(స్థిరరాశి UnicodeString &నమూనా, int32_t pos) {
    తిరిగి నమూనా.charAt(pos)==u'\\' && నమూనా.charAt(pos+1)==u'N';
}

అంతఃవరుస UBool
isPOSIXOpen(స్థిరరాశి UnicodeString &నమూనా, int32_t pos) {
    తిరిగి నమూనా.charAt(pos)==u'[' && నమూనా.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#నిర్వచించు _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(స్థిరరాశి CharString& name) {
    కొరకు (స్థిరరాశి UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_సంకేతం ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.దత్తాంశం(), &ec);
        ఒకవేళ (U_SUCCESS(ec)) {
            తిరిగి ch;
        }
    }
    తిరిగి U_SENTINEL;
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
Unicode_సమితి::Unicode_సమితి(స్థిరరాశి UnicodeString& నమూనా,
                       UError_సంకేతం& స్థితి_2) {
    apply_నమూనా(నమూనా, స్థితి_2);
    _dbgct(ఇది);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_సమితి& Unicode_సమితి::apply_నమూనా(స్థిరరాశి UnicodeString& నమూనా,
                                     UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_నమూనా_Ignore_Space(నమూనా, pos, శూన్యసూచిక, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి *ఇది;

    int32_t i = pos.get_సూచీ();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(నమూనా, i, నిజం);
    ఒకవేళ (i != నమూనా.length()) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి *ఇది;
}

శూన్య
Unicode_సమితి::apply_నమూనా_Ignore_Space(స్థిరరాశి UnicodeString& నమూనా,
                                    ParsePosition& pos,
                                    స్థిరరాశి చిహ్నం_పట్టిక* symbols,
                                    UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (isFrozen()) {
        స్థితి_2 = U_NO_WRITE_PERMISSION;
        తిరిగి;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    నియమం_Character_Iterator chars(నమూనా, symbols, pos);
    apply_నమూనా(నమూనా, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, శూన్యసూచిక, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    ఒకవేళ (chars.లోపల_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        స్థితి_2 = U_MALFORMED_సమితి;
        తిరిగి;
    }
    సమితి_నమూనా(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_సమితి::resembles_నమూనా(స్థిరరాశి UnicodeString& నమూనా, int32_t pos) {
    తిరిగి ((pos+1) < నమూనా.length() &&
            నమూనా.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_నమూనా(నమూనా, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#నిర్వచించు U_DEBUGGING_UNICODESET_PARSING 0

తరగతి Unicode_సమితి::Lexer {
  బహిరంగ:
    Lexer(స్థిరరాశి UnicodeString &నమూనా,
          స్థిరరాశి ParsePosition &parsePosition,
          నియమం_Character_Iterator &chars,
          uint32_t unicode_సమితి_Options,
          స్థిరరాశి చిహ్నం_పట్టిక *స్థిరరాశి symbols,
          Unicode_సమితి &(Unicode_సమితి::*case_సంవృతం)(int32_t attribute))
        : నమూనా_2(నమూనా), parsePosition_(parsePosition), chars_(chars),
          unicode_సమితి_Options_2(unicode_సమితి_Options),
          charsOptions_(నియమం_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_సమితి_Options & USET_IGNORE_SPACE) != 0
                             ? నియమం_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_సంవృతం_2(case_సంవృతం) {}

    తరగతి LexicalElement {
      బహిరంగ:
        తార్కిక is_సమితి_Operator(స్థిరరాశి char16_t op) స్థిరరాశి {
            తిరిగి U_SUCCESS(error_సంకేతం_2) && category_ == సమితి_OPERATOR && string_[0] == op;
        }

        తార్కిక isStringLiteral() స్థిరరాశి {
            తిరిగి U_SUCCESS(error_సంకేతం_2) && category_ == STRING_LITERAL;
        }

        తార్కిక isNamedElement() స్థిరరాశి {
            తిరిగి U_SUCCESS(error_సంకేతం_2) && category_ == NAMED_ELEMENT;
        }

        తార్కిక isBracketedElement() స్థిరరాశి {
            తిరిగి U_SUCCESS(error_సంకేతం_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() స్థిరరాశి {
            ఒకవేళ (U_SUCCESS(error_సంకేతం_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                తిరిగి string_;
            }
            తిరిగి std::nullopt;
        }

        std::optional<UChar32> సంకేతం_బిందువు() స్థిరరాశి {
            ఒకవేళ (U_SUCCESS(error_సంకేతం_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                తిరిగి string_.char32At(0);
            }
            తిరిగి std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        స్థిరరాశి Unicode_సమితి *సమితి() స్థిరరాశి {
            ఒకవేళ (U_FAILURE(error_సంకేతం_2)) {
                తిరిగి శూన్యసూచిక;
            }
            ఒకవేళ (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                ఒకవేళ (precomputed_సమితి != శూన్యసూచిక) {
                    తిరిగి precomputed_సమితి;
                } లేకపోతే {
                    తిరిగి &సమితి_2;
                }
            }
            తిరిగి శూన్యసూచిక;
        }

        స్థిరరాశి UError_సంకేతం& error_సంకేతం() స్థిరరాశి{
          తిరిగి error_సంకేతం_2;
        }

#ఒకవేళ U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() స్థిరరాశి {
            UnicodeString ఫలితం;
            ఒకవేళ (U_FAILURE(error_సంకేతం_2)) {
                ఫలితం.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_సంకేతం_2)))
                    .append(u"), possibly ");
            }
            తిరిగి ఫలితం.append(category_పేర్లు[category_])
                .append(u" '")
                .append(ఆధారం_పాఠ్యం)
                .append(u"'");
        }
#అయితే_ముగింపు

      అంతరంగ:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        గణన Category : std::uint8_t {
            సమితి_OPERATOR,
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
            చివర_యొక్క_పాఠ్యం,
        };
        స్థిర constexpr std::array<std::u16string_view, చివర_యొక్క_పాఠ్యం + 1> category_పేర్లు{{
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
        LexicalElement(Category category, UnicodeString string, నియమం_Character_Iterator::Pos after,
                       స్థిరరాశి UError_సంకేతం error_సంకేతం, స్థిరరాశి Unicode_సమితి *precomputed_సమితి_2, Unicode_సమితి సమితి,
                       std::u16string_view ఆధారం_పాఠ్యం_2, తార్కిక separated,
                       std::optional<LexicalElement> స్థిరరాశి &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_సంకేతం_2(error_సంకేతం),
              precomputed_సమితి(precomputed_సమితి_2), సమితి_2(సమితి), ఆధారం_పాఠ్యం(ఆధారం_పాఠ్యం_2) {
            ఒకవేళ (U_FAILURE(error_సంకేతం_2) || separated || !preceding.has_value()) {
                తిరిగి;
            }
            ఒకవేళ (preceding->is_సమితి_Operator(u'[') && category == LITERAL_ELEMENT &&
                *సంకేతం_బిందువు() == u':') {
                error_సంకేతం_2 = U_MALFORMED_సమితి;
            } లేకపోతే ఒకవేళ (preceding->category_ == ESCAPED_ELEMENT) {
                ఒకవేళ (U_IS_LEAD(*preceding->సంకేతం_బిందువు()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*సంకేతం_బిందువు())) {
                    error_సంకేతం_2 = U_MALFORMED_సమితి;
                } లేకపోతే ఒకవేళ (preceding->ఆధారం_పాఠ్యం.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *సంకేతం_బిందువు();
                    ఒకవేళ (preceding->ఆధారం_పాఠ్యం[1] == u'x') {
                        ఒకవేళ ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_సంకేతం_2 = U_MALFORMED_సమితి;
                        }
                    } లేకపోతే ఒకవేళ (preceding->ఆధారం_పాఠ్యం[1] >= u'0' &&
                               preceding->ఆధారం_పాఠ్యం[1] <= u'7') {
                        ఒకవేళ (cp >= u'0' && cp <= u'7') {
                            error_సంకేతం_2 = U_MALFORMED_సమితి;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        నియమం_Character_Iterator::Pos after_;
        UError_సంకేతం error_సంకేతం_2;
        స్థిరరాశి Unicode_సమితి *precomputed_సమితి;
        Unicode_సమితి సమితి_2;
        std::u16string_view ఆధారం_పాఠ్యం;

        మిత్ర తరగతి Lexer;
    };

    UnicodeString getPositionForDebugging() స్థిరరాశి {
        తిరిగి నమూనా_2.తాత్కాలిక_Sub_String(0, parsePosition_.get_సూచీ()) + u"☞" +
               నమూనా_2.తాత్కాలిక_Sub_String(parsePosition_.get_సూచీ(), 60);
    }

    తార్కిక accept_సమితి_Operator(char16_t op) {
        ఒకవేళ (lookahead().is_సమితి_Operator(op)) {
            advance();
            తిరిగి నిజం;
        }
        తిరిగి అబద్ధం;
    }

    స్థిరరాశి LexicalElement &lookahead() {
        ఒకవేళ (!ahead_.has_value()) {
            స్థిరరాశి నియమం_Character_Iterator::Pos before = getPos();
            ahead_.emplace(తదుపరి_Token(behind_));
            chars_.సమితి_Pos(before);
        }
        తిరిగి *ahead_;
    }

    స్థిరరాశి LexicalElement &lookahead2() {
        ఒకవేళ (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            స్థిరరాశి నియమం_Character_Iterator::Pos before = getPos();
            chars_.సమితి_Pos(lookahead().after_);
            ahead2_.emplace(తదుపరి_Token(ahead_));
            chars_.సమితి_Pos(before);
        }
        తిరిగి *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    నియమం_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        తిరిగి chars_;
    }

    int32_t charsOptions() {
        తిరిగి charsOptions_;
    }

    తార్కిక at_చివర() స్థిరరాశి {
        తిరిగి chars_.at_చివర();
    }

    శూన్య advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.సమితి_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  అంతరంగ:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    నియమం_Character_Iterator::Pos getPos() స్థిరరాశి {
        నియమం_Character_Iterator::Pos ఫలితం;
        chars_.getPos(ఫలితం);
        తిరిగి ఫలితం;
    }

    LexicalElement తదుపరి_Token(std::optional<LexicalElement> స్థిరరాశి& preceding) {
        UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
        తార్కిక separated = అబద్ధం;
        ఒకవేళ (charsOptions_ & నియమం_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            నియమం_Character_Iterator::Pos pos;
            కొరకు (;;) {
                UBool వాడని_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                  నియమం_Character_Iterator::SKIP_WHITESPACE),
                                వాడని_Escaped, error_సంకేతం);
                ఒకవేళ (!నమూనా_Props::isWhiteSpace(s)) {
                    chars_.సమితి_Pos(pos);
                    ఆపు;
                }
                ఒకవేళ (s != u'\u200E' && s != u'\u200F') {
                    separated = నిజం;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        ఒకవేళ (chars_.at_చివర()) {
            తిరిగి LexicalElement(LexicalElement::చివర_యొక్క_పాఠ్యం, {}, getPos(), error_సంకేతం,
                                  /*precomputedSet=*/శూన్యసూచిక,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        స్థిరరాశి int32_t ప్రారంభం = parsePosition_.get_సూచీ();
        స్థిరరాశి నియమం_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool వాడని_Escaped;
        స్థిరరాశి UChar32 first =
            chars_.next(charsOptions_ & ~నియమం_Character_Iterator::PARSE_ESCAPES, వాడని_Escaped, error_సంకేతం);
        ఒకవేళ (first == u'[' || first == u'\\') {
            స్థిరరాశి నియమం_Character_Iterator::Pos after_మొదటి = getPos();
            // This could be a property-query or named-element.
            స్థిరరాశి UChar32 రెండవ = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                                 నియమం_Character_Iterator::SKIP_WHITESPACE),
                                               వాడని_Escaped, error_సంకేతం);
            ఒకవేళ ((first == u'[' && రెండవ == u':') ||
                (first == u'\\' && (రెండవ == u'p' || రెండవ == u'P' || రెండవ == u'N'))) {
                ఒకవేళ (రెండవ == u'N') {
                    UChar32 స్థిరరాశి query_ఫలితం = పరిశీలన_Named_Element_Brackets(error_సంకేతం);
                    తిరిగి LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_ఫలితం), getPos(), error_సంకేతం,
                        /*precomputedSet=*/శూన్యసూచిక,
                        /*set=*/{},
                        std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                        separated, preceding);
                } లేకపోతే {
                    Unicode_సమితి query_ఫలితం = పరిశీలన_Property_Query_After_ప్రారంభం(first, రెండవ, ప్రారంభం, error_సంకేతం);
                    తిరిగి LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_సంకేతం,
                        /*precomputedSet=*/శూన్యసూచిక, /*set=*/std::move(query_ఫలితం),
                        std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.సమితి_Pos(after_మొదటి);
        }
        ఒకవేళ (first == u'$' && symbols_ != శూన్యసూచిక) {
            స్వయంచాలక name_చివర = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            ఒకవేళ (UnicodeString name = symbols_->parseReference(నమూనా_2, name_చివర, నమూనా_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_ఖాళీలు()) {
                chars_.jumpahead(name_చివర.get_సూచీ() - (ప్రారంభం + 1));
                స్థిరరాశి std::u16string_view ఆధారం =
                    std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం);
                స్థిరరాశి Unicode_సమితి *precomputed_సమితి_2 = symbols_->lookup_సమితి(name);
                ఒకవేళ (precomputed_సమితి_2 != శూన్యసూచిక) {
                    తిరిగి LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_సమితి_2, /*set=*/{}, ఆధారం, /*separated=*/నిజం,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                స్థిరరాశి UnicodeString *స్థిరరాశి expression = symbols_->lookup(name);
                ఒకవేళ (expression == శూన్యసూచిక) {
                    తిరిగి LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/శూన్యసూచిక,
                        /*set=*/{},
                        ఆధారం,
                        /*separated=*/నిజం,
                        /*preceding=*/std::nullopt);
                }
                తిరిగి evaluateVariable(*expression, ఆధారం);
            }
        }
        ఎంపిక (first) {
        స్థితి u'[':
            తిరిగి LexicalElement(
                LexicalElement::సమితి_OPERATOR, UnicodeString(u'['), getPos(), error_సంకేతం,
                /*precomputedSet=*/శూన్యసూచిక,
                /*set=*/{},
                std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                separated, preceding);
        స్థితి u'\\': {
            // Now try to parse the escape.
            chars_.సమితి_Pos(before);
            UChar32 సంకేతం_బిందువు = chars_.next(charsOptions_, వాడని_Escaped, error_సంకేతం);
            తిరిగి LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(సంకేతం_బిందువు), getPos(), error_సంకేతం,
                శూన్యసూచిక,
                /*set=*/{},
                std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                separated, preceding);
        }
        స్థితి u'&':
        స్థితి u'-':
        స్థితి u']':
        స్థితి u'^':
        స్థితి u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            తిరిగి LexicalElement(
                LexicalElement::సమితి_OPERATOR, UnicodeString(first), getPos(), error_సంకేతం,
                /*precomputedSet=*/శూన్యసూచిక,
                /*set=*/{},
                std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                separated, preceding);
        స్థితి u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t సంకేతం_బిందువు_లెక్క = 0;
            అయితే (!chars_.at_చివర() && U_SUCCESS(error_సంకేతం)) {
                స్థిరరాశి నియమం_Character_Iterator::Pos before_తదుపరి = getPos();
                next = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                     నియమం_Character_Iterator::SKIP_WHITESPACE),
                                   వాడని_Escaped, error_సంకేతం);
                ఒకవేళ (next == u'\\') {
                    స్థిరరాశి UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                      నియమం_Character_Iterator::SKIP_WHITESPACE),
                                    వాడని_Escaped, error_సంకేతం);
                    ఒకవేళ (afterBackslash == u'N') {
                        next = పరిశీలన_Named_Element_Brackets(error_సంకేతం);
                        escaped = నిజం;
                    } లేకపోతే ఒకవేళ (afterBackslash == u'p' || afterBackslash == u'P') {
                        తిరిగి LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_సమితి,
                                              /*precomputedSet=*/శూన్యసూచిక,
                                              /*set=*/{},
                                              std::u16string_view(నమూనా_2).substr(
                                                  ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                                              separated, preceding);
                    } లేకపోతే {
                        chars_.సమితి_Pos(before_తదుపరి);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_సంకేతం);
                    }
                } లేకపోతే {
#ఒకవేళ U_ICU_సంస్కరణ_MAJOR_అంకె < 81
                    ఒకవేళ (U_SUCCESS(error_సంకేతం) && నమూనా_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_సంకేతం = UError_సంకేతం::U_ILLEGAL_వాదం_ERROR;
                    }
#లేకపోతే
#దోషం Remove ఇది transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#అయితే_ముగింపు
                    escaped = అబద్ధం;
                }
                ఒకవేళ (!escaped && next == u'}') {
                    తిరిగి LexicalElement(
                        సంకేతం_బిందువు_లెక్క == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_సంకేతం,
                        /*precomputedSet=*/శూన్యసూచిక,
                        /*set=*/{},
                        std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                        separated, preceding);
                }
                string.append(next);
                సంకేతం_బిందువు_లెక్క += 1;
            }
            తిరిగి LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_సమితి,
                /*precomputedSet=*/శూన్యసూచిక,
                /*set=*/{},
                std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                separated, preceding);
        }
        అప్రమేయ:
            తిరిగి LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_సంకేతం, శూన్యసూచిక,
                /*set=*/{},
                std::u16string_view(నమూనా_2).substr(ప్రారంభం, parsePosition_.get_సూచీ() - ప్రారంభం),
                separated, preceding);
        }
    }

    UChar32 పరిశీలన_Named_Element_Brackets(UError_సంకేతం &error_సంకేతం) {
        UBool వాడని_Escaped;
        స్థిరరాశి UChar32 open = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                           నియమం_Character_Iterator::SKIP_WHITESPACE),
                                         వాడని_Escaped, error_సంకేతం);
        ఒకవేళ (open == u'{') {
            int32_t ప్రారంభం = parsePosition_.get_సూచీ();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            అయితే (!chars_.at_చివర() && U_SUCCESS(error_సంకేతం)) {
                UChar32 చివరి = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                             నియమం_Character_Iterator::SKIP_WHITESPACE),
                                           వాడని_Escaped, error_సంకేతం);
                ఒకవేళ (చివరి == u':') {
                    ఒకవేళ (!hex.has_value()) {
                        hex.emplace();
                        ఒకవేళ (ప్రారంభం == parsePosition_.get_సూచీ() - 1) {
                            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                            తిరిగి {};
                        }
                        కొరకు (char16_t digit : std::u16string_view(నమూనా_2).substr(
                                 ప్రారంభం, parsePosition_.get_సూచీ() - 1 - ప్రారంభం)) {
                            uint8_t nibble;
                            ఒకవేళ (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } లేకపోతే {
                                digit = digit & ~0x20;
                                ఒకవేళ (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } లేకపోతే {
                                    error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                                    తిరిగి {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            ఒకవేళ (hex > 0x10FFFF) {
                                error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                                తిరిగి {};
                            }
                        }
                    } లేకపోతే ఒకవేళ (!literal.has_value()) {
                        స్థిరరాశి స్వయంచాలక literal_సంకేతం_Points = utf_String_సంకేతం_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(నమూనా_2).substr(ప్రారంభం,
                                                                 parsePosition_.get_సూచీ() - 1 - ప్రారంభం));
                        స్వయంచాలక it = literal_సంకేతం_Points.begin();
                        ఒకవేళ (it == literal_సంకేతం_Points.చివర() || !it->wellFormed() ||
                                (literal = it->సంకేతం_బిందువు(), ++it) != literal_సంకేతం_Points.చివర()) {
                            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                            తిరిగి {};
                        }
                    } లేకపోతే {
                        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                        తిరిగి {};
                    }
                    ప్రారంభం = parsePosition_.get_సూచీ();
                } లేకపోతే ఒకవేళ (చివరి == u'}') {
                    స్థిరరాశి std::u16string_view u16name = std::u16string_view(నమూనా_2).substr(
                        ప్రారంభం, parsePosition_.get_సూచీ() - 1 - ప్రారంభం);
                    స్థిరరాశి UChar32 ఫలితం = getCharacterByName(CharString().appendInvariantChars(
                        u16name.దత్తాంశం(), static_cast<int32_t>(u16name.length()), error_సంకేతం));
                    ఒకవేళ (!U_SUCCESS(error_సంకేతం)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                        తిరిగి {};
                    }
                    ఒకవేళ (ఫలితం < 0 || (hex.has_value() && ఫలితం != hex) ||
                            (literal.has_value() && ఫలితం != literal)) {
                        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                        తిరిగి {};
                    }
                    తిరిగి ఫలితం;
                }
            }
        }
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        }
        తిరిగి {};
    }

    LexicalElement evaluateVariable(స్థిరరాశి UnicodeString &expression,
                                    స్థిరరాశి std::u16string_view ఆధారం) {
        UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
        ParsePosition వ్యక్తీకరణ_Position;
        నియమం_Character_Iterator వ్యక్తీకరణ_Iterator(expression, symbols_, వ్యక్తీకరణ_Position);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer వ్యక్తీకరణ_Lexer(expression, వ్యక్తీకరణ_Position, వ్యక్తీకరణ_Iterator, unicode_సమితి_Options_2,
                              /*symbols=*/శూన్యసూచిక, case_సంవృతం_2);
        స్వయంచాలక variableToken = వ్యక్తీకరణ_Lexer.lookahead();
        ఒకవేళ (variableToken.is_సమితి_Operator(u'[')) {
            UnicodeString rebuilt_నమూనా;
            Unicode_సమితి వ్యక్తీకరణ_విలువ =
                parse_Unicode_సమితి(వ్యక్తీకరణ_Lexer, rebuilt_నమూనా, unicode_సమితి_Options_2, case_సంవృతం_2,
                                /*depth=*/0, error_సంకేతం);
            వ్యక్తీకరణ_విలువ.సమితి_నమూనా(rebuilt_నమూనా);
            ఒకవేళ (!వ్యక్తీకరణ_Lexer.at_చివర()) {
                తిరిగి LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/శూన్యసూచిక,
                    /*set=*/{},
                    ఆధారం,
                    /*separated=*/నిజం,
                    /*preceding=*/std::nullopt);
            }
            తిరిగి LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_సంకేతం,
                /*precomputedSet=*/శూన్యసూచిక,
                /*set=*/std::move(వ్యక్తీకరణ_విలువ),
                ఆధారం,
                /*separated=*/నిజం,
                /*preceding=*/std::nullopt);
        } లేకపోతే {
            వ్యక్తీకరణ_Lexer.advance();
            ఒకవేళ (!వ్యక్తీకరణ_Lexer.at_చివర()) {
                తిరిగి LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/శూన్యసూచిక,
                    /*set=*/{},
                    ఆధారం,
                    /*separated=*/నిజం,
                    /*preceding=*/std::nullopt);
            }
            ఎంపిక (variableToken.category_) {
            స్థితి LexicalElement::LITERAL_ELEMENT:
            స్థితి LexicalElement::ESCAPED_ELEMENT:
            స్థితి LexicalElement::NAMED_ELEMENT:
            స్థితి LexicalElement::BRACKETED_ELEMENT:
            స్థితి LexicalElement::STRING_LITERAL:
            స్థితి LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                తిరిగి LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_సంకేతం_2, variableToken.precomputed_సమితి, std::move(variableToken.సమితి_2), ఆధారం,
                    /*separated=*/నిజం,
                    /*preceding=*/std::nullopt);
            అప్రమేయ:
                తిరిగి LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/శూన్యసూచిక,
                                      /*set=*/{}, ఆధారం,
                                      /*separated=*/నిజం,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_సమితి పరిశీలన_Property_Query_After_ప్రారంభం(UChar32 first, UChar32 రెండవ, int32_t query_ప్రారంభం, UError_సంకేతం &error_సంకేతం) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t query_వ్యక్తీకరణ_ప్రారంభం = parsePosition_.get_సూచీ();
        తార్కిక exteriorlyNegated = అబద్ధం;
        తార్కిక interiorlyNegated = అబద్ధం;
        UBool వాడని_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        స్థిరరాశి UChar32 మూడవ = chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                                            నియమం_Character_Iterator::SKIP_WHITESPACE),
                                          వాడని_Escaped, error_సంకేతం);
        ఒకవేళ (first == u'\\') {
            ఒకవేళ (మూడవ != u'{') {
                error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                తిరిగి {};
            }
            exteriorlyNegated = రెండవ == u'P';
            query_వ్యక్తీకరణ_ప్రారంభం = parsePosition_.get_సూచీ();
        } లేకపోతే {
            ఒకవేళ (మూడవ == u'^') {
                exteriorlyNegated = నిజం;
                query_వ్యక్తీకరణ_ప్రారంభం = parsePosition_.get_సూచీ();
            }
        }
        నియమం_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                          నియమం_Character_Iterator::SKIP_WHITESPACE),
                        వాడని_Escaped, error_సంకేతం);

        అయితే (!chars_.at_చివర() && U_SUCCESS(error_సంకేతం)) {
            స్థిరరాశి నియమం_Character_Iterator::Pos before_చివరి = getPos();
            UChar32 చివరి_Unescaped =
                chars_.next(charsOptions_ & ~(నియమం_Character_Iterator::PARSE_ESCAPES |
                                              నియమం_Character_Iterator::SKIP_WHITESPACE),
                            వాడని_Escaped, error_సంకేతం);
            ఒకవేళ (penultimateUnescaped == u'\\') {
                ఒకవేళ (చివరి_Unescaped == 'N') {
                    పరిశీలన_Named_Element_Brackets(error_సంకేతం);
                    ఒకవేళ (!U_SUCCESS(error_సంకేతం)) {
                        తిరిగి {};
                    }
                } లేకపోతే {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.సమితి_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~నియమం_Character_Iterator::SKIP_WHITESPACE, వాడని_Escaped,
                                error_సంకేతం);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                చివరి_Unescaped = -1;
            } లేకపోతే ఒకవేళ (!queryOperatorPosition.has_value() && చివరి_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_సూచీ() - 1;
            } లేకపోతే ఒకవేళ (!queryOperatorPosition.has_value() && చివరి_Unescaped == u'≠') {
                ఒకవేళ (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                    తిరిగి {};
                }
                interiorlyNegated = నిజం;
                queryOperatorPosition = parsePosition_.get_సూచీ() - 1;
            } లేకపోతే ఒకవేళ ((first == u'[' && penultimateUnescaped == u':' && చివరి_Unescaped == u']') ||
                       (first == u'\\' && చివరి_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                స్థిరరాశి int32_t query_వ్యక్తీకరణ_హద్దు =
                    first == u'[' ? parsePosition_.get_సూచీ() - 2 : parsePosition_.get_సూచీ() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                ఒకవేళ (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        నమూనా_2.తాత్కాలిక_Sub_String_Between(*queryOperatorPosition + 1, query_వ్యక్తీకరణ_హద్దు);
                    ఒకవేళ (propertyPredicate.is_ఖాళీలు()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                        తిరిగి {};
                    }
                }
                Unicode_సమితి ఫలితం;
                ఫలితం.applyPropertyAlias(
                    నమూనా_2.తాత్కాలిక_Sub_String_Between(query_వ్యక్తీకరణ_ప్రారంభం,
                                                  queryOperatorPosition.value_or(query_వ్యక్తీకరణ_హద్దు)),
                    propertyPredicate, error_సంకేతం);
                ఒకవేళ (exteriorlyNegated != interiorlyNegated) {
                    ఫలితం.complement().remove_All_అక్షరశ్రేణులు();
                }
                ఫలితం.సమితి_నమూనా(నమూనా_2.తాత్కాలిక_Sub_String_Between(query_ప్రారంభం, parsePosition_.get_సూచీ()));
                తిరిగి ఫలితం;
            }
            beforePenultimate = before_చివరి;
            penultimateUnescaped = చివరి_Unescaped;
        }
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి {};
    }

    స్థిరరాశి UnicodeString &నమూనా_2;
    స్థిరరాశి ParsePosition &parsePosition_;
    నియమం_Character_Iterator &chars_;
    స్థిరరాశి uint32_t unicode_సమితి_Options_2;
    స్థిరరాశి int32_t charsOptions_;
    స్థిరరాశి చిహ్నం_పట్టిక *స్థిరరాశి symbols_;
    Unicode_సమితి &(Unicode_సమితి::* స్థిరరాశి case_సంవృతం_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

నామాకాశ {

constexpr int32_t MAX_లోతు = 100;

#ఒకవేళ U_DEBUGGING_UNICODESET_PARSING

#నిర్వచించు U_UNICODESET_RETURN_IF_ERROR(ఫలితం, ec)                                                   \
    చేయి {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        ఒకవేళ (U_FAILURE(ec)) {                                                                       \
            ఒకవేళ (లోతు < 5) {                                                                       \
                రాయి("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } లేకపోతే ఒకవేళ (లోతు == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                రాయి("--- [...]\n");                                                             \
            }                                                                                      \
            తిరిగి ఫలితం;                                                                         \
        }                                                                                          \
    } అయితే (అబద్ధం)
#నిర్వచించు U_UNICODESET_RETURN_WITH_PARSE_ERROR(ఫలితం, expected, actual, lexer, ec)                  \
    చేయి {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        రాయి("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).గమ్యం_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().గమ్యం_UTF8_String(contextUTF8).c_str());                 \
        రాయి("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        ఒకవేళ (U_FAILURE(lexer.lookahead().error_సంకేతం())) {                                            \
            (ec) = lexer.lookahead().error_సంకేతం();                                                  \
        } లేకపోతే {                                                                                   \
            (ec) = U_MALFORMED_సమితి;                                                                \
        }                                                                                          \
        తిరిగి ఫలితం;                                                                             \
    } అయితే (అబద్ధం)

#లేకపోతే

#నిర్వచించు U_UNICODESET_RETURN_IF_ERROR(ఫలితం, ec)                                                   \
    చేయి {                                                                                           \
        ఒకవేళ (U_FAILURE(ec)) {                                                                       \
            తిరిగి ఫలితం;                                                                         \
        }                                                                                          \
    } అయితే (అబద్ధం)
#నిర్వచించు U_UNICODESET_RETURN_WITH_PARSE_ERROR(ఫలితం, expected, actual, lexer, ec)                  \
    చేయి {                                                                                           \
        ఒకవేళ (U_FAILURE(lexer.lookahead().error_సంకేతం())) {                                            \
            (ec) = lexer.lookahead().error_సంకేతం();                                                  \
        } లేకపోతే {                                                                                   \
            (ec) = U_MALFORMED_సమితి;                                                                \
        }                                                                                          \
        తిరిగి ఫలితం;                                                                             \
    } అయితే (అబద్ధం)

#అయితే_ముగింపు

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

శూన్య Unicode_సమితి::apply_నమూనా(స్థిరరాశి UnicodeString &నమూనా,
                              స్థిరరాశి ParsePosition &parsePosition,
                              నియమం_Character_Iterator &chars,
                              స్థిరరాశి చిహ్నం_పట్టిక *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_సమితి &(Unicode_సమితి::*case_సంవృతం)(int32_t attribute),
                              UError_సంకేతం &ec) {
    ఒకవేళ (U_FAILURE(ec)) తిరిగి;
    Lexer lexer(నమూనా, parsePosition, chars, options, symbols, case_సంవృతం);
    *ఇది = parse_Unicode_సమితి(lexer, rebuiltPat, options, case_సంవృతం, /*depth=*/0, ec);
}

Unicode_సమితి Unicode_సమితి::parse_Unicode_సమితి(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_సమితి& (Unicode_సమితి::*case_సంవృతం)(int32_t attribute),
                                       int32_t లోతు,
                                       UError_సంకేతం &ec) {
    Unicode_సమితి ఫలితం;

    ఒకవేళ (లోతు > MAX_లోతు) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_లోతు)).c_str(),
                                             ("depth = " + std::to_string(లోతు)).c_str(), lexer,
                                             ec);
    }

    తార్కిక isComplement = అబద్ధం;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    తార్కిక preserve_Syntax_లోపల_నమూనా = అబద్ధం;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_నమూనా;
    ఒకవేళ (lexer.lookahead().సమితి() != శూన్యసూచిక) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        ఫలితం = *lexer.lookahead().సమితి();
        ఫలితం.గమ్యం_నమూనా_2(pretty_Printed_నమూనా, /*escapeUnprintable=*/అబద్ధం);
        lexer.advance();
        preserve_Syntax_లోపల_నమూనా = నిజం;
    } లేకపోతే {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        ఒకవేళ (lexer.accept_సమితి_Operator(u'[')) {
            pretty_Printed_నమూనా.append(u'[');
            ఒకవేళ (lexer.accept_సమితి_Operator(u'^')) {
                pretty_Printed_నమూనా.append(u'^');
                isComplement = నిజం;
            }
            ఫలితం = parseContent(lexer, pretty_Printed_నమూనా, options, case_సంవృతం, లోతు,
                                  /*containsSetOperation=*/preserve_Syntax_లోపల_నమూనా, ec);
            U_UNICODESET_RETURN_IF_ERROR(ఫలితం, ec);
            ఒకవేళ (!lexer.accept_సమితి_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(ఫలితం, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_నమూనా.append(u']');
        } లేకపోతే {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(ఫలితం, "property-query | [",
                                                 lexer.lookahead().debugString(), lexer, ec);
        }
    }

    /**
     * Handle global flags (isComplement, case insensitivity).  If this
     * pattern should be compiled case-insensitive, then we need
     * to close over case BEFORE COMPLEMENTING.  This makes
     * patterns like /[^abc]/i work.
     */
    ఒకవేళ ((options & USET_CASE_ముసుగు) != 0) {
        (ఫలితం.*case_సంవృతం)(options);
    }
    ఒకవేళ (isComplement) {
        ఫలితం.complement().remove_All_అక్షరశ్రేణులు(); // code point complement
    }
    ఒకవేళ (preserve_Syntax_లోపల_నమూనా) {
        rebuiltPat.append(pretty_Printed_నమూనా);
    } లేకపోతే {
        ఫలితం.generate_నమూనా(rebuiltPat, /*escapeUnprintable=*/అబద్ధం);
    }
    తిరిగి ఫలితం;
}

Unicode_సమితి Unicode_సమితి::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_సమితి &(Unicode_సమితి::*case_సంవృతం)(int32_t attribute),
                                    int32_t లోతు,
                                    తార్కిక &కలిగియుంది_సమితి_Operation,
                                    UError_సంకేతం &ec) {
    Unicode_సమితి ఫలితం;
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
    ఒకవేళ (lexer.accept_సమితి_Operator(u'-')) {
        ఫలితం.కూడు(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    అయితే (!lexer.at_చివర()) {
        ఒకవేళ (lexer.accept_సమితి_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            ఫలితం.కూడు(u'-');
            తిరిగి ఫలితం;
        } లేకపోతే ఒకవేళ (lexer.lookahead().is_సమితి_Operator(u'$')) {
            ఒకవేళ (lexer.lookahead2().is_సమితి_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                ఫలితం.కూడు(U_ETHER);
                కలిగియుంది_సమితి_Operation = నిజం;
                తిరిగి ఫలితం;
            }
        }
        ఒకవేళ (lexer.lookahead().is_సమితి_Operator(u']')) {
            తిరిగి ఫలితం;
        }
        // Also handles FinalMutation.
        ఫలితం.parseMutation(lexer, rebuiltPat, options, case_సంవృతం, లోతు, కలిగియుంది_సమితి_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(ఫలితం, ec);
    }
    తిరిగి ఫలితం;
}

శూన్య Unicode_సమితి::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_సమితి &(Unicode_సమితి::*case_సంవృతం)(int32_t attribute),
                               int32_t లోతు,
                               తార్కిక &కలిగియుంది_సమితి_Operation,
                               UError_సంకేతం &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    ఒకవేళ (lexer.lookahead().is_సమితి_Operator('[') || lexer.lookahead().సమితి() != శూన్యసూచిక) {
        కలిగియుంది_సమితి_Operation = నిజం;
        parse_సమితి_Operations(lexer, rebuiltPat, options, case_సంవృతం, లోతు, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } లేకపోతే {
        // Also handles DollarElements.
        కూడు_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

శూన్య Unicode_సమితి::parse_సమితి_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_సమితి &(Unicode_సమితి::*case_సంవృతం)(int32_t attribute),
                                    int32_t లోతు,
                                    UError_సంకేతం &ec) {
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
    స్థిరరాశి Unicode_సమితి ఎడమ_Hand_Side =
        parse_Unicode_సమితి(lexer, rebuiltPat, options, case_సంవృతం, లోతు + 1, ec);
    కూడు_All(ఎడమ_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    కొరకు (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        ఒకవేళ (lexer.accept_సమితి_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            స్థిరరాశి Unicode_సమితి కుడి_Hand_Side =
                parse_Unicode_సమితి(lexer, rebuiltPat, options, case_సంవృతం, లోతు + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(కుడి_Hand_Side);
        } లేకపోతే ఒకవేళ (lexer.lookahead().is_సమితి_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            ఒకవేళ (lexer.lookahead2().is_సమితి_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                తిరిగి;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            స్థిరరాశి Unicode_సమితి కుడి_Hand_Side =
                parse_Unicode_సమితి(lexer, rebuiltPat, options, case_సంవృతం, లోతు + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(కుడి_Hand_Side);
        } లేకపోతే {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            తిరిగి;
        }
    }
}

Unicode_సమితి Unicode_సమితి::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_సంకేతం &ec) {
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
    ఒకవేళ (lexer.lookahead().isStringLiteral()) {
        స్థిరరాశి UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_గమ్యం_Pat(rebuiltPat, element, /*escapeUnprintable=*/అబద్ధం);
        rebuiltPat.append(u'}');
        lexer.advance();
        తిరిగి Unicode_సమితి().కూడు(element);
    }
    UChar32 first;
    ఒకవేళ (lexer.lookahead().is_సమితి_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } లేకపోతే ఒకవేళ (lexer.lookahead().సంకేతం_బిందువు().has_value()) {
        first = *lexer.lookahead().సంకేతం_బిందువు();
    } లేకపోతే {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_గమ్యం_Pat(rebuiltPat, first, /*escapeUnprintable=*/అబద్ధం);
    ఒకవేళ (!lexer.lookahead().is_సమితి_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        తిరిగి Unicode_సమితి(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    ఒకవేళ (lexer.lookahead2().is_సమితి_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        తిరిగి Unicode_సమితి(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 చివరి;
    ఒకవేళ (lexer.lookahead().is_సమితి_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        చివరి = u'$';
        ఒకవేళ (lexer.lookahead2().is_సమితి_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } లేకపోతే ఒకవేళ (lexer.lookahead().సంకేతం_బిందువు().has_value()) {
        చివరి = *lexer.lookahead().సంకేతం_బిందువు();
    } లేకపోతే {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    ఒకవేళ (చివరి <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(చివరి) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_గమ్యం_Pat(rebuiltPat, చివరి, /*escapeUnprintable=*/అబద్ధం);
    తిరిగి Unicode_సమితి(first, చివరి);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

నామాకాశ {

UBool numeric_విలువ_Filter(UChar32 ch, శూన్య* context) {
    తిరిగి u_get_Numeric_విలువ(ch) == *static_cast<ద్విగుణ*>(context);
}

UBool general_Category_ముసుగు_Filter(UChar32 ch, శూన్య* context) {
    int32_t value = *static_cast<int32_t*>(context);
    తిరిగి (U_GET_GC_ముసుగు((UChar32) ch) & value) != 0;
}

UBool సంస్కరణ_Filter(UChar32 ch, శూన్య* context) {
    స్థిర స్థిరరాశి UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* సంస్కరణ = static_cast<UVersionInfo*>(context);
    తిరిగి uprv_memcmp(&v, &none, పరిమాణం(v)) > 0 && uprv_memcmp(&v, సంస్కరణ, పరిమాణం(v)) <= 0;
}

రకనిర్వచనం నిర్మాణం {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, శూన్య* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    తిరిగి u_get_Int_Property_విలువ(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, శూన్య* context) {
    తిరిగి uscript_hasScript(ch, *static_cast<UScript_సంకేతం*>(context));
}

UBool idTypeFilter(UChar32 ch, శూన్య* context) {
    తిరిగి u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
శూన్య Unicode_సమితి::applyFilter(Unicode_సమితి::Filter filter,
                             శూన్య* context,
                             స్థిరరాశి Unicode_సమితి* inclusions,
                             UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;

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

    UChar32 ప్రారంభం_Has_Property = -1;
    int32_t హద్దు_Range = inclusions->get_Range_లెక్క();

    కొరకు (పూర్ణాంకం j=0; j<హద్దు_Range; ++j) {
        // get current range
        UChar32 ప్రారంభం = inclusions->get_Range_ప్రారంభం(j);
        UChar32 చివర = inclusions->get_Range_చివర(j);

        // for all the code points in the range, process
        కొరకు (UChar32 ch = ప్రారంభం; ch <= చివర; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            ఒకవేళ ((*filter)(ch, context)) {
                ఒకవేళ (ప్రారంభం_Has_Property < 0) {
                    ప్రారంభం_Has_Property = ch;
                }
            } లేకపోతే ఒకవేళ (ప్రారంభం_Has_Property >= 0) {
                కూడు(ప్రారంభం_Has_Property, ch-1);
                ప్రారంభం_Has_Property = -1;
            }
        }
    }
    ఒకవేళ (ప్రారంభం_Has_Property >= 0) {
        కూడు(ప్రారంభం_Has_Property, static_cast<UChar32>(0x10FFFF));
    }
    ఒకవేళ (isBogus() && U_SUCCESS(స్థితి_2)) {
        // We likely ran out of memory. AHHH!
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

నామాకాశ {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#నిర్వచించు FAIL(ec) UPRV_ఖండం_MACRO_BEGIN { \
    ec=U_ILLEGAL_వాదం_ERROR; \
    తిరిగి *ఇది; \
} UPRV_ఖండం_MACRO_చివర

Unicode_సమితి&
Unicode_సమితి::apply_Int_Property_విలువ(UProperty prop, int32_t value, UError_సంకేతం& ec) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(ec) || isFrozen()) { తిరిగి *ఇది; }
    ఒకవేళ (prop == UCHAR_GENERAL_CATEGORY_ముసుగు) {
        స్థిరరాశి Unicode_సమితి* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_ముసుగు_Filter, &value, inclusions, ec);
    } లేకపోతే ఒకవేళ (prop == UCHAR_SCRIPT_EXTENSIONS) {
        స్థిరరాశి Unicode_సమితి* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_సంకేతం script = static_cast<UScript_సంకేతం>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } లేకపోతే ఒకవేళ (prop == UCHAR_IDENTIFIER_TYPE) {
        స్థిరరాశి Unicode_సమితి* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } లేకపోతే ఒకవేళ (0 <= prop && prop < UCHAR_ద్విమాన_హద్దు) {
        ఒకవేళ (value == 0 || value == 1) {
            స్థిరరాశి USet *సమితి = u_get_ద్విమాన_Property_సమితి(prop, &ec);
            ఒకవేళ (U_FAILURE(ec)) { తిరిగి *ఇది; }
            copyFrom(*Unicode_సమితి::fromUSet(సమితి), నిజం);
            ఒకవేళ (value == 0) {
                complement().remove_All_అక్షరశ్రేణులు();  // code point complement
            }
        } లేకపోతే {
            clear();
        }
    } లేకపోతే ఒకవేళ (UCHAR_INT_ప్రారంభం <= prop && prop < UCHAR_INT_హద్దు) {
        స్థిరరాశి Unicode_సమితి* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } లేకపోతే {
        ec = U_ILLEGAL_వాదం_ERROR;
    }
    తిరిగి *ఇది;
}

Unicode_సమితి&
Unicode_సమితి::applyPropertyAlias(స్థిరరాశి UnicodeString& prop,
                               స్థిరరాశి UnicodeString& value,
                               UError_సంకేతం& ec) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(ec) || isFrozen()) తిరిగి *ఇది;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    ఒకవేళ( !uprv_isInvariantUString(prop.get_బఫరు(), prop.length()) ||
        !uprv_isInvariantUString(value.get_బఫరు(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    ఒకవేళ (U_FAILURE(ec)) తిరిగి *ఇది;

    UProperty p;
    int32_t v;
    UBool invert = అబద్ధం;

    ఒకవేళ (value.length() > 0) {
        p = u_getPropertyEnum(pname.దత్తాంశం());
        ఒకవేళ (p == UCHAR_INVALID_సంకేతం) FAIL(ec);

        // Treat gc as gcm
        ఒకవేళ (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_ముసుగు;
        }

        ఒకవేళ ((p >= UCHAR_ద్విమాన_ప్రారంభం && p < UCHAR_ద్విమాన_హద్దు) ||
            (p >= UCHAR_INT_ప్రారంభం && p < UCHAR_INT_హద్దు) ||
            (p >= UCHAR_ముసుగు_ప్రారంభం && p < UCHAR_ముసుగు_హద్దు)) {
            v = u_get_Property_విలువ_Enum(p, vname.దత్తాంశం());
            ఒకవేళ (v == UCHAR_INVALID_సంకేతం) {
                // Handle numeric CCC
                ఒకవేళ (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    అక్షరం* చివర;
                    ద్విగుణ val = uprv_strtod(vname.దత్తాంశం(), &చివర);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    ఒకవేళ (*చివర != 0 || !(0 <= val && val <= 255) ||
                            (v = static_cast<int32_t>(val)) != val) {
                        // non-integral value or outside 0..255, or trailing junk
                        FAIL(ec);
                    }
                } లేకపోతే {
                    FAIL(ec);
                }
            }
        }

        లేకపోతే {

            ఎంపిక (p) {
            స్థితి UCHAR_NUMERIC_విలువ:
                {
                    అక్షరం* చివర;
                    ద్విగుణ val = uprv_strtod(vname.దత్తాంశం(), &చివర);
                    ఒకవేళ (*చివర != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_విలువ_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    తిరిగి *ఇది;
                }
            స్థితి UCHAR_NAME:
                {
                    స్థిరరాశి UChar32 ch = getCharacterByName(vname);
                    ఒకవేళ (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    కూడు(ch);
                    తిరిగి *ఇది;
                }
            స్థితి UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            స్థితి UCHAR_AGE:
                {
                    UVersionInfo సంస్కరణ;
                    u_సంస్కరణ_From_String(సంస్కరణ, vname.దత్తాంశం());
                    applyFilter(సంస్కరణ_Filter, &సంస్కరణ,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    తిరిగి *ఇది;
                }
            స్థితి UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_విలువ_Enum(UCHAR_SCRIPT, vname.దత్తాంశం());
                ఒకవేళ (v == UCHAR_INVALID_సంకేతం) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ఆపు;
            స్థితి UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_విలువ_Enum(p, vname.దత్తాంశం());
                ఒకవేళ (v == UCHAR_INVALID_సంకేతం) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                ఆపు;
            అప్రమేయ:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    లేకపోతే {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_ముసుగు;
        v = u_get_Property_విలువ_Enum(p, pname.దత్తాంశం());
        ఒకవేళ (v == UCHAR_INVALID_సంకేతం) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_విలువ_Enum(p, pname.దత్తాంశం());
            ఒకవేళ (v == UCHAR_INVALID_సంకేతం) {
                p = u_getPropertyEnum(pname.దత్తాంశం());
                ఒకవేళ (p >= UCHAR_ద్విమాన_ప్రారంభం && p < UCHAR_ద్విమాన_హద్దు) {
                    v = 1;
                } లేకపోతే ఒకవేళ (0 == uprv_compare_Property_పేర్లు(ANY, pname.దత్తాంశం())) {
                    సమితి(MIN_విలువ, MAX_విలువ);
                    తిరిగి *ఇది;
                } లేకపోతే ఒకవేళ (0 == uprv_compare_Property_పేర్లు(ASCII, pname.దత్తాంశం())) {
                    సమితి(0, 0x7F);
                    తిరిగి *ఇది;
                } లేకపోతే ఒకవేళ (0 == uprv_compare_Property_పేర్లు(ASSIGNED, pname.దత్తాంశం())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_ముసుగు;
                    v = U_GC_CN_ముసుగు;
                    invert = నిజం;
                } లేకపోతే {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_విలువ(p, v, ec);
    ఒకవేళ(invert) {
        complement().remove_All_అక్షరశ్రేణులు();  // code point complement
    }

    ఒకవేళ (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి *ఇది;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool Unicode_సమితి::resembles_Property_నమూనా(స్థిరరాశి UnicodeString& నమూనా,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    ఒకవేళ ((pos+5) > నమూనా.length()) {
        తిరిగి అబద్ధం;
    }

    // Look for an opening [:, [:^, \p, or \P
    తిరిగి isPOSIXOpen(నమూనా, pos) || isPerlOpen(నమూనా, pos) || isNameOpen(నమూనా, pos);
}

U_NAMESPACE_చివర
