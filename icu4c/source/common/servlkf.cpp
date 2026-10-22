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
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ଚାବି_Factory::Locale_ଚାବି_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ଚାବି_Factory::Locale_ଚାବି_Factory(int32_t coverage, ଅଚଳ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ଚାବି_Factory::~Locale_ଚାବି_Factory() {
}

UObject*
Locale_ଚାବି_Factory::create(ଅଚଳ ICUService_ଚାବି& ଚାବି, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ {
    ଯଦି (handles_ଚାବି(ଚାବି, status)) {
        ଅଚଳ Locale_ଚାବି& lkey = static_cast<ଅଚଳ Locale_ଚାବି&>(ଚାବି);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        ଫେରନ୍ତୁ handleCreate(loc, kind, service, status);
    }
    ଫେରନ୍ତୁ nullptr;
}

UBool
Locale_ଚାବି_Factory::handles_ଚାବି(ଅଚଳ ICUService_ଚାବି& ଚାବି, UErrorCode& status) ଅଚଳ {
    ଅଚଳ Hashtable* supported = getSupportedIDs(status);
    ଯଦି (supported) {
        UnicodeString id;
        ଚାବି.currentID(id);
        ଫେରନ୍ତୁ supported->get(id) != nullptr;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ
Locale_ଚାବି_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ଅଚଳ {
    ଅଚଳ Hashtable* supported = getSupportedIDs(status);
    ଯଦି (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ଅଚଳ UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        ଯେତେବେଳେ ((elem = supported->nextElement(pos)) != nullptr) {
            ଅଚଳ UnicodeString& id = *static_cast<ଅଚଳ UnicodeString*>(elem->ଚାବି.pointer);
            ଯଦି (!visible) {
                result.remove(id);
            } ଅନ୍ୟଥା {
                result.put(id, (ଶୂନ୍ୟ*)ଏହି, status); // this is dummy non-void marker used for set semantics
                ଯଦି (U_FAILURE(status)) {
                    ଭାଙ୍ଗ;
                }
            }
        }
    }
}

UnicodeString&
Locale_ଚାବି_Factory::getDisplayName(ଅଚଳ UnicodeString& id, ଅଚଳ Locale& locale, UnicodeString& result) ଅଚଳ {
    ଯଦି ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ଫେରନ୍ତୁ loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    ଫେରନ୍ତୁ result;
}

UObject*
Locale_ଚାବି_Factory::handleCreate(ଅଚଳ Locale& /* loc */, 
                   int32_t /* kind */, 
                   ଅଚଳ ICUService* /* service */, 
                   UErrorCode& /* status */) ଅଚଳ {
    ଫେରନ୍ତୁ nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ଅଚଳ Hashtable*
Locale_ଚାବି_Factory::getSupportedIDs(UErrorCode& /* status */) ଅଚଳ {
    ଫେରନ୍ତୁ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ଚାବି_Factory::debug(UnicodeString& result) ଅଚଳ
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale_ଚାବି_Factory::debugClass(UnicodeString& result) ଅଚଳ
{
  ଫେରନ୍ତୁ result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ଚାବି_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


