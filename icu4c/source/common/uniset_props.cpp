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
constexpr वर्ण ANY[]   = "ANY";   // [\u0000-\U0010FFFF]
constexpr वर्ण ASCII[] = "ASCII"; // [\u0000-\u007F]
constexpr वर्ण ASSIGNED[] = "Assigned"; // [:^Cn:]

}  // namespace

// Cached sets ------------------------------------------------------------- ***

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uset_cleanup();

स्थिर Unicode_समुच्चयः *uni32Singleton;
स्थिर icu::UInitOnce uni32InitOnce {};

/**
 * Cleanup function for UnicodeSet
 */
स्थिर UBool U_CALLCONV uset_cleanup() {
    विलोपय uni32Singleton;
    uni32Singleton = शून्यसूचकः;
    uni32InitOnce.reset();
    प्रत्यागम सत्य;
}

U_CDECL_अन्तः

U_NAMESPACE_BEGIN

उपयुज्य U_शीर्षकम्_ONLY_NAMESPACE::utf_String_संहिता_Points;

नामाकाश {

// Cache some sets for other services -------------------------------------- ***
शून्य U_CALLCONV create_Uni32_समुच्चयः(UError_संहिता &error_संहिता) {
    U_ASSERT(uni32Singleton == शून्यसूचकः);
    uni32Singleton = नूतन Unicode_समुच्चयः(UnicodeString(u"[:age=3.2:]"), error_संहिता);
    यदि(uni32Singleton==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        uni32Singleton->freeze();
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_USET, uset_cleanup);
}


U_CFUNC Unicode_समुच्चयः *
uniset_getUnicode32Instance(UError_संहिता &error_संहिता) {
    umtx_initOnce(uni32InitOnce, &create_Uni32_समुच्चयः, error_संहिता);
    प्रत्यागम uni32Singleton;
}

// helper functions for matching of pattern syntax pieces ------------------ ***
// these functions are parallel to the PERL_OPEN etc. strings above

// using these functions is not only faster than UnicodeString::compare() and
// caseCompare(), but they also make UnicodeSet work for simple patterns when
// no Unicode properties data is available - when caseCompare() fails

अन्तःस्थ UBool
isPerlOpen(ध्रुव UnicodeString &आदर्शः, int32_t pos) {
    char16_t c;
    प्रत्यागम आदर्शः.charAt(pos)==u'\\' && ((c=आदर्शः.charAt(pos+1))==u'p' || c==u'P');
}

/*static inline UBool
isPerlClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u'}';
}*/

अन्तःस्थ UBool
isNameOpen(ध्रुव UnicodeString &आदर्शः, int32_t pos) {
    प्रत्यागम आदर्शः.charAt(pos)==u'\\' && आदर्शः.charAt(pos+1)==u'N';
}

अन्तःस्थ UBool
isPOSIXOpen(ध्रुव UnicodeString &आदर्शः, int32_t pos) {
    प्रत्यागम आदर्शः.charAt(pos)==u'[' && आदर्शः.charAt(pos+1)==u':';
}

/*static inline UBool
isPOSIXClose(const UnicodeString &pattern, int32_t pos) {
    return pattern.charAt(pos)==u':' && pattern.charAt(pos+1)==u']';
}*/

// TODO memory debugging provided inside uniset.cpp
// could be made available here but probably obsolete with use of modern
// memory leak checker tools
#परिभाषय _dbgct(me)

// Returns the character with the given name or name alias, or U_SENTINEL if no such character
// exists.
UChar32 getCharacterByName(ध्रुव CharString& name) {
    प्रति (ध्रुव UCharNameChoice nameChoice : std::array{U_EXTENDED_CHAR_NAME, U_CHAR_NAME_ALIAS}) {
        UError_संहिता ec = U_ZERO_ERROR;
        UChar32 ch = u_charFromName(nameChoice, name.दत्त(), &ec);
        यदि (U_SUCCESS(ec)) {
            प्रत्यागम ch;
        }
    }
    प्रत्यागम U_SENTINEL;
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
Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव UnicodeString& आदर्शः,
                       UError_संहिता& स्थितिः) {
    apply_आदर्शः(आदर्शः, स्थितिः);
    _dbgct(एतत्);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

Unicode_समुच्चयः& Unicode_समुच्चयः::apply_आदर्शः(ध्रुव UnicodeString& आदर्शः,
                                     UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    // Equivalent to
    //   return applyPattern(pattern, USET_IGNORE_SPACE, nullptr, status);
    // but without dependency on closeOver().
    ParsePosition pos(0);
    apply_आदर्शः_Ignore_Space(आदर्शः, pos, शून्यसूचकः, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम *एतत्;

    int32_t i = pos.get_सूचकांक();
    // Skip over trailing whitespace
    ICU_Utility::skipWhitespace(आदर्शः, i, सत्य);
    यदि (i != आदर्शः.length()) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम *एतत्;
}

शून्य
Unicode_समुच्चयः::apply_आदर्शः_Ignore_Space(ध्रुव UnicodeString& आदर्शः,
                                    ParsePosition& pos,
                                    ध्रुव चिह्नम्_सारणी* symbols,
                                    UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (isFrozen()) {
        स्थितिः = U_NO_WRITE_PERMISSION;
        प्रत्यागम;
    }
    // Need to build the pattern in a temporary string because
    // _applyPattern calls add() etc., which set pat to empty.
    UnicodeString rebuiltPat;
    नियमः_Character_Iterator chars(आदर्शः, symbols, pos);
    apply_आदर्शः(आदर्शः, pos, chars, symbols, rebuiltPat, USET_IGNORE_SPACE, शून्यसूचकः, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    यदि (chars.अन्तर्_Variable()) {
        // syntaxError(chars, "Extra chars in variable value");
        स्थितिः = U_MALFORMED_समुच्चयः;
        प्रत्यागम;
    }
    समुच्चयः_आदर्शः(rebuiltPat);
}

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 */
UBool Unicode_समुच्चयः::resembles_आदर्शः(ध्रुव UnicodeString& आदर्शः, int32_t pos) {
    प्रत्यागम ((pos+1) < आदर्शः.length() &&
            आदर्शः.charAt(pos) == static_cast<char16_t>(91)/*[*/) ||
        resembles_Property_आदर्शः(आदर्शः, pos);
}

//----------------------------------------------------------------
// Implementation: Pattern parsing
//----------------------------------------------------------------

#परिभाषय U_DEBUGGING_UNICODESET_PARSING 0

वर्ग Unicode_समुच्चयः::Lexer {
  सार्वजनिक:
    Lexer(ध्रुव UnicodeString &आदर्शः,
          ध्रुव ParsePosition &parsePosition,
          नियमः_Character_Iterator &chars,
          uint32_t unicode_समुच्चयः_Options,
          ध्रुव चिह्नम्_सारणी *ध्रुव symbols,
          Unicode_समुच्चयः &(Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute))
        : आदर्शः_2(आदर्शः), parsePosition_(parsePosition), chars_(chars),
          unicode_समुच्चयः_Options_2(unicode_समुच्चयः_Options),
          charsOptions_(नियमः_Character_Iterator::PARSE_ESCAPES |
                        ((unicode_समुच्चयः_Options & USET_IGNORE_SPACE) != 0
                             ? नियमः_Character_Iterator::SKIP_WHITESPACE
                             : 0)),
          symbols_(symbols),
          case_संवृतिः_2(case_संवृतिः) {}

    वर्ग LexicalElement {
      सार्वजनिक:
        तार्किक is_समुच्चयः_Operator(ध्रुव char16_t op) ध्रुव {
            प्रत्यागम U_SUCCESS(error_संहिता_2) && category_ == समुच्चयः_OPERATOR && string_[0] == op;
        }

        तार्किक isStringLiteral() ध्रुव {
            प्रत्यागम U_SUCCESS(error_संहिता_2) && category_ == STRING_LITERAL;
        }

        तार्किक isNamedElement() ध्रुव {
            प्रत्यागम U_SUCCESS(error_संहिता_2) && category_ == NAMED_ELEMENT;
        }

        तार्किक isBracketedElement() ध्रुव {
            प्रत्यागम U_SUCCESS(error_संहिता_2) && category_ == BRACKETED_ELEMENT;
        }

        std::optional<UnicodeString> element() ध्रुव {
            यदि (U_SUCCESS(error_संहिता_2) &&
                (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                 category_ == BRACKETED_ELEMENT || category_ == STRING_LITERAL)) {
                प्रत्यागम string_;
            }
            प्रत्यागम std::nullopt;
        }

        std::optional<UChar32> संहिता_Point() ध्रुव {
            यदि (U_SUCCESS(error_संहिता_2) && (category_ == LITERAL_ELEMENT || category_ == ESCAPED_ELEMENT ||
                                          category_ == BRACKETED_ELEMENT || category_ == NAMED_ELEMENT)) {
                प्रत्यागम string_.char32At(0);
            }
            प्रत्यागम std::nullopt;
        }

        // If `*this` is a valid property-query or set-valued-variable, returns the set represented
        // by this lexical element, which lives at least as long as `*this`.  Null otherwise.
        ध्रुव Unicode_समुच्चयः *समुच्चयः() ध्रुव {
            यदि (U_FAILURE(error_संहिता_2)) {
                प्रत्यागम शून्यसूचकः;
            }
            यदि (category_ == PROPERTY_QUERY || category_ == VARIABLE) {
                यदि (precomputed_समुच्चयः != शून्यसूचकः) {
                    प्रत्यागम precomputed_समुच्चयः;
                } अन्यथा {
                    प्रत्यागम &समुच्चयः_2;
                }
            }
            प्रत्यागम शून्यसूचकः;
        }

        ध्रुव UError_संहिता& error_संहिता() ध्रुव{
          प्रत्यागम error_संहिता_2;
        }

#यदि U_DEBUGGING_UNICODESET_PARSING
        UnicodeString debugString() ध्रुव {
            UnicodeString परिणाम;
            यदि (U_FAILURE(error_संहिता_2)) {
                परिणाम.append(u"Ill-formed token (")
                    .append(UnicodeString::fromUTF8(u_errorName(error_संहिता_2)))
                    .append(u"), possibly ");
            }
            प्रत्यागम परिणाम.append(category_नामानि[category_])
                .append(u" '")
                .append(sourceText_)
                .append(u"'");
        }
#यदि_अन्तः

      गुप्त:
        // See https://unicode.org/reports/tr61#Lexical-Elements.
        गणना Category : std::uint8_t {
            समुच्चयः_OPERATOR,
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
            अन्तः_सम्बन्धि_TEXT,
        };
        स्थिर constexpr std::array<std::u16string_view, अन्तः_सम्बन्धि_TEXT + 1> category_नामानि{{
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
        LexicalElement(Category category, UnicodeString string, नियमः_Character_Iterator::Pos after,
                       ध्रुव UError_संहिता error_संहिता, ध्रुव Unicode_समुच्चयः *precomputed_समुच्चयः_2, Unicode_समुच्चयः समुच्चयः,
                       std::u16string_view sourceText, तार्किक separated,
                       std::optional<LexicalElement> ध्रुव &preceding)
            : category_(category), string_(std::move(string)), after_(after), error_संहिता_2(error_संहिता),
              precomputed_समुच्चयः(precomputed_समुच्चयः_2), समुच्चयः_2(समुच्चयः), sourceText_(sourceText) {
            यदि (U_FAILURE(error_संहिता_2) || separated || !preceding.has_value()) {
                प्रत्यागम;
            }
            यदि (preceding->is_समुच्चयः_Operator(u'[') && category == LITERAL_ELEMENT &&
                *संहिता_Point() == u':') {
                error_संहिता_2 = U_MALFORMED_समुच्चयः;
            } अन्यथा यदि (preceding->category_ == ESCAPED_ELEMENT) {
                यदि (U_IS_LEAD(*preceding->संहिता_Point()) && category_ == ESCAPED_ELEMENT &&
                    U_IS_TRAIL(*संहिता_Point())) {
                    error_संहिता_2 = U_MALFORMED_समुच्चयः;
                } अन्यथा यदि (preceding->sourceText_.length() <= 3 && category == LITERAL_ELEMENT) {
                    UChar32 cp = *संहिता_Point();
                    यदि (preceding->sourceText_[1] == u'x') {
                        यदि ((cp >= u'0' && cp <= u'9') || (cp >= u'A' && cp <= u'F') ||
                            (cp >= u'a' && cp <= u'f')) {
                            error_संहिता_2 = U_MALFORMED_समुच्चयः;
                        }
                    } अन्यथा यदि (preceding->sourceText_[1] >= u'0' &&
                               preceding->sourceText_[1] <= u'7') {
                        यदि (cp >= u'0' && cp <= u'7') {
                            error_संहिता_2 = U_MALFORMED_समुच्चयः;
                        }
                    }
                }
            }
        }
        Category category_;
        UnicodeString string_;
        नियमः_Character_Iterator::Pos after_;
        UError_संहिता error_संहिता_2;
        ध्रुव Unicode_समुच्चयः *precomputed_समुच्चयः;
        Unicode_समुच्चयः समुच्चयः_2;
        std::u16string_view sourceText_;

        मित्रम् वर्ग Lexer;
    };

    UnicodeString getPositionForDebugging() ध्रुव {
        प्रत्यागम आदर्शः_2.अस्थायी_Sub_String(0, parsePosition_.get_सूचकांक()) + u"☞" +
               आदर्शः_2.अस्थायी_Sub_String(parsePosition_.get_सूचकांक(), 60);
    }

    तार्किक accept_समुच्चयः_Operator(char16_t op) {
        यदि (lookahead().is_समुच्चयः_Operator(op)) {
            advance();
            प्रत्यागम सत्य;
        }
        प्रत्यागम असत्य;
    }

    ध्रुव LexicalElement &lookahead() {
        यदि (!ahead_.has_value()) {
            ध्रुव नियमः_Character_Iterator::Pos before = getPos();
            ahead_.emplace(अग्र_Token(behind_));
            chars_.समुच्चयः_Pos(before);
        }
        प्रत्यागम *ahead_;
    }

    ध्रुव LexicalElement &lookahead2() {
        यदि (!ahead2_.has_value()) {
            // Note that if someone has called `getCharacterIterator` and played with the result,
            // `before` may not actually be before `ahead_`, but we do not actually depend on this here,
            // since we start from ahead_.after_.
            ध्रुव नियमः_Character_Iterator::Pos before = getPos();
            chars_.समुच्चयः_Pos(lookahead().after_);
            ahead2_.emplace(अग्र_Token(ahead_));
            chars_.समुच्चयः_Pos(before);
        }
        प्रत्यागम *ahead2_;
    }

    // For use in older functions that take the `RuleCharacterIterator` directly.
    // Any advancement of the resulting `RuleCharacterIterator` has no effect on the result of subsequent
    // calls to `lookahead`, `lookahead2`, `advance`, or `acceptSetOperator`.
    // Once `advance` or `acceptSetOperator` has been called, the result of a call to
    // `getCharacterIterator` preceding the call to `advance` or `acceptSetOperator` must no longer be
    // used.
    नियमः_Character_Iterator &getCharacterIterator() {
        // Make sure we compute a correct `ahead_.after_` so we do not depend on the current value of
        // `getPos()` for lexing.
        lookahead();
        प्रत्यागम chars_;
    }

    int32_t charsOptions() {
        प्रत्यागम charsOptions_;
    }

    तार्किक at_अन्तः() ध्रुव {
        प्रत्यागम chars_.at_अन्तः();
    }

    शून्य advance() {
        // If someone called `getCharacterIterator`, we are now changing the character iterator under
        // their feet; further, we may not have an `ahead_`, so if they keep playing with it we would be
        // working on incorrect values of `getPos`.  This is why the result of `getCharacterIterator`
        // must no longer be used.
        chars_.समुच्चयः_Pos(lookahead().after_);
        behind_ = ahead_;
        ahead_ = ahead2_;
        ahead2_.reset();
    }

  गुप्त:
    // A version of getPos that returns its position instead of taking it as at out parameter, so we
    // can have const positions.
    नियमः_Character_Iterator::Pos getPos() ध्रुव {
        नियमः_Character_Iterator::Pos परिणाम;
        chars_.getPos(परिणाम);
        प्रत्यागम परिणाम;
    }

    LexicalElement अग्र_Token(std::optional<LexicalElement> ध्रुव& preceding) {
        UError_संहिता error_संहिता = U_ZERO_ERROR;
        तार्किक separated = असत्य;
        यदि (charsOptions_ & नियमः_Character_Iterator::SKIP_WHITESPACE) {
            UChar32 s;
            नियमः_Character_Iterator::Pos pos;
            प्रति (;;) {
                UBool अप्रयुक्तम्_Escaped;
                chars_.getPos(pos);
                s = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                  नियमः_Character_Iterator::SKIP_WHITESPACE),
                                अप्रयुक्तम्_Escaped, error_संहिता);
                यदि (!आदर्शः_Props::isWhiteSpace(s)) {
                    chars_.समुच्चयः_Pos(pos);
                    भङ्ग;
                }
                यदि (s != u'\u200E' && s != u'\u200F') {
                    separated = सत्य;
                }
            }
        }
        chars_.skipIgnored(charsOptions_);
        यदि (chars_.at_अन्तः()) {
            प्रत्यागम LexicalElement(LexicalElement::अन्तः_सम्बन्धि_TEXT, {}, getPos(), error_संहिता,
                                  /*precomputedSet=*/शून्यसूचकः,
                                  /*set=*/{},
                                  u"", separated, preceding);
        }
        ध्रुव int32_t start = parsePosition_.get_सूचकांक();
        ध्रुव नियमः_Character_Iterator::Pos before = getPos();
        // First try to get the next character without parsing escapes.
        UBool अप्रयुक्तम्_Escaped;
        ध्रुव UChar32 first =
            chars_.next(charsOptions_ & ~नियमः_Character_Iterator::PARSE_ESCAPES, अप्रयुक्तम्_Escaped, error_संहिता);
        यदि (first == u'[' || first == u'\\') {
            ध्रुव नियमः_Character_Iterator::Pos afterFirst = getPos();
            // This could be a property-query or named-element.
            ध्रुव UChar32 second = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                                 नियमः_Character_Iterator::SKIP_WHITESPACE),
                                               अप्रयुक्तम्_Escaped, error_संहिता);
            यदि ((first == u'[' && second == u':') ||
                (first == u'\\' && (second == u'p' || second == u'P' || second == u'N'))) {
                यदि (second == u'N') {
                    UChar32 ध्रुव query_परिणाम = अवलोकनम्_Named_Element_Brackets(error_संहिता);
                    प्रत्यागम LexicalElement(
                        LexicalElement::NAMED_ELEMENT, UnicodeString(query_परिणाम), getPos(), error_संहिता,
                        /*precomputedSet=*/शून्यसूचकः,
                        /*set=*/{},
                        std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                } अन्यथा {
                    Unicode_समुच्चयः query_परिणाम = अवलोकनम्_Property_Query_After_Start(first, second, start, error_संहिता);
                    प्रत्यागम LexicalElement(
                        LexicalElement::PROPERTY_QUERY, {}, getPos(), error_संहिता,
                        /*precomputedSet=*/शून्यसूचकः, /*set=*/std::move(query_परिणाम),
                        std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                }
            }
            // Not a property-query.
            chars_.समुच्चयः_Pos(afterFirst);
        }
        यदि (first == u'$' && symbols_ != शून्यसूचकः) {
            स्वचालित name_अन्तः = parsePosition_;
            // The SymbolTable defines the lexing of variable names past the $.
            यदि (UnicodeString name = symbols_->parseReference(आदर्शः_2, name_अन्तः, आदर्शः_2.length());
                // LexicalElements returned here and from evaluateVariable are always constructed
                // with separated=true, preceding=nullopt: the separation constraints are lexical,
                // and variables are their own lexical elements; their values are not affected by
                // lexical constraints.
                !name.is_रिक्तानि()) {
                chars_.jumpahead(name_अन्तः.get_सूचकांक() - (start + 1));
                ध्रुव std::u16string_view source =
                    std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start);
                ध्रुव Unicode_समुच्चयः *precomputed_समुच्चयः_2 = symbols_->lookup_समुच्चयः(name);
                यदि (precomputed_समुच्चयः_2 != शून्यसूचकः) {
                    प्रत्यागम LexicalElement(LexicalElement::VARIABLE, {}, getPos(), U_ZERO_ERROR,
                                          precomputed_समुच्चयः_2, /*set=*/{}, source, /*separated=*/सत्य,
                                          /*preceding=*/std::nullopt);
                }
                // The variable was not a precomputed set.  Use the old-fashioned `lookup`, which
                // should give us its source text; if that parses as a single set or element, use
                // it.  Note that variables are not allowed in that expansion.
                // Implementers of higher-level syntaxes that pre-parse UnicodeSet-valued variables
                // can use variables in their variable definitions, but those that simply use the
                // source text substitution API cannot.
                ध्रुव UnicodeString *ध्रुव expression = symbols_->lookup(name);
                यदि (expression == शून्यसूचकः) {
                    प्रत्यागम LexicalElement(
                        LexicalElement::VARIABLE, {}, getPos(), U_UNDEFINED_VARIABLE,
                        /*precomputedSet=*/शून्यसूचकः,
                        /*set=*/{},
                        source,
                        /*separated=*/सत्य,
                        /*preceding=*/std::nullopt);
                }
                प्रत्यागम evaluateVariable(*expression, source);
            }
        }
        विकल्प (first) {
        पक्ष u'[':
            प्रत्यागम LexicalElement(
                LexicalElement::समुच्चयः_OPERATOR, UnicodeString(u'['), getPos(), error_संहिता,
                /*precomputedSet=*/शून्यसूचकः,
                /*set=*/{},
                std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        पक्ष u'\\': {
            // Now try to parse the escape.
            chars_.समुच्चयः_Pos(before);
            UChar32 संहिता_Point = chars_.next(charsOptions_, अप्रयुक्तम्_Escaped, error_संहिता);
            प्रत्यागम LexicalElement(
                LexicalElement::ESCAPED_ELEMENT,
                UnicodeString(संहिता_Point), getPos(), error_संहिता,
                शून्यसूचकः,
                /*set=*/{},
                std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
        पक्ष u'&':
        पक्ष u'-':
        पक्ष u']':
        पक्ष u'^':
        पक्ष u'$':
            // We make $ a set-operator to handle the ICU extensions involving $.
            प्रत्यागम LexicalElement(
                LexicalElement::समुच्चयः_OPERATOR, UnicodeString(first), getPos(), error_संहिता,
                /*precomputedSet=*/शून्यसूचकः,
                /*set=*/{},
                std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        पक्ष u'{': {
            UnicodeString string;
            UBool escaped;
            UChar32 next;
            int32_t संहिता_Point_गणनम् = 0;
            यावत् (!chars_.at_अन्तः() && U_SUCCESS(error_संहिता)) {
                ध्रुव नियमः_Character_Iterator::Pos before_अग्र = getPos();
                next = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                     नियमः_Character_Iterator::SKIP_WHITESPACE),
                                   अप्रयुक्तम्_Escaped, error_संहिता);
                यदि (next == u'\\') {
                    ध्रुव UChar32 afterBackslash =
                        chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                      नियमः_Character_Iterator::SKIP_WHITESPACE),
                                    अप्रयुक्तम्_Escaped, error_संहिता);
                    यदि (afterBackslash == u'N') {
                        next = अवलोकनम्_Named_Element_Brackets(error_संहिता);
                        escaped = सत्य;
                    } अन्यथा यदि (afterBackslash == u'p' || afterBackslash == u'P') {
                        प्रत्यागम LexicalElement(LexicalElement::STRING_LITERAL, {}, getPos(),
                                              U_MALFORMED_समुच्चयः,
                                              /*precomputedSet=*/शून्यसूचकः,
                                              /*set=*/{},
                                              std::u16string_view(आदर्शः_2).substr(
                                                  start, parsePosition_.get_सूचकांक() - start),
                                              separated, preceding);
                    } अन्यथा {
                        chars_.समुच्चयः_Pos(before_अग्र);
                        // Parse the escape.
                        next = chars_.next(charsOptions_, escaped, error_संहिता);
                    }
                } अन्यथा {
#यदि U_ICU_संस्करणम्_MAJOR_अङ्कः < 81
                    यदि (U_SUCCESS(error_संहिता) && आदर्शः_Props::isWhiteSpace(next)) {
                        // Transitional prohibition of unescaped spaces in string literals (in
                        // ICU 78 and earlier, these were ignored; in ICU 81 they will mean
                        // themselves).
                        error_संहिता = UError_संहिता::U_ILLEGAL_तर्कः_ERROR;
                    }
#अन्यथा
#दोषः Remove एतत् transitional check, see ICU-23307 and ICU-TC minutes of 2026-01-16.
#यदि_अन्तः
                    escaped = असत्य;
                }
                यदि (!escaped && next == u'}') {
                    प्रत्यागम LexicalElement(
                        संहिता_Point_गणनम् == 1 ? LexicalElement::BRACKETED_ELEMENT
                                            : LexicalElement::STRING_LITERAL,
                        std::move(string), getPos(), error_संहिता,
                        /*precomputedSet=*/शून्यसूचकः,
                        /*set=*/{},
                        std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                        separated, preceding);
                }
                string.append(next);
                संहिता_Point_गणनम् += 1;
            }
            प्रत्यागम LexicalElement(
                LexicalElement::STRING_LITERAL, {}, getPos(), U_MALFORMED_समुच्चयः,
                /*precomputedSet=*/शून्यसूचकः,
                /*set=*/{},
                std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
        स्वभाव:
            प्रत्यागम LexicalElement(
                LexicalElement::LITERAL_ELEMENT, UnicodeString(first), getPos(), error_संहिता, शून्यसूचकः,
                /*set=*/{},
                std::u16string_view(आदर्शः_2).substr(start, parsePosition_.get_सूचकांक() - start),
                separated, preceding);
        }
    }

    UChar32 अवलोकनम्_Named_Element_Brackets(UError_संहिता &error_संहिता) {
        UBool अप्रयुक्तम्_Escaped;
        ध्रुव UChar32 उद्घाटय = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                           नियमः_Character_Iterator::SKIP_WHITESPACE),
                                         अप्रयुक्तम्_Escaped, error_संहिता);
        यदि (उद्घाटय == u'{') {
            int32_t start = parsePosition_.get_सूचकांक();
            std::optional<UChar32> hex;
            std::optional<UChar32> literal;
            यावत् (!chars_.at_अन्तः() && U_SUCCESS(error_संहिता)) {
                UChar32 चरमः = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                             नियमः_Character_Iterator::SKIP_WHITESPACE),
                                           अप्रयुक्तम्_Escaped, error_संहिता);
                यदि (चरमः == u':') {
                    यदि (!hex.has_value()) {
                        hex.emplace();
                        यदि (start == parsePosition_.get_सूचकांक() - 1) {
                            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                            प्रत्यागम {};
                        }
                        प्रति (char16_t digit : std::u16string_view(आदर्शः_2).substr(
                                 start, parsePosition_.get_सूचकांक() - 1 - start)) {
                            uint8_t nibble;
                            यदि (digit >= u'0' && digit <= u'9') {
                                nibble = digit - '0';
                            } अन्यथा {
                                digit = digit & ~0x20;
                                यदि (digit >= u'A' && digit <= u'F') {
                                    nibble = digit - u'A' + 0xA;
                                } अन्यथा {
                                    error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                                    प्रत्यागम {};
                                }
                            }
                            *hex = (*hex << 4) + nibble;
                            यदि (hex > 0x10FFFF) {
                                error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                                प्रत्यागम {};
                            }
                        }
                    } अन्यथा यदि (!literal.has_value()) {
                        ध्रुव स्वचालित literal_संहिता_Points = utf_String_संहिता_Points<UChar32, UTF_BEHAVIOR_FFFD>(
                            std::u16string_view(आदर्शः_2).substr(start,
                                                                 parsePosition_.get_सूचकांक() - 1 - start));
                        स्वचालित it = literal_संहिता_Points.begin();
                        यदि (it == literal_संहिता_Points.अन्तः() || !it->wellFormed() ||
                                (literal = it->संहिता_Point(), ++it) != literal_संहिता_Points.अन्तः()) {
                            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                            प्रत्यागम {};
                        }
                    } अन्यथा {
                        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                        प्रत्यागम {};
                    }
                    start = parsePosition_.get_सूचकांक();
                } अन्यथा यदि (चरमः == u'}') {
                    ध्रुव std::u16string_view u16name = std::u16string_view(आदर्शः_2).substr(
                        start, parsePosition_.get_सूचकांक() - 1 - start);
                    ध्रुव UChar32 परिणाम = getCharacterByName(CharString().appendInvariantChars(
                        u16name.दत्त(), static_cast<int32_t>(u16name.length()), error_संहिता));
                    यदि (!U_SUCCESS(error_संहिता)) {
                        // Convert U_INVARIANT_CONVERSION_ERROR to U_ILLEGAL_ARGUMENT_ERROR.
                        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                        प्रत्यागम {};
                    }
                    यदि (परिणाम < 0 || (hex.has_value() && परिणाम != hex) ||
                            (literal.has_value() && परिणाम != literal)) {
                        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                        प्रत्यागम {};
                    }
                    प्रत्यागम परिणाम;
                }
            }
        }
        यदि (U_SUCCESS(error_संहिता)) {
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        }
        प्रत्यागम {};
    }

    LexicalElement evaluateVariable(ध्रुव UnicodeString &expression,
                                    ध्रुव std::u16string_view source) {
        UError_संहिता error_संहिता = U_ZERO_ERROR;
        ParsePosition expressionPosition;
        नियमः_Character_Iterator expressionIterator(expression, symbols_, expressionPosition);
        // Do not pass the symbols: we do not support recursive expansion of variables.
        Lexer expressionLexer(expression, expressionPosition, expressionIterator, unicode_समुच्चयः_Options_2,
                              /*symbols=*/शून्यसूचकः, case_संवृतिः_2);
        स्वचालित variableToken = expressionLexer.lookahead();
        यदि (variableToken.is_समुच्चयः_Operator(u'[')) {
            UnicodeString rebuilt_आदर्शः;
            Unicode_समुच्चयः expression_मानम् =
                parse_Unicode_समुच्चयः(expressionLexer, rebuilt_आदर्शः, unicode_समुच्चयः_Options_2, case_संवृतिः_2,
                                /*depth=*/0, error_संहिता);
            expression_मानम्.समुच्चयः_आदर्शः(rebuilt_आदर्शः);
            यदि (!expressionLexer.at_अन्तः()) {
                प्रत्यागम LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/शून्यसूचकः,
                    /*set=*/{},
                    source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            }
            प्रत्यागम LexicalElement(
                LexicalElement::VARIABLE, {}, getPos(), error_संहिता,
                /*precomputedSet=*/शून्यसूचकः,
                /*set=*/std::move(expression_मानम्),
                source,
                /*separated=*/सत्य,
                /*preceding=*/std::nullopt);
        } अन्यथा {
            expressionLexer.advance();
            यदि (!expressionLexer.at_अन्तः()) {
                प्रत्यागम LexicalElement(
                    LexicalElement::VARIABLE, {}, getPos(), U_MALFORMED_VARIABLE_DEFINITION,
                    /*precomputedSet=*/शून्यसूचकः,
                    /*set=*/{},
                    source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            }
            विकल्प (variableToken.category_) {
            पक्ष LexicalElement::LITERAL_ELEMENT:
            पक्ष LexicalElement::ESCAPED_ELEMENT:
            पक्ष LexicalElement::NAMED_ELEMENT:
            पक्ष LexicalElement::BRACKETED_ELEMENT:
            पक्ष LexicalElement::STRING_LITERAL:
            पक्ष LexicalElement::PROPERTY_QUERY:
                // Return the same lexical element that we found while parsing the variable contents,
                // except the source position corresponds to the position of the variable rather than 0
                // in its expansion, and the source is the name of the variable rather than its
                // expansion.
                प्रत्यागम LexicalElement(
                    variableToken.category_, std::move(variableToken.string_), getPos(),
                    variableToken.error_संहिता_2, variableToken.precomputed_समुच्चयः, std::move(variableToken.समुच्चयः_2), source,
                    /*separated=*/सत्य,
                    /*preceding=*/std::nullopt);
            स्वभाव:
                प्रत्यागम LexicalElement(LexicalElement::VARIABLE, {}, getPos(),
                                      U_MALFORMED_VARIABLE_DEFINITION,
                                      /*precomputedSet=*/शून्यसूचकः,
                                      /*set=*/{}, source,
                                      /*separated=*/सत्य,
                                      /*preceding=*/std::nullopt);
            }
        }
    }

    Unicode_समुच्चयः अवलोकनम्_Property_Query_After_Start(UChar32 first, UChar32 second, int32_t queryStart, UError_संहिता &error_संहिता) {
        std::optional<int32_t> queryOperatorPosition;
        int32_t queryExpressionStart = parsePosition_.get_सूचकांक();
        तार्किक exteriorlyNegated = असत्य;
        तार्किक interiorlyNegated = असत्य;
        UBool अप्रयुक्तम्_Escaped;
        // Do not skip whitespace so we can recognize unspaced :].  Lex escapes and
        // named-element: while ICU does not support string-valued properties and thus has no
        // use for escapes, we still want to lex through escapes to allow downstream
        // implementations (mostly unicodetools) to implement string-valued properties.
        ध्रुव UChar32 third = chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                                            नियमः_Character_Iterator::SKIP_WHITESPACE),
                                          अप्रयुक्तम्_Escaped, error_संहिता);
        यदि (first == u'\\') {
            यदि (third != u'{') {
                error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम {};
            }
            exteriorlyNegated = second == u'P';
            queryExpressionStart = parsePosition_.get_सूचकांक();
        } अन्यथा {
            यदि (third == u'^') {
                exteriorlyNegated = सत्य;
                queryExpressionStart = parsePosition_.get_सूचकांक();
            }
        }
        नियमः_Character_Iterator::Pos beforePenultimate = getPos();
        UChar32 penultimateUnescaped =
            chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                          नियमः_Character_Iterator::SKIP_WHITESPACE),
                        अप्रयुक्तम्_Escaped, error_संहिता);

        यावत् (!chars_.at_अन्तः() && U_SUCCESS(error_संहिता)) {
            ध्रुव नियमः_Character_Iterator::Pos before_चरमः = getPos();
            UChar32 चरमः_Unescaped =
                chars_.next(charsOptions_ & ~(नियमः_Character_Iterator::PARSE_ESCAPES |
                                              नियमः_Character_Iterator::SKIP_WHITESPACE),
                            अप्रयुक्तम्_Escaped, error_संहिता);
            यदि (penultimateUnescaped == u'\\') {
                यदि (चरमः_Unescaped == 'N') {
                    अवलोकनम्_Named_Element_Brackets(error_संहिता);
                    यदि (!U_SUCCESS(error_संहिता)) {
                        प्रत्यागम {};
                    }
                } अन्यथा {
                    // There must be an escaped-element starting at beforePenultimate.  Go
                    // back there and advance through it.
                    chars_.समुच्चयः_Pos(beforePenultimate);
                    chars_.next(charsOptions_ & ~नियमः_Character_Iterator::SKIP_WHITESPACE, अप्रयुक्तम्_Escaped,
                                error_संहिता);
                }
                // Neither a named-element nor an escaped-element can be part of a closing :].
                चरमः_Unescaped = -1;
            } अन्यथा यदि (!queryOperatorPosition.has_value() && चरमः_Unescaped == u'=') {
                queryOperatorPosition = parsePosition_.get_सूचकांक() - 1;
            } अन्यथा यदि (!queryOperatorPosition.has_value() && चरमः_Unescaped == u'≠') {
                यदि (exteriorlyNegated) {
                    // Reject doubly negated property queries.
                    error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                    प्रत्यागम {};
                }
                interiorlyNegated = सत्य;
                queryOperatorPosition = parsePosition_.get_सूचकांक() - 1;
            } अन्यथा यदि ((first == u'[' && penultimateUnescaped == u':' && चरमः_Unescaped == u']') ||
                       (first == u'\\' && चरमः_Unescaped == u'}')) {
                // Note that no unescaping is performed here, as ICU does not support string-valued or
                // or miscellaneous properties.
                ध्रुव int32_t query_Expression_सीमा =
                    first == u'[' ? parsePosition_.get_सूचकांक() - 2 : parsePosition_.get_सूचकांक() - 1;
                // Contrary to Java, applyPropertyAlias does not support a null property-predicate in
                // C++; instead "" indicates the absence of a property-predicate.  This is OK with the
                // properties supported by ICU, but not with string-valued or miscellaneous properties;
                // see https://github.com/unicode-org/icu/pull/3456.
                UnicodeString propertyPredicate;
                यदि (queryOperatorPosition.has_value()) {
                    propertyPredicate =
                        आदर्शः_2.अस्थायी_Sub_String_Between(*queryOperatorPosition + 1, query_Expression_सीमा);
                    यदि (propertyPredicate.is_रिक्तानि()) {
                        // \p{X=} is valid if X is a string-valued or miscellaneous property, but
                        // ICU does not support those.  Thus, it is invalid for ICU purposes, and
                        // passing an empty propertyPredicate to applyPropertyAlias can be valid
                        // (this is how we represent \p{X}), so we need to return the error here.
                        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                        प्रत्यागम {};
                    }
                }
                Unicode_समुच्चयः परिणाम;
                परिणाम.applyPropertyAlias(
                    आदर्शः_2.अस्थायी_Sub_String_Between(queryExpressionStart,
                                                  queryOperatorPosition.value_or(query_Expression_सीमा)),
                    propertyPredicate, error_संहिता);
                यदि (exteriorlyNegated != interiorlyNegated) {
                    परिणाम.complement().remove_All_वाक्यानि();
                }
                परिणाम.समुच्चयः_आदर्शः(आदर्शः_2.अस्थायी_Sub_String_Between(queryStart, parsePosition_.get_सूचकांक()));
                प्रत्यागम परिणाम;
            }
            beforePenultimate = before_चरमः;
            penultimateUnescaped = चरमः_Unescaped;
        }
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम {};
    }

    ध्रुव UnicodeString &आदर्शः_2;
    ध्रुव ParsePosition &parsePosition_;
    नियमः_Character_Iterator &chars_;
    ध्रुव uint32_t unicode_समुच्चयः_Options_2;
    ध्रुव int32_t charsOptions_;
    ध्रुव चिह्नम्_सारणी *ध्रुव symbols_;
    Unicode_समुच्चयः &(Unicode_समुच्चयः::* ध्रुव case_संवृतिः_2)(int32_t attribute);
    std::optional<LexicalElement> behind_;
    std::optional<LexicalElement> ahead_;
    std::optional<LexicalElement> ahead2_;
};

