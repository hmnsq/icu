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
#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_مفتاح_Factory::Locale_مفتاح_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_مفتاح_Factory::Locale_مفتاح_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_مفتاح_Factory::~Locale_مفتاح_Factory() {
}

UObject*
Locale_مفتاح_Factory::create(ثابت ICUService_مفتاح& مفتاح, ثابت ICUService* service, UErrorCode& status) ثابت {
    إذا (handles_مفتاح(مفتاح, status)) {
        ثابت Locale_مفتاح& lkey = static_cast<ثابت Locale_مفتاح&>(مفتاح);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        إرجاع handleCreate(loc, kind, service, status);
    }
    إرجاع nullptr;
}

UBool
Locale_مفتاح_Factory::handles_مفتاح(ثابت ICUService_مفتاح& مفتاح, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    إذا (supported) {
        UnicodeString id;
        مفتاح.currentID(id);
        إرجاع supported->get(id) != nullptr;
    }
    إرجاع خطأ;
}

فراغ
Locale_مفتاح_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        بينما ((elem = supported->nextElement(pos)) != nullptr) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->مفتاح.pointer);
            إذا (!visible) {
                result.remove(id);
            } وإلا {
                result.put(id, (فراغ*)هذا, status); // this is dummy non-void marker used for set semantics
                إذا (U_FAILURE(status)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
Locale_مفتاح_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& result) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            إرجاع loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    إرجاع result;
}

UObject*
Locale_مفتاح_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /* kind */, 
                   ثابت ICUService* /* service */, 
                   UErrorCode& /* status */) ثابت {
    إرجاع nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_مفتاح_Factory::getSupportedIDs(UErrorCode& /* status */) ثابت {
    إرجاع nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_مفتاح_Factory::debug(UnicodeString& result) ثابت
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    إرجاع result;
}

UnicodeString&
Locale_مفتاح_Factory::debugClass(UnicodeString& result) ثابت
{
  إرجاع result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_مفتاح_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


