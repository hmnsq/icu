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
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ಕೀಲಿ_Factory::Simple_Locale_ಕೀಲಿ_Factory(UObject* obj_ಗಮ್ಯ_Adopt,
                                               ಸ್ಥಿರಾಂಕ UnicodeString& locale,
                                               int32_t ಬಗೆ,
                                               int32_t coverage)
  : Locale_ಕೀಲಿ_Factory(coverage)
  , _obj(obj_ಗಮ್ಯ_Adopt)
  , _id(locale)
  , ಬಗೆ_2(ಬಗೆ)
{
}

Simple_Locale_ಕೀಲಿ_Factory::Simple_Locale_ಕೀಲಿ_Factory(UObject* obj_ಗಮ್ಯ_Adopt,
                                               ಸ್ಥಿರಾಂಕ Locale& locale,
                                               int32_t ಬಗೆ,
                                               int32_t coverage)
  : Locale_ಕೀಲಿ_Factory(coverage)
  , _obj(obj_ಗಮ್ಯ_Adopt)
  , _id()
  , ಬಗೆ_2(ಬಗೆ)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ಕೀಲಿ_Factory::~Simple_Locale_ಕೀಲಿ_Factory()
{
  ಅಳಿಸು _obj;
  _obj = ಶೂನ್ಯಸೂಚಕ;
}

UObject*
Simple_Locale_ಕೀಲಿ_Factory::create(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಸ್ಥಿರಾಂಕ Locale_ಕೀಲಿ& lkey = static_cast<ಸ್ಥಿರಾಂಕ Locale_ಕೀಲಿ&>(ಕೀಲಿ);
        ಆದರೆ (ಬಗೆ_2 == Locale_ಕೀಲಿ::ಬಗೆ_ANY || ಬಗೆ_2 == lkey.ಬಗೆ()) {
            UnicodeString ಕೀಲಿ_ID;
            lkey.currentID(ಕೀಲಿ_ID);
            ಆದರೆ (_id == ಕೀಲಿ_ID) {
                ಹಿಂತಿರುಗಿ service->cloneInstance(_obj);
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

ಶೂನ್ಯ
Simple_Locale_ಕೀಲಿ_Factory::updateVisibleIDs(Hashtable& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (_coverage & 0x1) {
            ಫಲಿತಾಂಶ.remove(_id);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ.put(_id, (ಶೂನ್ಯ*)ಇದು, ಸ್ಥಿತಿ_2);
        }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString&
Simple_Locale_ಕೀಲಿ_Factory::debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    Locale_ಕೀಲಿ_Factory::debug(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append((UnicodeString)", id: ");
    ಫಲಿತಾಂಶ.append(_id);
    ಫಲಿತಾಂಶ.append((UnicodeString)", kind: ");
    ಫಲಿತಾಂಶ.append(ಬಗೆ_2);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Simple_Locale_ಕೀಲಿ_Factory::debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ಕೀಲಿ_Factory)

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ


