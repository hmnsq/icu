// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#अगर_अघोषित __LOCALEPRIORITYLIST_H__
#घोषणा __LOCALEPRIORITYLIST_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uobject.h"

काष्ठा UHashtable;

U_NAMESPACE_BEGIN

काष्ठा LocaleAndWeightArray;

/**
 * Parses a list of locales from an accept-language string.
 * We are a bit more lenient than the spec:
 * We accept extra whitespace in more places, empty range fields,
 * and any number of qvalue fraction digits.
 *
 * https://tools.ietf.org/html/rfc2616#section-14.4
 * 14.4 Accept-Language
 *
 *        Accept-Language = "Accept-Language" ":"
 *                          1#( language-range [ ";" "q" "=" qvalue ] )
 *        language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
 *
 *    Each language-range MAY be given an associated quality value which
 *    represents an estimate of the user's preference for the languages
 *    specified by that range. The quality value defaults to "q=1". For
 *    example,
 *
 *        Accept-Language: da, en-gb;q=0.8, en;q=0.7
 *
 * https://tools.ietf.org/html/rfc2616#section-3.9
 * 3.9 Quality Values
 *
 *    HTTP content negotiation (section 12) uses short "floating point"
 *    numbers to indicate the relative importance ("weight") of various
 *    negotiable parameters.  A weight is normalized to a real number in
 *    the range 0 through 1, where 0 is the minimum and 1 the maximum
 *    value. If a parameter has a quality value of 0, then content with
 *    this parameter is `not acceptable' for the client. HTTP/1.1
 *    applications MUST NOT generate more than three digits after the
 *    decimal point. User configuration of these values SHOULD also be
 *    limited in this fashion.
 *
 *        qvalue         = ( "0" [ "." 0*3DIGIT ] )
 *                       | ( "1" [ "." 0*3("0") ] )
 */
class U_COMMON_API LocalePriorityList : खुला UMemory {
खुला:
    class Iterator : खुला Locale::Iterator {
    खुला:
        UBool has_अग्र() स्थिर override { वापस गिनती < लंबाई; }

        स्थिर Locale &अग्र() override {
            क्रम(;;) {
                स्थिर Locale *locale = list.localeAt(सूचकांक++);
                अगर (locale != nullptr) {
                    ++गिनती;
                    वापस *locale;
                }
            }
        }

    निजी:
        मित्र class LocalePriorityList;

        Iterator(स्थिर LocalePriorityList &list) : list(list), लंबाई(list.get_लंबाई()) {}

        स्थिर LocalePriorityList &list;
        int32_t सूचकांक = 0;
        int32_t गिनती = 0;
        स्थिर int32_t लंबाई;
    };

    LocalePriorityList(StringPiece s, UErrorCode &errorCode);

    ~LocalePriorityList();

    int32_t get_लंबाई() स्थिर { वापस list_लंबाई - अंक_Removed; }

    int32_t get_लंबाई_Including_Removed() स्थिर { वापस list_लंबाई; }

    Iterator iterator() स्थिर { वापस Iterator(*this); }

    स्थिर Locale *localeAt(int32_t i) स्थिर;

    Locale *orphanLocaleAt(int32_t i);

निजी:
    LocalePriorityList(स्थिर LocalePriorityList &) = delete;
    LocalePriorityList &चालक=(स्थिर LocalePriorityList &) = delete;

    bool add(स्थिर Locale &locale, int32_t weight, UErrorCode &errorCode);

    व्योम sort(UErrorCode &errorCode);

    LocaleAndWeightArray *list = nullptr;
    int32_t list_लंबाई = 0;
    int32_t अंक_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_END

#पूर्ण_अगर  // __LOCALEPRIORITYLIST_H__
