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

#অন্তর্ভাব "unicode/utypes.h"

#যদি UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_END

#অন্যথা

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/unistr.h"

#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : সর্বজনীন UObject {
সর্বজনীন: 
    আভাসী ~EventListener();

সর্বজনীন:
    স্থির UClassID U_EXPORT2 getStaticClassID();

    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

সর্বজনীন:
#ifdef SERVICE_DEBUG
    আভাসী UnicodeString& debug(UnicodeString& ফলাফল) ধ্রুবক {
      ফেরত debugClass(ফলাফল);
    }

    আভাসী UnicodeString& debugClass(UnicodeString& ফলাফল) ধ্রুবক {
      ফেরত ফলাফল.append((UnicodeString)"Key");
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

class U_COMMON_API ICUNotifier : সর্বজনীন UMemory  {
ব্যক্তিগত: UVector* listeners;
         
সর্বজনীন: 
    ICUNotifier();
    
    আভাসী ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    আভাসী শূন্য addListener(ধ্রুবক EventListener* l, UErrorCode& status);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    আভাসী শূন্য removeListener(ধ্রুবক EventListener* l, UErrorCode& status);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    আভাসী শূন্য notifyChanged();
    
সুরক্ষিত: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    আভাসী UBool acceptsListener(ধ্রুবক EventListener& l) ধ্রুবক = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    আভাসী শূন্য notifyListener(EventListener& l) ধ্রুবক = 0;
};

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

/* ICUNOTIF_H */
#endif
