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
    ثابت خپلکاری* locale = static_cast<ثابت Locale*>(token.اشاره_ګر);
    أرجع locale->hash_کوډ();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت خپلکاری* l1 = static_cast<ثابت Locale*>(t1.اشاره_ګر);
    ثابت خپلکاری* l2 = static_cast<ثابت Locale*>(t2.اشاره_ګر);
    أرجع *l1 == *l2;
}

constexpr int32_t وزن_ONE = 1000;

بنية Locale_And_وزن {
    Locale *locale;
    int32_t وزن;  // 0..1000 = 0.0..1.0
    int32_t شاخص;  // force stable sort

    int32_t compare(ثابت Locale_And_وزن &other) ثابت {
        int32_t diff = other.وزن - وزن;  // descending: other-this
        إذا (diff != 0) { أرجع diff; }
        أرجع شاخص - other.شاخص;
    }
};

int32_t U_CALLCONV
compare_Locale_And_وزن(ثابت فراغ * /*context*/, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    أرجع static_cast<ثابت Locale_And_وزن *>(کیڼ)->
        compare(*static_cast<ثابت Locale_And_وزن *>(ښی));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *حد) {
    طالما (p < حد && *p == ' ') { ++p; }
    أرجع p;
}

int32_t پیدا_کړه_نښان_اوږدتیا(ثابت حرف *p, ثابت حرف *حد) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    لكل (q = p; q < حد; ++q) {
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
int32_t parse_وزن(ثابت حرف *&p, ثابت حرف *حد) {
    p = skipSpaces(p, حد);
    حرف c;
    إذا (p == حد || ((c = *p) != '0' && c != '1')) { أرجع -1; }
    int32_t وزن = (c - '0') * 1000;
    إذا (++p == حد || *p != '.') { أرجع وزن; }
    int32_t multiplier = 100;
    طالما (++p != حد && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        إذا (multiplier > 0) {
            وزن += c * multiplier;
            multiplier /= 10;
        } وإلا إذا (multiplier == 0) {
            // round up
            إذا (c >= 5) { ++وزن; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    أرجع وزن <= وزن_ONE ? وزن : -1;  // bad if > 1.0
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
بنية Locale_And_وزن_Array : عام UMemory {
    Maybe_ډېری_Array<Locale_And_وزن, 20> array;
};

Locale_لومړیتوب_List::Locale_لومړیتوب_List(StringPiece s, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    list = جديد Locale_And_وزن_Array();
    إذا (list == تش_اشاره) {
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ثابت حرف *p = s.معلومات();
    ثابت حرف *حد = p + s.length();
    طالما ((p = skipSpaces(p, حد)) != حد) {
        إذا (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t نښان_اوږدتیا = پیدا_کړه_نښان_اوږدتیا(p, حد);
        إذا (نښان_اوږدتیا == 0) {
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        CharString نښان(p, نښان_اوږدتیا, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        Locale locale = Locale(نښان.معلومات());
        إذا (locale.isBogus()) {
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        int32_t وزن = وزن_ONE;
        إذا ((p = skipSpaces(p + نښان_اوږدتیا, حد)) != حد && *p == ';') {
            إذا ((p = skipSpaces(p + 1, حد)) == حد || *p != 'q' ||
                    (p = skipSpaces(p + 1, حد)) == حد || *p != '=' ||
                    (++p, (وزن = parse_وزن(p, حد)) < 0)) {
                error_کوډ = U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            p = skipSpaces(p, حد);
        }
        إذا (p != حد && *p != ',') {  // trailing junk
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            أرجع;
        }
        جمع_کړه(locale, وزن, error_کوډ);
        إذا (p == حد) { اكسر; }
        ++p;
    }
    sort(error_کوډ);
}

Locale_لومړیتوب_List::~Locale_لومړیتوب_List() {
    إذا (list != تش_اشاره) {
        لكل (int32_t i = 0; i < list_اوږدتیا; ++i) {
            ړنګول list->array[i].locale;
        }
        ړنګول list;
    }
    uhash_close(map);
}

ثابت Locale *Locale_لومړیتوب_List::localeAt(int32_t i) ثابت {
    أرجع list->array[i].locale;
}

Locale *Locale_لومړیتوب_List::orphanLocaleAt(int32_t i) {
    إذا (list == تش_اشاره) { أرجع تش_اشاره; }
    Locale_And_وزن &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = تش_اشاره;
    أرجع l;
}

منطقي Locale_لومړیتوب_List::جمع_کړه(ثابت Locale &locale, int32_t وزن, UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    إذا (map == تش_اشاره) {
        إذا (وزن <= 0) { أرجع ریښتیا; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    }
    Local_اشاره_ګر<Locale> clone;
    UBool وموندل = خطأ;
    int32_t شاخص = uhash_geti_And_وموندل(map, &locale, &وموندل);
    إذا (وموندل) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_وزن &lw = list->array[شاخص];
        clone.adoptInstead(lw.locale);
        lw.locale = تش_اشاره;
        lw.وزن = 0;
        ++عدد_Removed;
    }
    إذا (وزن <= 0) {  // do not add q=0
        إذا (وموندل) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        أرجع ریښتیا;
    }
    إذا (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        إذا (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع خطأ;
        }
    }
    إذا (list_اوږدتیا == list->array.get_ظرفیت()) {
        int32_t new_ظرفیت = list_اوږدتیا < 50 ? 100 : 4 * list_اوږدتیا;
        إذا (list->array.resize(new_ظرفیت, list_اوږدتیا) == تش_اشاره) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع خطأ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_اوږدتیا, &error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    Locale_And_وزن &lw = list->array[list_اوږدتیا];
    lw.locale = clone.orphan();
    lw.وزن = وزن;
    lw.شاخص = list_اوږدتیا++;
    إذا (وزن < وزن_ONE) { hasWeights = ریښتیا; }
    U_ASSERT(uhash_شمېر(map) == get_اوږدتیا());
    أرجع ریښتیا;
}

فراغ Locale_لومړیتوب_List::sort(UError_کوډ &error_کوډ) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    إذا (U_FAILURE(error_کوډ) || get_اوږدتیا() <= 1 || !hasWeights) { أرجع; }
    uprv_ترتیبول_Array(list->array.getAlias(), list_اوږدتیا, حجم(Locale_And_وزن),
                   compare_Locale_And_وزن, تش_اشاره, خطأ, &error_کوډ);
}

U_NAMESPACE_پای_ټکی
