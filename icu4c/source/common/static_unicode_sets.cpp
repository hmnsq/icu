// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#nếu !UCONFIG_NO_FORMATTING

// Allow implicit conversion from char16_t* to UnicodeString for this file:
// Helpful in toString methods and elsewhere.
#define UNISTR_FROM_STRING_EXPLICIT

#include "static_unicode_sets.h"
#include "umutex.h"
#include "ucln_cmn.h"
#include "unicode/uniset.h"
#include "uresimp.h"
#include "cstring.h"
#include "uassert.h"

using namespace icu;
using namespace icu::unisets;


namespace {

UnicodeSet* gUnicodeSets[UNISETS_KEY_COUNT] = {};

// Save the empty instance in static memory to have well-defined behavior if a
// regular UnicodeSet cannot be allocated.
alignas(UnicodeSet)
char gEmptyUnicodeSet[sizeof(UnicodeSet)];

// Whether the gEmptyUnicodeSet is initialized and ready to use.
UBool gEmptyUnicodeSetInitialized = false;

inline UnicodeSet* getImpl(Key key) {
    UnicodeSet* candidate = gUnicodeSets[key];
    nếu (candidate == nullptr) {
        trả_về reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    trả_về candidate;
}

UnicodeSet* computeUnion(Key k1, Key k2) {
    UnicodeSet* result = new UnicodeSet();
    nếu (result == nullptr) {
        trả_về nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->freeze();
    trả_về result;
}

UnicodeSet* computeUnion(Key k1, Key k2, Key k3) {
    UnicodeSet* result = new UnicodeSet();
    nếu (result == nullptr) {
        trả_về nullptr;
    }
    result->addAll(*getImpl(k1));
    result->addAll(*getImpl(k2));
    result->addAll(*getImpl(k3));
    result->freeze();
    trả_về result;
}


void saveSet(Key key, const UnicodeString& unicodeSetPattern, UErrorCode& status) {
    // assert unicodeSets.get(key) == null;
    gUnicodeSets[key] = new UnicodeSet(unicodeSetPattern, status);
}

class ParseDataSink : public ResourceSink {
  public:
    void put(const char* key, ResourceValue& value, UBool /*noFallback*/, UErrorCode& status) override {
        ResourceTable contextsTable = value.getTable(status);
        nếu (U_FAILURE(status)) { trả_về; }
        cho (số_nguyên i = 0; contextsTable.getKeyAndValue(i, key, value); i++) {
            nếu (uprv_strcmp(key, "date") == 0) {
                // ignore
            } không_thì {
                ResourceTable strictnessTable = value.getTable(status);
                nếu (U_FAILURE(status)) { trả_về; }
                cho (số_nguyên j = 0; strictnessTable.getKeyAndValue(j, key, value); j++) {
                    bool isLenient = (uprv_strcmp(key, "lenient") == 0);
                    ResourceArray array = value.getArray(status);
                    nếu (U_FAILURE(status)) { trả_về; }
                    cho (số_nguyên k = 0; k < array.getSize(); k++) {
                        array.getValue(k, value);
                        UnicodeString str = value.getUnicodeString(status);
                        nếu (U_FAILURE(status)) { trả_về; }
                        // There is both lenient and strict data for comma/period,
                        // but not for any of the other symbols.
                        nếu (str.indexOf(u'.') != -1) {
                            saveSet(isLenient ? PERIOD : STRICT_PERIOD, str, status);
                        } không_thì nếu (str.indexOf(u',') != -1) {
                            saveSet(isLenient ? COMMA : STRICT_COMMA, str, status);
                        } không_thì nếu (str.indexOf(u'+') != -1) {
                            saveSet(PLUS_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'-') != -1) {
                            saveSet(MINUS_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'$') != -1) {
                            saveSet(DOLLAR_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'£') != -1) {
                            saveSet(POUND_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'₹') != -1) {
                            saveSet(RUPEE_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'¥') != -1) {
                            saveSet(YEN_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'₩') != -1) {
                            saveSet(WON_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'%') != -1) {
                            saveSet(PERCENT_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'‰') != -1) {
                            saveSet(PERMILLE_SIGN, str, status);
                        } không_thì nếu (str.indexOf(u'’') != -1) {
                            saveSet(APOSTROPHE_SIGN, str, status);
                        } không_thì {
                            // Unknown class of parse lenients
                            // TODO(ICU-20428): Make ICU automatically accept new classes?
                            U_ASSERT(false);
                        }
                        nếu (U_FAILURE(status)) { trả_về; }
                    }
                }
            }
        }
    }
};


icu::UInitOnce gNumberParseUniSetsInitOnce {};

UBool U_CALLCONV cleanupNumberParseUniSets() {
    nếu (gEmptyUnicodeSetInitialized) {
        reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->~UnicodeSet();
        gEmptyUnicodeSetInitialized = false;
    }
    cho (int32_t i = 0; i < UNISETS_KEY_COUNT; i++) {
        delete gUnicodeSets[i];
        gUnicodeSets[i] = nullptr;
    }
    gNumberParseUniSetsInitOnce.reset();
    trả_về true;
}

void U_CALLCONV initNumberParseUniSets(UErrorCode& status) {
    ucln_common_registerCleanup(UCLN_COMMON_NUMPARSE_UNISETS, cleanupNumberParseUniSets);

    // Initialize the empty instance for well-defined fallback behavior
    new(gEmptyUnicodeSet) UnicodeSet();
    reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet)->freeze();
    gEmptyUnicodeSetInitialized = true;

    // These sets were decided after discussion with icu-design@. See tickets #13084 and #13309.
    // Zs+TAB is "horizontal whitespace" according to UTS #18 (blank property).
    gUnicodeSets[DEFAULT_IGNORABLES] = new UnicodeSet(
            u"[[:Zs:][\\u0009][:Bidi_Control:][:Variation_Selector:]]", status);
    gUnicodeSets[STRICT_IGNORABLES] = new UnicodeSet(u"[[:Bidi_Control:]]", status);

    LocalUResourceBundlePointer rb(ures_open(nullptr, "root", &status));
    nếu (U_FAILURE(status)) { trả_về; }
    ParseDataSink sink;
    ures_getAllItemsWithFallback(rb.getAlias(), "parse", sink, status);
    nếu (U_FAILURE(status)) { trả_về; }

    // NOTE: It is OK for these assertions to fail if there was a no-data build.
    U_ASSERT(gUnicodeSets[COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_COMMA] != nullptr);
    U_ASSERT(gUnicodeSets[PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[STRICT_PERIOD] != nullptr);
    U_ASSERT(gUnicodeSets[APOSTROPHE_SIGN] != nullptr);

    LocalPointer<UnicodeSet> otherGrouping(new UnicodeSet(
        u"[٬‘\\u0020\\u00A0\\u2000-\\u200A\\u202F\\u205F\\u3000]",
        status
    ), status);
    nếu (U_FAILURE(status)) { trả_về; }
    otherGrouping->addAll(*gUnicodeSets[APOSTROPHE_SIGN]);
    gUnicodeSets[OTHER_GROUPING_SEPARATORS] = otherGrouping.orphan();
    gUnicodeSets[ALL_SEPARATORS] = computeUnion(COMMA, PERIOD, OTHER_GROUPING_SEPARATORS);
    gUnicodeSets[STRICT_ALL_SEPARATORS] = computeUnion(
            STRICT_COMMA, STRICT_PERIOD, OTHER_GROUPING_SEPARATORS);

    U_ASSERT(gUnicodeSets[MINUS_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PLUS_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PERCENT_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[PERMILLE_SIGN] != nullptr);

    // The following don't currently have parseLenients in data.
    U_ASSERT(gUnicodeSets[INFINITY_SIGN] == nullptr);
    gUnicodeSets[INFINITY_SIGN] = new UnicodeSet(u"[∞]", status);
    U_ASSERT(gUnicodeSets[APPROXIMATELY_SIGN] == nullptr);
    // This set of characters was manually curated from the
    // values of the approximatelySign element of CLDR common/main/*.xml files.
    gUnicodeSets[APPROXIMATELY_SIGN] = new UnicodeSet(u"[∼~≈≃約]", status);
    nếu (U_FAILURE(status)) { trả_về; }

    U_ASSERT(gUnicodeSets[DOLLAR_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[POUND_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[RUPEE_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[YEN_SIGN] != nullptr);
    U_ASSERT(gUnicodeSets[WON_SIGN] != nullptr);

    gUnicodeSets[DIGITS] = new UnicodeSet(u"[:digit:]", status);
    nếu (U_FAILURE(status)) { trả_về; }
    gUnicodeSets[DIGITS_OR_ALL_SEPARATORS] = computeUnion(DIGITS, ALL_SEPARATORS);
    gUnicodeSets[DIGITS_OR_STRICT_ALL_SEPARATORS] = computeUnion(DIGITS, STRICT_ALL_SEPARATORS);

    cho (auto* uniset : gUnicodeSets) {
        nếu (uniset != nullptr) {
            uniset->freeze();
        }
    }
}

}

const UnicodeSet* unisets::get(Key key) {
    UErrorCode localStatus = U_ZERO_ERROR;
    umtx_initOnce(gNumberParseUniSetsInitOnce, &initNumberParseUniSets, localStatus);
    nếu (U_FAILURE(localStatus)) {
        trả_về reinterpret_cast<UnicodeSet*>(gEmptyUnicodeSet);
    }
    trả_về getImpl(key);
}

Key unisets::chooseFrom(UnicodeString str, Key key1) {
    trả_về get(key1)->contains(str) ? key1 : NONE;
}

Key unisets::chooseFrom(UnicodeString str, Key key1, Key key2) {
    trả_về get(key1)->contains(str) ? key1 : chooseFrom(str, key2);
}

//Key unisets::chooseCurrency(UnicodeString str) {
//    if (get(DOLLAR_SIGN)->contains(str)) {
//        return DOLLAR_SIGN;
//    } else if (get(POUND_SIGN)->contains(str)) {
//        return POUND_SIGN;
//    } else if (get(RUPEE_SIGN)->contains(str)) {
//        return RUPEE_SIGN;
//    } else if (get(YEN_SIGN)->contains(str)) {
//        return YEN_SIGN;
//    } else {
//        return NONE;
//    }
//}


#endif /* #if !UCONFIG_NO_FORMATTING */
