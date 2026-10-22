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
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ಕೀಲಿ_Factory::Locale_ಕೀಲಿ_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ಕೀಲಿ_Factory::Locale_ಕೀಲಿ_Factory(int32_t coverage, ಧ್ರುವ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ಕೀಲಿ_Factory::~Locale_ಕೀಲಿ_Factory() {
}

UObject*
Locale_ಕೀಲಿ_Factory::create(ಧ್ರುವ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಧ್ರುವ ICUService* service, UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (handles_ಕೀಲಿ(ಕೀಲಿ, status)) {
        ಧ್ರುವ Locale_ಕೀಲಿ& lkey = static_cast<ಧ್ರುವ Locale_ಕೀಲಿ&>(ಕೀಲಿ);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        ಪ್ರತ್ಯಾಗಮ handleCreate(loc, kind, service, status);
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

UBool
Locale_ಕೀಲಿ_Factory::handles_ಕೀಲಿ(ಧ್ರುವ ICUService_ಕೀಲಿ& ಕೀಲಿ, UErrorCode& status) ಧ್ರುವ {
    ಧ್ರುವ Hashtable* supported = getSupportedIDs(status);
    ಯದಿ (supported) {
        UnicodeString id;
        ಕೀಲಿ.currentID(id);
        ಪ್ರತ್ಯಾಗಮ supported->get(id) != nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ
Locale_ಕೀಲಿ_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ಧ್ರುವ {
    ಧ್ರುವ Hashtable* supported = getSupportedIDs(status);
    ಯದಿ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ಧ್ರುವ UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        ಯಾವತ್ ((elem = supported->nextElement(pos)) != nullptr) {
            ಧ್ರುವ UnicodeString& id = *static_cast<ಧ್ರುವ UnicodeString*>(elem->ಕೀಲಿ.pointer);
            ಯದಿ (!visible) {
                result.remove(id);
            } ಅನ್ಯಥಾ {
                result.put(id, (ಶೂನ್ಯ*)ಏತತ್, status); // this is dummy non-void marker used for set semantics
                ಯದಿ (U_FAILURE(status)) {
                    ಭಙ್ಗ;
                }
            }
        }
    }
}

UnicodeString&
Locale_ಕೀಲಿ_Factory::getDisplayName(ಧ್ರುವ UnicodeString& id, ಧ್ರುವ Locale& locale, UnicodeString& result) ಧ್ರುವ {
    ಯದಿ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ಪ್ರತ್ಯಾಗಮ loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    ಪ್ರತ್ಯಾಗಮ result;
}

UObject*
Locale_ಕೀಲಿ_Factory::handleCreate(ಧ್ರುವ Locale& /* loc */, 
                   int32_t /* kind */, 
                   ಧ್ರುವ ICUService* /* service */, 
                   UErrorCode& /* status */) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ಧ್ರುವ Hashtable*
Locale_ಕೀಲಿ_Factory::getSupportedIDs(UErrorCode& /* status */) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ಕೀಲಿ_Factory::debug(UnicodeString& result) ಧ್ರುವ
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale_ಕೀಲಿ_Factory::debugClass(UnicodeString& result) ಧ್ರುವ
{
  ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ಕೀಲಿ_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


