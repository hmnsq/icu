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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ICULSERV_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICULSERV_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_ਸਿਰਾ

#ਨਹੀਂਤਾਂ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/strenum.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "serv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_ਕੁੰਜੀ;
class Locale_ਕੁੰਜੀ_Factory;
class Simple_Locale_ਕੁੰਜੀ_Factory;
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

class U_COMMON_API Locale_ਕੁੰਜੀ : ਜਨਤਕ ICUService_ਕੁੰਜੀ {
  ਨਿਜੀ: 
    int32_t ਕਿਸਮ_2;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  ਜਨਤਕ:
    ਗਿਣਤੀ {
        ਕਿਸਮ_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ਸਥਿਰ Locale_ਕੁੰਜੀ* createWithCanonicalFallback(ਅਚਲ UnicodeString* primaryID, 
                                                  ਅਚਲ UnicodeString* canonicalFallbackID,
                                                  UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ਸਥਿਰ Locale_ਕੁੰਜੀ* createWithCanonicalFallback(ਅਚਲ UnicodeString* primaryID, 
                                                  ਅਚਲ UnicodeString* canonicalFallbackID, 
                                                  int32_t ਕਿਸਮ,
                                                  UError_ਕੋਡ& ਸਥਿਤੀ);

  ਸੁਰੱਖਿਅਤ:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_ਕੁੰਜੀ(ਅਚਲ UnicodeString& primaryID, 
              ਅਚਲ UnicodeString& canonicalPrimaryID, 
              ਅਚਲ UnicodeString* canonicalFallbackID, 
              int32_t ਕਿਸਮ);

 ਜਨਤਕ:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    ਆਭਾਸੀ UnicodeString& ਅਗੇਤਰ(UnicodeString& ਨਤੀਜਾ) ਅਚਲ override;

    /**
     * Return the kind code associated with this key.
     */
    ਆਭਾਸੀ int32_t ਕਿਸਮ() ਅਚਲ;

    /**
     * Return the canonicalID.
     */
    ਆਭਾਸੀ UnicodeString& canonicalID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ override;

    /**
     * Return the currentID.
     */
    ਆਭਾਸੀ UnicodeString& currentID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    ਆਭਾਸੀ UnicodeString& currentDescriptor(UnicodeString& ਨਤੀਜਾ) ਅਚਲ override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    ਆਭਾਸੀ Locale& canonicalLocale(Locale& ਨਤੀਜਾ) ਅਚਲ;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    ਆਭਾਸੀ Locale& currentLocale(Locale& ਨਤੀਜਾ) ਅਚਲ;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    ਆਭਾਸੀ UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    ਆਭਾਸੀ UBool is_Fallback_ਦਾ(ਅਚਲ UnicodeString& id) ਅਚਲ override;
    
 ਜਨਤਕ:
    /**
     * UObject boilerplate.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

    /**
     * Destructor.
     */
    ਆਭਾਸੀ ~Locale_ਕੁੰਜੀ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
 ਜਨਤਕ:
    ਆਭਾਸੀ UnicodeString& debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
    ਆਭਾਸੀ UnicodeString& debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
#ਜੇ_ਅੰਤ

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
class U_COMMON_API Locale_ਕੁੰਜੀ_Factory : ਜਨਤਕ ICUServiceFactory {
ਸੁਰੱਖਿਅਤ:
    ਅਚਲ UnicodeString _name;
    ਅਚਲ int32_t _coverage;

ਜਨਤਕ:
    ਗਿਣਤੀ {
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
    ਆਭਾਸੀ ~Locale_ਕੁੰਜੀ_Factory();

ਸੁਰੱਖਿਅਤ:
    /**
     * Constructor used by subclasses.
     */
    Locale_ਕੁੰਜੀ_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_ਕੁੰਜੀ_Factory(int32_t coverage, ਅਚਲ UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
ਜਨਤਕ:
    ਆਭਾਸੀ UObject* create(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

ਸੁਰੱਖਿਅਤ:
    ਆਭਾਸੀ UBool handles_ਕੁੰਜੀ(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

ਜਨਤਕ:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ਆਭਾਸੀ ਖਾਲੀ updateVisibleIDs(Hashtable& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

    /**
     * Return a localized name for the locale represented by id.
     */
    ਆਭਾਸੀ UnicodeString& getDisplayName(ਅਚਲ UnicodeString& id, ਅਚਲ Locale& locale, UnicodeString& ਨਤੀਜਾ) ਅਚਲ override;

ਸੁਰੱਖਿਅਤ:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    ਆਭਾਸੀ UObject* handleCreate(ਅਚਲ Locale& loc, int32_t ਕਿਸਮ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

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
    ਆਭਾਸੀ ਅਚਲ Hashtable* getSupportedIDs(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

ਜਨਤਕ:
    /**
     * UObject boilerplate.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
 ਜਨਤਕ:
    ਆਭਾਸੀ UnicodeString& debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
    ਆਭਾਸੀ UnicodeString& debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
#ਜੇ_ਅੰਤ

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_ਕੁੰਜੀ_Factory : ਜਨਤਕ Locale_ਕੁੰਜੀ_Factory {
 ਨਿਜੀ:
    UObject* _obj;
    UnicodeString _id;
    ਅਚਲ int32_t ਕਿਸਮ_2;

 ਜਨਤਕ:
    Simple_Locale_ਕੁੰਜੀ_Factory(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, 
                           ਅਚਲ UnicodeString& locale, 
                           int32_t ਕਿਸਮ, 
                           int32_t coverage);

    Simple_Locale_ਕੁੰਜੀ_Factory(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, 
                           ਅਚਲ Locale& locale, 
                           int32_t ਕਿਸਮ, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    ਆਭਾਸੀ ~Simple_Locale_ਕੁੰਜੀ_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    ਆਭਾਸੀ UObject* create(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ਆਭਾਸੀ ਖਾਲੀ updateVisibleIDs(Hashtable& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

 ਸੁਰੱਖਿਅਤ:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


ਜਨਤਕ:
    /**
     * UObject boilerplate.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
 ਜਨਤਕ:
    ਆਭਾਸੀ UnicodeString& debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
    ਆਭਾਸੀ UnicodeString& debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
#ਜੇ_ਅੰਤ

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
class U_COMMON_API ICUResourceBundleFactory : ਜਨਤਕ Locale_ਕੁੰਜੀ_Factory 
{
 ਸੁਰੱਖਿਅਤ:
    UnicodeString _bundleName;

 ਜਨਤਕ:
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
    ICUResourceBundleFactory(ਅਚਲ UnicodeString& bundleName);

    /**
     * Destructor
     */
    ਆਭਾਸੀ ~ICUResourceBundleFactory();

ਸੁਰੱਖਿਅਤ:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    ਆਭਾਸੀ ਅਚਲ Hashtable* getSupportedIDs(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    ਆਭਾਸੀ UObject* handleCreate(ਅਚਲ Locale& loc, int32_t ਕਿਸਮ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

ਜਨਤਕ:
    /**
     * UObject boilerplate.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
 ਜਨਤਕ:
    ਆਭਾਸੀ UnicodeString& debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
    ਆਭਾਸੀ UnicodeString& debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ;
#ਜੇ_ਅੰਤ

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : ਜਨਤਕ ICUService
{
 ਨਿਜੀ:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 ਜਨਤਕ:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ਅਚਲ UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API ਆਭਾਸੀ ~ICULocaleService();

#ਜੇ 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ਅਚਲ UnicodeString& descriptor, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਵਾਪਸ ICUService::get(descriptor, ਸਥਿਤੀ);
  }

  UObject* get(ਅਚਲ UnicodeString& descriptor, UnicodeString* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਵਾਪਸ ICUService::get(descriptor, actualReturn, ਸਥਿਤੀ);
  }
#ਜੇ_ਅੰਤ

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ਅਚਲ Locale& locale, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ਅਚਲ Locale& locale, Locale* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, Locale* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API ਆਭਾਸੀ URegistry_ਕੁੰਜੀ ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, UError_ਕੋਡ& ਸਥਿਤੀ);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API ਆਭਾਸੀ URegistry_ਕੁੰਜੀ ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API ਆਭਾਸੀ URegistry_ਕੁੰਜੀ ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, int32_t coverage, UError_ਕੋਡ& ਸਥਿਤੀ);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API ਆਭਾਸੀ URegistry_ਕੁੰਜੀ ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& locale, UBool visible, UError_ਕੋਡ& ਸਥਿਤੀ) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API ਆਭਾਸੀ StringEnumeration* get_ਉਪਲਬਧ_Locales() ਅਚਲ U_LIFETIME_BOUND;

 ਸੁਰੱਖਿਅਤ:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ਅਚਲ UnicodeString& validateFallbackLocale() ਅਚਲ;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API ਆਭਾਸੀ ICUService_ਕੁੰਜੀ* create_ਕੁੰਜੀ(ਅਚਲ UnicodeString* id, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API ਆਭਾਸੀ ICUService_ਕੁੰਜੀ* create_ਕੁੰਜੀ(ਅਚਲ UnicodeString* id, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ;

  ਮਿੱਤਰ class ServiceEnumeration;
};

U_NAMESPACE_ਸਿਰਾ

    /* UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ

    /* ICULSERV_H */
#ਜੇ_ਅੰਤ

