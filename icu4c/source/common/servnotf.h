// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#ifndef ICUNOTIF_H
#define ICUNOTIF_H

#شامل "unicode/utypes.h"

#اگر UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#وگرنه

#شامل "unicode/uobject.h"
#شامل "unicode/unistr.h"

#شامل "mutex.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : عمومی UObject {
عمومی: 
    مجازی ~EventListener();

عمومی:
    ایستا UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

عمومی:
#ifdef SERVICE_DEBUG
    مجازی UnicodeString& debug(UnicodeString& result) ثابت {
      بازگشت debugClass(result);
    }

    مجازی UnicodeString& debugClass(UnicodeString& result) ثابت {
      بازگشت result.append((UnicodeString)"Key");
    }
#endif
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

class U_COMMON_API ICUNotifier : عمومی UMemory  {
خصوصی: UVector* listeners;
         
عمومی: 
    ICUNotifier();
    
    مجازی ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    مجازی تهی addListener(ثابت EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    مجازی تهی removeListener(ثابت EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    مجازی تهی notifyChanged();
    
محافظتشده: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    مجازی UBool acceptsListener(ثابت EventListener& l) ثابت = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    مجازی تهی notifyListener(EventListener& l) ثابت = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
