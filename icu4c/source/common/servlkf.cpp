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

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_کلی_Factory::Locale_کلی_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_کلی_Factory::Locale_کلی_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_کلی_Factory::~Locale_کلی_Factory() {
}

UObject*
Locale_کلی_Factory::create(ثابت ICUService_کلی& کلی, ثابت ICUService* service, UError_کوډ& حالت) ثابت {
    إذا (handles_کلی(کلی, حالت)) {
        ثابت Locale_کلی& lkey = static_cast<ثابت Locale_کلی&>(کلی);
        int32_t ډول = lkey.ډول();
        Locale loc;
        lkey.currentLocale(loc);

        أرجع handleCreate(loc, ډول, service, حالت);
    }
    أرجع تش_اشاره;
}

UBool
Locale_کلی_Factory::handles_کلی(ثابت ICUService_کلی& کلی, UError_کوډ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UnicodeString id;
        کلی.currentID(id);
        أرجع supported->get(id) != تش_اشاره;
    }
    أرجع خطأ;
}

فراغ
Locale_کلی_Factory::updateVisibleIDs(Hashtable& پایله, UError_کوډ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = تش_اشاره;
        int32_t pos = UHASH_لومړی;
        طالما ((elem = supported->بل_Element(pos)) != تش_اشاره) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->کلی.اشاره_ګر);
            إذا (!visible) {
                پایله.remove(id);
            } وإلا {
                پایله.put(id, (فراغ*)هذا, حالت); // this is dummy non-void marker used for set semantics
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
Locale_کلی_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& پایله) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            أرجع loc.getDisplayName(locale, پایله);
        // }
    }
    پایله.سېټ_منزل_Bogus();
    أرجع پایله;
}

UObject*
Locale_کلی_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /*ډول*/, 
                   ثابت ICUService* /* service */, 
                   UError_کوډ& /* status */) ثابت {
    أرجع تش_اشاره;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_کلی_Factory::getSupportedIDs(UError_کوډ& /* status */) ثابت {
    أرجع تش_اشاره;
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString&
Locale_کلی_Factory::debug(UnicodeString& پایله) ثابت
{
    debugClass(پایله);
    پایله.append((UnicodeString)", name: ");
    پایله.append(_name);
    پایله.append((UnicodeString)", coverage: ");
    پایله.append(_coverage);
    أرجع پایله;
}

UnicodeString&
Locale_کلی_Factory::debugClass(UnicodeString& پایله) ثابت
{
  أرجع پایله.append((UnicodeString)"LocaleKeyFactory");
}
#د_که_پای

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلی_Factory)

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای


