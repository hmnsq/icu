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
#که_نه_تعریف_شوی ICULSERV_H
#تعریف_کړه ICULSERV_H

#ضمن "unicode/utypes.h"

#إذا UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_پای_ټکی

#وإلا

#ضمن "unicode/unistr.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/strenum.h"

#ضمن "hash.h"
#ضمن "uvector.h"

#ضمن "serv.h"
#ضمن "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_کلی;
class Locale_کلی_Factory;
class Simple_Locale_کلی_Factory;
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

class U_COMMON_API Locale_کلی : عام ICUService_کلی {
  خاص: 
    int32_t ډول_2;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  عام:
    تعداد {
        ډول_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساكن Locale_کلی* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID,
                                                  UError_کوډ& حالت);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساكن Locale_کلی* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID, 
                                                  int32_t ډول,
                                                  UError_کوډ& حالت);

  محمي:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_کلی(ثابت UnicodeString& primaryID, 
              ثابت UnicodeString& canonicalPrimaryID, 
              ثابت UnicodeString* canonicalFallbackID, 
              int32_t ډول);

 عام:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    مجازي UnicodeString& مختاړی(UnicodeString& پایله) ثابت override;

    /**
     * Return the kind code associated with this key.
     */
    مجازي int32_t ډول() ثابت;

    /**
     * Return the canonicalID.
     */
    مجازي UnicodeString& canonicalID(UnicodeString& پایله) ثابت override;

    /**
     * Return the currentID.
     */
    مجازي UnicodeString& currentID(UnicodeString& پایله) ثابت override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    مجازي UnicodeString& currentDescriptor(UnicodeString& پایله) ثابت override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    مجازي Locale& canonicalLocale(Locale& پایله) ثابت;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    مجازي Locale& currentLocale(Locale& پایله) ثابت;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    مجازي UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    مجازي UBool is_Fallback_د(ثابت UnicodeString& id) ثابت override;
    
 عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    مجازي UClassID getDynamicClassID() ثابت override;

    /**
     * Destructor.
     */
    مجازي ~Locale_کلی();

#که_تعریف_شوی SERVICE_DEBUG
 عام:
    مجازي UnicodeString& debug(UnicodeString& پایله) ثابت;
    مجازي UnicodeString& debugClass(UnicodeString& پایله) ثابت;
#د_که_پای

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
class U_COMMON_API Locale_کلی_Factory : عام ICUServiceFactory {
محمي:
    ثابت UnicodeString _name;
    ثابت int32_t _coverage;

عام:
    تعداد {
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
    مجازي ~Locale_کلی_Factory();

محمي:
    /**
     * Constructor used by subclasses.
     */
    Locale_کلی_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_کلی_Factory(int32_t coverage, ثابت UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
عام:
    مجازي UObject* create(ثابت ICUService_کلی& کلی, ثابت ICUService* service, UError_کوډ& حالت) ثابت override;

محمي:
    مجازي UBool handles_کلی(ثابت ICUService_کلی& کلی, UError_کوډ& حالت) ثابت;

عام:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    مجازي فراغ updateVisibleIDs(Hashtable& پایله, UError_کوډ& حالت) ثابت override;

    /**
     * Return a localized name for the locale represented by id.
     */
    مجازي UnicodeString& getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& پایله) ثابت override;

محمي:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    مجازي UObject* handleCreate(ثابت Locale& loc, int32_t ډول, ثابت ICUService* service, UError_کوډ& حالت) ثابت;

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
    مجازي ثابت Hashtable* getSupportedIDs(UError_کوډ& حالت) ثابت;

عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    مجازي UClassID getDynamicClassID() ثابت override;

#که_تعریف_شوی SERVICE_DEBUG
 عام:
    مجازي UnicodeString& debug(UnicodeString& پایله) ثابت;
    مجازي UnicodeString& debugClass(UnicodeString& پایله) ثابت;
#د_که_پای

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_کلی_Factory : عام Locale_کلی_Factory {
 خاص:
    UObject* _obj;
    UnicodeString _id;
    ثابت int32_t ډول_2;

 عام:
    Simple_Locale_کلی_Factory(UObject* obj_منزل_Adopt, 
                           ثابت UnicodeString& locale, 
                           int32_t ډول, 
                           int32_t coverage);

    Simple_Locale_کلی_Factory(UObject* obj_منزل_Adopt, 
                           ثابت Locale& locale, 
                           int32_t ډول, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    مجازي ~Simple_Locale_کلی_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    مجازي UObject* create(ثابت ICUService_کلی& کلی, ثابت ICUService* service, UError_کوډ& حالت) ثابت override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    مجازي فراغ updateVisibleIDs(Hashtable& پایله, UError_کوډ& حالت) ثابت override;

 محمي:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    مجازي UClassID getDynamicClassID() ثابت override;

#که_تعریف_شوی SERVICE_DEBUG
 عام:
    مجازي UnicodeString& debug(UnicodeString& پایله) ثابت;
    مجازي UnicodeString& debugClass(UnicodeString& پایله) ثابت;
#د_که_پای

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
class U_COMMON_API ICUResourceBundleFactory : عام Locale_کلی_Factory 
{
 محمي:
    UnicodeString _bundleName;

 عام:
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
    مجازي ~ICUResourceBundleFactory();

محمي:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    مجازي ثابت Hashtable* getSupportedIDs(UError_کوډ& حالت) ثابت override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    مجازي UObject* handleCreate(ثابت Locale& loc, int32_t ډول, ثابت ICUService* service, UError_کوډ& حالت) ثابت override;

عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();
    مجازي UClassID getDynamicClassID() ثابت override;


#که_تعریف_شوی SERVICE_DEBUG
 عام:
    مجازي UnicodeString& debug(UnicodeString& پایله) ثابت;
    مجازي UnicodeString& debugClass(UnicodeString& پایله) ثابت;
#د_که_پای

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : عام ICUService
{
 خاص:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 عام:
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
  U_COMMON_API مجازي ~ICULocaleService();

#إذا 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ثابت UnicodeString& descriptor, UError_کوډ& حالت) ثابت {
    أرجع ICUService::get(descriptor, حالت);
  }

  UObject* get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_کوډ& حالت) ثابت {
    أرجع ICUService::get(descriptor, actualReturn, حالت);
  }
#د_که_پای

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, UError_کوډ& حالت) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t ډول, UError_کوډ& حالت) ثابت;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, Locale* actualReturn, UError_کوډ& حالت) ثابت;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t ډول, Locale* actualReturn, UError_کوډ& حالت) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API مجازي URegistry_کلی ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, UError_کوډ& حالت);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API مجازي URegistry_کلی ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t ډول, UError_کوډ& حالت);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API مجازي URegistry_کلی ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t ډول, int32_t coverage, UError_کوډ& حالت);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API مجازي URegistry_کلی ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UBool visible, UError_کوډ& حالت) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API مجازي StringEnumeration* get_شته_Locales() ثابت U_LIFETIME_BOUND;

 محمي:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ثابت UnicodeString& validateFallbackLocale() ثابت;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API مجازي ICUService_کلی* create_کلی(ثابت UnicodeString* id, UError_کوډ& حالت) ثابت override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API مجازي ICUService_کلی* create_کلی(ثابت UnicodeString* id, int32_t ډول, UError_کوډ& حالت) ثابت;

  ملګری class ServiceEnumeration;
};

U_NAMESPACE_پای_ټکی

    /* UCONFIG_NO_SERVICE */
#د_که_پای

    /* ICULSERV_H */
#د_که_پای

