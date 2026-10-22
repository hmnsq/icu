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
#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "servloc.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ચાવી_Factory::Locale_ચાવી_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ચાવી_Factory::Locale_ચાવી_Factory(int32_t coverage, અચલ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ચાવી_Factory::~Locale_ચાવી_Factory() {
}

UObject*
Locale_ચાવી_Factory::create(અચલ ICUService_ચાવી& ચાવી, અચલ ICUService* service, UErrorCode& status) અચલ {
    જો (handles_ચાવી(ચાવી, status)) {
        અચલ Locale_ચાવી& lkey = static_cast<અચલ Locale_ચાવી&>(ચાવી);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        પાછા handleCreate(loc, kind, service, status);
    }
    પાછા nullptr;
}

UBool
Locale_ચાવી_Factory::handles_ચાવી(અચલ ICUService_ચાવી& ચાવી, UErrorCode& status) અચલ {
    અચલ Hashtable* supported = getSupportedIDs(status);
    જો (supported) {
        UnicodeString id;
        ચાવી.currentID(id);
        પાછા supported->get(id) != nullptr;
    }
    પાછા ખોટું;
}

શૂન્ય
Locale_ચાવી_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) અચલ {
    અચલ Hashtable* supported = getSupportedIDs(status);
    જો (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        અચલ UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        જ્યારે ((elem = supported->nextElement(pos)) != nullptr) {
            અચલ UnicodeString& id = *static_cast<અચલ UnicodeString*>(elem->ચાવી.pointer);
            જો (!visible) {
                result.remove(id);
            } નહીંતર {
                result.put(id, (શૂન્ય*)આ, status); // this is dummy non-void marker used for set semantics
                જો (U_FAILURE(status)) {
                    તોડો;
                }
            }
        }
    }
}

UnicodeString&
Locale_ચાવી_Factory::getDisplayName(અચલ UnicodeString& id, અચલ Locale& locale, UnicodeString& result) અચલ {
    જો ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            પાછા loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    પાછા result;
}

UObject*
Locale_ચાવી_Factory::handleCreate(અચલ Locale& /* loc */, 
                   int32_t /* kind */, 
                   અચલ ICUService* /* service */, 
                   UErrorCode& /* status */) અચલ {
    પાછા nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

અચલ Hashtable*
Locale_ચાવી_Factory::getSupportedIDs(UErrorCode& /* status */) અચલ {
    પાછા nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ચાવી_Factory::debug(UnicodeString& result) અચલ
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    પાછા result;
}

UnicodeString&
Locale_ચાવી_Factory::debugClass(UnicodeString& result) અચલ
{
  પાછા result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ચાવી_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


