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

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "serv.h"
#अन्तर्भाव "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ध्रुव char16_t ICUService_किल्ली::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_किल्ली::ICUService_किल्ली(ध्रुव UnicodeString& id) 
: _id(id) {
}

ICUService_किल्ली::~ICUService_किल्ली() 
{
}

ध्रुव UnicodeString& 
ICUService_किल्ली::getID() ध्रुव 
{
    प्रत्यागम _id;
}

UnicodeString& 
ICUService_किल्ली::canonicalID(UnicodeString& result) ध्रुव 
{
    प्रत्यागम result.append(_id);
}

UnicodeString& 
ICUService_किल्ली::currentID(UnicodeString& result) ध्रुव 
{
    प्रत्यागम canonicalID(result);
}

UnicodeString& 
ICUService_किल्ली::currentDescriptor(UnicodeString& result) ध्रुव 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    प्रत्यागम currentID(result);
}

UBool 
ICUService_किल्ली::fallback() 
{
    प्रत्यागम असत्य;
}

UBool 
ICUService_किल्ली::isFallbackOf(ध्रुव UnicodeString& id) ध्रुव 
{
    प्रत्यागम id == _id;
}

UnicodeString& 
ICUService_किल्ली::prefix(UnicodeString& result) ध्रुव 
{
    प्रत्यागम result;
}

UnicodeString& 
ICUService_किल्ली::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    यदि (n < 0) {
        n = 0;
    }
    result.remove(n);
    प्रत्यागम result;
}

UnicodeString& 
ICUService_किल्ली::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    यदि (n >= 0) {
        result.remove(0, n+1);
    }
    प्रत्यागम result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_किल्ली::debug(UnicodeString& result) ध्रुव 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    प्रत्यागम result;
}

UnicodeString& 
ICUService_किल्ली::debugClass(UnicodeString& result) ध्रुव 
{
    प्रत्यागम result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_किल्ली)

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
SimpleFactory::create(ध्रुव ICUService_किल्ली& किल्ली, ध्रुव ICUService* service, UErrorCode& status) ध्रुव 
{
    यदि (U_SUCCESS(status)) {
        UnicodeString temp;
        यदि (_id == किल्ली.currentID(temp)) {
            प्रत्यागम service->cloneInstance(_instance); 
        }
    }
    प्रत्यागम nullptr;
}

