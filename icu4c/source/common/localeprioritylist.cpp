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
    ثابت خودکار* locale = static_cast<ثابت Locale*>(token.اشاره_گر);
    أرجع locale->hash_کد();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت خودکار* l1 = static_cast<ثابت Locale*>(t1.اشاره_گر);
    ثابت خودکار* l2 = static_cast<ثابت Locale*>(t2.اشاره_گر);
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
compare_Locale_And_وزن(ثابت فراغ * /*context*/, ثابت فراغ *چپ, ثابت فراغ *راست) {
    أرجع static_cast<ثابت Locale_And_وزن *>(چپ)->
        compare(*static_cast<ثابت Locale_And_وزن *>(راست));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *حد) {
    طالما (p < حد && *p == ' ') { ++p; }
    أرجع p;
}

int32_t پیدا_کن_برچسب_درازی(ثابت حرف *p, ثابت حرف *حد) {
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
    Maybe_پشته_Array<Locale_And_وزن, 20> array;
};

Locale_اولویت_List::Locale_اولویت_List(StringPiece s, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    list = جديد Locale_And_وزن_Array();
    إذا (list == اشاره_تهی) {
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    ثابت حرف *p = s.داده();
    ثابت حرف *حد = p + s.length();
    طالما ((p = skipSpaces(p, حد)) != حد) {
        إذا (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t برچسب_درازی = پیدا_کن_برچسب_درازی(p, حد);
        إذا (برچسب_درازی == 0) {
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        CharString برچسب(p, برچسب_درازی, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        Locale locale = Locale(برچسب.داده());
        إذا (locale.isBogus()) {
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        int32_t وزن = وزن_ONE;
        إذا ((p = skipSpaces(p + برچسب_درازی, حد)) != حد && *p == ';') {
            إذا ((p = skipSpaces(p + 1, حد)) == حد || *p != 'q' ||
                    (p = skipSpaces(p + 1, حد)) == حد || *p != '=' ||
                    (++p, (وزن = parse_وزن(p, حد)) < 0)) {
                error_کد = U_ILLEGAL_آرگومان_ERROR;
                أرجع;
            }
            p = skipSpaces(p, حد);
        }
        إذا (p != حد && *p != ',') {  // trailing junk
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        جمع_کن(locale, وزن, error_کد);
        إذا (p == حد) { اكسر; }
        ++p;
    }
    sort(error_کد);
}

Locale_اولویت_List::~Locale_اولویت_List() {
    إذا (list != اشاره_تهی) {
        لكل (int32_t i = 0; i < list_درازی; ++i) {
            حذف list->array[i].locale;
        }
        حذف list;
    }
    uhash_close(map);
}

ثابت Locale *Locale_اولویت_List::localeAt(int32_t i) ثابت {
    أرجع list->array[i].locale;
}

Locale *Locale_اولویت_List::orphanLocaleAt(int32_t i) {
    إذا (list == اشاره_تهی) { أرجع اشاره_تهی; }
    Locale_And_وزن &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = اشاره_تهی;
    أرجع l;
}

منطقي Locale_اولویت_List::جمع_کن(ثابت Locale &locale, int32_t وزن, UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    إذا (map == اشاره_تهی) {
        إذا (وزن <= 0) { أرجع درست; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    }
    Local_اشاره_گر<Locale> clone;
    UBool یافت = خطأ;
    int32_t شاخص = uhash_geti_And_یافت(map, &locale, &یافت);
    إذا (یافت) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_وزن &lw = list->array[شاخص];
        clone.adoptInstead(lw.locale);
        lw.locale = اشاره_تهی;
        lw.وزن = 0;
        ++رقم_Removed;
    }
    إذا (وزن <= 0) {  // do not add q=0
        إذا (یافت) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        أرجع درست;
    }
    إذا (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        إذا (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }
    }
    إذا (list_درازی == list->array.get_ظرفیت()) {
        int32_t new_ظرفیت = list_درازی < 50 ? 100 : 4 * list_درازی;
        إذا (list->array.resize(new_ظرفیت, list_درازی) == اشاره_تهی) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_درازی, &error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    Locale_And_وزن &lw = list->array[list_درازی];
    lw.locale = clone.orphan();
    lw.وزن = وزن;
    lw.شاخص = list_درازی++;
    إذا (وزن < وزن_ONE) { hasWeights = درست; }
    U_ASSERT(uhash_شمار(map) == get_درازی());
    أرجع درست;
}

فراغ Locale_اولویت_List::sort(UError_کد &error_کد) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    إذا (U_FAILURE(error_کد) || get_درازی() <= 1 || !hasWeights) { أرجع; }
    uprv_مرتب_سازی_Array(list->array.getAlias(), list_درازی, حجم(Locale_And_وزن),
                   compare_Locale_And_وزن, اشاره_تهی, خطأ, &error_کد);
}

U_NAMESPACE_انتها
