// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাৱ "unicode/resbund.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "servloc.h"
#অন্তর্ভাৱ "ustrfmt.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "ucln_cmn.h"
#অন্তর্ভাৱ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_চাবি_Factory::Locale_চাবি_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_চাবি_Factory::Locale_চাবি_Factory(int32_t coverage, ধ্রুৱ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_চাবি_Factory::~Locale_চাবি_Factory() {
}

UObject*
Locale_চাবি_Factory::create(ধ্রুৱ ICUService_চাবি& চাবি, ধ্রুৱ ICUService* service, UErrorCode& status) ধ্রুৱ {
    যদি (handles_চাবি(চাবি, status)) {
        ধ্রুৱ Locale_চাবি& lkey = static_cast<ধ্রুৱ Locale_চাবি&>(চাবি);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        ঘূৰাই handleCreate(loc, kind, service, status);
    }
    ঘূৰাই nullptr;
}

UBool
Locale_চাবি_Factory::handles_চাবি(ধ্রুৱ ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুৱ {
    ধ্রুৱ Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UnicodeString id;
        চাবি.currentID(id);
        ঘূৰাই supported->get(id) != nullptr;
    }
    ঘূৰাই মিছা;
}

শূন্য
Locale_চাবি_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ধ্রুৱ {
    ধ্রুৱ Hashtable* supported = getSupportedIDs(status);
    যদি (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ধ্রুৱ UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        যেতিয়ালৈ ((elem = supported->nextElement(pos)) != nullptr) {
            ধ্রুৱ UnicodeString& id = *static_cast<ধ্রুৱ UnicodeString*>(elem->চাবি.pointer);
            যদি (!visible) {
                result.remove(id);
            } অন্যথা {
                result.put(id, (শূন্য*)এতত্, status); // this is dummy non-void marker used for set semantics
                যদি (U_FAILURE(status)) {
                    ভঙ্গ;
                }
            }
        }
    }
}

UnicodeString&
Locale_চাবি_Factory::getDisplayName(ধ্রুৱ UnicodeString& id, ধ্রুৱ Locale& locale, UnicodeString& result) ধ্রুৱ {
    যদি ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ঘূৰাই loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    ঘূৰাই result;
}

UObject*
Locale_চাবি_Factory::handleCreate(ধ্রুৱ Locale& /* loc */, 
                   int32_t /* kind */, 
                   ধ্রুৱ ICUService* /* service */, 
                   UErrorCode& /* status */) ধ্রুৱ {
    ঘূৰাই nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ধ্রুৱ Hashtable*
Locale_চাবি_Factory::getSupportedIDs(UErrorCode& /* status */) ধ্রুৱ {
    ঘূৰাই nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_চাবি_Factory::debug(UnicodeString& result) ধ্রুৱ
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    ঘূৰাই result;
}

UnicodeString&
Locale_চাবি_Factory::debugClass(UnicodeString& result) ধ্রুৱ
{
  ঘূৰাই result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_চাবি_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


