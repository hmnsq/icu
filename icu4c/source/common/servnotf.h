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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#অন্যথা

#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/unistr.h"

#অন্তর্ভাৱ "mutex.h"
#অন্তর্ভাৱ "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : সার্ৱজনিক UObject {
সার্ৱজনিক: 
    আভাস ~EventListener();

সার্ৱজনিক:
    স্থির UClassID U_EXPORT2 getStaticClassID();

    আভাস UClassID getDynamicClassID() ধ্রুৱ override;

সার্ৱজনিক:
#ifdef SERVICE_DEBUG
    আভাস UnicodeString& debug(UnicodeString& result) ধ্রুৱ {
      ঘূৰাই debugClass(result);
    }

    আভাস UnicodeString& debugClass(UnicodeString& result) ধ্রুৱ {
      ঘূৰাই result.append((UnicodeString)"Key");
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

class U_COMMON_API ICUNotifier : সার্ৱজনিক UMemory  {
গুপ্ত: UVector* listeners;
         
সার্ৱজনিক: 
    ICUNotifier();
    
    আভাস ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    আভাস শূন্য addListener(ধ্রুৱ EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    আভাস শূন্য removeListener(ধ্রুৱ EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    আভাস শূন্য notifyChanged();
    
রক্ষিত: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    আভাস UBool acceptsListener(ধ্রুৱ EventListener& l) ধ্রুৱ = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    আভাস শূন্য notifyListener(EventListener& l) ধ্রুৱ = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
