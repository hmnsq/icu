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

ধ্রুবক char16_t ICUService_চাবি::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_চাবি::ICUService_চাবি(ধ্রুবক UnicodeString& id) 
: _id(id) {
}

ICUService_চাবি::~ICUService_চাবি() 
{
}

ধ্রুবক UnicodeString& 
ICUService_চাবি::getID() ধ্রুবক 
{
    ফেরত _id;
}

UnicodeString& 
ICUService_চাবি::canonicalID(UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত ফলাফল.append(_id);
}

UnicodeString& 
ICUService_চাবি::currentID(UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত canonicalID(ফলাফল);
}

UnicodeString& 
ICUService_চাবি::currentDescriptor(UnicodeString& ফলাফল) ধ্রুবক 
{
    prefix(ফলাফল);
    ফলাফল.append(PREFIX_DELIMITER);
    ফেরত currentID(ফলাফল);
}

UBool 
ICUService_চাবি::fallback() 
{
    ফেরত মিথ্যা;
}

UBool 
ICUService_চাবি::isFallbackOf(ধ্রুবক UnicodeString& id) ধ্রুবক 
{
    ফেরত id == _id;
}

UnicodeString& 
ICUService_চাবি::prefix(UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত ফলাফল;
}

UnicodeString& 
ICUService_চাবি::parsePrefix(UnicodeString& ফলাফল) 
{
    int32_t n = ফলাফল.সূচক_Of(PREFIX_DELIMITER);
    যদি (n < 0) {
        n = 0;
    }
    ফলাফল.remove(n);
    ফেরত ফলাফল;
}

