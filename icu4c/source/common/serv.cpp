// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"

#सचे !UCONFIG_NO_SERVICE

#अन्तर्भाव "serv.h"
#अन्तर्भाव "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ध्रुव char16_t ICUService_कुञ्चिका::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_कुञ्चिका::ICUService_कुञ्चिका(ध्रुव UnicodeString& id) 
: _id(id) {
}

ICUService_कुञ्चिका::~ICUService_कुञ्चिका() 
{
}

ध्रुव UnicodeString& 
ICUService_कुञ्चिका::getID() ध्रुव 
{
    पटिनिवत्त _id;
}

UnicodeString& 
ICUService_कुञ्चिका::canonicalID(UnicodeString& result) ध्रुव 
{
    पटिनिवत्त result.append(_id);
}

UnicodeString& 
ICUService_कुञ्चिका::currentID(UnicodeString& result) ध्रुव 
{
    पटिनिवत्त canonicalID(result);
}

UnicodeString& 
ICUService_कुञ्चिका::currentDescriptor(UnicodeString& result) ध्रुव 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    पटिनिवत्त currentID(result);
}

UBool 
ICUService_कुञ्चिका::fallback() 
{
    पटिनिवत्त मिच्छा;
}

UBool 
ICUService_कुञ्चिका::isFallbackOf(ध्रुव UnicodeString& id) ध्रुव 
{
    पटिनिवत्त id == _id;
}

UnicodeString& 
ICUService_कुञ्चिका::prefix(UnicodeString& result) ध्रुव 
{
    पटिनिवत्त result;
}

UnicodeString& 
ICUService_कुञ्चिका::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    सचे (n < 0) {
        n = 0;
    }
    result.remove(n);
    पटिनिवत्त result;
}

UnicodeString& 
ICUService_कुञ्चिका::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    सचे (n >= 0) {
        result.remove(0, n+1);
    }
    पटिनिवत्त result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_कुञ्चिका::debug(UnicodeString& result) ध्रुव 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    पटिनिवत्त result;
}

UnicodeString& 
ICUService_कुञ्चिका::debugClass(UnicodeString& result) ध्रुव 
{
    पटिनिवत्त result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_कुञ्चिका)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ध्रुव UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UErrorCode& status) ध्रुव 
{
    सचे (U_SUCCESS(status)) {
        UnicodeString temp;
        सचे (_id == कुञ्चिका.currentID(temp)) {
            पटिनिवत्त service->cloneInstance(_instance); 
        }
    }
    पटिनिवत्त nullptr;
}

शून्य 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव 
{
    सचे (_visible) {
        result.put(_id, (शून्य*)एतत्, status); // cast away const
    } अञ्ञथा {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& /* locale */, UnicodeString& result) ध्रुव 
{
    सचे (_visible && _id == id) {
        result = _id;
    } अञ्ञथा {
        result.setToBogus();
    }
    पटिनिवत्त result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ध्रुव 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    पटिनिवत्त toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ध्रुव 
{
    पटिनिवत्त toAppendTo.append((UnicodeString)"SimpleFactory");
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
वग्ग CacheEntry : सार्वजनिक UMemory {
गुप्त:
    int32_t refcount;

सार्वजनिक:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ध्रुव UnicodeString& _actualDescriptor, UObject* _service) 
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
        पटिनिवत्त एतत्;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        सचे ((--refcount) == 0) {
            delete एतत्;
            पटिनिवत्त nullptr;
        }
        पटिनिवत्त एतत्;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ध्रुव {
        पटिनिवत्त refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
cacheDeleter(शून्य* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

वग्ग DNCache : सार्वजनिक UMemory {
सार्वजनिक:
    Hashtable cache;
    ध्रुव Locale locale;

    DNCache(ध्रुव Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ध्रुव UnicodeString& displayName, 
                   ध्रुव UnicodeString& id,
                   UErrorCode& status)
{
    सचे (U_SUCCESS(status)) {
        StringPair* sp = नूतन StringPair(displayName, id);
        सचे (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            पटिनिवत्त nullptr;
        }
        पटिनिवत्त sp;
    }
    पटिनिवत्त nullptr;
}

UBool 
StringPair::isBogus() ध्रुव {
    पटिनिवत्त displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ध्रुव UnicodeString& _displayName, 
                       ध्रुव UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
userv_deleteStringPair(शून्य *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

स्थिर UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ध्रुव UnicodeString& newName) 
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
ICUService::get(ध्रुव UnicodeString& descriptor, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ध्रुव UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ध्रुव 
{
    UObject* result = nullptr;
    ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&descriptor, status);
    सचे (कुञ्चिका) {
        result = get_कुञ्चिका(*कुञ्चिका, actualReturn, status);
        delete कुञ्चिका;
    }
    पटिनिवत्त result;
}

UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त get_कुञ्चिका(कुञ्चिका, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त get_कुञ्चिका(कुञ्चिका, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
वग्ग XMutex : सार्वजनिक UMemory {
सार्वजनिक:
    अन्तःस्थ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        सचे (fActive) umtx_lock(fMutex);
    }
    अन्तःस्थ ~XMutex() {
        सचे (fActive) umtx_unlock(fMutex);
    }

गुप्त:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_कुञ्चिका(ICUService_कुञ्चिका& कुञ्चिका, UnicodeString* actualReturn, ध्रुव ICUServiceFactory* factory, UErrorCode& status) ध्रुव 
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    सचे (isDefault()) {
        पटिनिवत्त handleDefault(कुञ्चिका, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const

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

        सचे (serviceCache == nullptr) {
            ncthis->serviceCache = नूतन Hashtable(status);
            सचे (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त nullptr;
            }
            सचे (U_FAILURE(status)) {
                delete serviceCache;
                पटिनिवत्त nullptr;
            }
            serviceCache->set_मानम्_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = मिच्छा;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = सच्च;

        सचे (factory != nullptr) {
            अत्थाय (int32_t i = 0; i < limit; ++i) {
                सचे (factory == static_cast<ध्रुव ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    भङ्ग;
                }
            }
            सचे (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त nullptr;
            }
            cacheResult = मिच्छा;
        }

        करोहि {
            currentDescriptor.remove();
            कुञ्चिका.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            सचे (result != nullptr) {
                भङ्ग;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = सच्च;

            int32_t index = startIndex;
            याव (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(कुञ्चिका, एतत्, status));
                सचे (U_FAILURE(status)) {
                    पटिनिवत्त nullptr;
                }
                सचे (service.isValid()) {
                    result = नूतन CacheEntry(currentDescriptor, service.getAlias());
                    सचे (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        पटिनिवत्त nullptr;
                    }
                    service.orphan(); // result now owns service.

                    गच्छ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            सचे (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(नूतन UVector(uprv_deleteUObject, nullptr, 5, status), status);
                सचे (U_FAILURE(status)) {
                    पटिनिवत्त nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(नूतन UnicodeString(currentDescriptor), status);
            सचे (U_FAILURE(status)) {
                पटिनिवत्त nullptr;
            }
            सचे (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            सचे (U_FAILURE(status)) {
                पटिनिवत्त nullptr;
            }
        } याव (कुञ्चिका.fallback());
outerEnd:

        सचे (result != nullptr) {
            सचे (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                सचे (U_FAILURE(status)) {
                    पटिनिवत्त nullptr;
                }

                सचे (cacheDescriptorList.isValid()) {
                    अत्थाय (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        सचे (U_FAILURE(status)) {
                            पटिनिवत्त nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            सचे (actualReturn != nullptr) {
                // strip null prefix
                सचे (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } अञ्ञथा {
                    *actualReturn = result->actualDescriptor;
                }

                सचे (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    पटिनिवत्त nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            सचे (putInCache && !cacheResult) {
                delete result;
            }
            पटिनिवत्त service;
        }
    }

    पटिनिवत्त handleDefault(कुञ्चिका, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ध्रुव ICUService_कुञ्चिका& /*कुञ्चिका*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ध्रुव 
{
    पटिनिवत्त nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ध्रुव {
    पटिनिवत्त getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ध्रुव UnicodeString* matchID, UErrorCode& status) ध्रुव 
{
    result.removeAllElements();

    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ध्रुव Hashtable* map = getVisibleIDMap(status);
        सचे (map != nullptr) {
            ICUService_कुञ्चिका* fallback_कुञ्चिका = create_कुञ्चिका(matchID, status);

            अत्थाय (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ध्रुव UHashElement* e = map->nextElement(pos);
                सचे (e == nullptr) {
                    भङ्ग;
                }

                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(e->कुञ्चिका.pointer);
                सचे (fallback_कुञ्चिका != nullptr) {
                    सचे (!fallback_कुञ्चिका->isFallbackOf(*id)) {
                        अनुवर्त;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_कुञ्चिका;
        }
    }
    सचे (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    पटिनिवत्त result;
}

ध्रुव Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ध्रुव {
    सचे (U_FAILURE(status)) पटिनिवत्त nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
    सचे (idCache == nullptr) {
        ncthis->idCache = नूतन Hashtable(status);
        सचे (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा सचे (factories != nullptr) {
            अत्थाय (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            सचे (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    पटिनिवत्त idCache;
}


UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result) ध्रुव 
{
    पटिनिवत्त getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result, ध्रुव Locale& locale) ध्रुव 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ध्रुव Hashtable* map = getVisibleIDMap(status);
        सचे (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            सचे (f != nullptr) {
                f->getDisplayName(id, locale, result);
                पटिनिवत्त result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_कुञ्चिका* fallback_कुञ्चिका = create_कुञ्चिका(&id, status);
            याव (fallback_कुञ्चिका != nullptr && fallback_कुञ्चिका->fallback()) {
                UnicodeString us;
                fallback_कुञ्चिका->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                सचे (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_कुञ्चिका;
                    पटिनिवत्त result;
                }
            }
            delete fallback_कुञ्चिका;
        }
    }
    result.setToBogus();
    पटिनिवत्त result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ध्रुव Locale& locale, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ध्रुव Locale& locale, 
                            ध्रुव UnicodeString* matchID, 
                            UErrorCode& status) ध्रुव 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    सचे (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
        Mutex mutex(&lock);

        सचे (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        सचे (dnCache == nullptr) {
            ध्रुव Hashtable* m = getVisibleIDMap(status);
            सचे (U_FAILURE(status)) {
                पटिनिवत्त result;
            }
            ncthis->dnCache = नूतन DNCache(locale); 
            सचे (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त result;
            }

            int32_t pos = UHASH_FIRST;
            ध्रुव UHashElement* entry = nullptr;
            याव ((entry = m->nextElement(pos)) != nullptr) {
                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(entry->कुञ्चिका.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                सचे (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अञ्ञथा {
                    dnCache->cache.put(dname, (शून्य*)id, status); // share pointer with visibleIDMap
                    सचे (U_SUCCESS(status)) {
                        अनुवर्त;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                पटिनिवत्त result;
            }
        }
    }

    ICUService_कुञ्चिका* match_कुञ्चिका = create_कुञ्चिका(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ध्रुव UHashElement *entry = nullptr;
    याव ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(entry->value.pointer);
        सचे (match_कुञ्चिका != nullptr && !match_कुञ्चिका->isFallbackOf(*id)) {
            अनुवर्त;
        }
        ध्रुव UnicodeString* dn = static_cast<ध्रुव UnicodeString*>(entry->कुञ्चिका.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        सचे (U_FAILURE(status)) {
            result.removeAllElements();
            भङ्ग;
        }
    }
    delete match_कुञ्चिका;

    पटिनिवत्त result;
}

URegistry_कुञ्चिका
ICUService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UErrorCode& status) 
{
    पटिनिवत्त registerInstance(objToAdopt, id, सच्च, status);
}

URegistry_कुञ्चिका
ICUService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&id, status);
    सचे (कुञ्चिका != nullptr) {
        UnicodeString canonicalID;
        कुञ्चिका->canonicalID(canonicalID);
        delete कुञ्चिका;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        सचे (f != nullptr) {
            पटिनिवत्त registerFactory(f, status);
        }
    }
    delete objToAdopt;
    पटिनिवत्त nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status)
{
    सचे (U_SUCCESS(status)) {
        सचे ((objToAdopt != nullptr) && (!id.isBogus())) {
            पटिनिवत्त नूतन SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    पटिनिवत्त nullptr;
}

URegistry_कुञ्चिका
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    सचे (U_FAILURE(status) || factoryToAdopt == nullptr) {
        पटिनिवत्त nullptr;
    }
    {
        Mutex mutex(&lock);

        सचे (factories == nullptr) {
            LocalPointer<UVector> lpFactories(नूतन UVector(uprv_deleteUObject, nullptr, status), status);
            सचे (U_FAILURE(status)) {
                पटिनिवत्त nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->निवेशय_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        सचे (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    सचे (U_SUCCESS(status)) {
        notifyChanged();
        पटिनिवत्त (URegistry_कुञ्चिका)factoryToAdopt;
    } अञ्ञथा {
        पटिनिवत्त nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_कुञ्चिका rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = मिच्छा;
    सचे (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        सचे (factories->removeElement(factory)) {
            clearCaches();
            result = सच्च;
        } अञ्ञथा {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    सचे (result) {
        notifyChanged();
    }
    पटिनिवत्त result;
}

शून्य 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

शून्य 
ICUService::reInitializeFactories() 
{
    सचे (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ध्रुव 
{
    पटिनिवत्त गणनम्_Factories() == 0;
}

ICUService_कुञ्चिका* 
ICUService::create_कुञ्चिका(ध्रुव UnicodeString* id, UErrorCode& status) ध्रुव 
{
    पटिनिवत्त (U_FAILURE(status) || id == nullptr) ? nullptr : नूतन ICUService_कुञ्चिका(*id);
}

शून्य 
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

शून्य 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ध्रुव EventListener& l) ध्रुव 
{
    पटिनिवत्त dynamic_cast<ध्रुव ServiceListener*>(&l) != nullptr;
}

शून्य 
ICUService::notifyListener(EventListener& l) ध्रुव 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*एतत्);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ध्रुव 
{
    पटिनिवत्त result.append(name);
}

int32_t 
ICUService::गणनम्_Factories() ध्रुव 
{
    पटिनिवत्त factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ध्रुव
{
    पटिनिवत्त timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