नामाकाश {

constexpr int32_t MAX_गभीरता = 100;

#यदि U_DEBUGGING_UNICODESET_PARSING

#परिभाषय U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec)                                                   \
    कुरु {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        यदि (U_FAILURE(ec)) {                                                                       \
            यदि (गभीरता < 5) {                                                                       \
                प्रारूप_लिख("--- in %s l. %d\n", __func__ + 5, __LINE__);                               \
            } अन्यथा यदि (गभीरता == 5 && std::string_view(__func__ + 5) == "UnicodeSet") {             \
                प्रारूप_लिख("--- [...]\n");                                                             \
            }                                                                                      \
            प्रत्यागम परिणाम;                                                                         \
        }                                                                                          \
    } यावत् (असत्य)
#परिभाषय U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, expected, actual, lexer, ec)                  \
    कुरु {                                                                                           \
        constexpr std::string_view functionName = __func__;                                        \
        static_assert(functionName.substr(0, 5) == "parse");                                       \
        std::string actualUTF8;                                                                    \
        std::string contextUTF8;                                                                   \
        प्रारूप_लिख("*** Expected %s, got %s %s\n", (expected),                                         \
               UnicodeString(actual).गन्तव्यम्_UTF8_String(actualUTF8).c_str(),                             \
               lexer.getPositionForDebugging().गन्तव्यम्_UTF8_String(contextUTF8).c_str());                 \
        प्रारूप_लिख("--- in %s l. %d\n", __func__ + 5, __LINE__);                                       \
        यदि (U_FAILURE(lexer.lookahead().error_संहिता())) {                                            \
            (ec) = lexer.lookahead().error_संहिता();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_समुच्चयः;                                                                \
        }                                                                                          \
        प्रत्यागम परिणाम;                                                                             \
    } यावत् (असत्य)

#अन्यथा

#परिभाषय U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec)                                                   \
    कुरु {                                                                                           \
        यदि (U_FAILURE(ec)) {                                                                       \
            प्रत्यागम परिणाम;                                                                         \
        }                                                                                          \
    } यावत् (असत्य)
#परिभाषय U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, expected, actual, lexer, ec)                  \
    कुरु {                                                                                           \
        यदि (U_FAILURE(lexer.lookahead().error_संहिता())) {                                            \
            (ec) = lexer.lookahead().error_संहिता();                                                  \
        } अन्यथा {                                                                                   \
            (ec) = U_MALFORMED_समुच्चयः;                                                                \
        }                                                                                          \
        प्रत्यागम परिणाम;                                                                             \
    } यावत् (असत्य)

#यदि_अन्तः

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

शून्य Unicode_समुच्चयः::apply_आदर्शः(ध्रुव UnicodeString &आदर्शः,
                              ध्रुव ParsePosition &parsePosition,
                              नियमः_Character_Iterator &chars,
                              ध्रुव चिह्नम्_सारणी *symbols,
                              UnicodeString &rebuiltPat,
                              uint32_t options,
                              Unicode_समुच्चयः &(Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute),
                              UError_संहिता &ec) {
    यदि (U_FAILURE(ec)) प्रत्यागम;
    Lexer lexer(आदर्शः, parsePosition, chars, options, symbols, case_संवृतिः);
    *एतत् = parse_Unicode_समुच्चयः(lexer, rebuiltPat, options, case_संवृतिः, /*depth=*/0, ec);
}

Unicode_समुच्चयः Unicode_समुच्चयः::parse_Unicode_समुच्चयः(Lexer &lexer,
                                       UnicodeString& rebuiltPat,
                                       uint32_t options,
                                       Unicode_समुच्चयः& (Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute),
                                       int32_t गभीरता,
                                       UError_संहिता &ec) {
    Unicode_समुच्चयः परिणाम;

    यदि (गभीरता > MAX_गभीरता) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, ("depth <= " + std::to_string(MAX_गभीरता)).c_str(),
                                             ("depth = " + std::to_string(गभीरता)).c_str(), lexer,
                                             ec);
    }

    तार्किक isComplement = असत्य;
    // Whether to keep the syntax of the pattern at this level, only doing basic pretty-printing, e.g.,
    // turn [ c - z[a]a - b ] into [c-z[a]a-b], but not into [a-z].
    // This is true for a property query, or when there is a nested set.  Note that since we recurse,
    // innermost sets consisting only of ranges will get simplified.
    तार्किक preserve_Syntax_अन्तर्_आदर्शः = असत्य;
    // A pattern that preserves the original syntax but strips spaces, normalizes escaping, etc.
    UnicodeString pretty_Printed_आदर्शः;
    यदि (lexer.lookahead().समुच्चयः() != शून्यसूचकः) {
        // UnicodeSet ::= property-query
        // Extension:
        //              | set-valued-variable
        परिणाम = *lexer.lookahead().समुच्चयः();
        परिणाम.गन्तव्यम्_आदर्शः_2(pretty_Printed_आदर्शः, /*escapeUnprintable=*/असत्य);
        lexer.advance();
        preserve_Syntax_अन्तर्_आदर्शः = सत्य;
    } अन्यथा {
        // UnicodeSet ::=                [   Content ]
        //              | Complement ::= [ ^ Content ]
        यदि (lexer.accept_समुच्चयः_Operator(u'[')) {
            pretty_Printed_आदर्शः.append(u'[');
            यदि (lexer.accept_समुच्चयः_Operator(u'^')) {
                pretty_Printed_आदर्शः.append(u'^');
                isComplement = सत्य;
            }
            परिणाम = parseContent(lexer, pretty_Printed_आदर्शः, options, case_संवृतिः, गभीरता,
                                  /*containsSetOperation=*/preserve_Syntax_अन्तर्_आदर्शः, ec);
            U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec);
            यदि (!lexer.accept_समुच्चयः_Operator(u']')) {
                U_UNICODESET_RETURN_WITH_PARSE_ERROR(परिणाम, "]", lexer.lookahead().debugString(),
                                                     lexer, ec);
            }
            pretty_Printed_आदर्शः.append(u']');
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
    यदि ((options & USET_CASE_आवरणम्) != 0) {
        (परिणाम.*case_संवृतिः)(options);
    }
    यदि (isComplement) {
        परिणाम.complement().remove_All_वाक्यानि(); // code point complement
    }
    यदि (preserve_Syntax_अन्तर्_आदर्शः) {
        rebuiltPat.append(pretty_Printed_आदर्शः);
    } अन्यथा {
        परिणाम.generate_आदर्शः(rebuiltPat, /*escapeUnprintable=*/असत्य);
    }
    प्रत्यागम परिणाम;
}

