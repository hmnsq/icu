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

Locale_ئاچقۇچ_Factory::Locale_ئاچقۇچ_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ئاچقۇچ_Factory::Locale_ئاچقۇچ_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ئاچقۇچ_Factory::~Locale_ئاچقۇچ_Factory() {
}

UObject*
Locale_ئاچقۇچ_Factory::create(ثابت ICUService_ئاچقۇچ& ئاچقۇچ, ثابت ICUService* service, UErrorCode& status) ثابت {
    إذا (handles_ئاچقۇچ(ئاچقۇچ, status)) {
        ثابت Locale_ئاچقۇچ& lkey = static_cast<ثابت Locale_ئاچقۇچ&>(ئاچقۇچ);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        أرجع handleCreate(loc, kind, service, status);
    }
    أرجع nullptr;
}

UBool
Locale_ئاچقۇچ_Factory::handles_ئاچقۇچ(ثابت ICUService_ئاچقۇچ& ئاچقۇچ, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    إذا (supported) {
        UnicodeString id;
        ئاچقۇچ.currentID(id);
        أرجع supported->get(id) != nullptr;
    }
    أرجع خطأ;
}

فراغ
Locale_ئاچقۇچ_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        طالما ((elem = supported->nextElement(pos)) != nullptr) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->ئاچقۇچ.pointer);
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
Locale_ئاچقۇچ_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& result) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            أرجع loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    أرجع result;
}

UObject*
Locale_ئاچقۇچ_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /* kind */, 
                   ثابت ICUService* /* service */, 
                   UErrorCode& /* status */) ثابت {
    أرجع nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_ئاچقۇچ_Factory::getSupportedIDs(UErrorCode& /* status */) ثابت {
    أرجع nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ئاچقۇچ_Factory::debug(UnicodeString& result) ثابت
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    أرجع result;
}

UnicodeString&
Locale_ئاچقۇچ_Factory::debugClass(UnicodeString& result) ثابت
{
  أرجع result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ئاچقۇچ_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


