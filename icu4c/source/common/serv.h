// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2011, International Business Machines Corporation.       *
 * All Rights Reserved.                                                        *
 *******************************************************************************
 */

#ifndef ICUSERV_H
#define ICUSERV_H

#अन्तर्भाव "unicode/utypes.h"

#जइ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICUService;

U_NAMESPACE_END

#अण्णहा

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/umisc.h"

#अन्तर्भाव "hash.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "servnotf.h"

class ICUServiceTest;

U_NAMESPACE_BEGIN

class ICUService_कुञ्चिका;
class ICUServiceFactory;
class SimpleFactory;
class ServiceListener;
class ICUService;

class DNCache;

/*******************************************************************
 * ICUServiceKey
 */

/**
 * <p>ICUServiceKeys are used to communicate with factories to
 * generate an instance of the service.  ICUServiceKeys define how
 * ids are canonicalized, provide both a current id and a current
 * descriptor to use in querying the cache and factories, and
 * determine the fallback strategy.</p>
 *
 * <p>ICUServiceKeys provide both a currentDescriptor and a currentID.
 * The descriptor contains an optional prefix, followed by '/'
 * and the currentID.  Factories that handle complex keys,
 * for example number format factories that generate multiple
 * kinds of formatters for the same locale, use the descriptor 
 * to provide a fully unique identifier for the service object, 
 * while using the currentID (in this case, the locale string),
 * as the visible IDs that can be localized.</p>
 *
 * <p>The default implementation of ICUServiceKey has no fallbacks and
 * has no custom descriptors.</p> 
 */
class U_COMMON_API ICUService_कुञ्चिका : सार्वजनिक UObject {
 गुप्त: 
  ध्रुव UnicodeString _id;

 रक्षित:
  स्थिर ध्रुव char16_t PREFIX_DELIMITER;

 सार्वजनिक:

  /**
   * <p>Construct a key from an id.</p>
   *
   * @param id the ID from which to construct the key.
   */
  ICUService_कुञ्चिका(ध्रुव UnicodeString& id);

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~ICUService_कुञ्चिका();

 /**
  * <p>Return the original ID used to construct this key.</p>
  *
  * @return the ID used to construct this key.
  */
  आभास ध्रुव UnicodeString& getID() ध्रुव;

 /**
  * <p>Return the canonical version of the original ID.  This implementation
  * appends the original ID to result.  Result is returned as a convenience.</p>
  *
  * @param result the output parameter to which the id will be appended.
  * @return the modified result.
  */
  आभास UnicodeString& canonicalID(UnicodeString& result) ध्रुव;

 /**
  * <p>Return the (canonical) current ID.  This implementation appends
  * the canonical ID to result.  Result is returned as a convenience.</p>
  *
  * @param result the output parameter to which the current id will be appended.
  * @return the modified result.  
  */
  आभास UnicodeString& currentID(UnicodeString& result) ध्रुव;

 /**
  * <p>Return the current descriptor.  This implementation appends
  * the current descriptor to result.  Result is returned as a convenience.</p>
  *
  * <p>The current descriptor is used to fully
  * identify an instance of the service in the cache.  A
  * factory may handle all descriptors for an ID, or just a
  * particular descriptor.  The factory can either parse the
  * descriptor or use custom API on the key in order to
  * instantiate the service.</p>
  *
  * @param result the output parameter to which the current id will be appended.
  * @return the modified result.  
  */
  आभास UnicodeString& currentDescriptor(UnicodeString& result) ध्रुव;

 /**
  * <p>If the key has a fallback, modify the key and return true,
  * otherwise return false.  The current ID will change if there
  * is a fallback.  No currentIDs should be repeated, and fallback
  * must eventually return false.  This implementation has no fallbacks
  * and always returns false.</p>
  *
  * @return true if the ICUServiceKey changed to a valid fallback value.
  */
  आभास UBool fallback();

 /**
  * <p>Return true if a key created from id matches, or would eventually
  * fallback to match, the canonical ID of this ICUServiceKey.</p>
  *
  * @param id the id to test.
  * @return true if this ICUServiceKey's canonical ID is a fallback of id.
  */
  आभास UBool isFallbackOf(ध्रुव UnicodeString& id) ध्रुव;