UnicodeString& 
ICUService_চাবি::parseSuffix(UnicodeString& ফলাফল) 
{
    int32_t n = ফলাফল.সূচক_Of(PREFIX_DELIMITER);
    যদি (n >= 0) {
        ফলাফল.remove(0, n+1);
    }
    ফেরত ফলাফল;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_চাবি::debug(UnicodeString& ফলাফল) ধ্রুবক 
{
    debugClass(ফলাফল);
    ফলাফল.append((UnicodeString)" id: ");
    ফলাফল.append(_id);
    ফেরত ফলাফল;
}

UnicodeString& 
ICUService_চাবি::debugClass(UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত ফলাফল.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_চাবি)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ধ্রুবক UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ধ্রুবক ICUService_চাবি& চাবি, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক 
{
    যদি (U_SUCCESS(status)) {
        UnicodeString অস্থায়ী;
        যদি (_id == চাবি.currentID(অস্থায়ী)) {
            ফেরত service->cloneInstance(_instance); 
        }
    }
    ফেরত nullptr;
}

শূন্য 
SimpleFactory::updateVisibleIDs(Hashtable& ফলাফল, UErrorCode& status) ধ্রুবক 
{
    যদি (_visible) {
        ফলাফল.put(_id, (শূন্য*)এই, status); // cast away const
    } অন্যথা {
        ফলাফল.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ধ্রুবক UnicodeString& id, ধ্রুবক Locale& /* locale */, UnicodeString& ফলাফল) ধ্রুবক 
{
    যদি (_visible && _id == id) {
        ফলাফল = _id;
    } অন্যথা {
        ফলাফল.setToBogus();
    }
    ফেরত ফলাফল;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ধ্রুবক 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ফেরত toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ধ্রুবক 
{
    ফেরত toAppendTo.append((UnicodeString)"SimpleFactory");
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
শ্রেণী CacheEntry : সর্বজনীন UMemory {
ব্যক্তিগত:
    int32_t refcount;

সর্বজনীন:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ধ্রুবক UnicodeString& _actualDescriptor, UObject* _service) 
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
        ফেরত এই;
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
            delete এই;
            ফেরত nullptr;
        }
        ফেরত এই;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ধ্রুবক {
        ফেরত refcount > 1;
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

শ্রেণী DNCache : সর্বজনীন UMemory {
সর্বজনীন:
    Hashtable cache;
    ধ্রুবক Locale locale;

    DNCache(ধ্রুবক Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ধ্রুবক UnicodeString& displayName, 
                   ধ্রুবক UnicodeString& id,
                   UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        StringPair* sp = নতুন StringPair(displayName, id);
        যদি (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ফেরত nullptr;
        }
        ফেরত sp;
    }
    ফেরত nullptr;
}

UBool 
StringPair::isBogus() ধ্রুবক {
    ফেরত displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ধ্রুবক UnicodeString& _displayName, 
                       ধ্রুবক UnicodeString& _id)
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

ICUService::ICUService(ধ্রুবক UnicodeString& newName) 
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
ICUService::get(ধ্রুবক UnicodeString& descriptor, UErrorCode& status) ধ্রুবক 
{
    ফেরত get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ধ্রুবক UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ধ্রুবক 
{
    UObject* ফলাফল = nullptr;
    ICUService_চাবি* চাবি = create_চাবি(&descriptor, status);
    যদি (চাবি) {
        ফলাফল = get_চাবি(*চাবি, actualReturn, status);
        delete চাবি;
    }
    ফেরত ফলাফল;
}

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুবক 
{
    ফেরত get_চাবি(চাবি, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, UErrorCode& status) ধ্রুবক 
{
    ফেরত get_চাবি(চাবি, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
শ্রেণী XMutex : সর্বজনীন UMemory {
সর্বজনীন:
    অন্তঃরেখ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        যদি (fActive) umtx_lock(fMutex);
    }
    অন্তঃরেখ ~XMutex() {
        যদি (fActive) umtx_unlock(fMutex);
    }

ব্যক্তিগত:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_চাবি(ICUService_চাবি& চাবি, UnicodeString* actualReturn, ধ্রুবক ICUServiceFactory* factory, UErrorCode& status) ধ্রুবক 
{
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    যদি (isDefault()) {
        ফেরত handleDefault(চাবি, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(এই); // cast away semantic const

    CacheEntry* ফলাফল = nullptr;
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
            ncthis->serviceCache = নতুন Hashtable(status);
            যদি (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত nullptr;
            }
            যদি (U_FAILURE(status)) {
                delete serviceCache;
                ফেরত nullptr;
            }
            serviceCache->set_মান_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = মিথ্যা;

        int32_t start_সূচক = 0;
        int32_t limit = factories->size();
        UBool cache_ফলাফল = সত্য;

        যদি (factory != nullptr) {
            জন্য (int32_t i = 0; i < limit; ++i) {
                যদি (factory == static_cast<ধ্রুবক ICUServiceFactory*>(factories->elementAt(i))) {
                    start_সূচক = i + 1;
                    বিরতি;
                }
            }
            যদি (start_সূচক == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত nullptr;
            }
            cache_ফলাফল = মিথ্যা;
        }

        করো {
            currentDescriptor.remove();
            চাবি.currentDescriptor(currentDescriptor);
            ফলাফল = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            যদি (ফলাফল != nullptr) {
                বিরতি;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = সত্য;

            int32_t সূচক = start_সূচক;
            যখন (সূচক < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(সূচক++));
                LocalPointer<UObject> service(f->create(চাবি, এই, status));
                যদি (U_FAILURE(status)) {
                    ফেরত nullptr;
                }
                যদি (service.isValid()) {
                    ফলাফল = নতুন CacheEntry(currentDescriptor, service.getAlias());
                    যদি (ফলাফল == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ফেরত nullptr;
                    }
                    service.orphan(); // result now owns service.

                    যাও outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            যদি (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(নতুন UVector(uprv_deleteUObject, nullptr, 5, status), status);
                যদি (U_FAILURE(status)) {
                    ফেরত nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(নতুন UnicodeString(currentDescriptor), status);
            যদি (U_FAILURE(status)) {
                ফেরত nullptr;
            }
            যদি (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            যদি (U_FAILURE(status)) {
                ফেরত nullptr;
            }
        } যখন (চাবি.fallback());
outerEnd:

        যদি (ফলাফল != nullptr) {
            যদি (putInCache && cache_ফলাফল) {
                serviceCache->put(ফলাফল->actualDescriptor, ফলাফল, status);
                যদি (U_FAILURE(status)) {
                    ফেরত nullptr;
                }

                যদি (cacheDescriptorList.isValid()) {
                    জন্য (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, ফলাফল, status);
                        যদি (U_FAILURE(status)) {
                            ফেরত nullptr;
                        }

                        ফলাফল->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            যদি (actualReturn != nullptr) {
                // strip null prefix
                যদি (ফলাফল->actualDescriptor.সূচক_Of(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(ফলাফল->actualDescriptor, 
                        1, 
                        ফলাফল->actualDescriptor.length() - 1);
                } অন্যথা {
                    *actualReturn = ফলাফল->actualDescriptor;
                }

                যদি (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete ফলাফল;
                    ফেরত nullptr;
                }
            }

            UObject* service = cloneInstance(ফলাফল->service);
            যদি (putInCache && !cache_ফলাফল) {
                delete ফলাফল;
            }
            ফেরত service;
        }
    }

    ফেরত handleDefault(চাবি, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ধ্রুবক ICUService_চাবি& /*চাবি*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ধ্রুবক 
{
    ফেরত nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& ফলাফল, UErrorCode& status) ধ্রুবক {
    ফেরত getVisibleIDs(ফলাফল, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& ফলাফল, ধ্রুবক UnicodeString* matchID, UErrorCode& status) ধ্রুবক 
{
    ফলাফল.removeAllElements();

    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
    }
    UObjectDeleter *savedDeleter = ফলাফল.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ধ্রুবক Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUService_চাবি* fallback_চাবি = create_চাবি(matchID, status);

            জন্য (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ধ্রুবক UHashElement* e = map->পরবর্তী_Element(pos);
                যদি (e == nullptr) {
                    বিরতি;
                }

                ধ্রুবক UnicodeString* id = static_cast<ধ্রুবক UnicodeString*>(e->চাবি.pointer);
                যদি (fallback_চাবি != nullptr) {
                    যদি (!fallback_চাবি->isFallbackOf(*id)) {
                        চালু;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                ফলাফল.adoptElement(idClone.orphan(), status);
            }
            delete fallback_চাবি;
        }
    }
    যদি (U_FAILURE(status)) {
        ফলাফল.removeAllElements();
    }
    ফলাফল.setDeleter(savedDeleter);
    ফেরত ফলাফল;
}

ধ্রুবক Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(এই); // cast away semantic const
    যদি (idCache == nullptr) {
        ncthis->idCache = নতুন Hashtable(status);
        যদি (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা যদি (factories != nullptr) {
            জন্য (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            যদি (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ফেরত idCache;
}


UnicodeString& 
ICUService::getDisplayName(ধ্রুবক UnicodeString& id, UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত getDisplayName(id, ফলাফল, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ধ্রুবক UnicodeString& id, UnicodeString& ফলাফল, ধ্রুবক Locale& locale) ধ্রুবক 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ধ্রুবক Hashtable* map = getVisibleIDMap(status);
        যদি (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            যদি (f != nullptr) {
                f->getDisplayName(id, locale, ফলাফল);
                ফেরত ফলাফল;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_চাবি* fallback_চাবি = create_চাবি(&id, status);
            যখন (fallback_চাবি != nullptr && fallback_চাবি->fallback()) {
                UnicodeString us;
                fallback_চাবি->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                যদি (f != nullptr) {
                    f->getDisplayName(id, locale, ফলাফল);
                    delete fallback_চাবি;
                    ফেরত ফলাফল;
                }
            }
            delete fallback_চাবি;
        }
    }
    ফলাফল.setToBogus();
    ফেরত ফলাফল;
}

UVector& 
ICUService::getDisplayNames(UVector& ফলাফল, UErrorCode& status) ধ্রুবক 
{
    ফেরত getDisplayNames(ফলাফল, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& ফলাফল, ধ্রুবক Locale& locale, UErrorCode& status) ধ্রুবক 
{
    ফেরত getDisplayNames(ফলাফল, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& ফলাফল, 
                            ধ্রুবক Locale& locale, 
                            ধ্রুবক UnicodeString* matchID, 
                            UErrorCode& status) ধ্রুবক 
{
    ফলাফল.removeAllElements();
    ফলাফল.setDeleter(userv_deleteStringPair);
    যদি (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(এই); // cast away semantic const
        Mutex mutex(&lock);

        যদি (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        যদি (dnCache == nullptr) {
            ধ্রুবক Hashtable* m = getVisibleIDMap(status);
            যদি (U_FAILURE(status)) {
                ফেরত ফলাফল;
            }
            ncthis->dnCache = নতুন DNCache(locale); 
            যদি (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত ফলাফল;
            }

            int32_t pos = UHASH_FIRST;
            ধ্রুবক UHashElement* entry = nullptr;
            যখন ((entry = m->পরবর্তী_Element(pos)) != nullptr) {
                ধ্রুবক UnicodeString* id = static_cast<ধ্রুবক UnicodeString*>(entry->চাবি.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                যদি (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } অন্যথা {
                    dnCache->cache.put(dname, (শূন্য*)id, status); // share pointer with visibleIDMap
                    যদি (U_SUCCESS(status)) {
                        চালু;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ফেরত ফলাফল;
            }
        }
    }

    ICUService_চাবি* match_চাবি = create_চাবি(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ধ্রুবক UHashElement *entry = nullptr;
    যখন ((entry = dnCache->cache.পরবর্তী_Element(pos)) != nullptr) {
        ধ্রুবক UnicodeString* id = static_cast<ধ্রুবক UnicodeString*>(entry->value.pointer);
        যদি (match_চাবি != nullptr && !match_চাবি->isFallbackOf(*id)) {
            চালু;
        }
        ধ্রুবক UnicodeString* dn = static_cast<ধ্রুবক UnicodeString*>(entry->চাবি.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        ফলাফল.adoptElement(sp, status);
        যদি (U_FAILURE(status)) {
            ফলাফল.removeAllElements();
            বিরতি;
        }
    }
    delete match_চাবি;

    ফেরত ফলাফল;
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& id, UErrorCode& status) 
{
    ফেরত registerInstance(objToAdopt, id, সত্য, status);
}

URegistry_চাবি
ICUService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_চাবি* চাবি = create_চাবি(&id, status);
    যদি (চাবি != nullptr) {
        UnicodeString canonicalID;
        চাবি->canonicalID(canonicalID);
        delete চাবি;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        যদি (f != nullptr) {
            ফেরত registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ফেরত nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ধ্রুবক UnicodeString& id, UBool visible, UErrorCode& status)
{
    যদি (U_SUCCESS(status)) {
        যদি ((objToAdopt != nullptr) && (!id.isBogus())) {
            ফেরত নতুন SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ফেরত nullptr;
}

URegistry_চাবি
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    যদি (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ফেরত nullptr;
    }
    {
        Mutex mutex(&lock);

        যদি (factories == nullptr) {
            LocalPointer<UVector> lpFactories(নতুন UVector(uprv_deleteUObject, nullptr, status), status);
            যদি (U_FAILURE(status)) {
                ফেরত nullptr;
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
        ফেরত (URegistry_চাবি)factoryToAdopt;
    } অন্যথা {
        ফেরত nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_চাবি rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool ফলাফল = মিথ্যা;
    যদি (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        যদি (factories->removeElement(factory)) {
            clearCaches();
            ফলাফল = সত্য;
        } অন্যথা {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    যদি (ফলাফল) {
        notifyChanged();
    }
    ফেরত ফলাফল;
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
ICUService::isDefault() ধ্রুবক 
{
    ফেরত গুনতি_Factories() == 0;
}

ICUService_চাবি* 
ICUService::create_চাবি(ধ্রুবক UnicodeString* id, UErrorCode& status) ধ্রুবক 
{
    ফেরত (U_FAILURE(status) || id == nullptr) ? nullptr : নতুন ICUService_চাবি(*id);
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
ICUService::acceptsListener(ধ্রুবক EventListener& l) ধ্রুবক 
{
    ফেরত dynamic_cast<ধ্রুবক ServiceListener*>(&l) != nullptr;
}

শূন্য 
ICUService::notifyListener(EventListener& l) ধ্রুবক 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*এই);
}

UnicodeString&
ICUService::getName(UnicodeString& ফলাফল) ধ্রুবক 
{
    ফেরত ফলাফল.append(name);
}

int32_t 
ICUService::গুনতি_Factories() ধ্রুবক 
{
    ফেরত factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ধ্রুবক
{
    ফেরত timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
