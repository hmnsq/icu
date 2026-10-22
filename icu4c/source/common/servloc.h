// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#اگر_غیر_تعریف_شدہ ICULSERV_H
#تعریف_کرو ICULSERV_H

#شامل "unicode/utypes.h"

#اگر UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_انتہا

#ورنہ

#شامل "unicode/unistr.h"
#شامل "unicode/locid.h"
#شامل "unicode/strenum.h"

#شامل "hash.h"
#شامل "uvector.h"

#شامل "serv.h"
#شامل "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_کلید;
class Locale_کلید_Factory;
class Simple_Locale_کلید_Factory;
class ServiceListener;

/*
 ******************************************************************
 */

/**
 * A subclass of Key that implements a locale fallback mechanism.
 * The first locale to search for is the locale provided by the
 * client, and the fallback locale to search for is the current
 * default locale.  If a prefix is present, the currentDescriptor
 * includes it before the locale proper, separated by "/".  This
 * is the default key instantiated by ICULocaleService.</p>
 *
 * <p>Canonicalization adjusts the locale string so that the
 * section before the first understore is in lower case, and the rest
 * is in upper case, with no trailing underscores.</p> 
 */

class U_COMMON_API Locale_کلید : عوامی ICUService_کلید {
  نجی: 
    int32_t قسم_2;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  عوامی:
    شمار {
        قسم_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساکن Locale_کلید* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID,
                                                  UError_رمز& حالت_2);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساکن Locale_کلید* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID, 
                                                  int32_t قسم,
                                                  UError_رمز& حالت_2);

  محفوظ:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_کلید(ثابت UnicodeString& primaryID, 
              ثابت UnicodeString& canonicalPrimaryID, 
              ثابت UnicodeString* canonicalFallbackID, 
              int32_t قسم);

 عوامی:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    مجازی UnicodeString& سابقہ(UnicodeString& نتیجہ) ثابت override;

    /**
     * Return the kind code associated with this key.
     */
    مجازی int32_t قسم() ثابت;

    /**
     * Return the canonicalID.
     */
    مجازی UnicodeString& canonicalID(UnicodeString& نتیجہ) ثابت override;

    /**
     * Return the currentID.
     */
    مجازی UnicodeString& currentID(UnicodeString& نتیجہ) ثابت override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    مجازی UnicodeString& currentDescriptor(UnicodeString& نتیجہ) ثابت override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    مجازی Locale& canonicalLocale(Locale& نتیجہ) ثابت;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    مجازی Locale& currentLocale(Locale& نتیجہ) ثابت;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    مجازی UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    مجازی UBool is_Fallback_کا(ثابت UnicodeString& id) ثابت override;
    
 عوامی:
    /**
     * UObject boilerplate.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

    /**
     * Destructor.
     */
    مجازی ~Locale_کلید();

#اگر_تعریف_شدہ SERVICE_DEBUG
 عوامی:
    مجازی UnicodeString& debug(UnicodeString& نتیجہ) ثابت;
    مجازی UnicodeString& debugClass(UnicodeString& نتیجہ) ثابت;
#اگر_ختم

};

/*
 ******************************************************************
 */

/**
 * A subclass of ICUServiceFactory that uses LocaleKeys, and is able to
 * 'cover' more specific locales with more general locales that it
 * supports.  
 *
 * <p>Coverage may be either of the values VISIBLE or INVISIBLE.
 *
 * <p>'Visible' indicates that the specific locale(s) supported by
 * the factory are registered in getSupportedIDs, 'Invisible'
 * indicates that they are not.
 *
 * <p>Localization of visible ids is handled
 * by the handling factory, regardless of kind.
 */
class U_COMMON_API Locale_کلید_Factory : عوامی ICUServiceFactory {
محفوظ:
    ثابت UnicodeString _name;
    ثابت int32_t _coverage;

عوامی:
    شمار {
        /**
         * Coverage value indicating that the factory makes
         * its locales visible, and does not cover more specific 
         * locales.
         */
        VISIBLE = 0,

        /**
         * Coverage value indicating that the factory does not make
         * its locales visible, and does not cover more specific
         * locales.
         */
        INVISIBLE = 1
    };

    /**
     * Destructor.
     */
    مجازی ~Locale_کلید_Factory();

محفوظ:
    /**
     * Constructor used by subclasses.
     */
    Locale_کلید_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_کلید_Factory(int32_t coverage, ثابت UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
عوامی:
    مجازی UObject* create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_رمز& حالت_2) ثابت override;

محفوظ:
    مجازی UBool handles_کلید(ثابت ICUService_کلید& کلید, UError_رمز& حالت_2) ثابت;

عوامی:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    مجازی خالی updateVisibleIDs(Hashtable& نتیجہ, UError_رمز& حالت_2) ثابت override;

    /**
     * Return a localized name for the locale represented by id.
     */
    مجازی UnicodeString& getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& نتیجہ) ثابت override;

