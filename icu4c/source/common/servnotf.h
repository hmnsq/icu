// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#اگر_تعریف_نشده ICUNOTIF_H
#تعریف_کن ICUNOTIF_H

#ضمن "unicode/utypes.h"

#إذا UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_انتها

#وإلا

#ضمن "unicode/uobject.h"
#ضمن "unicode/unistr.h"

#ضمن "mutex.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : عام UObject {
عام: 
    مجازی ~EventListener();

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

عام:
#اگر_تعریف_شده SERVICE_DEBUG
    مجازی UnicodeString& debug(UnicodeString& نتیجه) ثابت {
      أرجع debugClass(نتیجه);
    }

    مجازی UnicodeString& debugClass(UnicodeString& نتیجه) ثابت {
      أرجع نتیجه.append((UnicodeString)"Key");
    }
#پایان_اگر
};

/**
 * <p>Abstract implementation of a notification facility.  Clients add
 * EventListeners with addListener and remove them with removeListener.
 * Notifiers call notifyChanged when they wish to notify listeners.
 * This queues the listener list on the notification thread, which
 * eventually dequeues the list and calls notifyListener on each
 * listener in the list.</p>
 *
 * <p>Subclasses override acceptsListener and notifyListener 
 * to add type-safe notification.  AcceptsListener should return
 * true if the listener is of the appropriate type; ICUNotifier
 * itself will ensure the listener is non-null and that the
 * identical listener is not already registered with the Notifier.
 * NotifyListener should cast the listener to the appropriate 
 * type and call the appropriate method on the listener.
 */

class U_COMMON_API ICUNotifier : عام UMemory  {
خاص: UVector* listeners;
         
عام: 
    ICUNotifier();
    
    مجازی ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    مجازی فراغ جمع_کن_Listener(ثابت EventListener* l, UError_کد& وضعیت);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    مجازی فراغ removeListener(ثابت EventListener* l, UError_کد& وضعیت);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    مجازی فراغ notify_تغییریافته();
    
محمي: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    مجازی UBool میپذیرد_Listener(ثابت EventListener& l) ثابت = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    مجازی فراغ notifyListener(EventListener& l) ثابت = 0;
};

U_NAMESPACE_انتها

/* UCONFIG_NO_SERVICE */
#پایان_اگر

/* ICUNOTIF_H */
#پایان_اگر