शून्य 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव 
{
    यदि (_visible) {
        result.put(_id, (शून्य*)एतत्, status); // cast away const
    } अन्यथा {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ध्रुव UnicodeString& id, ध्रुव Locale& /* locale */, UnicodeString& result) ध्रुव 
{
    यदि (_visible && _id == id) {
        result = _id;
    } अन्यथा {
        result.setToBogus();
    }
    प्रत्यागम result;
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
    प्रत्यागम toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ध्रुव 
{
    प्रत्यागम toAppendTo.append((UnicodeString)"SimpleFactory");
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
वर्ग CacheEntry : सार्वजनिक UMemory {
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
        प्रत्यागम एतत्;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        यदि ((--refcount) == 0) {
            delete एतत्;
            प्रत्यागम nullptr;
        }
        प्रत्यागम एतत्;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ध्रुव {
        प्रत्यागम refcount > 1;
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

वर्ग DNCache : सार्वजनिक UMemory {
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
    यदि (U_SUCCESS(status)) {
        StringPair* sp = नूतन StringPair(displayName, id);
        यदि (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            प्रत्यागम nullptr;
        }
        प्रत्यागम sp;
    }
    प्रत्यागम nullptr;
}

UBool 
StringPair::isBogus() ध्रुव {
    प्रत्यागम displayName.isBogus() || id.isBogus();
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
    प्रत्यागम get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ध्रुव UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ध्रुव 
{
    UObject* result = nullptr;
    ICUService_किल्ली* किल्ली = create_किल्ली(&descriptor, status);
    यदि (किल्ली) {
        result = get_किल्ली(*किल्ली, actualReturn, status);
        delete किल्ली;
    }
    प्रत्यागम result;
}

UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UErrorCode& status) ध्रुव 
{
    प्रत्यागम get_किल्ली(किल्ली, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UnicodeString* actualReturn, UErrorCode& status) ध्रुव 
{
    प्रत्यागम get_किल्ली(किल्ली, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
वर्ग XMutex : सार्वजनिक UMemory {
सार्वजनिक:
    अन्तःस्थ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        यदि (fActive) umtx_lock(fMutex);
    }
    अन्तःस्थ ~XMutex() {
        यदि (fActive) umtx_unlock(fMutex);
    }

गुप्त:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_किल्ली(ICUService_किल्ली& किल्ली, UnicodeString* actualReturn, ध्रुव ICUServiceFactory* factory, UErrorCode& status) ध्रुव 
{
    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }

    यदि (isDefault()) {
        प्रत्यागम handleDefault(किल्ली, actualReturn, status);
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

        यदि (serviceCache == nullptr) {
            ncthis->serviceCache = नूतन Hashtable(status);
            यदि (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम nullptr;
            }
            यदि (U_FAILURE(status)) {
                delete serviceCache;
                प्रत्यागम nullptr;
            }
            serviceCache->set_मूल्य_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = असत्य;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = सत्य;

        यदि (factory != nullptr) {
            प्रति (int32_t i = 0; i < limit; ++i) {
                यदि (factory == static_cast<ध्रुव ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    भङ्ग;
                }
            }
            यदि (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                प्रत्यागम nullptr;
            }
            cacheResult = असत्य;
        }

        कुरु {
            currentDescriptor.remove();
            किल्ली.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            यदि (result != nullptr) {
                भङ्ग;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = सत्य;

            int32_t index = startIndex;
            यावत् (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(किल्ली, एतत्, status));
                यदि (U_FAILURE(status)) {
                    प्रत्यागम nullptr;
                }
                यदि (service.isValid()) {
                    result = नूतन CacheEntry(currentDescriptor, service.getAlias());
                    यदि (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        प्रत्यागम nullptr;
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
            यदि (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(नूतन UVector(uprv_deleteUObject, nullptr, 5, status), status);
                यदि (U_FAILURE(status)) {
                    प्रत्यागम nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(नूतन UnicodeString(currentDescriptor), status);
            यदि (U_FAILURE(status)) {
                प्रत्यागम nullptr;
            }
            यदि (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            यदि (U_FAILURE(status)) {
                प्रत्यागम nullptr;
            }
        } यावत् (किल्ली.fallback());
outerEnd:

        यदि (result != nullptr) {
            यदि (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                यदि (U_FAILURE(status)) {
                    प्रत्यागम nullptr;
                }

                यदि (cacheDescriptorList.isValid()) {
                    प्रति (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        यदि (U_FAILURE(status)) {
                            प्रत्यागम nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            यदि (actualReturn != nullptr) {
                // strip null prefix
                यदि (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } अन्यथा {
                    *actualReturn = result->actualDescriptor;
                }

                यदि (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    प्रत्यागम nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            यदि (putInCache && !cacheResult) {
                delete result;
            }
            प्रत्यागम service;
        }
    }

    प्रत्यागम handleDefault(किल्ली, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ध्रुव ICUService_किल्ली& /*किल्ली*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ध्रुव 
{
    प्रत्यागम nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ध्रुव {
    प्रत्यागम getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ध्रुव UnicodeString* matchID, UErrorCode& status) ध्रुव 
{
    result.removeAllElements();

    यदि (U_FAILURE(status)) {
        प्रत्यागम result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ध्रुव Hashtable* map = getVisibleIDMap(status);
        यदि (map != nullptr) {
            ICUService_किल्ली* fallback_किल्ली = create_किल्ली(matchID, status);

            प्रति (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ध्रुव UHashElement* e = map->nextElement(pos);
                यदि (e == nullptr) {
                    भङ्ग;
                }

                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(e->किल्ली.pointer);
                यदि (fallback_किल्ली != nullptr) {
                    यदि (!fallback_किल्ली->isFallbackOf(*id)) {
                        अनुवर्त;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_किल्ली;
        }
    }
    यदि (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    प्रत्यागम result;
}

ध्रुव Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ध्रुव {
    यदि (U_FAILURE(status)) प्रत्यागम nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
    यदि (idCache == nullptr) {
        ncthis->idCache = नूतन Hashtable(status);
        यदि (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा यदि (factories != nullptr) {
            प्रति (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            यदि (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    प्रत्यागम idCache;
}


UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result) ध्रुव 
{
    प्रत्यागम getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ध्रुव UnicodeString& id, UnicodeString& result, ध्रुव Locale& locale) ध्रुव 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ध्रुव Hashtable* map = getVisibleIDMap(status);
        यदि (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            यदि (f != nullptr) {
                f->getDisplayName(id, locale, result);
                प्रत्यागम result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_किल्ली* fallback_किल्ली = create_किल्ली(&id, status);
            यावत् (fallback_किल्ली != nullptr && fallback_किल्ली->fallback()) {
                UnicodeString us;
                fallback_किल्ली->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                यदि (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_किल्ली;
                    प्रत्यागम result;
                }
            }
            delete fallback_किल्ली;
        }
    }
    result.setToBogus();
    प्रत्यागम result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ध्रुव 
{
    प्रत्यागम getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ध्रुव Locale& locale, UErrorCode& status) ध्रुव 
{
    प्रत्यागम getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ध्रुव Locale& locale, 
                            ध्रुव UnicodeString* matchID, 
                            UErrorCode& status) ध्रुव 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    यदि (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(एतत्); // cast away semantic const
        Mutex mutex(&lock);

        यदि (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        यदि (dnCache == nullptr) {
            ध्रुव Hashtable* m = getVisibleIDMap(status);
            यदि (U_FAILURE(status)) {
                प्रत्यागम result;
            }
            ncthis->dnCache = नूतन DNCache(locale); 
            यदि (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम result;
            }

            int32_t pos = UHASH_FIRST;
            ध्रुव UHashElement* entry = nullptr;
            यावत् ((entry = m->nextElement(pos)) != nullptr) {
                ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(entry->किल्ली.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                यदि (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } अन्यथा {
                    dnCache->cache.put(dname, (शून्य*)id, status); // share pointer with visibleIDMap
                    यदि (U_SUCCESS(status)) {
                        अनुवर्त;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                प्रत्यागम result;
            }
        }
    }

    ICUService_किल्ली* match_किल्ली = create_किल्ली(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ध्रुव UHashElement *entry = nullptr;
    यावत् ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ध्रुव UnicodeString* id = static_cast<ध्रुव UnicodeString*>(entry->value.pointer);
        यदि (match_किल्ली != nullptr && !match_किल्ली->isFallbackOf(*id)) {
            अनुवर्त;
        }
        ध्रुव UnicodeString* dn = static_cast<ध्रुव UnicodeString*>(entry->किल्ली.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        यदि (U_FAILURE(status)) {
            result.removeAllElements();
            भङ्ग;
        }
    }
    delete match_किल्ली;

    प्रत्यागम result;
}

URegistry_किल्ली
ICUService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UErrorCode& status) 
{
    प्रत्यागम registerInstance(objToAdopt, id, सत्य, status);
}

URegistry_किल्ली
ICUService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_किल्ली* किल्ली = create_किल्ली(&id, status);
    यदि (किल्ली != nullptr) {
        UnicodeString canonicalID;
        किल्ली->canonicalID(canonicalID);
        delete किल्ली;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        यदि (f != nullptr) {
            प्रत्यागम registerFactory(f, status);
        }
    }
    delete objToAdopt;
    प्रत्यागम nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ध्रुव UnicodeString& id, UBool visible, UErrorCode& status)
{
    यदि (U_SUCCESS(status)) {
        यदि ((objToAdopt != nullptr) && (!id.isBogus())) {
            प्रत्यागम नूतन SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    प्रत्यागम nullptr;
}

URegistry_किल्ली
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    यदि (U_FAILURE(status) || factoryToAdopt == nullptr) {
        प्रत्यागम nullptr;
    }
    {
        Mutex mutex(&lock);

        यदि (factories == nullptr) {
            LocalPointer<UVector> lpFactories(नूतन UVector(uprv_deleteUObject, nullptr, status), status);
            यदि (U_FAILURE(status)) {
                प्रत्यागम nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->घाला_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        यदि (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    यदि (U_SUCCESS(status)) {
        notifyChanged();
        प्रत्यागम (URegistry_किल्ली)factoryToAdopt;
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_किल्ली rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = असत्य;
    यदि (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        यदि (factories->removeElement(factory)) {
            clearCaches();
            result = सत्य;
        } अन्यथा {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    यदि (result) {
        notifyChanged();
    }
    प्रत्यागम result;
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
    यदि (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ध्रुव 
{
    प्रत्यागम मोजणी_Factories() == 0;
}

ICUService_किल्ली* 
ICUService::create_किल्ली(ध्रुव UnicodeString* id, UErrorCode& status) ध्रुव 
{
    प्रत्यागम (U_FAILURE(status) || id == nullptr) ? nullptr : नूतन ICUService_किल्ली(*id);
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
    प्रत्यागम dynamic_cast<ध्रुव ServiceListener*>(&l) != nullptr;
}

शून्य 
ICUService::notifyListener(EventListener& l) ध्रुव 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*एतत्);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ध्रुव 
{
    प्रत्यागम result.append(name);
}

int32_t 
ICUService::मोजणी_Factories() ध्रुव 
{
    प्रत्यागम factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ध्रुव
{
    प्रत्यागम timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