محفوظ:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t قسم, ثابت ICUService* service, UError_رمز& حالت_2) ثابت;

   /**
     * Return true if this id is one the factory supports (visible or 
     * otherwise).
     */
 //   virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;

   /**
     * Return the set of ids that this factory supports (visible or 
     * otherwise).  This can be called often and might need to be
     * cached if it is expensive to create.
     */
    مجازی ثابت Hashtable* getSupportedIDs(UError_رمز& حالت_2) ثابت;

عوامی:
    /**
     * UObject boilerplate.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

#اگر_تعریف_شدہ SERVICE_DEBUG
 عوامی:
    مجازی UnicodeString& debug(UnicodeString& نتیجہ) ثابت;
    مجازی UnicodeString& debugClass(UnicodeString& نتیجہ) ثابت;
#اگر_ختم

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_کلید_Factory : عوامی Locale_کلید_Factory {
 نجی:
    UObject* _obj;
    UnicodeString _id;
    ثابت int32_t قسم_2;

 عوامی:
    Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt, 
                           ثابت UnicodeString& locale, 
                           int32_t قسم, 
                           int32_t coverage);

    Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt, 
                           ثابت Locale& locale, 
                           int32_t قسم, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    مجازی ~Simple_Locale_کلید_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    مجازی UObject* create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_رمز& حالت_2) ثابت override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    مجازی خالی updateVisibleIDs(Hashtable& نتیجہ, UError_رمز& حالت_2) ثابت override;

 محفوظ:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


عوامی:
    /**
     * UObject boilerplate.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

#اگر_تعریف_شدہ SERVICE_DEBUG
 عوامی:
    مجازی UnicodeString& debug(UnicodeString& نتیجہ) ثابت;
    مجازی UnicodeString& debugClass(UnicodeString& نتیجہ) ثابت;
#اگر_ختم

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that creates a service based on the ICU locale data.
 * This is a base class for most ICU factories.  Subclasses instantiate it
 * with a constructor that takes a bundle name, which determines the supported
 * IDs.  Subclasses then override handleCreate to create the actual service
 * object.  The default implementation returns a resource bundle.
 */
class U_COMMON_API ICUResourceBundleFactory : عوامی Locale_کلید_Factory 
{
 محفوظ:
    UnicodeString _bundleName;

 عوامی:
    /**
     * Convenience constructor that uses the main ICU bundle name.
     */
    ICUResourceBundleFactory();

    /**
     * A service factory based on ICU resource data in resources with
     * the given name.  This should be a 'path' that can be passed to
     * ures_openAvailableLocales, such as U_ICUDATA or U_ICUDATA_COLL.
     * The empty string is equivalent to U_ICUDATA.
     */
    ICUResourceBundleFactory(ثابت UnicodeString& bundleName);

    /**
     * Destructor
     */
    مجازی ~ICUResourceBundleFactory();

محفوظ:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    مجازی ثابت Hashtable* getSupportedIDs(UError_رمز& حالت_2) ثابت override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    مجازی UObject* handleCreate(ثابت Locale& loc, int32_t قسم, ثابت ICUService* service, UError_رمز& حالت_2) ثابت override;

عوامی:
    /**
     * UObject boilerplate.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();
    مجازی UClassID getDynamicClassID() ثابت override;


#اگر_تعریف_شدہ SERVICE_DEBUG
 عوامی:
    مجازی UnicodeString& debug(UnicodeString& نتیجہ) ثابت;
    مجازی UnicodeString& debugClass(UnicodeString& نتیجہ) ثابت;
#اگر_ختم

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : عوامی ICUService
{
 نجی:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 عوامی:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ثابت UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API مجازی ~ICULocaleService();

#اگر 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ثابت UnicodeString& descriptor, UError_رمز& حالت_2) ثابت {
    واپس ICUService::get(descriptor, حالت_2);
  }

  UObject* get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_رمز& حالت_2) ثابت {
    واپس ICUService::get(descriptor, actualReturn, حالت_2);
  }
#اگر_ختم

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, UError_رمز& حالت_2) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t قسم, UError_رمز& حالت_2) ثابت;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, Locale* actualReturn, UError_رمز& حالت_2) ثابت;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t قسم, Locale* actualReturn, UError_رمز& حالت_2) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API مجازی URegistry_کلید رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, UError_رمز& حالت_2);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API مجازی URegistry_کلید رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t قسم, UError_رمز& حالت_2);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API مجازی URegistry_کلید رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t قسم, int32_t coverage, UError_رمز& حالت_2);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API مجازی URegistry_کلید رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, UBool visible, UError_رمز& حالت_2) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API مجازی StringEnumeration* get_دستیاب_Locales() ثابت U_LIFETIME_BOUND;

 محفوظ:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ثابت UnicodeString& validateFallbackLocale() ثابت;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API مجازی ICUService_کلید* create_کلید(ثابت UnicodeString* id, UError_رمز& حالت_2) ثابت override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API مجازی ICUService_کلید* create_کلید(ثابت UnicodeString* id, int32_t قسم, UError_رمز& حالت_2) ثابت;

  دوست class ServiceEnumeration;
};

U_NAMESPACE_انتہا

    /* UCONFIG_NO_SERVICE */
#اگر_ختم

    /* ICULSERV_H */
#اگر_ختم

