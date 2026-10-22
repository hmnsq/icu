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

#אם !UCONFIG_NO_SERVICE

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

LocaleKeyFactory::LocaleKeyFactory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

LocaleKeyFactory::LocaleKeyFactory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

LocaleKeyFactory::~LocaleKeyFactory() {
}

UObject*
LocaleKeyFactory::create(ثابت ICUServiceKey& key, ثابت ICUService* service, UErrorCode& status) ثابت {
    אם (handlesKey(key, status)) {
        ثابت LocaleKey& lkey = static_cast<ثابت LocaleKey&>(key);
        int32_t kind = lkey.kind();
        Locale loc;
        lkey.currentLocale(loc);

        החזר handleCreate(loc, kind, service, status);
    }
    החזר nullptr;
}

UBool
LocaleKeyFactory::handlesKey(ثابت ICUServiceKey& key, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    אם (supported) {
        UnicodeString id;
        key.currentID(id);
        החזר supported->get(id) != nullptr;
    }
    החזר שקר;
}

فراغ
LocaleKeyFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(status);
    אם (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = nullptr;
        int32_t pos = UHASH_FIRST;
        כלעוד ((elem = supported->nextElement(pos)) != nullptr) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->key.pointer);
            אם (!visible) {
                result.remove(id);
            } אחרת {
                result.put(id, (فراغ*)هذا, status); // this is dummy non-void marker used for set semantics
                אם (U_FAILURE(status)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
LocaleKeyFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& result) ثابت {
    אם ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            החזר loc.getDisplayName(locale, result);
        // }
    }
    result.setToBogus();
    החזר result;
}

UObject*
LocaleKeyFactory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /* kind */, 
                   ثابت ICUService* /* service */, 
                   UErrorCode& /* status */) ثابت {
    החזר nullptr;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
LocaleKeyFactory::getSupportedIDs(UErrorCode& /* status */) ثابت {
    החזר nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
LocaleKeyFactory::debug(UnicodeString& result) ثابت
{
    debugClass(result);
    result.append((UnicodeString)", name: ");
    result.append(_name);
    result.append((UnicodeString)", coverage: ");
    result.append(_coverage);
    החזר result;
}

UnicodeString&
LocaleKeyFactory::debugClass(UnicodeString& result) ثابت
{
  החזר result.append((UnicodeString)"LocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LocaleKeyFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