Unicode_समुच्चयः Unicode_समुच्चयः::parseContent(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_समुच्चयः &(Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute),
                                    int32_t गभीरता,
                                    तार्किक &अन्तर्भवति_समुच्चयः_Operation,
                                    UError_संहिता &ec) {
    Unicode_समुच्चयः परिणाम;
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
    यदि (lexer.accept_समुच्चयः_Operator(u'-')) {
        परिणाम.सङ्कलय(u'-');
        // When we otherwise preserve the syntax, we escape an initial UnescapedHyphenMinus, but not a
        // final one, for consistency with older ICU behaviour.
        rebuiltPat.append(u"\\-");
    }
    यावत् (!lexer.at_अन्तः()) {
        यदि (lexer.accept_समुच्चयः_Operator(u'-')) {
            // We can be here on the first iteration: [--] is allowed by the
            // grammar and by the old parser.
            rebuiltPat.append(u'-');
            परिणाम.सङ्कलय(u'-');
            प्रत्यागम परिणाम;
        } अन्यथा यदि (lexer.lookahead().is_समुच्चयः_Operator(u'$')) {
            यदि (lexer.lookahead2().is_समुच्चयः_Operator(u']')) {
                // ICU extensions: A $ is allowed in an ElementList if followed by Elements, or
                // if followed by UnicodeSet (in a Union).
                // A $ at the end of a Content is an Æther.
                rebuiltPat.append(u'$');
                // Consume the dollar.
                lexer.advance();
                परिणाम.सङ्कलय(U_ETHER);
                अन्तर्भवति_समुच्चयः_Operation = सत्य;
                प्रत्यागम परिणाम;
            }
        }
        यदि (lexer.lookahead().is_समुच्चयः_Operator(u']')) {
            प्रत्यागम परिणाम;
        }
        // Also handles FinalMutation.
        परिणाम.parseMutation(lexer, rebuiltPat, options, case_संवृतिः, गभीरता, अन्तर्भवति_समुच्चयः_Operation,
                             ec);
        U_UNICODESET_RETURN_IF_ERROR(परिणाम, ec);
    }
    प्रत्यागम परिणाम;
}

शून्य Unicode_समुच्चयः::parseMutation(Lexer &lexer,
                               UnicodeString &rebuiltPat,
                               uint32_t options,
                               Unicode_समुच्चयः &(Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute),
                               int32_t गभीरता,
                               तार्किक &अन्तर्भवति_समुच्चयः_Operation,
                               UError_संहिता &ec) {
    //   Mutation ::= Elements
    //              | SetOperations
    //              | DollarElements  -- ICU extension
    //   FinalMutation ::= Elements
    //                   | SetOperations
    //   SetOperations ::= UnicodeSet RightHandSides
    // So if we see the beginning of a UnicodeSet, we have SetOperations.
    यदि (lexer.lookahead().is_समुच्चयः_Operator('[') || lexer.lookahead().समुच्चयः() != शून्यसूचकः) {
        अन्तर्भवति_समुच्चयः_Operation = सत्य;
        parse_समुच्चयः_Operations(lexer, rebuiltPat, options, case_संवृतिः, गभीरता, ec);
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    } अन्यथा {
        // Also handles DollarElements.
        सङ्कलय_All(parseElements(lexer, rebuiltPat, ec));
        U_UNICODESET_RETURN_IF_ERROR(, ec);
    }
}

शून्य Unicode_समुच्चयः::parse_समुच्चयः_Operations(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    uint32_t options,
                                    Unicode_समुच्चयः &(Unicode_समुच्चयः::*case_संवृतिः)(int32_t attribute),
                                    int32_t गभीरता,
                                    UError_संहिता &ec) {
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
    ध्रुव Unicode_समुच्चयः वामः_Hand_Side =
        parse_Unicode_समुच्चयः(lexer, rebuiltPat, options, case_संवृतिः, गभीरता + 1, ec);
    सङ्कलय_All(वामः_Hand_Side);
    U_UNICODESET_RETURN_IF_ERROR(, ec);
    // In terms of the LR expression grammar, at this point this object is the Union, which is a
    // SetOperation.

    // Keep looking for an operator that would continue the RightHandSides in the LL grammar.
    // The loop terminates because when we run out of source text, the lookahead token will not be a
    // set operator, so that we hit the else branch and return.
    प्रति (;;) {
        // In terms of the LR expression grammar, at this point this object is a SetOperation; if &
        // follows, we have an Intersection; if - follows, we have a Difference.
        यदि (lexer.accept_समुच्चयः_Operator(u'&')) {
            // Intersection ::= SetOperation & UnicodeSet
            rebuiltPat.append(u'&');
            ध्रुव Unicode_समुच्चयः दक्षिणः_Hand_Side =
                parse_Unicode_समुच्चयः(lexer, rebuiltPat, options, case_संवृतिः, गभीरता + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            retainAll(दक्षिणः_Hand_Side);
        } अन्यथा यदि (lexer.lookahead().is_समुच्चयः_Operator(u'-')) {
            // Here the grammar requires two tokens of lookahead to figure out whether the - is
            // the operator of a Difference or an UnescapedHyphenMinus in the enclosing Union.
            यदि (lexer.lookahead2().is_समुच्चयः_Operator(u']')) {
                // The operator is actually an UnescapedHyphenMinus; terminate the SetOperation
                // before it.  We return to parseMutation, which immediately returns to
                // parseContent, which will accept the - and add it to *this.
                प्रत्यागम;
            }
            // Consume the hyphen-minus.
            lexer.advance();
            // Difference ::= SetOperation - UnicodeSet
            rebuiltPat.append(u'-');
            ध्रुव Unicode_समुच्चयः दक्षिणः_Hand_Side =
                parse_Unicode_समुच्चयः(lexer, rebuiltPat, options, case_संवृतिः, गभीरता + 1, ec);
            U_UNICODESET_RETURN_IF_ERROR(, ec);
            removeAll(दक्षिणः_Hand_Side);
        } अन्यथा {
            // Not an operator, end of the SetOperation (and of the SetOperations in the LL
            // grammar).
            प्रत्यागम;
        }
    }
}

Unicode_समुच्चयः Unicode_समुच्चयः::parseElements(Lexer &lexer,
                                     UnicodeString &rebuiltPat,
                                     UError_संहिता &ec) {
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
    यदि (lexer.lookahead().isStringLiteral()) {
        ध्रुव UnicodeString element = *lexer.lookahead().element();
        rebuiltPat.append(u'{');
        append_गन्तव्यम्_Pat(rebuiltPat, element, /*escapeUnprintable=*/असत्य);
        rebuiltPat.append(u'}');
        lexer.advance();
        प्रत्यागम Unicode_समुच्चयः().सङ्कलय(element);
    }
    UChar32 first;
    यदि (lexer.lookahead().is_समुच्चयः_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU.  This is an extension.
        first = u'$';
    } अन्यथा यदि (lexer.lookahead().संहिता_Point().has_value()) {
        first = *lexer.lookahead().संहिता_Point();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement | string-literal",
                                             lexer.lookahead().debugString(), lexer, ec);
    }
    lexer.advance();
    append_गन्तव्यम्_Pat(rebuiltPat, first, /*escapeUnprintable=*/असत्य);
    यदि (!lexer.lookahead().is_समुच्चयः_Operator(u'-')) {
        // No operator,
        // Elements ::= Element
        प्रत्यागम Unicode_समुच्चयः(first, first);
    }
    // Here the grammar requires two tokens of lookahead to figure out whether the - is the operator
    // of a Range or an UnescapedHyphenMinus in the enclosing Union.
    यदि (lexer.lookahead2().is_समुच्चयः_Operator(u']')) {
        // The operator is actually an UnescapedHyphenMinus; terminate the Elements before it.
        प्रत्यागम Unicode_समुच्चयः(first, first);
    }
    // Consume the hyphen-minus.
    lexer.advance();
    // Elements ::= Range ::= RangeElement - RangeElement
    rebuiltPat.append(u'-');
    UChar32 चरमः;
    यदि (lexer.lookahead().is_समुच्चयः_Operator(u'$')) {
        // Disallowed by UTS #61, but historically accepted by ICU except at the end of a Content.
        // This is an extension.
        चरमः = u'$';
        यदि (lexer.lookahead2().is_समुच्चयः_Operator(u']')) {
            U_UNICODESET_RETURN_WITH_PARSE_ERROR(
                {}, "Elements or UnicodeSet after Range ending in unescaped $",
                lexer.lookahead().debugString() + u" followed by " +
                    lexer.lookahead2().debugString(),
                lexer, ec);
        }
    } अन्यथा यदि (lexer.lookahead().संहिता_Point().has_value()) {
        चरमः = *lexer.lookahead().संहिता_Point();
    } अन्यथा {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "RangeElement", lexer.lookahead().debugString(),
                                             lexer, ec);
    }
    यदि (चरमः <= first) {
        U_UNICODESET_RETURN_WITH_PARSE_ERROR({}, "first < last in Range",
                                             UnicodeString(चरमः) + u"-" + UnicodeString(first),
                                             lexer, ec);
    }
    lexer.advance();
    append_गन्तव्यम्_Pat(rebuiltPat, चरमः, /*escapeUnprintable=*/असत्य);
    प्रत्यागम Unicode_समुच्चयः(first, चरमः);
}