 /**
  * <p>Return the prefix.  This implementation leaves result unchanged.
  * Result is returned as a convenience.</p>
  *
  * @param result the output parameter to which the prefix will be appended.
  * @return the modified result.
  */
  आभास UnicodeString& prefix(UnicodeString& result) ध्रुव;

 /**
  * <p>A utility to parse the prefix out of a descriptor string.  Only
  * the (undelimited) prefix, if any, remains in result.  Result is returned as a 
  * convenience.</p>
  *
  * @param result an input/output parameter that on entry is a descriptor, and 
  * on exit is the prefix of that descriptor.
  * @return the modified result.
  */
  स्थिर UnicodeString& parsePrefix(UnicodeString& result);

  /**
  * <p>A utility to parse the suffix out of a descriptor string.  Only
  * the (undelimited) suffix, if any, remains in result.  Result is returned as a 
  * convenience.</p>
  *
  * @param result an input/output parameter that on entry is a descriptor, and 
  * on exit is the suffix of that descriptor.
  * @return the modified result.
  */
  स्थिर UnicodeString& parseSuffix(UnicodeString& result);

सार्वजनिक:
  /**
   * UObject RTTI boilerplate.
   */
  स्थिर UClassID U_EXPORT2 getStaticClassID();

  /**
   * UObject RTTI boilerplate.
   */
  आभास UClassID getDynamicClassID() ध्रुव override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
  आभास UnicodeString& debug(UnicodeString& result) ध्रुव;
  आभास UnicodeString& debugClass(UnicodeString& result) ध्रुव;
#endif

};

 /*******************************************************************
  * ICUServiceFactory
  */

 /**
  * <p>An implementing ICUServiceFactory generates the service objects maintained by the
  * service.  A factory generates a service object from a key,
  * updates id->factory mappings, and returns the display name for
  * a supported id.</p>
  */
class U_COMMON_API ICUServiceFactory : सार्वजनिक UObject {
 सार्वजनिक:
    आभास ~ICUServiceFactory();

    /**
     * <p>Create a service object from the key, if this factory
     * supports the key.  Otherwise, return nullptr.</p>
     *
     * <p>If the factory supports the key, then it can call
     * the service's getKey(ICUServiceKey, String[], ICUServiceFactory) method
     * passing itself as the factory to get the object that
     * the service would have created prior to the factory's
     * registration with the service.  This can change the
     * key, so any information required from the key should
     * be extracted before making such a callback.</p>
     *
     * @param key the service key.
     * @param service the service with which this factory is registered.
     * @param status the error code status.
     * @return the service object, or nullptr if the factory does not support the key.
     */
    आभास UObject* create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UErrorCode& status) ध्रुव = 0;

    /**
     * <p>Update result to reflect the IDs (not descriptors) that this
     * factory publicly handles.  Result contains mappings from ID to
     * factory.  On entry it will contain all (visible) mappings from
     * previously-registered factories.</p>
     *
     * <p>This function, together with getDisplayName, are used to
     * support ICUService::getDisplayNames.  The factory determines
     * which IDs (of those it supports) it will make visible, and of
     * those, which it will provide localized display names for.  In
     * most cases it will register mappings from all IDs it supports
     * to itself.</p>
     *
     * @param result the mapping table to update.
     * @param status the error code status.
     */
    आभास शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव = 0;

    /**
     * <p>Return, in result, the display name of the id in the provided locale.
     * This is an id, not a descriptor.  If the id is 
     * not visible, sets result to bogus.  If the
     * incoming result is bogus, it remains bogus.  Result is returned as a
     * convenience.  Results are not defined if id is not one supported by this
         * factory.</p>
     *
     * @param id a visible id supported by this factory.
     * @param locale the locale for which to generate the corresponding localized display name.
     * @param result output parameter to hold the display name.
     * @return result.
     */
    आभास UnicodeString& getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& locale, UnicodeString& result) ध्रुव = 0;
};

/*
 ******************************************************************
 */

 /**
  * <p>A default implementation of factory.  This provides default
  * implementations for subclasses, and implements a singleton
  * factory that matches a single ID and returns a single
  * (possibly deferred-initialized) instance.  This implements
  * updateVisibleIDs to add a mapping from its ID to itself
  * if visible is true, or to remove any existing mapping
  * for its ID if visible is false.  No localization of display
  * names is performed.</p>
  */
class U_COMMON_API SimpleFactory : सार्वजनिक ICUServiceFactory {
 रक्षित:
  UObject* _instance;
  ध्रुव UnicodeString _id;
  ध्रुव UBool _visible;

 सार्वजनिक:
  /**
   * <p>Construct a SimpleFactory that maps a single ID to a single 
   * service instance.  If visible is true, the ID will be visible.
   * The instance must not be nullptr.  The SimpleFactory will adopt
   * the instance, which must not be changed subsequent to this call.</p>
   *
   * @param instanceToAdopt the service instance to adopt.
   * @param id the ID to assign to this service instance.
   * @param visible if true, the ID will be visible.
   */
  SimpleFactory(UObject* instanceToAdopt, ध्रुव UnicodeString& id, UBool visible = true);

  /**
   * <p>Destructor.</p>
   */
  आभास ~SimpleFactory();

  /**
   * <p>This implementation returns a clone of the service instance if the factory's ID is equal to
   * the key's currentID.  Service and prefix are ignored.</p>
   *
   * @param key the service key.
   * @param service the service with which this factory is registered.
   * @param status the error code status.
   * @return the service object, or nullptr if the factory does not support the key.
   */
  आभास UObject* create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UErrorCode& status) ध्रुव override;

  /**
   * <p>This implementation adds a mapping from ID -> this to result if visible is true, 
   * otherwise it removes ID from result.</p>
   *
   * @param result the mapping table to update.
   * @param status the error code status.
   */
  आभास शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव override;

  /**
   * <p>This implementation returns the factory ID if it equals id and visible is true,
   * otherwise it returns the empty string.  (This implementation provides
   * no localized id information.)</p>
   *
   * @param id a visible id supported by this factory.
   * @param locale the locale for which to generate the corresponding localized display name.
   * @param result output parameter to hold the display name.
   * @return result.
   */
  आभास UnicodeString& getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& locale, UnicodeString& result) ध्रुव override;

सार्वजनिक:
 /**
  * UObject RTTI boilerplate.
  */
  स्थिर UClassID U_EXPORT2 getStaticClassID();

 /**
  * UObject RTTI boilerplate.
  */
  आभास UClassID getDynamicClassID() ध्रुव override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
  आभास UnicodeString& debug(UnicodeString& toAppendTo) ध्रुव;
  आभास UnicodeString& debugClass(UnicodeString& toAppendTo) ध्रुव;
#endif

};

/*
 ******************************************************************
 */

/**
 * <p>ServiceListener is the listener that ICUService provides by default.
 * ICUService will notify this listener when factories are added to
 * or removed from the service.  Subclasses can provide
 * different listener interfaces that extend EventListener, and modify
 * acceptsListener and notifyListener as appropriate.</p>
 */
class U_COMMON_API ServiceListener : सार्वजनिक EventListener {
सार्वजनिक:
    आभास ~ServiceListener();

    /**
     * <p>This method is called when the service changes. At the time of the
     * call this listener is registered with the service.  It must
     * not modify the notifier in the context of this call.</p>
     * 
     * @param service the service that changed.
     */
    आभास शून्य serviceChanged(ध्रुव ICUService& service) ध्रुव = 0;
    
सार्वजनिक:
    /**
     * UObject RTTI boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    
    /**
     * UObject RTTI boilerplate.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;
    
};

/*
 ******************************************************************
 */

/**
 * <p>A StringPair holds a displayName/ID pair.  ICUService uses it
 * as the array elements returned by getDisplayNames.
 */
class U_COMMON_API StringPair : सार्वजनिक UMemory {
सार्वजनिक:
  /**
   * <p>The display name of the pair.</p>
   */
  ध्रुव UnicodeString displayName;

  /**
   * <p>The ID of the pair.</p>
   */
  ध्रुव UnicodeString id;

  /**
   * <p>Creates a string pair from a displayName and an ID.</p>
   *
   * @param displayName the displayName.
   * @param id the ID.
   * @param status the error code status.
   * @return a StringPair if the creation was successful, otherwise nullptr.
   */
  स्थिर StringPair* create(ध्रुव UnicodeString& displayName, 
                            ध्रुव UnicodeString& id,
                            UErrorCode& status);

  /**
   * <p>Return true if either string of the pair is bogus.</p>
   * @return true if either string of the pair is bogus.
   */
  UBool isBogus() ध्रुव;

गुप्त:
  StringPair(ध्रुव UnicodeString& displayName, ध्रुव UnicodeString& id);
};

/*******************************************************************
 * ICUService
 */

 /**
 * <p>A Service provides access to service objects that implement a
 * particular service, e.g. transliterators.  Users provide a String
 * id (for example, a locale string) to the service, and get back an
 * object for that id.  Service objects can be any kind of object.  A
 * new service object is returned for each query. The caller is
 * responsible for deleting it.</p>
 *
 * <p>Services 'canonicalize' the query ID and use the canonical ID to
 * query for the service.  The service also defines a mechanism to
 * 'fallback' the ID multiple times.  Clients can optionally request
 * the actual ID that was matched by a query when they use an ID to
 * retrieve a service object.</p>
 *
 * <p>Service objects are instantiated by ICUServiceFactory objects
 * registered with the service.  The service queries each
 * ICUServiceFactory in turn, from most recently registered to
 * earliest registered, until one returns a service object.  If none
 * responds with a service object, a fallback ID is generated, and the
 * process repeats until a service object is returned or until the ID
 * has no further fallbacks.</p>
 *
 * <p>In ICU 2.4, UObject (the base class of service instances) does
 * not define a polymorphic clone function.  ICUService uses clones to
 * manage ownership.  Thus, for now, ICUService defines an abstract
 * method, cloneInstance, that clients must implement to create clones
 * of the service instances.  This may change in future releases of
 * ICU.</p>
 *
 * <p>ICUServiceFactories can be dynamically registered and
 * unregistered with the service.  When registered, an
 * ICUServiceFactory is installed at the head of the factory list, and
 * so gets 'first crack' at any keys or fallback keys.  When
 * unregistered, it is removed from the service and can no longer be
 * located through it.  Service objects generated by this factory and
 * held by the client are unaffected.</p>
 *
 * <p>If a service has variants (e.g., the different variants of
 * BreakIterator) an ICUServiceFactory can use the prefix of the
 * ICUServiceKey to determine the variant of a service to generate.
 * If it does not support all variants, it can request
 * previously-registered factories to handle the ones it does not
 * support.</p>
 *
 * <p>ICUService uses ICUServiceKeys to query factories and perform
 * fallback.  The ICUServiceKey defines the canonical form of the ID,
 * and implements the fallback strategy.  Custom ICUServiceKeys can be
 * defined that parse complex IDs into components that
 * ICUServiceFactories can more easily use.  The ICUServiceKey can
 * cache the results of this parsing to save repeated effort.
 * ICUService provides convenience APIs that take UnicodeStrings and
 * generate default ICUServiceKeys for use in querying.</p>
 *
 * <p>ICUService provides API to get the list of IDs publicly
 * supported by the service (although queries aren't restricted to
 * this list).  This list contains only 'simple' IDs, and not fully
 * unique IDs.  ICUServiceFactories are associated with each simple ID
 * and the responsible factory can also return a human-readable
 * localized version of the simple ID, for use in user interfaces.
 * ICUService can also provide an array of the all the localized
 * visible IDs and their corresponding internal IDs.</p>
 *
 * <p>ICUService implements ICUNotifier, so that clients can register
 * to receive notification when factories are added or removed from
 * the service.  ICUService provides a default EventListener
 * subinterface, ServiceListener, which can be registered with the
 * service.  When the service changes, the ServiceListener's
 * serviceChanged method is called with the service as the
 * argument.</p>
 *
 * <p>The ICUService API is both rich and generic, and it is expected
 * that most implementations will statically 'wrap' ICUService to
 * present a more appropriate API-- for example, to declare the type
 * of the objects returned from get, to limit the factories that can
 * be registered with the service, or to define their own listener
 * interface with a custom callback method.  They might also customize
 * ICUService by overriding it, for example, to customize the
 * ICUServiceKey and fallback strategy.  ICULocaleService is a
 * subclass of ICUService that uses Locale names as IDs and uses
 * ICUServiceKeys that implement the standard resource bundle fallback
 * strategy.  Most clients will wish to subclass it instead of
 * ICUService.</p> 
 */
class U_COMMON_API ICUService : सार्वजनिक ICUNotifier {
 रक्षित: 
    /**
     * Name useful for debugging.
     */
    ध्रुव UnicodeString name;

 गुप्त:

    /**
     * Timestamp so iterators can be fail-fast.
     */
    uint32_t timestamp;

    /**
     * All the factories registered with this service.
     */
    UVector* factories;

    /**
     * The service cache.
     */
    Hashtable* serviceCache;

    /**
     * The ID cache.
     */
    Hashtable* idCache;

    /**
     * The name cache.
     */
    DNCache* dnCache;

    /**
     * Constructor.
     */
 सार्वजनिक:
    /**
     * <p>Construct a new ICUService.</p>
     */
    ICUService();

    /**
     * <p>Construct with a name (useful for debugging).</p>
     *
     * @param name a name to use in debugging.
     */
    ICUService(ध्रुव UnicodeString& name);

    /**
     * <p>Destructor.</p>
     */
    आभास ~ICUService();

    /**
     * <p>Return the name of this service. This will be the empty string if none was assigned.
     * Returns result as a convenience.</p>
     *
     * @param result an output parameter to contain the name of this service.
     * @return the name of this service.
     */
    UnicodeString& getName(UnicodeString& result) ध्रुव;

    /**
     * <p>Convenience override for get(ICUServiceKey&, UnicodeString*). This uses
     * createKey to create a key for the provided descriptor.</p>
     *
     * @param descriptor the descriptor.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    UObject* get(ध्रुव UnicodeString& descriptor, UErrorCode& status) ध्रुव;

    /**
     * <p>Convenience override for get(ICUServiceKey&, UnicodeString*).  This uses
     * createKey to create a key from the provided descriptor.</p>
     *
     * @param descriptor the descriptor.
     * @param actualReturn a pointer to a UnicodeString to hold the matched descriptor, or nullptr.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    UObject* get(ध्रुव UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ध्रुव;

    /**
     * <p>Convenience override for get(ICUServiceKey&, UnicodeString*).</p>
     *
     * @param key the key.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    UObject* get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UErrorCode& status) ध्रुव;

    /**
     * <p>Given a key, return a service object, and, if actualReturn
     * is not nullptr, the descriptor with which it was found in the
     * first element of actualReturn.  If no service object matches
     * this key, returns nullptr and leaves actualReturn unchanged.</p>
     *
     * <p>This queries the cache using the key's descriptor, and if no
     * object in the cache matches, tries the key on each
     * registered factory, in order.  If none generates a service
     * object for the key, repeats the process with each fallback of
     * the key, until either a factory returns a service object, or the key
     * has no fallback.  If no object is found, the result of handleDefault
     * is returned.</p>
     *
     * <p>Subclasses can override this method to further customize the 
     * result before returning it.
     *
     * @param key the key.
     * @param actualReturn a pointer to a UnicodeString to hold the matched descriptor, or nullptr.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    आभास UObject* get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, UErrorCode& status) ध्रुव;

    /**
     * <p>This version of getKey is only called by ICUServiceFactories within the scope
     * of a previous getKey call, to determine what previously-registered factories would
     * have returned.  For details, see getKey(ICUServiceKey&, UErrorCode&).  Subclasses
     * should not call it directly, but call through one of the other get functions.</p>
     * 
     * @param key the key.
     * @param actualReturn a pointer to a UnicodeString to hold the matched descriptor, or nullptr.
     * @param factory the factory making the recursive call.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    UObject* get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, ध्रुव ICUServiceFactory* factory, UErrorCode& status) ध्रुव;

    /**
     * <p>Convenience override for getVisibleIDs(String) that passes null
     * as the fallback, thus returning all visible IDs.</p>
     *
     * @param result a vector to hold the returned IDs.
     * @param status the error code status.
     * @return the result vector.
     */
    UVector& getVisibleIDs(UVector& result, UErrorCode& status) ध्रुव;

    /**
     * <p>Return a snapshot of the visible IDs for this service.  This
     * list will not change as ICUServiceFactories are added or removed, but the
     * supported IDs will, so there is no guarantee that all and only
     * the IDs in the returned list will be visible and supported by the
     * service in subsequent calls.</p>
     *
     * <p>The IDs are returned as pointers to UnicodeStrings.  The
     * caller owns the IDs.  Previous contents of result are discarded before
     * new elements, if any, are added.</p>
     *
     * <p>matchID is passed to createKey to create a key.  If the key
     * is not nullptr, its isFallbackOf method is used to filter out IDs
     * that don't match the key or have it as a fallback.</p>
     *
     * @param result a vector to hold the returned IDs.
     * @param matchID an ID used to filter the result, or nullptr if all IDs are desired.
     * @param status the error code status.
     * @return the result vector.
     */
    UVector& getVisibleIDs(UVector& result, ध्रुव UnicodeString* matchID, UErrorCode& status) ध्रुव;

    /**
     * <p>Convenience override for getDisplayName(const UnicodeString&, const Locale&, UnicodeString&) that
     * uses the current default locale.</p>
     *
     * @param id the ID for which to retrieve the localized displayName.
     * @param result an output parameter to hold the display name.
     * @return the modified result.
     */
    UnicodeString& getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result) ध्रुव;

    /**
     * <p>Given a visible ID, return the display name in the requested locale.
     * If there is no directly supported ID corresponding to this ID, result is
     * set to bogus.</p>
     *
     * @param id the ID for which to retrieve the localized displayName.
     * @param result an output parameter to hold the display name.
     * @param locale the locale in which to localize the ID.
     * @return the modified result.
     */
    UnicodeString& getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result, ध्रुव Locale& locale) ध्रुव;

    /**
     * <p>Convenience override of getDisplayNames(const Locale&, const UnicodeString*) that 
     * uses the current default Locale as the locale and nullptr for
     * the matchID.</p>
     *
     * @param result a vector to hold the returned displayName/id StringPairs.
     * @param status the error code status.
     * @return the modified result vector.
     */
    UVector& getDisplayNames(UVector& result, UErrorCode& status) ध्रुव;

    /**
     * <p>Convenience override of getDisplayNames(const Locale&, const UnicodeString*) that 
     * uses nullptr for the matchID.</p>
     *
     * @param result a vector to hold the returned displayName/id StringPairs.
     * @param locale the locale in which to localize the ID.
     * @param status the error code status.
     * @return the modified result vector.
     */
    UVector& getDisplayNames(UVector& result, ध्रुव Locale& locale, UErrorCode& status) ध्रुव;

    /**
     * <p>Return a snapshot of the mapping from display names to visible
     * IDs for this service.  This set will not change as factories
     * are added or removed, but the supported IDs will, so there is
     * no guarantee that all and only the IDs in the returned map will
     * be visible and supported by the service in subsequent calls,
     * nor is there any guarantee that the current display names match
     * those in the result.</p>
     *
     * <p>The names are returned as pointers to StringPairs, which
     * contain both the displayName and the corresponding ID.  The
     * caller owns the StringPairs.  Previous contents of result are
     * discarded before new elements, if any, are added.</p>
     *
     * <p>matchID is passed to createKey to create a key.  If the key
     * is not nullptr, its isFallbackOf method is used to filter out IDs
     * that don't match the key or have it as a fallback.</p>
     *
     * @param result a vector to hold the returned displayName/id StringPairs.
     * @param locale the locale in which to localize the ID.
     * @param matchID an ID used to filter the result, or nullptr if all IDs are desired.
     * @param status the error code status.
     * @return the result vector.  */
    UVector& getDisplayNames(UVector& result,
                             ध्रुव Locale& locale, 
                             ध्रुव UnicodeString* matchID, 
                             UErrorCode& status) ध्रुव;

    /**
     * <p>A convenience override of registerInstance(UObject*, const UnicodeString&, UBool)
     * that defaults visible to true.</p>
     *
     * @param objToAdopt the object to register and adopt.
     * @param id the ID to assign to this object.
     * @param status the error code status.
     * @return a registry key that can be passed to unregister to unregister
     * (and discard) this instance.
     */
    URegistry_कुञ्चिका registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UErrorCode& status);

    /**
     * <p>Register a service instance with the provided ID.  The ID will be 
     * canonicalized.  The canonicalized ID will be returned by
     * getVisibleIDs if visible is true.  The service instance will be adopted and
     * must not be modified subsequent to this call.</p>
     *
     * <p>This issues a serviceChanged notification to registered listeners.</p>
     *
     * <p>This implementation wraps the object using
     * createSimpleFactory, and calls registerFactory.</p>
     *
     * @param objToAdopt the object to register and adopt.
     * @param id the ID to assign to this object.
     * @param visible true if getVisibleIDs is to return this ID.
     * @param status the error code status.
     * @return a registry key that can be passed to unregister() to unregister
     * (and discard) this instance.
     */
    आभास URegistry_कुञ्चिका registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status);

    /**
     * <p>Register an ICUServiceFactory.  Returns a registry key that
     * can be used to unregister the factory.  The factory
     * must not be modified subsequent to this call.  The service owns
     * all registered factories. In case of an error, the factory is
     * deleted.</p>
     *
     * <p>This issues a serviceChanged notification to registered listeners.</p>
     *
     * <p>The default implementation accepts all factories.</p>
     *
     * @param factoryToAdopt the factory to register and adopt.
     * @param status the error code status.
     * @return a registry key that can be passed to unregister to unregister
     * (and discard) this factory.
     */
    आभास URegistry_कुञ्चिका registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status);

    /**
     * <p>Unregister a factory using a registry key returned by
     * registerInstance or registerFactory.  After a successful call,
     * the factory will be removed from the service factory list and
     * deleted, and the key becomes invalid.</p>
     *
     * <p>This issues a serviceChanged notification to registered
     * listeners.</p>
     *
     * @param rkey the registry key.
     * @param status the error code status.  
     * @return true if the call successfully unregistered the factory.
     */
    आभास UBool unregister(URegistry_कुञ्चिका rkey, UErrorCode& status);

    /**
     * </p>Reset the service to the default factories.  The factory
     * lock is acquired and then reInitializeFactories is called.</p>
     *
     * <p>This issues a serviceChanged notification to registered listeners.</p>
     */
    आभास शून्य reset();

    /**
     * <p>Return true if the service is in its default state.</p>
     *
     * <p>The default implementation returns true if there are no 
     * factories registered.</p>
     */
    आभास UBool isDefault() ध्रुव;

    /**
     * <p>Create a key from an ID.  If ID is nullptr, returns nullptr.</p>
     *
     * <p>The default implementation creates an ICUServiceKey instance.
     * Subclasses can override to define more useful keys appropriate
     * to the factories they accept.</p>
     *
     * @param a pointer to the ID for which to create a default ICUServiceKey.
     * @param status the error code status.
     * @return the ICUServiceKey corresponding to ID, or nullptr.
     */
    आभास ICUService_कुञ्चिका* create_कुञ्चिका(ध्रुव UnicodeString* id, UErrorCode& status) ध्रुव;

    /**
     * <p>Clone object so that caller can own the copy.  In ICU2.4, UObject doesn't define
     * clone, so we need an instance-aware method that knows how to do this.
     * This is public so factories can call it, but should really be protected.</p>
     *
     * @param instance the service instance to clone.
     * @return a clone of the passed-in instance, or nullptr if cloning was unsuccessful.
     */
    आभास UObject* cloneInstance(UObject* instance) ध्रुव = 0;


    /************************************************************************
     * Subclassing API
     */

 रक्षित:

    /**
     * <p>Create a factory that wraps a single service object.  Called by registerInstance.</p>
     *
     * <p>The default implementation returns an instance of SimpleFactory.</p>
     *
     * @param instanceToAdopt the service instance to adopt.
     * @param id the ID to assign to this service instance.
     * @param visible if true, the ID will be visible.
     * @param status the error code status.
     * @return an instance of ICUServiceFactory that maps this instance to the provided ID.
     */
    आभास ICUServiceFactory* createSimpleFactory(UObject* instanceToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status);

    /**
     * <p>Reinitialize the factory list to its default state.  After this call, isDefault()
     * must return true.</p>
     *
     * <p>This issues a serviceChanged notification to registered listeners.</p>
     *
     * <p>The default implementation clears the factory list.
     * Subclasses can override to provide other default initialization
     * of the factory list.  Subclasses must not call this method
     * directly, since it must only be called while holding write
     * access to the factory list.</p>
     */
    आभास शून्य reInitializeFactories();

    /**
     * <p>Default handler for this service if no factory in the factory list
     * handled the key passed to getKey.</p>
     *
     * <p>The default implementation returns nullptr.</p>
     *
     * @param key the key.
     * @param actualReturn a pointer to a UnicodeString to hold the matched descriptor, or nullptr.
     * @param status the error code status.
     * @return the service instance, or nullptr.
     */
    आभास UObject* handleDefault(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, UErrorCode& status) ध्रुव;

    /**
     * <p>Clear caches maintained by this service.</p>
     *
     * <p>Subclasses can override if they implement additional caches
     * that need to be cleared when the service changes.  Subclasses
     * should generally not call this method directly, as it must only
     * be called while synchronized on the factory lock.</p>
     */
    आभास शून्य clearCaches();

    /**
     * <p>Return true if the listener is accepted.</p>
     *
     * <p>The default implementation accepts the listener if it is
     * a ServiceListener.  Subclasses can override this to accept
     * different listeners.</p>
     *
     * @param l the listener to test.
     * @return true if the service accepts the listener.
     */
    आभास UBool acceptsListener(ध्रुव EventListener& l) ध्रुव override;

    /**
     * <p>Notify the listener of a service change.</p>
     *
     * <p>The default implementation assumes a ServiceListener.
     * If acceptsListener has been overridden to accept different
     * listeners, this should be overridden as well.</p>
     *
     * @param l the listener to notify.
     */
    आभास शून्य notifyListener(EventListener& l) ध्रुव override;

    /************************************************************************
     * Utilities for subclasses.
     */

    /**
     * <p>Clear only the service cache.</p>
     *
     * <p>This can be called by subclasses when a change affects the service
     * cache but not the ID caches, e.g., when the default locale changes
     * the resolution of IDs also changes, requiring the cache to be
     * flushed, but not the visible IDs themselves.</p>
     */
    शून्य clearServiceCache();

    /**
     * <p>Return a map from visible IDs to factories.
     * This must only be called when the mutex is held.</p>
     *
     * @param status the error code status.
     * @return a Hashtable containing mappings from visible
     * IDs to factories.
     */
    ध्रुव Hashtable* getVisibleIDMap(UErrorCode& status) ध्रुव;

    /**
     * <p>Allow subclasses to read the time stamp.</p>
     *
     * @return the timestamp.
     */
    int32_t getTimestamp() ध्रुव;

    /**
     * <p>Return the number of registered factories.</p>
     *
     * @return the number of factories registered at the time of the call.
     */
    int32_t गणनम्_Factories() ध्रुव;

गुप्त:

    friend class ::ICUServiceTest; // give tests access to countFactories.
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICUSERV_H */
#endif

