// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/localpointer.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "serv.h"
#অন্তর্ভাব "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ধ্রুব char16_t ICUService_চাবি::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_চাবি::ICUService_চাবি(ধ্রুব UnicodeString& id) 
: _id(id) {
}

ICUService_চাবি::~ICUService_চাবি() 
{
}

ধ্রুব UnicodeString& 
ICUService_চাবি::getID() ধ্রুব 
{
    প্রত্যাগম _id;
}

UnicodeString& 
ICUService_চাবি::canonicalID(UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম result.append(_id);
}

UnicodeString& 
ICUService_চাবি::currentID(UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম canonicalID(result);
}

UnicodeString& 
ICUService_চাবি::currentDescriptor(UnicodeString& result) ধ্রুব 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    প্রত্যাগম currentID(result);
}

UBool 
ICUService_চাবি::fallback() 
{
    প্রত্যাগম অসত্য;
}

UBool 
ICUService_চাবি::isFallbackOf(ধ্রুব UnicodeString& id) ধ্রুব 
{
    প্রত্যাগম id == _id;
}

UnicodeString& 
ICUService_চাবি::prefix(UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম result;
}

UnicodeString& 
ICUService_চাবি::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    যদি (n < 0) {
        n = 0;
    }
    result.remove(n);
    প্রত্যাগম result;
}

UnicodeString& 
ICUService_চাবি::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    যদি (n >= 0) {
        result.remove(0, n+1);
    }
    প্রত্যাগম result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_চাবি::debug(UnicodeString& result) ধ্রুব 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    প্রত্যাগম result;
}

UnicodeString& 
ICUService_চাবি::debugClass(UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_চাবি)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ধ্রুব UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ধ্রুব ICUService_চাবি& চাবি, ধ্রুব ICUService* service, UErrorCode& status) ধ্রুব 
{
    যদি (U_SUCCESS(status)) {
        UnicodeString temp;
        যদি (_id == চাবি.currentID(temp)) {
            প্রত্যাগম service->cloneInstance(_instance); 
        }
    }
    প্রত্যাগম nullptr;
}