//----------------------------------------------------------------
// Property set implementation
//----------------------------------------------------------------

नामाकाश {

UBool numeric_मानम्_Filter(UChar32 ch, शून्य* context) {
    प्रत्यागम u_get_Numeric_मानम्(ch) == *static_cast<द्विगुण*>(context);
}

UBool general_Category_आवरणम्_Filter(UChar32 ch, शून्य* context) {
    int32_t value = *static_cast<int32_t*>(context);
    प्रत्यागम (U_GET_GC_आवरणम्((UChar32) ch) & value) != 0;
}

UBool संस्करणम्_Filter(UChar32 ch, शून्य* context) {
    स्थिर ध्रुव UVersionInfo none = { 0, 0, 0, 0 };
    UVersionInfo v;
    u_charAge(ch, v);
    UVersionInfo* संस्करणम् = static_cast<UVersionInfo*>(context);
    प्रत्यागम uprv_memcmp(&v, &none, परिमाण(v)) > 0 && uprv_memcmp(&v, संस्करणम्, परिमाण(v)) <= 0;
}

प्रकारनिर्देश संरचना {
    UProperty prop;
    int32_t value;
} IntPropertyContext;

UBool intPropertyFilter(UChar32 ch, शून्य* context) {
    IntPropertyContext* c = static_cast<IntPropertyContext*>(context);
    प्रत्यागम u_get_Int_Property_मानम्(ch, c->prop) == c->value;
}

UBool scriptExtensionsFilter(UChar32 ch, शून्य* context) {
    प्रत्यागम uscript_hasScript(ch, *static_cast<UScript_संहिता*>(context));
}

UBool idTypeFilter(UChar32 ch, शून्य* context) {
    प्रत्यागम u_hasIDType(ch, *static_cast<UIdentifierType*>(context));
}

}  // namespace

