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
    ثابت خودڪار* locale = static_cast<ثابت Locale*>(token.اشارو);
    أرجع locale->hash_ڪوڊ();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت خودڪار* l1 = static_cast<ثابت Locale*>(t1.اشارو);
    ثابت خودڪار* l2 = static_cast<ثابت Locale*>(t2.اشارو);
    أرجع *l1 == *l2;
}

constexpr int32_t وزن_ONE = 1000;

بنية Locale_And_وزن {
    Locale *locale;
    int32_t وزن;  // 0..1000 = 0.0..1.0
    int32_t اشاريو;  // force stable sort

    int32_t compare(ثابت Locale_And_وزن &other) ثابت {
        int32_t diff = other.وزن - وزن;  // descending: other-this
        إذا (diff != 0) { أرجع diff; }
        أرجع اشاريو - other.اشاريو;
    }
};

int32_t U_CALLCONV
compare_Locale_And_وزن(ثابت فراغ * /*context*/, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    أرجع static_cast<ثابت Locale_And_وزن *>(کاٻي)->
        compare(*static_cast<ثابت Locale_And_وزن *>(ساڄي));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *حد_بندي) {
    طالما (p < حد_بندي && *p == ' ') { ++p; }
    أرجع p;
}

int32_t ڳوليو_ٽيگ_ڊيگهائي(ثابت حرف *p, ثابت حرف *حد_بندي) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    لكل (q = p; q < حد_بندي; ++q) {
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
int32_t parse_وزن(ثابت حرف *&p, ثابت حرف *حد_بندي) {
    p = skipSpaces(p, حد_بندي);
    حرف c;
    إذا (p == حد_بندي || ((c = *p) != '0' && c != '1')) { أرجع -1; }
    int32_t وزن = (c - '0') * 1000;
    إذا (++p == حد_بندي || *p != '.') { أرجع وزن; }
    int32_t multiplier = 100;
    طالما (++p != حد_بندي && '0' <= (c = *p) && c <= '9') {
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
    Maybe_ڍير_Array<Locale_And_وزن, 20> array;
};

Locale_ترجيح_List::Locale_ترجيح_List(StringPiece s, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    list = جديد Locale_And_وزن_Array();
    إذا (list == خالي_اشارو) {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ثابت حرف *p = s.معلومات();
    ثابت حرف *حد_بندي = p + s.length();
    طالما ((p = skipSpaces(p, حد_بندي)) != حد_بندي) {
        إذا (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t ٽيگ_ڊيگهائي = ڳوليو_ٽيگ_ڊيگهائي(p, حد_بندي);
        إذا (ٽيگ_ڊيگهائي == 0) {
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        CharString ٽيگ(p, ٽيگ_ڊيگهائي, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        Locale locale = Locale(ٽيگ.معلومات());
        إذا (locale.isBogus()) {
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        int32_t وزن = وزن_ONE;
        إذا ((p = skipSpaces(p + ٽيگ_ڊيگهائي, حد_بندي)) != حد_بندي && *p == ';') {
            إذا ((p = skipSpaces(p + 1, حد_بندي)) == حد_بندي || *p != 'q' ||
                    (p = skipSpaces(p + 1, حد_بندي)) == حد_بندي || *p != '=' ||
                    (++p, (وزن = parse_وزن(p, حد_بندي)) < 0)) {
                error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
                أرجع;
            }
            p = skipSpaces(p, حد_بندي);
        }
        إذا (p != حد_بندي && *p != ',') {  // trailing junk
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        جوڙيو(locale, وزن, error_ڪوڊ);
        إذا (p == حد_بندي) { اكسر; }
        ++p;
    }
    sort(error_ڪوڊ);
}

Locale_ترجيح_List::~Locale_ترجيح_List() {
    إذا (list != خالي_اشارو) {
        لكل (int32_t i = 0; i < list_ڊيگهائي; ++i) {
            ميٽيو list->array[i].locale;
        }
        ميٽيو list;
    }
    uhash_close(map);
}

ثابت Locale *Locale_ترجيح_List::localeAt(int32_t i) ثابت {
    أرجع list->array[i].locale;
}

Locale *Locale_ترجيح_List::orphanLocaleAt(int32_t i) {
    إذا (list == خالي_اشارو) { أرجع خالي_اشارو; }
    Locale_And_وزن &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = خالي_اشارو;
    أرجع l;
}

منطقي Locale_ترجيح_List::جوڙيو(ثابت Locale &locale, int32_t وزن, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    إذا (map == خالي_اشارو) {
        إذا (وزن <= 0) { أرجع سچ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    }
    Local_اشارو<Locale> clone;
    UBool مليو = خطأ;
    int32_t اشاريو = uhash_geti_And_مليو(map, &locale, &مليو);
    إذا (مليو) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_وزن &lw = list->array[اشاريو];
        clone.adoptInstead(lw.locale);
        lw.locale = خالي_اشارو;
        lw.وزن = 0;
        ++انگ_Removed;
    }
    إذا (وزن <= 0) {  // do not add q=0
        إذا (مليو) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        أرجع سچ;
    }
    إذا (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        إذا (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }
    }
    إذا (list_ڊيگهائي == list->array.get_صلاحيت()) {
        int32_t new_صلاحيت = list_ڊيگهائي < 50 ? 100 : 4 * list_ڊيگهائي;
        إذا (list->array.resize(new_صلاحيت, list_ڊيگهائي) == خالي_اشارو) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_ڊيگهائي, &error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    Locale_And_وزن &lw = list->array[list_ڊيگهائي];
    lw.locale = clone.orphan();
    lw.وزن = وزن;
    lw.اشاريو = list_ڊيگهائي++;
    إذا (وزن < وزن_ONE) { hasWeights = سچ; }
    U_ASSERT(uhash_ڳڻپ(map) == get_ڊيگهائي());
    أرجع سچ;
}

فراغ Locale_ترجيح_List::sort(UError_ڪوڊ &error_ڪوڊ) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    إذا (U_FAILURE(error_ڪوڊ) || get_ڊيگهائي() <= 1 || !hasWeights) { أرجع; }
    uprv_ترتيب_ڏيو_Array(list->array.getAlias(), list_ڊيگهائي, حجم(Locale_And_وزن),
                   compare_Locale_And_وزن, خالي_اشارو, خطأ, &error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي
