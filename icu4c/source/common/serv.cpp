// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "serv.h"
#ضمن "umutex.h"

#تعریف_رد SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_کلید::اگیتر_DELIMITER = 0x002F;   /* '/' */

ICUService_کلید::ICUService_کلید(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_کلید::~ICUService_کلید() 
{
}

ثابت UnicodeString& 
ICUService_کلید::getID() ثابت 
{
    أرجع _id;
}

UnicodeString& 
ICUService_کلید::canonicalID(UnicodeString& نتیجہ) ثابت 
{
    أرجع نتیجہ.append(_id);
}

UnicodeString& 
ICUService_کلید::currentID(UnicodeString& نتیجہ) ثابت 
{
    أرجع canonicalID(نتیجہ);
}

UnicodeString& 
ICUService_کلید::currentDescriptor(UnicodeString& نتیجہ) ثابت 
{
    اگیتر(نتیجہ);
    نتیجہ.append(اگیتر_DELIMITER);
    أرجع currentID(نتیجہ);
}

UBool 
ICUService_کلید::fallback() 
{
    أرجع خطأ;
}

UBool 
ICUService_کلید::is_Fallback_دا(ثابت UnicodeString& id) ثابت 
{
    أرجع id == _id;
}

UnicodeString& 
ICUService_کلید::اگیتر(UnicodeString& نتیجہ) ثابت 
{
    أرجع نتیجہ;
}

UnicodeString& 
ICUService_کلید::parse_اگیتر(UnicodeString& نتیجہ) 
{
    int32_t n = نتیجہ.اشاریہ_دا(اگیتر_DELIMITER);
    إذا (n < 0) {
        n = 0;
    }
    نتیجہ.remove(n);
    أرجع نتیجہ;
}

UnicodeString& 
ICUService_کلید::parseSuffix(UnicodeString& نتیجہ) 
{
    int32_t n = نتیجہ.اشاریہ_دا(اگیتر_DELIMITER);
    إذا (n >= 0) {
        نتیجہ.remove(0, n+1);
    }
    أرجع نتیجہ;
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString& 
ICUService_کلید::debug(UnicodeString& نتیجہ) ثابت 
{
    debugClass(نتیجہ);
    نتیجہ.append((UnicodeString)" id: ");
    نتیجہ.append(_id);
    أرجع نتیجہ;
}

UnicodeString& 
ICUService_کلید::debugClass(UnicodeString& نتیجہ) ثابت 
{
    أرجع نتیجہ.append((UnicodeString)"ICUServiceKey");
}
#جے_انت

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_کلید)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_منزل_Adopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instance_منزل_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    مٹاؤ _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کوڈ& حالت) ثابت 
{
    إذا (U_SUCCESS(حالت)) {
        UnicodeString عارضی;
        إذا (_id == کلید.currentID(عارضی)) {
            أرجع service->cloneInstance(_instance); 
        }
    }
    أرجع خالی_سوچک;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& نتیجہ, UError_کوڈ& حالت) ثابت 
{
    إذا (_visible) {
        نتیجہ.put(_id, (فراغ*)هذا, حالت); // cast away const
    } وإلا {
        نتیجہ.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& نتیجہ) ثابت 
{
    إذا (_visible && _id == id) {
        نتیجہ = _id;
    } وإلا {
        نتیجہ.سیٹ_منزل_Bogus();
    }
    أرجع نتیجہ;
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& منزل_Append_منزل) ثابت 
{
    debugClass(منزل_Append_منزل);
    منزل_Append_منزل.append((UnicodeString)" id: ");
    منزل_Append_منزل.append(_id);
    منزل_Append_منزل.append((UnicodeString)", visible: ");
    منزل_Append_منزل.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    أرجع منزل_Append_منزل;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& منزل_Append_منزل) ثابت 
{
    أرجع منزل_Append_منزل.append((UnicodeString)"SimpleFactory");
}
#جے_انت

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
صنف Cache_اندراج : عام UMemory {
خاص:
    int32_t refcount;

عام:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_اندراج() {
        مٹاؤ service;
    }

    Cache_اندراج(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_اندراج* ref() {
        ++refcount;
        أرجع هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_اندراج* unref() {
        إذا ((--refcount) == 0) {
            مٹاؤ هذا;
            أرجع خالی_سوچک;
        }
        أرجع هذا;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        أرجع refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
cacheDeleter(فراغ* obj) {
    U_NAMESPACE_USE ((Cache_اندراج*)obj)->unref();
}

U_CDECL_سرا

/*
******************************************************************
*/

صنف DNCache : عام UMemory {
عام:
    Hashtable cache;
    ثابت Locale locale;

    DNCache(ثابت Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ثابت UnicodeString& displayName, 
                   ثابت UnicodeString& id,
                   UError_کوڈ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        StringPair* sp = جديد StringPair(displayName, id);
        إذا (sp == خالی_سوچک || sp->isBogus()) {
            حالت = U_حافظہ_ونڈ_ERROR;
            مٹاؤ sp;
            أرجع خالی_سوچک;
        }
        أرجع sp;
    }
    أرجع خالی_سوچک;
}

UBool 
StringPair::isBogus() ثابت {
    أرجع displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ثابت UnicodeString& _displayName, 
                       ثابت UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
userv_deleteStringPair(فراغ *obj) {
    U_NAMESPACE_USE مٹاؤ (StringPair*) obj;
}
U_CDECL_سرا

/*
******************************************************************
*/

ساكن UMutex جندرا;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(خالی_سوچک)
, serviceCache(خالی_سوچک)
, idCache(خالی_سوچک)
, dnCache(خالی_سوچک)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(خالی_سوچک)
, serviceCache(خالی_سوچک)
, idCache(خالی_سوچک)
, dnCache(خالی_سوچک)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&جندرا);
        clearCaches();
        مٹاؤ factories;
        factories = خالی_سوچک;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UError_کوڈ& حالت) ثابت 
{
    أرجع get(descriptor, خالی_سوچک, حالت);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_کوڈ& حالت) ثابت 
{
    UObject* نتیجہ = خالی_سوچک;
    ICUService_کلید* کلید = create_کلید(&descriptor, حالت);
    إذا (کلید) {
        نتیجہ = get_کلید(*کلید, actualReturn, حالت);
        مٹاؤ کلید;
    }
    أرجع نتیجہ;
}

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UError_کوڈ& حالت) ثابت 
{
    أرجع get_کلید(کلید, خالی_سوچک, حالت);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, UError_کوڈ& حالت) ثابت 
{
    أرجع get_کلید(کلید, actualReturn, خالی_سوچک, حالت);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_سرگرم(!reentering) 
    {
        إذا (f_سرگرم) umtx_جندرا(fMutex);
    }
    مضمن ~XMutex() {
        إذا (f_سرگرم) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool f_سرگرم;
};

// called only by factories, treat as private
UObject* 
ICUService::get_کلید(ICUService_کلید& کلید, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UError_کوڈ& حالت) ثابت 
{
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    إذا (isDefault()) {
        أرجع handleDefault(کلید, actualReturn, حالت);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

    Cache_اندراج* نتیجہ = خالی_سوچک;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&جندرا, factory != خالی_سوچک);

        إذا (serviceCache == خالی_سوچک) {
            ncthis->serviceCache = جديد Hashtable(حالت);
            إذا (ncthis->serviceCache == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع خالی_سوچک;
            }
            إذا (U_FAILURE(حالت)) {
                مٹاؤ serviceCache;
                أرجع خالی_سوچک;
            }
            serviceCache->سیٹ_قدر_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_اشارہ_گر<UVector> cacheDescriptorList;
        UBool put_وچ_Cache = خطأ;

        int32_t شروع_اشاریہ = 0;
        int32_t حد_بندی = factories->size();
        UBool cache_نتیجہ = سچ;

        إذا (factory != خالی_سوچک) {
            لكل (int32_t i = 0; i < حد_بندی; ++i) {
                إذا (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    شروع_اشاریہ = i + 1;
                    اكسر;
                }
            }
            إذا (شروع_اشاریہ == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                حالت = U_ILLEGAL_دلیل_ERROR;
                أرجع خالی_سوچک;
            }
            cache_نتیجہ = خطأ;
        }

        افعل {
            currentDescriptor.remove();
            کلید.currentDescriptor(currentDescriptor);
            نتیجہ = static_cast<Cache_اندراج*>(serviceCache->get(currentDescriptor));
            إذا (نتیجہ != خالی_سوچک) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_وچ_Cache = سچ;

            int32_t اشاریہ = شروع_اشاریہ;
            طالما (اشاریہ < حد_بندی) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(اشاریہ++));
                Local_اشارہ_گر<UObject> service(f->create(کلید, هذا, حالت));
                إذا (U_FAILURE(حالت)) {
                    أرجع خالی_سوچک;
                }
                إذا (service.isValid()) {
                    نتیجہ = جديد Cache_اندراج(currentDescriptor, service.getAlias());
                    إذا (نتیجہ == خالی_سوچک) {
                        حالت = U_حافظہ_ونڈ_ERROR;
                        أرجع خالی_سوچک;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outer_سرا;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            إذا (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_کوڈ(جديد UVector(uprv_deleteUObject, خالی_سوچک, 5, حالت), حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع خالی_سوچک;
                }
            }

            Local_اشارہ_گر<UnicodeString> id_منزل_Cache(جديد UnicodeString(currentDescriptor), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالی_سوچک;
            }
            إذا (id_منزل_Cache->isBogus()) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع خالی_سوچک;
            }
            cacheDescriptorList->adoptElement(id_منزل_Cache.orphan(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالی_سوچک;
            }
        } طالما (کلید.fallback());
outer_سرا:

        إذا (نتیجہ != خالی_سوچک) {
            إذا (put_وچ_Cache && cache_نتیجہ) {
                serviceCache->put(نتیجہ->actualDescriptor, نتیجہ, حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع خالی_سوچک;
                }

                إذا (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, نتیجہ, حالت);
                        إذا (U_FAILURE(حالت)) {
                            أرجع خالی_سوچک;
                        }

                        نتیجہ->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            إذا (actualReturn != خالی_سوچک) {
                // strip null prefix
                إذا (نتیجہ->actualDescriptor.اشاریہ_دا(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(نتیجہ->actualDescriptor, 
                        1, 
                        نتیجہ->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = نتیجہ->actualDescriptor;
                }

                إذا (actualReturn->isBogus()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                    مٹاؤ نتیجہ;
                    أرجع خالی_سوچک;
                }
            }

            UObject* service = cloneInstance(نتیجہ->service);
            إذا (put_وچ_Cache && !cache_نتیجہ) {
                مٹاؤ نتیجہ;
            }
            أرجع service;
        }
    }

    أرجع handleDefault(کلید, actualReturn, حالت);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_کلید& /*کلید*/, UnicodeString* /* actualIDReturn */, UError_کوڈ& /* status */) ثابت 
{
    أرجع خالی_سوچک;
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجہ, UError_کوڈ& حالت) ثابت {
    أرجع getVisibleIDs(نتیجہ, خالی_سوچک, حالت);
}

UVector& 
ICUService::getVisibleIDs(UVector& نتیجہ, ثابت UnicodeString* میل_ID, UError_کوڈ& حالت) ثابت 
{
    نتیجہ.removeAllElements();

    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
    }
    UObjectDeleter *savedDeleter = نتیجہ.سیٹ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&جندرا);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != خالی_سوچک) {
            ICUService_کلید* fallback_کلید = create_کلید(میل_ID, حالت);

            لكل (int32_t pos = UHASH_پہلا; U_SUCCESS(حالت); ) {
                ثابت UHashElement* e = map->اگلا_Element(pos);
                إذا (e == خالی_سوچک) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->کلید.اشارہ_گر);
                إذا (fallback_کلید != خالی_سوچک) {
                    إذا (!fallback_کلید->is_Fallback_دا(*id)) {
                        استمر;
                    }
                }

                Local_اشارہ_گر<UnicodeString> idClone(id->clone(), حالت);
                نتیجہ.adoptElement(idClone.orphan(), حالت);
            }
            مٹاؤ fallback_کلید;
        }
    }
    إذا (U_FAILURE(حالت)) {
        نتیجہ.removeAllElements();
    }
    نتیجہ.سیٹ_Deleter(savedDeleter);
    أرجع نتیجہ;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UError_کوڈ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع خالی_سوچک;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    إذا (idCache == خالی_سوچک) {
        ncthis->idCache = جديد Hashtable(حالت);
        إذا (idCache == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
        } وإلا إذا (factories != خالی_سوچک) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                مٹاؤ idCache;
                ncthis->idCache = خالی_سوچک;
            }
        }
    }

    أرجع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجہ) ثابت 
{
    أرجع getDisplayName(id, نتیجہ, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& نتیجہ, ثابت Locale& locale) ثابت 
{
    {
        UError_کوڈ حالت = U_ZERO_ERROR;
        Mutex mutex(&جندرا);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != خالی_سوچک) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            إذا (f != خالی_سوچک) {
                f->getDisplayName(id, locale, نتیجہ);
                أرجع نتیجہ;
            }

            // fallback
            حالت = U_ZERO_ERROR;
            ICUService_کلید* fallback_کلید = create_کلید(&id, حالت);
            طالما (fallback_کلید != خالی_سوچک && fallback_کلید->fallback()) {
                UnicodeString us;
                fallback_کلید->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                إذا (f != خالی_سوچک) {
                    f->getDisplayName(id, locale, نتیجہ);
                    مٹاؤ fallback_کلید;
                    أرجع نتیجہ;
                }
            }
            مٹاؤ fallback_کلید;
        }
    }
    نتیجہ.سیٹ_منزل_Bogus();
    أرجع نتیجہ;
}

UVector& 
ICUService::get_Display_ناں(UVector& نتیجہ, UError_کوڈ& حالت) ثابت 
{
    أرجع get_Display_ناں(نتیجہ, Locale::getDefault(), خالی_سوچک, حالت);
}


UVector& 
ICUService::get_Display_ناں(UVector& نتیجہ, ثابت Locale& locale, UError_کوڈ& حالت) ثابت 
{
    أرجع get_Display_ناں(نتیجہ, locale, خالی_سوچک, حالت);
}

UVector& 
ICUService::get_Display_ناں(UVector& نتیجہ, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* میل_ID, 
                            UError_کوڈ& حالت) ثابت 
{
    نتیجہ.removeAllElements();
    نتیجہ.سیٹ_Deleter(userv_deleteStringPair);
    إذا (U_SUCCESS(حالت)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&جندرا);

        إذا (dnCache != خالی_سوچک && dnCache->locale != locale) {
            مٹاؤ dnCache;
            ncthis->dnCache = خالی_سوچک;
        }

        إذا (dnCache == خالی_سوچک) {
            ثابت Hashtable* m = getVisibleIDMap(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع نتیجہ;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            إذا (dnCache == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع نتیجہ;
            }

            int32_t pos = UHASH_پہلا;
            ثابت UHashElement* اندراج = خالی_سوچک;
            طالما ((اندراج = m->اگلا_Element(pos)) != خالی_سوچک) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(اندراج->کلید.اشارہ_گر);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(اندراج->value.اشارہ_گر);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                إذا (dname.isBogus()) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, حالت); // share pointer with visibleIDMap
                    إذا (U_SUCCESS(حالت)) {
                        استمر;
                    }
                }
                مٹاؤ dnCache;
                ncthis->dnCache = خالی_سوچک;
                أرجع نتیجہ;
            }
        }
    }

    ICUService_کلید* میل_کلید = create_کلید(میل_ID, حالت);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_پہلا; 
    ثابت UHashElement *اندراج = خالی_سوچک;
    طالما ((اندراج = dnCache->cache.اگلا_Element(pos)) != خالی_سوچک) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(اندراج->value.اشارہ_گر);
        إذا (میل_کلید != خالی_سوچک && !میل_کلید->is_Fallback_دا(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(اندراج->کلید.اشارہ_گر);
        StringPair* sp = StringPair::create(*id, *dn, حالت);
        نتیجہ.adoptElement(sp, حالت);
        إذا (U_FAILURE(حالت)) {
            نتیجہ.removeAllElements();
            اكسر;
        }
    }
    مٹاؤ میل_کلید;

    أرجع نتیجہ;
}

URegistry_کلید
ICUService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UError_کوڈ& حالت) 
{
    أرجع رجسٹر_Instance(obj_منزل_Adopt, id, سچ, حالت);
}

URegistry_کلید
ICUService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_کوڈ& حالت) 
{
    ICUService_کلید* کلید = create_کلید(&id, حالت);
    إذا (کلید != خالی_سوچک) {
        UnicodeString canonicalID;
        کلید->canonicalID(canonicalID);
        مٹاؤ کلید;

        ICUServiceFactory* f = createSimpleFactory(obj_منزل_Adopt, canonicalID, visible, حالت);
        إذا (f != خالی_سوچک) {
            أرجع رجسٹر_Factory(f, حالت);
        }
    }
    مٹاؤ obj_منزل_Adopt;
    أرجع خالی_سوچک;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_کوڈ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا ((obj_منزل_Adopt != خالی_سوچک) && (!id.isBogus())) {
            أرجع جديد SimpleFactory(obj_منزل_Adopt, id, visible);
        }
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع خالی_سوچک;
}

URegistry_کلید
ICUService::رجسٹر_Factory(ICUServiceFactory* factory_منزل_Adopt, UError_کوڈ& حالت)
{
    Local_اشارہ_گر<ICUServiceFactory>lp_Factory_منزل_Adopt(factory_منزل_Adopt);
    إذا (U_FAILURE(حالت) || factory_منزل_Adopt == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    {
        Mutex mutex(&جندرا);

        إذا (factories == خالی_سوچک) {
            Local_اشارہ_گر<UVector> lpFactories(جديد UVector(uprv_deleteUObject, خالی_سوچک, حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع خالی_سوچک;
            }
            factories = lpFactories.orphan();
        }
        factories->داخل_کرو_Element_At(lp_Factory_منزل_Adopt.orphan(), 0, حالت);
        إذا (U_SUCCESS(حالت)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    إذا (U_SUCCESS(حالت)) {
        notify_بدلیا();
        أرجع (URegistry_کلید)factory_منزل_Adopt;
    } وإلا {
        أرجع خالی_سوچک;
    }
}

UBool 
ICUService::unregister(URegistry_کلید rkey, UError_کوڈ& حالت) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool نتیجہ = خطأ;
    إذا (factory != خالی_سوچک && factories != خالی_سوچک) {
        Mutex mutex(&جندرا);

        إذا (factories->removeElement(factory)) {
            clearCaches();
            نتیجہ = سچ;
        } وإلا {
            حالت = U_ILLEGAL_دلیل_ERROR;
            مٹاؤ factory;
        }
    }
    إذا (نتیجہ) {
        notify_بدلیا();
    }
    أرجع نتیجہ;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&جندرا);
        reInitializeFactories();
        clearCaches();
    }
    notify_بدلیا();
}

فراغ 
ICUService::reInitializeFactories() 
{
    إذا (factories != خالی_سوچک) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    أرجع گنتی_Factories() == 0;
}

ICUService_کلید* 
ICUService::create_کلید(ثابت UnicodeString* id, UError_کوڈ& حالت) ثابت 
{
    أرجع (U_FAILURE(حالت) || id == خالی_سوچک) ? خالی_سوچک : جديد ICUService_کلید(*id);
}

فراغ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    مٹاؤ dnCache;
    dnCache = خالی_سوچک;
    مٹاؤ idCache;
    idCache = خالی_سوچک;
    مٹاؤ serviceCache; serviceCache = خالی_سوچک;
}

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    مٹاؤ serviceCache; serviceCache = خالی_سوچک;
}

UBool 
ICUService::مندا_Listener(ثابت EventListener& l) ثابت 
{
    أرجع dynamic_cast<ثابت ServiceListener*>(&l) != خالی_سوچک;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).service_بدلیا(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& نتیجہ) ثابت 
{
    أرجع نتیجہ.append(name);
}

int32_t 
ICUService::گنتی_Factories() ثابت 
{
    أرجع factories == خالی_سوچک ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    أرجع timestamp;
}

U_NAMESPACE_سرا

/* UCONFIG_NO_SERVICE */
#جے_انت
