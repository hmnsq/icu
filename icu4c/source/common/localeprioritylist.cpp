// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "localeprioritylist.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

نامخلا {

int32_t hashLocale(ثابت UHashTok token) {
    ثابت خودکار* locale = static_cast<ثابت Locale*>(token.اشارہ_گر);
    واپس locale->hash_رمز();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت خودکار* l1 = static_cast<ثابت Locale*>(t1.اشارہ_گر);
    ثابت خودکار* l2 = static_cast<ثابت Locale*>(t2.اشارہ_گر);
    واپس *l1 == *l2;
}

constexpr int32_t وزن_ONE = 1000;

ساخت Locale_And_وزن {
    Locale *locale;
    int32_t وزن;  // 0..1000 = 0.0..1.0
    int32_t اشاریہ;  // force stable sort

    int32_t compare(ثابت Locale_And_وزن &other) ثابت {
        int32_t diff = other.وزن - وزن;  // descending: other-this
        اگر (diff != 0) { واپس diff; }
        واپس اشاریہ - other.اشاریہ;
    }
};

int32_t U_CALLCONV
compare_Locale_And_وزن(ثابت خالی * /*context*/, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    واپس static_cast<ثابت Locale_And_وزن *>(بائیں)->
        compare(*static_cast<ثابت Locale_And_وزن *>(دائیں));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *حد_بندی) {
    جب (p < حد_بندی && *p == ' ') { ++p; }
    واپس p;
}

int32_t ڈھونڈو_لیبل_لمبائی(ثابت حرف *p, ثابت حرف *حد_بندی) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    برائے (q = p; q < حد_بندی; ++q) {
        حرف c = *q;
        اگر (c == ' ' || c == ',' || c == ';') { توڑو; }
    }
    واپس static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parse_وزن(ثابت حرف *&p, ثابت حرف *حد_بندی) {
    p = skipSpaces(p, حد_بندی);
    حرف c;
    اگر (p == حد_بندی || ((c = *p) != '0' && c != '1')) { واپس -1; }
    int32_t وزن = (c - '0') * 1000;
    اگر (++p == حد_بندی || *p != '.') { واپس وزن; }
    int32_t multiplier = 100;
    جب (++p != حد_بندی && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        اگر (multiplier > 0) {
            وزن += c * multiplier;
            multiplier /= 10;
        } ورنہ اگر (multiplier == 0) {
            // round up
            اگر (c >= 5) { ++وزن; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    واپس وزن <= وزن_ONE ? وزن : -1;  // bad if > 1.0
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
ساخت Locale_And_وزن_Array : عوامی UMemory {
    Maybe_ڈھیر_Array<Locale_And_وزن, 20> array;
};

Locale_ترجیح_List::Locale_ترجیح_List(StringPiece s, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    list = نیا Locale_And_وزن_Array();
    اگر (list == خالی_اشاریہ) {
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    ثابت حرف *p = s.ڈیٹا();
    ثابت حرف *حد_بندی = p + s.length();
    جب ((p = skipSpaces(p, حد_بندی)) != حد_بندی) {
        اگر (*p == ',') {  // empty range field
            ++p;
            جاری;
        }
        int32_t لیبل_لمبائی = ڈھونڈو_لیبل_لمبائی(p, حد_بندی);
        اگر (لیبل_لمبائی == 0) {
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        CharString لیبل(p, لیبل_لمبائی, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        Locale locale = Locale(لیبل.ڈیٹا());
        اگر (locale.isBogus()) {
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        int32_t وزن = وزن_ONE;
        اگر ((p = skipSpaces(p + لیبل_لمبائی, حد_بندی)) != حد_بندی && *p == ';') {
            اگر ((p = skipSpaces(p + 1, حد_بندی)) == حد_بندی || *p != 'q' ||
                    (p = skipSpaces(p + 1, حد_بندی)) == حد_بندی || *p != '=' ||
                    (++p, (وزن = parse_وزن(p, حد_بندی)) < 0)) {
                error_رمز = U_ILLEGAL_دلیل_ERROR;
                واپس;
            }
            p = skipSpaces(p, حد_بندی);
        }
        اگر (p != حد_بندی && *p != ',') {  // trailing junk
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        جوڑو(locale, وزن, error_رمز);
        اگر (p == حد_بندی) { توڑو; }
        ++p;
    }
    sort(error_رمز);
}

Locale_ترجیح_List::~Locale_ترجیح_List() {
    اگر (list != خالی_اشاریہ) {
        برائے (int32_t i = 0; i < list_لمبائی; ++i) {
            حذف list->array[i].locale;
        }
        حذف list;
    }
    uhash_close(map);
}

ثابت Locale *Locale_ترجیح_List::localeAt(int32_t i) ثابت {
    واپس list->array[i].locale;
}

Locale *Locale_ترجیح_List::orphanLocaleAt(int32_t i) {
    اگر (list == خالی_اشاریہ) { واپس خالی_اشاریہ; }
    Locale_And_وزن &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = خالی_اشاریہ;
    واپس l;
}

منطقی Locale_ترجیح_List::جوڑو(ثابت Locale &locale, int32_t وزن, UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    اگر (map == خالی_اشاریہ) {
        اگر (وزن <= 0) { واپس سچ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    }
    Local_اشارہ_گر<Locale> clone;
    UBool ملا = جھوٹ;
    int32_t اشاریہ = uhash_geti_And_ملا(map, &locale, &ملا);
    اگر (ملا) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_وزن &lw = list->array[اشاریہ];
        clone.adoptInstead(lw.locale);
        lw.locale = خالی_اشاریہ;
        lw.وزن = 0;
        ++عدد_Removed;
    }
    اگر (وزن <= 0) {  // do not add q=0
        اگر (ملا) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        واپس سچ;
    }
    اگر (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        اگر (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }
    }
    اگر (list_لمبائی == list->array.get_استعداد()) {
        int32_t new_استعداد = list_لمبائی < 50 ? 100 : 4 * list_لمبائی;
        اگر (list->array.resize(new_استعداد, list_لمبائی) == خالی_اشاریہ) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_لمبائی, &error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    Locale_And_وزن &lw = list->array[list_لمبائی];
    lw.locale = clone.orphan();
    lw.وزن = وزن;
    lw.اشاریہ = list_لمبائی++;
    اگر (وزن < وزن_ONE) { hasWeights = سچ; }
    U_ASSERT(uhash_گنتی(map) == get_لمبائی());
    واپس سچ;
}

خالی Locale_ترجیح_List::sort(UError_رمز &error_رمز) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    اگر (U_FAILURE(error_رمز) || get_لمبائی() <= 1 || !hasWeights) { واپس; }
    uprv_ترتیب_دو_Array(list->array.getAlias(), list_لمبائی, حجم(Locale_And_وزن),
                   compare_Locale_And_وزن, خالی_اشاریہ, جھوٹ, &error_رمز);
}

U_NAMESPACE_انتہا
