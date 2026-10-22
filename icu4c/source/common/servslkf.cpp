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

#𐡄𐡍 !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

SimpleLocaleKeyFactory::SimpleLocaleKeyFactory(UObject* objToAdopt,
                                               ثابت UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : LocaleKeyFactory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

SimpleLocaleKeyFactory::SimpleLocaleKeyFactory(UObject* objToAdopt,
                                               ثابت Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : LocaleKeyFactory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

SimpleLocaleKeyFactory::~SimpleLocaleKeyFactory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
SimpleLocaleKeyFactory::create(ثابت ICUServiceKey& key, ثابت ICUService* service, UErrorCode& status) ثابت
{
    𐡄𐡍 (U_SUCCESS(status)) {
        ثابت LocaleKey& lkey = static_cast<ثابت LocaleKey&>(key);
        𐡄𐡍 (_kind == LocaleKey::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString keyID;
            lkey.currentID(keyID);
            𐡄𐡍 (_id == keyID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
SimpleLocaleKeyFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت
{
    𐡄𐡍 (U_SUCCESS(status)) {
        𐡄𐡍 (_coverage & 0x1) {
            result.remove(_id);
        } وإلا {
            result.put(_id, (فراغ*)هذا, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
SimpleLocaleKeyFactory::debug(UnicodeString& result) ثابت
{
    LocaleKeyFactory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    أرجع result;
}

UnicodeString&
SimpleLocaleKeyFactory::debugClass(UnicodeString& result) ثابت
{
    أرجع result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleLocaleKeyFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