শূন্য 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ধ্রুব 
{
    যদি (_visible) {
        result.put(_id, (শূন্য*)এতত্, status); // cast away const
    } অন্যথা {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ধ্রুব UnicodeString& id, ধ্রুব Locale& /* locale */, UnicodeString& result) ধ্রুব 
{
    যদি (_visible && _id == id) {
        result = _id;
    } অন্যথা {
        result.setToBogus();
    }
    প্রত্যাগম result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ধ্রুব 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    প্রত্যাগম toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ধ্রুব 
{
    প্রত্যাগম toAppendTo.append((UnicodeString)"SimpleFactory");
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
বর্গ CacheEntry : সার্বজনিক UMemory {
গুপ্ত:
    int32_t refcount;

সার্বজনিক:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ধ্রুব UnicodeString& _actualDescriptor, UObject* _service) 
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
        প্রত্যাগম এতত্;
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
            প্রত্যাগম nullptr;
        }
        প্রত্যাগম এতত্;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ধ্রুব {
        প্রত্যাগম refcount > 1;
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

বর্গ DNCache : সার্বজনিক UMemory {
সার্বজনিক:
    Hashtable cache;
    ধ্রুব Locale locale;

    DNCache(ধ্রুব Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ধ্রুব UnicodeString& displayName, 
                   ধ্রুব UnicodeString& id,
                   UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        StringPair* sp = নূতন StringPair(displayName, id);
        যদি (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            প্রত্যাগম nullptr;
        }
        প্রত্যাগম sp;
    }
    প্রত্যাগম nullptr;
}

UBool 
StringPair::isBogus() ধ্রুব {
    প্রত্যাগম displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ধ্রুব UnicodeString& _displayName, 
                       ধ্রুব UnicodeString& _id)
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

ICUService::ICUService(ধ্রুব UnicodeString& newName) 
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
ICUService::get(ধ্রুব UnicodeString& descriptor, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ধ্রুব UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ধ্রুব 
{
    UObject* result = nullptr;
    ICUService_চাবি* চাবি = create_চাবি(&descriptor, status);
    যদি (চাবি) {
        result = get_চাবি(*চাবি, actualReturn, status);
        delete চাবি;
    }
    প্রত্যাগম result;
}

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম get_চাবি(চাবি, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম get_চাবি(চাবি, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
বর্গ XMutex : সার্বজনিক UMemory {
সার্বজনিক:
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
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, ধ্রুব ICUServiceFactory* factory, UErrorCode& status) ধ্রুব 
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }

    যদি (isDefault()) {
        প্রত্যাগম handleDefault(চাবি, actualReturn, status);
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
                প্রত্যাগম nullptr;
            }
            যদি (U_FAILURE(status)) {
                delete serviceCache;
                প্রত্যাগম nullptr;
            }
            serviceCache->set_মান_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = অসত্য;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = সত্য;

        যদি (factory != nullptr) {
            প্রতি (int32_t i = 0; i < limit; ++i) {
                যদি (factory == static_cast<ধ্রুব ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    ভঙ্গ;
                }
            }
            যদি (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                প্রত্যাগম nullptr;
            }
            cacheResult = অসত্য;
        }

        কুরু {
            currentDescriptor.remove();
            চাবি.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            যদি (result != nullptr) {
                ভঙ্গ;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = সত্য;

            int32_t index = startIndex;
            যাবত্ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(চাবি, এতত্, status));
                যদি (U_FAILURE(status)) {
                    প্রত্যাগম nullptr;
                }
                যদি (service.isValid()) {
                    result = নূতন CacheEntry(currentDescriptor, service.getAlias());
                    যদি (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        প্রত্যাগম nullptr;
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
                    প্রত্যাগম nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(নূতন UnicodeString(currentDescriptor), status);
            যদি (U_FAILURE(status)) {
                প্রত্যাগম nullptr;
            }
            যদি (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            যদি (U_FAILURE(status)) {
                প্রত্যাগম nullptr;
            }
        } যাবত্ (চাবি.fallback());
outerEnd:

        যদি (result != nullptr) {
            যদি (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                যদি (U_FAILURE(status)) {
                    প্রত্যাগম nullptr;
                }

                যদি (cacheDescriptorList.isValid()) {
                    প্রতি (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        যদি (U_FAILURE(status)) {
                            প্রত্যাগম nullptr;
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
                    প্রত্যাগম nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            যদি (putInCache && !cacheResult) {
                delete result;
            }
            প্রত্যাগম service;
        }
    }

    প্রত্যাগম handleDefault(চাবি, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ধ্রুব ICUService_চাবি& /*চাবি*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ধ্রুব 
{
    প্রত্যাগম nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ধ্রুব {
    প্রত্যাগম getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ধ্রুব UnicodeString* matchID, UErrorCode& status) ধ্রুব 
{
    result.removeAllElements();

    যদি (U_FAILURE(status)) {
        প্রত্যাগম result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ধ্রুব Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUService_চাবি* fallback_চাবি = create_চাবি(matchID, status);

            প্রতি (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ধ্রুব UHashElement* e = map->nextElement(pos);
                যদি (e == nullptr) {
                    ভঙ্গ;
                }

                ধ্রুব UnicodeString* id = static_cast<ধ্রুব UnicodeString*>(e->চাবি.pointer);
                যদি (fallback_চাবি != nullptr) {
                    যদি (!fallback_চাবি->isFallbackOf(*id)) {
                        অনুবর্ত;
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
    প্রত্যাগম result;
}

ধ্রুব Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ধ্রুব {
    যদি (U_FAILURE(status)) প্রত্যাগম nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(এতত্); // cast away semantic const
    যদি (idCache == nullptr) {
        ncthis->idCache = নূতন Hashtable(status);
        যদি (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা যদি (factories != nullptr) {
            প্রতি (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            যদি (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    প্রত্যাগম idCache;
}


UnicodeString& 
ICUService::getDisplayName(ধ্রুব UnicodeString& id, UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ধ্রুব UnicodeString& id, UnicodeString& result, ধ্রুব Locale& locale) ধ্রুব 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ধ্রুব Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            যদি (f != nullptr) {
                f->getDisplayName(id, locale, result);
                প্রত্যাগম result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_চাবি* fallback_চাবি = create_চাবি(&id, status);
            যাবত্ (fallback_চাবি != nullptr && fallback_চাবি->fallback()) {
                UnicodeString us;
                fallback_চাবি->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                যদি (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_চাবি;
                    প্রত্যাগম result;
                }
            }
            delete fallback_চাবি;
        }
    }
    result.setToBogus();
    প্রত্যাগম result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ধ্রুব Locale& locale, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ধ্রুব Locale& locale, 
                            ধ্রুব UnicodeString* matchID, 
                            UErrorCode& status) ধ্রুব 
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
            ধ্রুব Hashtable* m = getVisibleIDMap(status);
            যদি (U_FAILURE(status)) {
                প্রত্যাগম result;
            }
            ncthis->dnCache = নূতন DNCache(locale); 
            যদি (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম result;
            }

            int32_t pos = UHASH_FIRST;
            ধ্রুব UHashElement* entry = nullptr;
            যাবত্ ((entry = m->nextElement(pos)) != nullptr) {
                ধ্রুব UnicodeString* id = static_cast<ধ্রুব UnicodeString*>(entry->চাবি.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                যদি (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    dnCache->cache.put(dname, (শূন্য*)id, status); // share pointer with visibleIDMap
                    যদি (U_SUCCESS(status)) {
                        অনুবর্ত;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                প্রত্যাগম result;
            }
        }
    }

    ICUService_চাবি* match_চাবি = create_চাবি(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ধ্রুব UHashElement *entry = nullptr;
    যাবত্ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ধ্রুব UnicodeString* id = static_cast<ধ্রুব UnicodeString*>(entry->value.pointer);
        যদি (match_চাবি != nullptr && !match_চাবি->isFallbackOf(*id)) {
            অনুবর্ত;
        }
        ধ্রুব UnicodeString* dn = static_cast<ধ্রুব UnicodeString*>(entry->চাবি.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        যদি (U_FAILURE(status)) {
            result.removeAllElements();
            ভঙ্গ;
        }
    }
    delete match_চাবি;

    প্রত্যাগম result;
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& id, UErrorCode& status) 
{
    প্রত্যাগম registerInstance(objToAdopt, id, সত্য, status);
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_চাবি* চাবি = create_চাবি(&id, status);
    যদি (চাবি != nullptr) {
        UnicodeString canonicalID;
        চাবি->canonicalID(canonicalID);
        delete চাবি;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        যদি (f != nullptr) {
            প্রত্যাগম registerFactory(f, status);
        }
    }
    delete objToAdopt;
    প্রত্যাগম nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ধ্রুব UnicodeString& id, UBool visible, UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        যদি ((objToAdopt != nullptr) && (!id.isBogus())) {
            প্রত্যাগম নূতন SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    প্রত্যাগম nullptr;
}

URegistry_চাবি
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    যদি (U_FAILURE(status) || factoryToAdopt == nullptr) {
        প্রত্যাগম nullptr;
    }
    {
        Mutex mutex(&lock);

        যদি (factories == nullptr) {
            LocalPointer<UVector> lpFactories(নূতন UVector(uprv_deleteUObject, nullptr, status), status);
            যদি (U_FAILURE(status)) {
                প্রত্যাগম nullptr;
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
        প্রত্যাগম (URegistry_চাবি)factoryToAdopt;
    } অন্যথা {
        প্রত্যাগম nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_চাবি rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = অসত্য;
    যদি (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        যদি (factories->removeElement(factory)) {
            clearCaches();
            result = সত্য;
        } অন্যথা {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    যদি (result) {
        notifyChanged();
    }
    প্রত্যাগম result;
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
ICUService::isDefault() ধ্রুব 
{
    প্রত্যাগম গুনতি_Factories() == 0;
}

ICUService_চাবি* 
ICUService::create_চাবি(ধ্রুব UnicodeString* id, UErrorCode& status) ধ্রুব 
{
    প্রত্যাগম (U_FAILURE(status) || id == nullptr) ? nullptr : নূতন ICUService_চাবি(*id);
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
ICUService::acceptsListener(ধ্রুব EventListener& l) ধ্রুব 
{
    প্রত্যাগম dynamic_cast<ধ্রুব ServiceListener*>(&l) != nullptr;
}

শূন্য 
ICUService::notifyListener(EventListener& l) ধ্রুব 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*এতত্);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ধ্রুব 
{
    প্রত্যাগম result.append(name);
}

int32_t 
ICUService::গুনতি_Factories() ধ্রুব 
{
    প্রত্যাগম factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ধ্রুব
{
    প্রত্যাগম timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
