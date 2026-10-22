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
#include "unicode/utypes.h"

#ᮚᮦᮔ᮪ !UCONFIG_NO_SERVICE

#include "unicode/resbund.h"
#include "uresimp.h"
#include "cmemory.h"
#include "servloc.h"
#include "ustrfmt.h"
#include "uhash.h"
#include "charstr.h"
#include "ucln_cmn.h"
#include "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

LocaleKeyFactory::LocaleKeyFactory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

LocaleKeyFactory::LocaleKeyFactory(int32_t coverage, const UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

LocaleKeyFactory::~LocaleKeyFactory() {
}

UObject*
LocaleKeyFactory::create(const ICUServiceKey& key, const ICUService* service, UErrorCode& status) const {
    ᮚᮦᮔ᮪ (handlesKey(key, status)) {
        const LocaleKey& lkey = static_cast<const LocaleKey&>(key);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        ᮝᮜᮤ handleCreate(loc, kind, service, status);
    }
    ᮝᮜᮤ nullptr;
}

UBool
LocaleKeyFactory::handlesKey(const ICUServiceKey& key, UErrorCode& status) const {
    const Hashtable* supported = getSupportedIDs(status);
    ᮚᮦᮔ᮪ (supported) {
        UnicodeString id;
        key.currentID(id);
        ᮝᮜᮤ supported->get(id) != nullptr;
    }
    ᮝᮜᮤ false;
}

void
LocaleKeyFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) const {
    const Hashtable* supported = getSupportedIDs(status);
    ᮚᮦᮔ᮪ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        const UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        ᮞᮨᮓᮨᮜ ((elem = supported->nextElement(pos)) != nullptr) {
            const UnicodeString& id = *static_cast<const UnicodeString*>(elem->key.pointer);
            ᮚᮦᮔ᮪ (!visible) {
                result.remove(id);
            } ᮜᮤᮚ {
                result.put(id, (void*)this, status); // this is dummy non-void marker used for set semantics
                ᮚᮦᮔ᮪ (U_FAILURE(status)) {
                    bha_nga;
                }
            }
        }
    }
}

UnicodeString&
LocaleKeyFactory::getDisplayName(const UnicodeString& id, const Locale& locale, UnicodeString& result) const {
    ᮚᮦᮔ᮪ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ᮝᮜᮤ loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    ᮝᮜᮤ result;
}

UObject*
LocaleKeyFactory::handleCreate(const Locale& /* loc */, 
                   int32_t /* kind */, 
                   const ICUService* /* service */, 
                   UErrorCode& /* status */) const {
    ᮝᮜᮤ nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

const Hashtable*
LocaleKeyFactory::getSupportedIDs(UErrorCode& /* status */) const {
    ᮝᮜᮤ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
LocaleKeyFactory::debug(UnicodeString& result) const
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    ᮝᮜᮤ result;
}

UnicodeString&
LocaleKeyFactory::debugClass(UnicodeString& result) const
{
  ᮝᮜᮤ result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LocaleKeyFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


