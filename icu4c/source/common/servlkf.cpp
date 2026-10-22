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
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "unicode/resbund.h"
#شامل "uresimp.h"
#شامل "cmemory.h"
#شامل "servloc.h"
#شامل "ustrfmt.h"
#شامل "uhash.h"
#شامل "charstr.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_کلید_Factory::Locale_کلید_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_کلید_Factory::Locale_کلید_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_کلید_Factory::~Locale_کلید_Factory() {
}

UObject*
Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UErrorCode& status) ثابت {
    اگر (handles_کلید(کلید, status)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        بازگشت handleCreate(loc, kind, service, status);
    }
    بازگشت nullptr;
}

UBool
Locale_کلید_Factory::handles_کلید(ثابت ICUService_کلید& کلید, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    اگر (supported) {
        UnicodeString id;
        کلید.currentID(id);
        بازگشت supported->get(id) != nullptr;
    }
    بازگشت نادرست;
}

تهی
Locale_کلید_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    اگر (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        وقتی ((elem = supported->nextElement(pos)) != nullptr) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->کلید.pointer);
            اگر (!visible) {
                result.remove(id);
            } وگرنه {
                result.put(id, (تهی*)این, status); // this is dummy non-void marker used for set semantics
                اگر (U_FAILURE(status)) {
                    بشکن;
                }
            }
        }
    }
}

UnicodeString&
Locale_کلید_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& result) ثابت {
    اگر ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            بازگشت loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    بازگشت result;
}

UObject*
Locale_کلید_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /* kind */, 
                   ثابت ICUService* /* service */, 
                   UErrorCode& /* status */) ثابت {
    بازگشت nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_کلید_Factory::getSupportedIDs(UErrorCode& /* status */) ثابت {
    بازگشت nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_کلید_Factory::debug(UnicodeString& result) ثابت
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    بازگشت result;
}

UnicodeString&
Locale_کلید_Factory::debugClass(UnicodeString& result) ثابت
{
  بازگشت result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


