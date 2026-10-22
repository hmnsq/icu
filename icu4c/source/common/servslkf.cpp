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
#include "unicode/utypes.h"

#if !UCONFIG_NO_SERVICE

#include "unicode/resbund.h"
#include "uresimp.h"
#include "cmemory.h"
#include "servloc.h"
#include "ustrfmt.h"
#include "uhash.h"
#include "charstr.h"
#include "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_सुसि_Factory::Simple_Locale_सुसि_Factory(UObject* objToAdopt,
                                               const UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_सुसि_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_सुसि_Factory::Simple_Locale_सुसि_Factory(UObject* objToAdopt,
                                               const Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_सुसि_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_सुसि_Factory::~Simple_Locale_सुसि_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_सुसि_Factory::create(const ICUService_सुसि& सुसि, const ICUService* service, UErrorCode& status) const
{
    if (U_SUCCESS(status)) {
        const Locale_सुसि& lkey = static_cast<const Locale_सुसि&>(सुसि);
        if (_kind == Locale_सुसि::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString सुसि_ID;
            lkey.currentID(सुसि_ID);
            if (_id == सुसि_ID) {
                return service->cloneInstance(_obj);
            }
        }
    }
    return nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

void
Simple_Locale_सुसि_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) const
{
    if (U_SUCCESS(status)) {
        if (_coverage & 0x1) {
            result.remove(_id);
        } else {
            result.put(_id, (void*)this, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_सुसि_Factory::debug(UnicodeString& result) const
{
    Locale_सुसि_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    return result;
}

UnicodeString&
Simple_Locale_सुसि_Factory::debugClass(UnicodeString& result) const
{
    return result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_सुसि_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


