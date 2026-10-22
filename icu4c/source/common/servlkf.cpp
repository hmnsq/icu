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
#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_साँचो_Factory::Locale_साँचो_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_साँचो_Factory::Locale_साँचो_Factory(int32_t coverage, अचर UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_साँचो_Factory::~Locale_साँचो_Factory() {
}

UObject*
Locale_साँचो_Factory::create(अचर ICUService_साँचो& साँचो, अचर ICUService* service, UErrorCode& status) अचर {
    यदि (handles_साँचो(साँचो, status)) {
        अचर Locale_साँचो& lkey = static_cast<अचर Locale_साँचो&>(साँचो);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        फर्काऊ handleCreate(loc, kind, service, status);
    }
    फर्काऊ nullptr;
}

UBool
Locale_साँचो_Factory::handles_साँचो(अचर ICUService_साँचो& साँचो, UErrorCode& status) अचर {
    अचर Hashtable* supported = getSupportedIDs(status);
    यदि (supported) {
        UnicodeString id;
        साँचो.currentID(id);
        फर्काऊ supported->get(id) != nullptr;
    }
    फर्काऊ असत्य;
}

शून्य
Locale_साँचो_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) अचर {
    अचर Hashtable* supported = getSupportedIDs(status);
    यदि (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        अचर UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        जबसम्म ((elem = supported->nextElement(pos)) != nullptr) {
            अचर UnicodeString& id = *static_cast<अचर UnicodeString*>(elem->साँचो.pointer);
            यदि (!visible) {
                result.हटाओ(id);
            } अन्यथा {
                result.put(id, (शून्य*)एतत्, status); // this is dummy non-void marker used for set semantics
                यदि (U_FAILURE(status)) {
                    रोक;
                }
            }
        }
    }
}

UnicodeString&
Locale_साँचो_Factory::getDisplayName(अचर UnicodeString& id, अचर Locale& locale, UnicodeString& result) अचर {
    यदि ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            फर्काऊ loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    फर्काऊ result;
}

UObject*
Locale_साँचो_Factory::handleCreate(अचर Locale& /* loc */, 
                   int32_t /* kind */, 
                   अचर ICUService* /* service */, 
                   UErrorCode& /* status */) अचर {
    फर्काऊ nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

अचर Hashtable*
Locale_साँचो_Factory::getSupportedIDs(UErrorCode& /* status */) अचर {
    फर्काऊ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_साँचो_Factory::debug(UnicodeString& result) अचर
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    फर्काऊ result;
}

UnicodeString&
Locale_साँचो_Factory::debugClass(UnicodeString& result) अचर
{
  फर्काऊ result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_साँचो_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


