// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uobject.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "localeprioritylist.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

نطاق {

int32_t hashLocale(ثابت UHashTok token) {
    ثابت خودکار* locale = static_cast<ثابت Locale*>(token.اشارہ_گر);
    أرجع locale->hash_کوڈ();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت خودکار* l1 = static_cast<ثابت Locale*>(t1.اشارہ_گر);
    ثابت خودکار* l2 = static_cast<ثابت Locale*>(t2.اشارہ_گر);
    أرجع *l1 == *l2;
}

constexpr int32_t بھار_ONE = 1000;

بنية Locale_And_بھار {
    Locale *locale;
    int32_t بھار;  // 0..1000 = 0.0..1.0
    int32_t اشاریہ;  // force stable sort

    int32_t compare(ثابت Locale_And_بھار &other) ثابت {
        int32_t diff = other.بھار - بھار;  // descending: other-this
        إذا (diff != 0) { أرجع diff; }
        أرجع اشاریہ - other.اشاریہ;
    }
};

int32_t U_CALLCONV
compare_Locale_And_بھار(ثابت فراغ * /*context*/, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    أرجع static_cast<ثابت Locale_And_بھار *>(کھبے)->
        compare(*static_cast<ثابت Locale_And_بھار *>(سجے));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *حد_بندی) {
    طالما (p < حد_بندی && *p == ' ') { ++p; }
    أرجع p;
}

int32_t لبھو_ٹیگ_لمائی(ثابت حرف *p, ثابت حرف *حد_بندی) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    لكل (q = p; q < حد_بندی; ++q) {
        حرف c = *q;
        إذا (c == ' ' || c == ',' || c == ';') { اكسر; }
    }
    أرجع static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parse_بھار(ثابت حرف *&p, ثابت حرف *حد_بندی) {
    p = skipSpaces(p, حد_بندی);
    حرف c;
    إذا (p == حد_بندی || ((c = *p) != '0' && c != '1')) { أرجع -1; }
    int32_t بھار = (c - '0') * 1000;
    إذا (++p == حد_بندی || *p != '.') { أرجع بھار; }
    int32_t multiplier = 100;
    طالما (++p != حد_بندی && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        إذا (multiplier > 0) {
            بھار += c * multiplier;
            multiplier /= 10;
        } وإلا إذا (multiplier == 0) {
            // round up
            إذا (c >= 5) { ++بھار; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    أرجع بھار <= بھار_ONE ? بھار : -1;  // bad if > 1.0
}

}  // namespace

/**
 * Nothing but a wrapper over a MaybeStackArray of LocaleAndWeight.
 *
 * This wrapper exists (and is not in an anonymous namespace)
 * so that we can forward-declare it in the header file and
 * don't have to expose the MaybeStackArray specialization and
 * the LocaleAndWeight to code (like the test) that #includes localeprioritylist.h.
 * Also, otherwise we would have to do a platform-specific
 * template export declaration of some kind for the MaybeStackArray specialization
 * to be properly exported from the common DLL.
 */
بنية Locale_And_بھار_Array : عام UMemory {
    Maybe_ڈھیر_Array<Locale_And_بھار, 20> array;
};

Locale_ترجیح_List::Locale_ترجیح_List(StringPiece s, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    list = جديد Locale_And_بھار_Array();
    إذا (list == خالی_سوچک) {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    ثابت حرف *p = s.انکڑے();
    ثابت حرف *حد_بندی = p + s.length();
    طالما ((p = skipSpaces(p, حد_بندی)) != حد_بندی) {
        إذا (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t ٹیگ_لمائی = لبھو_ٹیگ_لمائی(p, حد_بندی);
        إذا (ٹیگ_لمائی == 0) {
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        CharString ٹیگ(p, ٹیگ_لمائی, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        Locale locale = Locale(ٹیگ.انکڑے());
        إذا (locale.isBogus()) {
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        int32_t بھار = بھار_ONE;
        إذا ((p = skipSpaces(p + ٹیگ_لمائی, حد_بندی)) != حد_بندی && *p == ';') {
            إذا ((p = skipSpaces(p + 1, حد_بندی)) == حد_بندی || *p != 'q' ||
                    (p = skipSpaces(p + 1, حد_بندی)) == حد_بندی || *p != '=' ||
                    (++p, (بھار = parse_بھار(p, حد_بندی)) < 0)) {
                error_کوڈ = U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            p = skipSpaces(p, حد_بندی);
        }
        إذا (p != حد_بندی && *p != ',') {  // trailing junk
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        جوڑو(locale, بھار, error_کوڈ);
        إذا (p == حد_بندی) { اكسر; }
        ++p;
    }
    sort(error_کوڈ);
}

Locale_ترجیح_List::~Locale_ترجیح_List() {
    إذا (list != خالی_سوچک) {
        لكل (int32_t i = 0; i < list_لمائی; ++i) {
            مٹاؤ list->array[i].locale;
        }
        مٹاؤ list;
    }
    uhash_close(map);
}

ثابت Locale *Locale_ترجیح_List::localeAt(int32_t i) ثابت {
    أرجع list->array[i].locale;
}

Locale *Locale_ترجیح_List::orphanLocaleAt(int32_t i) {
    إذا (list == خالی_سوچک) { أرجع خالی_سوچک; }
    Locale_And_بھار &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = خالی_سوچک;
    أرجع l;
}

منطقي Locale_ترجیح_List::جوڑو(ثابت Locale &locale, int32_t بھار, UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    إذا (map == خالی_سوچک) {
        إذا (بھار <= 0) { أرجع سچ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    }
    Local_اشارہ_گر<Locale> clone;
    UBool لبھیا = خطأ;
    int32_t اشاریہ = uhash_geti_And_لبھیا(map, &locale, &لبھیا);
    إذا (لبھیا) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_بھار &lw = list->array[اشاریہ];
        clone.adoptInstead(lw.locale);
        lw.locale = خالی_سوچک;
        lw.بھار = 0;
        ++عدد_Removed;
    }
    إذا (بھار <= 0) {  // do not add q=0
        إذا (لبھیا) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        أرجع سچ;
    }
    إذا (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        إذا (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع خطأ;
        }
    }
    إذا (list_لمائی == list->array.get_سمرتھا()) {
        int32_t new_سمرتھا = list_لمائی < 50 ? 100 : 4 * list_لمائی;
        إذا (list->array.resize(new_سمرتھا, list_لمائی) == خالی_سوچک) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع خطأ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_لمائی, &error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    Locale_And_بھار &lw = list->array[list_لمائی];
    lw.locale = clone.orphan();
    lw.بھار = بھار;
    lw.اشاریہ = list_لمائی++;
    إذا (بھار < بھار_ONE) { hasWeights = سچ; }
    U_ASSERT(uhash_گنتی(map) == get_لمائی());
    أرجع سچ;
}

فراغ Locale_ترجیح_List::sort(UError_کوڈ &error_کوڈ) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    إذا (U_FAILURE(error_کوڈ) || get_لمائی() <= 1 || !hasWeights) { أرجع; }
    uprv_ترتیب_دیو_Array(list->array.getAlias(), list_لمائی, حجم(Locale_And_بھار),
                   compare_Locale_And_بھار, خالی_سوچک, خطأ, &error_کوڈ);
}

U_NAMESPACE_سرا
