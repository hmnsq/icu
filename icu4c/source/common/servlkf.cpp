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
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_താക്കോൽ_Factory::Locale_താക്കോൽ_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_താക്കോൽ_Factory::Locale_താക്കോൽ_Factory(int32_t coverage, സ്ഥിരം UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_താക്കോൽ_Factory::~Locale_താക്കോൽ_Factory() {
}

UObject*
Locale_താക്കോൽ_Factory::create(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (handles_താക്കോൽ(താക്കോൽ, status)) {
        സ്ഥിരം Locale_താക്കോൽ& lkey = static_cast<സ്ഥിരം Locale_താക്കോൽ&>(താക്കോൽ);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        തിരികെ handleCreate(loc, kind, service, status);
    }
    തിരികെ nullptr;
}

UBool
Locale_താക്കോൽ_Factory::handles_താക്കോൽ(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, UErrorCode& status) സ്ഥിരം {
    സ്ഥിരം Hashtable* supported = getSupportedIDs(status);
    എങ്കിൽ (supported) {
        UnicodeString id;
        താക്കോൽ.currentID(id);
        തിരികെ supported->get(id) != nullptr;
    }
    തിരികെ അസത്യം;
}

ശൂന്യം
Locale_താക്കോൽ_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) സ്ഥിരം {
    സ്ഥിരം Hashtable* supported = getSupportedIDs(status);
    എങ്കിൽ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        സ്ഥിരം UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        ആകുമ്പോൾ ((elem = supported->nextElement(pos)) != nullptr) {
            സ്ഥിരം UnicodeString& id = *static_cast<സ്ഥിരം UnicodeString*>(elem->താക്കോൽ.pointer);
            എങ്കിൽ (!visible) {
                result.remove(id);
            } അല്ലെങ്കിൽ {
                result.put(id, (ശൂന്യം*)ഇത്, status); // this is dummy non-void marker used for set semantics
                എങ്കിൽ (U_FAILURE(status)) {
                    നിർത്തുക;
                }
            }
        }
    }
}

UnicodeString&
Locale_താക്കോൽ_Factory::getDisplayName(സ്ഥിരം UnicodeString& id, സ്ഥിരം Locale& locale, UnicodeString& result) സ്ഥിരം {
    എങ്കിൽ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            തിരികെ loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    തിരികെ result;
}

UObject*
Locale_താക്കോൽ_Factory::handleCreate(സ്ഥിരം Locale& /* loc */, 
                   int32_t /* kind */, 
                   സ്ഥിരം ICUService* /* service */, 
                   UErrorCode& /* status */) സ്ഥിരം {
    തിരികെ nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

സ്ഥിരം Hashtable*
Locale_താക്കോൽ_Factory::getSupportedIDs(UErrorCode& /* status */) സ്ഥിരം {
    തിരികെ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_താക്കോൽ_Factory::debug(UnicodeString& result) സ്ഥിരം
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    തിരികെ result;
}

UnicodeString&
Locale_താക്കോൽ_Factory::debugClass(UnicodeString& result) സ്ഥിരം
{
  തിരികെ result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_താക്കോൽ_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