/**
 * Generic filter-based scanning code for UCD property UnicodeSets.
 */
शून्य Unicode_समुच्चयः::applyFilter(Unicode_समुच्चयः::Filter filter,
                             शून्य* context,
                             ध्रुव Unicode_समुच्चयः* inclusions,
                             UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;

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
    int32_t सीमा_Range = inclusions->get_Range_गणनम्();

    प्रति (पूर्णाङ्क j=0; j<सीमा_Range; ++j) {
        // get current range
        UChar32 start = inclusions->getRangeStart(j);
        UChar32 अन्तः = inclusions->get_Range_अन्तः(j);

        // for all the code points in the range, process
        प्रति (UChar32 ch = start; ch <= अन्तः; ++ch) {
            // only add to this UnicodeSet on inflection points --
            // where the hasProperty value changes to false
            यदि ((*filter)(ch, context)) {
                यदि (startHasProperty < 0) {
                    startHasProperty = ch;
                }
            } अन्यथा यदि (startHasProperty >= 0) {
                सङ्कलय(startHasProperty, ch-1);
                startHasProperty = -1;
            }
        }
    }
    यदि (startHasProperty >= 0) {
        सङ्कलय(startHasProperty, static_cast<UChar32>(0x10FFFF));
    }
    यदि (isBogus() && U_SUCCESS(स्थितिः)) {
        // We likely ran out of memory. AHHH!
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

नामाकाश {

}  // namespace

//----------------------------------------------------------------
// Property set API
//----------------------------------------------------------------

#परिभाषय FAIL(ec) UPRV_खण्डम्_MACRO_BEGIN { \
    ec=U_ILLEGAL_तर्कः_ERROR; \
    प्रत्यागम *एतत्; \
} UPRV_खण्डम्_MACRO_अन्तः

Unicode_समुच्चयः&
Unicode_समुच्चयः::apply_Int_Property_मानम्(UProperty prop, int32_t value, UError_संहिता& ec) U_LIFETIME_BOUND {
    यदि (U_FAILURE(ec) || isFrozen()) { प्रत्यागम *एतत्; }
    यदि (prop == UCHAR_GENERAL_CATEGORY_आवरणम्) {
        ध्रुव Unicode_समुच्चयः* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        applyFilter(general_Category_आवरणम्_Filter, &value, inclusions, ec);
    } अन्यथा यदि (prop == UCHAR_SCRIPT_EXTENSIONS) {
        ध्रुव Unicode_समुच्चयः* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UScript_संहिता script = static_cast<UScript_संहिता>(value);
        applyFilter(scriptExtensionsFilter, &script, inclusions, ec);
    } अन्यथा यदि (prop == UCHAR_IDENTIFIER_TYPE) {
        ध्रुव Unicode_समुच्चयः* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        UIdentifierType idType = static_cast<UIdentifierType>(value);
        applyFilter(idTypeFilter, &idType, inclusions, ec);
    } अन्यथा यदि (0 <= prop && prop < UCHAR_द्विमान_सीमा) {
        यदि (value == 0 || value == 1) {
            ध्रुव USet *समुच्चयः = u_get_द्विमान_Property_समुच्चयः(prop, &ec);
            यदि (U_FAILURE(ec)) { प्रत्यागम *एतत्; }
            copyFrom(*Unicode_समुच्चयः::fromUSet(समुच्चयः), सत्य);
            यदि (value == 0) {
                complement().remove_All_वाक्यानि();  // code point complement
            }
        } अन्यथा {
            clear();
        }
    } अन्यथा यदि (UCHAR_INT_START <= prop && prop < UCHAR_INT_सीमा) {
        ध्रुव Unicode_समुच्चयः* inclusions = CharacterProperties::getInclusionsForProperty(prop, ec);
        IntPropertyContext c = {prop, value};
        applyFilter(intPropertyFilter, &c, inclusions, ec);
    } अन्यथा {
        ec = U_ILLEGAL_तर्कः_ERROR;
    }
    प्रत्यागम *एतत्;
}

Unicode_समुच्चयः&
Unicode_समुच्चयः::applyPropertyAlias(ध्रुव UnicodeString& prop,
                               ध्रुव UnicodeString& value,
                               UError_संहिता& ec) U_LIFETIME_BOUND {
    यदि (U_FAILURE(ec) || isFrozen()) प्रत्यागम *एतत्;

    // prop and value used to be converted to char * using the default
    // converter instead of the invariant conversion.
    // This should not be necessary because all Unicode property and value
    // names use only invariant characters.
    // If there are any variant characters, then we won't find them anyway.
    // Checking first avoids assertion failures in the conversion.
    यदि( !uprv_isInvariantUString(prop.get_बफर(), prop.length()) ||
        !uprv_isInvariantUString(value.get_बफर(), value.length())
    ) {
        FAIL(ec);
    }
    CharString pname, vname;
    pname.appendInvariantChars(prop, ec);
    vname.appendInvariantChars(value, ec);
    यदि (U_FAILURE(ec)) प्रत्यागम *एतत्;

    UProperty p;
    int32_t v;
    UBool invert = असत्य;

    यदि (value.length() > 0) {
        p = u_getPropertyEnum(pname.दत्त());
        यदि (p == UCHAR_INVALID_संहिता) FAIL(ec);

        // Treat gc as gcm
        यदि (p == UCHAR_GENERAL_CATEGORY) {
            p = UCHAR_GENERAL_CATEGORY_आवरणम्;
        }

        यदि ((p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_सीमा) ||
            (p >= UCHAR_INT_START && p < UCHAR_INT_सीमा) ||
            (p >= UCHAR_आवरणम्_START && p < UCHAR_आवरणम्_सीमा)) {
            v = u_get_Property_मानम्_Enum(p, vname.दत्त());
            यदि (v == UCHAR_INVALID_संहिता) {
                // Handle numeric CCC
                यदि (p == UCHAR_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_TRAIL_CANONICAL_COMBINING_CLASS ||
                    p == UCHAR_LEAD_CANONICAL_COMBINING_CLASS) {
                    वर्ण* अन्तः;
                    द्विगुण val = uprv_strtod(vname.दत्त(), &अन्तः);
                    // Anything between 0 and 255 is valid even if unused.
                    // Cast double->int only after range check.
                    // We catch NaN here because comparing it with both 0 and 255 will be false
                    // (as are all comparisons with NaN).
                    यदि (*अन्तः != 0 || !(0 <= val && val <= 255) ||
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

            विकल्प (p) {
            पक्ष UCHAR_NUMERIC_मानम्:
                {
                    वर्ण* अन्तः;
                    द्विगुण val = uprv_strtod(vname.दत्त(), &अन्तः);
                    यदि (*अन्तः != 0) {
                        FAIL(ec);
                    }
                    applyFilter(numeric_मानम्_Filter, &val,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    प्रत्यागम *एतत्;
                }
            पक्ष UCHAR_NAME:
                {
                    ध्रुव UChar32 ch = getCharacterByName(vname);
                    यदि (ch < 0) {
                        FAIL(ec);
                    }
                    clear();
                    सङ्कलय(ch);
                    प्रत्यागम *एतत्;
                }
            पक्ष UCHAR_UNICODE_1_NAME:
                // ICU 49 deprecates the Unicode_1_Name property APIs.
                FAIL(ec);
            पक्ष UCHAR_AGE:
                {
                    UVersionInfo संस्करणम्;
                    u_संस्करणम्_From_String(संस्करणम्, vname.दत्त());
                    applyFilter(संस्करणम्_Filter, &संस्करणम्,
                                CharacterProperties::getInclusionsForProperty(p, ec), ec);
                    प्रत्यागम *एतत्;
                }
            पक्ष UCHAR_SCRIPT_EXTENSIONS:
                v = u_get_Property_मानम्_Enum(UCHAR_SCRIPT, vname.दत्त());
                यदि (v == UCHAR_INVALID_संहिता) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                भङ्ग;
            पक्ष UCHAR_IDENTIFIER_TYPE:
                v = u_get_Property_मानम्_Enum(p, vname.दत्त());
                यदि (v == UCHAR_INVALID_संहिता) {
                    FAIL(ec);
                }
                // fall through to calling applyIntPropertyValue()
                भङ्ग;
            स्वभाव:
                // p is a non-binary, non-enumerated property that we
                // don't support (yet).
                FAIL(ec);
            }
        }
    }

    अन्यथा {
        // value is empty.  Interpret as General Category, Script, or
        // Binary property.
        p = UCHAR_GENERAL_CATEGORY_आवरणम्;
        v = u_get_Property_मानम्_Enum(p, pname.दत्त());
        यदि (v == UCHAR_INVALID_संहिता) {
            p = UCHAR_SCRIPT;
            v = u_get_Property_मानम्_Enum(p, pname.दत्त());
            यदि (v == UCHAR_INVALID_संहिता) {
                p = u_getPropertyEnum(pname.दत्त());
                यदि (p >= UCHAR_द्विमान_START && p < UCHAR_द्विमान_सीमा) {
                    v = 1;
                } अन्यथा यदि (0 == uprv_compare_Property_नामानि(ANY, pname.दत्त())) {
                    समुच्चयः(MIN_मानम्, MAX_मानम्);
                    प्रत्यागम *एतत्;
                } अन्यथा यदि (0 == uprv_compare_Property_नामानि(ASCII, pname.दत्त())) {
                    समुच्चयः(0, 0x7F);
                    प्रत्यागम *एतत्;
                } अन्यथा यदि (0 == uprv_compare_Property_नामानि(ASSIGNED, pname.दत्त())) {
                    // [:Assigned:]=[:^Cn:]
                    p = UCHAR_GENERAL_CATEGORY_आवरणम्;
                    v = U_GC_CN_आवरणम्;
                    invert = सत्य;
                } अन्यथा {
                    FAIL(ec);
                }
            }
        }
    }

    apply_Int_Property_मानम्(p, v, ec);
    यदि(invert) {
        complement().remove_All_वाक्यानि();  // code point complement
    }

    यदि (isBogus() && U_SUCCESS(ec)) {
        // We likely ran out of memory. AHHH!
        ec = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम *एतत्;
}

//----------------------------------------------------------------
// Property set patterns
//----------------------------------------------------------------

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a property set pattern.
 */
UBool Unicode_समुच्चयः::resembles_Property_आदर्शः(ध्रुव UnicodeString& आदर्शः,
                                           int32_t pos) {
    // Patterns are at least 5 characters long
    यदि ((pos+5) > आदर्शः.length()) {
        प्रत्यागम असत्य;
    }

    // Look for an opening [:, [:^, \p, or \P
    प्रत्यागम isPOSIXOpen(आदर्शः, pos) || isPerlOpen(आदर्शः, pos) || isNameOpen(आदर्शः, pos);
}

U_NAMESPACE_अन्तः
