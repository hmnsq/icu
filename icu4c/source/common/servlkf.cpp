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

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ಕೀಲಿ_Factory::Locale_ಕೀಲಿ_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ಕೀಲಿ_Factory::Locale_ಕೀಲಿ_Factory(int32_t coverage, ಸ್ಥಿರಾಂಕ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ಕೀಲಿ_Factory::~Locale_ಕೀಲಿ_Factory() {
}

UObject*
Locale_ಕೀಲಿ_Factory::create(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (handles_ಕೀಲಿ(ಕೀಲಿ, ಸ್ಥಿತಿ_2)) {
        ಸ್ಥಿರಾಂಕ Locale_ಕೀಲಿ& lkey = static_cast<ಸ್ಥಿರಾಂಕ Locale_ಕೀಲಿ&>(ಕೀಲಿ);
        int32_t ಬಗೆ = lkey.ಬಗೆ();
        Locale loc;
        lkey.currentLocale(loc);

        ಹಿಂತಿರುಗಿ handleCreate(loc, ಬಗೆ, service, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

UBool
Locale_ಕೀಲಿ_Factory::handles_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ Hashtable* supported = getSupportedIDs(ಸ್ಥಿತಿ_2);
    ಆದರೆ (supported) {
        UnicodeString id;
        ಕೀಲಿ.currentID(id);
        ಹಿಂತಿರುಗಿ supported->get(id) != ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ
Locale_ಕೀಲಿ_Factory::updateVisibleIDs(Hashtable& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ Hashtable* supported = getSupportedIDs(ಸ್ಥಿತಿ_2);
    ಆದರೆ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ಸ್ಥಿರಾಂಕ UHashElement* elem = ಶೂನ್ಯಸೂಚಕ;
        int32_t pos = UHASH_ಮೊದಲ;
        ಆಗ ((elem = supported->ಮುಂದಿನ_Element(pos)) != ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿರಾಂಕ UnicodeString& id = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(elem->ಕೀಲಿ.ಸೂಚಕ);
            ಆದರೆ (!visible) {
                ಫಲಿತಾಂಶ.remove(id);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಫಲಿತಾಂಶ.put(id, (ಶೂನ್ಯ*)ಇದು, ಸ್ಥಿತಿ_2); // this is dummy non-void marker used for set semantics
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }
}

UnicodeString&
Locale_ಕೀಲಿ_Factory::getDisplayName(ಸ್ಥಿರಾಂಕ UnicodeString& id, ಸ್ಥಿರಾಂಕ Locale& locale, UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ಹಿಂತಿರುಗಿ loc.getDisplayName(locale, ಫಲಿತಾಂಶ);
        // }
    }
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UObject*
Locale_ಕೀಲಿ_Factory::handleCreate(ಸ್ಥಿರಾಂಕ Locale& /* loc */, 
                   int32_t /*ಬಗೆ*/, 
                   ಸ್ಥಿರಾಂಕ ICUService* /* service */, 
                   UError_ಸಂಕೇತ& /* status */) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ಸ್ಥಿರಾಂಕ Hashtable*
Locale_ಕೀಲಿ_Factory::getSupportedIDs(UError_ಸಂಕೇತ& /* status */) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString&
Locale_ಕೀಲಿ_Factory::debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    debugClass(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append((UnicodeString)", name: ");
    ಫಲಿತಾಂಶ.append(_name);
    ಫಲಿತಾಂಶ.append((UnicodeString)", coverage: ");
    ಫಲಿತಾಂಶ.append(_coverage);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale_ಕೀಲಿ_Factory::debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"LocaleKeyFactory");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ಕೀಲಿ_Factory)

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ


