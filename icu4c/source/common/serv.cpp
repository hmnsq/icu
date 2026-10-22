// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/localpointer.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাৱ "serv.h"
#অন্তর্ভাৱ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ধ্রুৱ char16_t ICUService_চাবি::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_চাবি::ICUService_চাবি(ধ্রুৱ UnicodeString& id) 
: _id(id) {
}

ICUService_চাবি::~ICUService_চাবি() 
{
}

ধ্রুৱ UnicodeString& 
ICUService_চাবি::getID() ধ্রুৱ 
{
    ঘূৰাই _id;
}

UnicodeString& 
ICUService_চাবি::canonicalID(UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই result.append(_id);
}

UnicodeString& 
ICUService_চাবি::currentID(UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই canonicalID(result);
}

UnicodeString& 
ICUService_চাবি::currentDescriptor(UnicodeString& result) ধ্রুৱ 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ঘূৰাই currentID(result);
}

UBool 
ICUService_চাবি::fallback() 
{
    ঘূৰাই মিছা;
}

UBool 
ICUService_চাবি::isFallbackOf(ধ্রুৱ UnicodeString& id) ধ্রুৱ 
{
    ঘূৰাই id == _id;
}

UnicodeString& 
ICUService_চাবি::prefix(UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই result;
}

UnicodeString& 
ICUService_চাবি::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    যদি (n < 0) {
        n = 0;
    }
    result.remove(n);
    ঘূৰাই result;
}

UnicodeString& 
ICUService_চাবি::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    যদি (n >= 0) {
        result.remove(0, n+1);
    }
    ঘূৰাই result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_চাবি::debug(UnicodeString& result) ধ্রুৱ 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ঘূৰাই result;
}

UnicodeString& 
ICUService_চাবি::debugClass(UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_চাবি)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ধ্রুৱ UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ধ্রুৱ ICUService_চাবি& চাবি, ধ্রুৱ ICUService* service, UErrorCode& status) ধ্রুৱ 
{
    যদি (U_SUCCESS(status)) {
        UnicodeString temp;
        যদি (_id == চাবি.currentID(temp)) {
            ঘূৰাই service->cloneInstance(_instance); 
        }
    }
    ঘূৰাই nullptr;
}

শূন্য 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ধ্রুৱ 
{
    যদি (_visible) {
        result.put(_id, (শূন্য*)এতত্, status); // cast away const
    } অন্যথা {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ধ্রুৱ UnicodeString& id, ধ্রুৱ Locale& /* locale */, UnicodeString& result) ধ্রুৱ 
{
    যদি (_visible && _id == id) {
        result = _id;
    } অন্যথা {
        result.setToBogus();
    }
    ঘূৰাই result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ধ্রুৱ 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ঘূৰাই toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ধ্রুৱ 
{
    ঘূৰাই toAppendTo.append((UnicodeString)"SimpleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleFactory)

/*
******************************************************************
*/

ServiceListener::~ServiceListener() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceListener)

/*
******************************************************************
*/

// Record the actual id for this service in the cache, so we can return it
// even if we succeed later with a different id.
শ্ৰেণী CacheEntry : সার্ৱজনিক UMemory {
গুপ্ত:
    int32_t refcount;

সার্ৱজনিক:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ধ্রুৱ UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    CacheEntry* ref() {
        ++refcount;
        ঘূৰাই এতত্;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        যদি ((--refcount) == 0) {
            delete এতত্;
            ঘূৰাই nullptr;
        }
        ঘূৰাই এতত্;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ধ্রুৱ {
        ঘূৰাই refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
cacheDeleter(শূন্য* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

শ্ৰেণী DNCache : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    Hashtable cache;
    ধ্রুৱ Locale locale;

    DNCache(ধ্রুৱ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ধ্রুৱ UnicodeString& displayName, 
                   ধ্রুৱ UnicodeString& id,
                   UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        StringPair* sp = নূতন StringPair(displayName, id);
        যদি (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ঘূৰাই nullptr;
        }
        ঘূৰাই sp;
    }
    ঘূৰাই nullptr;
}

UBool 
StringPair::isBogus() ধ্রুৱ {
    ঘূৰাই displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ধ্রুৱ UnicodeString& _displayName, 
                       ধ্রুৱ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
userv_deleteStringPair(শূন্য *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

স্থির UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ধ্রুৱ UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&lock);
        clearCaches();
        delete factories;
        factories = nullptr;
    }
}

UObject* 
ICUService::get(ধ্রুৱ UnicodeString& descriptor, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ধ্রুৱ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ধ্রুৱ 
{
    UObject* result = nullptr;
    ICUService_চাবি* চাবি = create_চাবি(&descriptor, status);
    যদি (চাবি) {
        result = get_চাবি(*চাবি, actualReturn, status);
        delete চাবি;
    }
    ঘূৰাই result;
}

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই get_চাবি(চাবি, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই get_চাবি(চাবি, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
শ্ৰেণী XMutex : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    অন্তঃস্থ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        যদি (fActive) umtx_lock(fMutex);
    }
    অন্তঃস্থ ~XMutex() {
        যদি (fActive) umtx_unlock(fMutex);
    }

গুপ্ত:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, ধ্রুৱ ICUServiceFactory* factory, UErrorCode& status) ধ্রুৱ 
{
    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

    যদি (isDefault()) {
        ঘূৰাই handleDefault(চাবি, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(এতত্); // cast away semantic const

    CacheEntry* result = nullptr;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&lock, factory != nullptr);

        যদি (serviceCache == nullptr) {
            ncthis->serviceCache = নূতন Hashtable(status);
            যদি (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ঘূৰাই nullptr;
            }
            যদি (U_FAILURE(status)) {
                delete serviceCache;
                ঘূৰাই nullptr;
            }
            serviceCache->set_মান_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = মিছা;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = সঁচা;

        যদি (factory != nullptr) {
            বাবে (int32_t i = 0; i < limit; ++i) {
                যদি (factory == static_cast<ধ্রুৱ ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    ভঙ্গ;
                }
            }
            যদি (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই nullptr;
            }
            cacheResult = মিছা;
        }

        কৰা {
            currentDescriptor.remove();
            চাবি.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            যদি (result != nullptr) {
                ভঙ্গ;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = সঁচা;

            int32_t index = startIndex;
            যেতিয়ালৈ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(চাবি, এতত্, status));
                যদি (U_FAILURE(status)) {
                    ঘূৰাই nullptr;
                }
                যদি (service.isValid()) {
                    result = নূতন CacheEntry(currentDescriptor, service.getAlias());
                    যদি (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ঘূৰাই nullptr;
                    }
                    service.orphan(); // result now owns service.

                    গচ্ছ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            যদি (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(নূতন UVector(uprv_deleteUObject, nullptr, 5, status), status);
                যদি (U_FAILURE(status)) {
                    ঘূৰাই nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(নূতন UnicodeString(currentDescriptor), status);
            যদি (U_FAILURE(status)) {
                ঘূৰাই nullptr;
            }
            যদি (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ঘূৰাই nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            যদি (U_FAILURE(status)) {
                ঘূৰাই nullptr;
            }
        } যেতিয়ালৈ (চাবি.fallback());
outerEnd:

        যদি (result != nullptr) {
            যদি (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                যদি (U_FAILURE(status)) {
                    ঘূৰাই nullptr;
                }

                যদি (cacheDescriptorList.isValid()) {
                    বাবে (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        যদি (U_FAILURE(status)) {
                            ঘূৰাই nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            যদি (actualReturn != nullptr) {
                // strip null prefix
                যদি (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } অন্যথা {
                    *actualReturn = result->actualDescriptor;
                }

                যদি (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ঘূৰাই nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            যদি (putInCache && !cacheResult) {
                delete result;
            }
            ঘূৰাই service;
        }
    }

    ঘূৰাই handleDefault(চাবি, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ধ্রুৱ ICUService_চাবি& /*চাবি*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ধ্রুৱ 
{
    ঘূৰাই nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ধ্রুৱ {
    ঘূৰাই getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ধ্রুৱ UnicodeString* matchID, UErrorCode& status) ধ্রুৱ 
{
    result.removeAllElements();

    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ধ্রুৱ Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUService_চাবি* fallback_চাবি = create_চাবি(matchID, status);

            বাবে (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ধ্রুৱ UHashElement* e = map->nextElement(pos);
                যদি (e == nullptr) {
                    ভঙ্গ;
                }

                ধ্রুৱ UnicodeString* id = static_cast<ধ্রুৱ UnicodeString*>(e->চাবি.pointer);
                যদি (fallback_চাবি != nullptr) {
                    যদি (!fallback_চাবি->isFallbackOf(*id)) {
                        অনুৱর্ত;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_চাবি;
        }
    }
    যদি (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ঘূৰাই result;
}

ধ্রুৱ Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ধ্রুৱ {
    যদি (U_FAILURE(status)) ঘূৰাই nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(এতত্); // cast away semantic const
    যদি (idCache == nullptr) {
        ncthis->idCache = নূতন Hashtable(status);
        যদি (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা যদি (factories != nullptr) {
            বাবে (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            যদি (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ঘূৰাই idCache;
}


UnicodeString& 
ICUService::getDisplayName(ধ্রুৱ UnicodeString& id, UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ধ্রুৱ UnicodeString& id, UnicodeString& result, ধ্রুৱ Locale& locale) ধ্রুৱ 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ধ্রুৱ Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            যদি (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ঘূৰাই result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_চাবি* fallback_চাবি = create_চাবি(&id, status);
            যেতিয়ালৈ (fallback_চাবি != nullptr && fallback_চাবি->fallback()) {
                UnicodeString us;
                fallback_চাবি->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                যদি (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_চাবি;
                    ঘূৰাই result;
                }
            }
            delete fallback_চাবি;
        }
    }
    result.setToBogus();
    ঘূৰাই result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ধ্রুৱ Locale& locale, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ধ্রুৱ Locale& locale, 
                            ধ্রুৱ UnicodeString* matchID, 
                            UErrorCode& status) ধ্রুৱ 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    যদি (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(এতত্); // cast away semantic const
        Mutex mutex(&lock);

        যদি (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        যদি (dnCache == nullptr) {
            ধ্রুৱ Hashtable* m = getVisibleIDMap(status);
            যদি (U_FAILURE(status)) {
                ঘূৰাই result;
            }
            ncthis->dnCache = নূতন DNCache(locale); 
            যদি (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ঘূৰাই result;
            }

            int32_t pos = UHASH_FIRST;
            ধ্রুৱ UHashElement* entry = nullptr;
            যেতিয়ালৈ ((entry = m->nextElement(pos)) != nullptr) {
                ধ্রুৱ UnicodeString* id = static_cast<ধ্রুৱ UnicodeString*>(entry->চাবি.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                যদি (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    dnCache->cache.put(dname, (শূন্য*)id, status); // share pointer with visibleIDMap
                    যদি (U_SUCCESS(status)) {
                        অনুৱর্ত;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ঘূৰাই result;
            }
        }
    }

    ICUService_চাবি* match_চাবি = create_চাবি(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ধ্রুৱ UHashElement *entry = nullptr;
    যেতিয়ালৈ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ধ্রুৱ UnicodeString* id = static_cast<ধ্রুৱ UnicodeString*>(entry->value.pointer);
        যদি (match_চাবি != nullptr && !match_চাবি->isFallbackOf(*id)) {
            অনুৱর্ত;
        }
        ধ্রুৱ UnicodeString* dn = static_cast<ধ্রুৱ UnicodeString*>(entry->চাবি.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        যদি (U_FAILURE(status)) {
            result.removeAllElements();
            ভঙ্গ;
        }
    }
    delete match_চাবি;

    ঘূৰাই result;
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& id, UErrorCode& status) 
{
    ঘূৰাই registerInstance(objToAdopt, id, সঁচা, status);
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_চাবি* চাবি = create_চাবি(&id, status);
    যদি (চাবি != nullptr) {
        UnicodeString canonicalID;
        চাবি->canonicalID(canonicalID);
        delete চাবি;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        যদি (f != nullptr) {
            ঘূৰাই registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ঘূৰাই nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ধ্রুৱ UnicodeString& id, UBool visible, UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        যদি ((objToAdopt != nullptr) && (!id.isBogus())) {
            ঘূৰাই নূতন SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ঘূৰাই nullptr;
}

URegistry_চাবি
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    যদি (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ঘূৰাই nullptr;
    }
    {
        Mutex mutex(&lock);

        যদি (factories == nullptr) {
            LocalPointer<UVector> lpFactories(নূতন UVector(uprv_deleteUObject, nullptr, status), status);
            যদি (U_FAILURE(status)) {
                ঘূৰাই nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->ঢোকাও_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        যদি (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    যদি (U_SUCCESS(status)) {
        notifyChanged();
        ঘূৰাই (URegistry_চাবি)factoryToAdopt;
    } অন্যথা {
        ঘূৰাই nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_চাবি rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = মিছা;
    যদি (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        যদি (factories->removeElement(factory)) {
            clearCaches();
            result = সঁচা;
        } অন্যথা {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    যদি (result) {
        notifyChanged();
    }
    ঘূৰাই result;
}

শূন্য 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

শূন্য 
ICUService::reInitializeFactories() 
{
    যদি (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ধ্রুৱ 
{
    ঘূৰাই গুনতি_Factories() == 0;
}

ICUService_চাবি* 
ICUService::create_চাবি(ধ্রুৱ UnicodeString* id, UErrorCode& status) ধ্রুৱ 
{
    ঘূৰাই (U_FAILURE(status) || id == nullptr) ? nullptr : নূতন ICUService_চাবি(*id);
}

শূন্য 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    delete dnCache;
    dnCache = nullptr;
    delete idCache;
    idCache = nullptr;
    delete serviceCache; serviceCache = nullptr;
}

শূন্য 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ধ্রুৱ EventListener& l) ধ্রুৱ 
{
    ঘূৰাই dynamic_cast<ধ্রুৱ ServiceListener*>(&l) != nullptr;
}

শূন্য 
ICUService::notifyListener(EventListener& l) ধ্রুৱ 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*এতত্);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ধ্রুৱ 
{
    ঘূৰাই result.append(name);
}

int32_t 
ICUService::গুনতি_Factories() ধ্রুৱ 
{
    ঘূৰাই factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ধ্রুৱ
{
    ঘূৰাই timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
