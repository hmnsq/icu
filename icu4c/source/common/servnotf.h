// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
#اگر_غیر_تعریف_شدہ ICUNOTIF_H
#تعریف_کرو ICUNOTIF_H

#شامل "unicode/utypes.h"

#اگر UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to BreakIterator
 * even when break iteration is removed from the build.
 */
class ICUNotifier;

U_NAMESPACE_انتہا

#ورنہ

#شامل "unicode/uobject.h"
#شامل "unicode/unistr.h"

#شامل "mutex.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

class U_COMMON_API EventListener : عوامی UObject {
عوامی: 
    مجازی ~EventListener();

عوامی:
    ساکن UClassID U_EXPORT2 getStaticClassID();

    مجازی UClassID getDynamicClassID() ثابت override;

عوامی:
#اگر_تعریف_شدہ SERVICE_DEBUG
    مجازی UnicodeString& debug(UnicodeString& نتیجہ) ثابت {
      واپس debugClass(نتیجہ);
    }

    مجازی UnicodeString& debugClass(UnicodeString& نتیجہ) ثابت {
      واپس نتیجہ.append((UnicodeString)"Key");
    }
#اگر_ختم
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

class U_COMMON_API ICUNotifier : عوامی UMemory  {
نجی: UVector* listeners;
         
عوامی: 
    ICUNotifier();
    
    مجازی ~ICUNotifier();
    
    /**
     * Add a listener to be notified when notifyChanged is called.
     * The listener must not be null. AcceptsListener must return
     * true for the listener. Attempts to concurrently
     * register the identical listener more than once will be
     * silently ignored.  
     */
    مجازی خالی جوڑو_Listener(ثابت EventListener* l, UError_رمز& حالت_2);
    
    /**
     * Stop notifying this listener.  The listener must
     * not be null. Attempts to remove a listener that is
     * not registered will be silently ignored.
     */
    مجازی خالی removeListener(ثابت EventListener* l, UError_رمز& حالت_2);
    
    /**
     * ICU doesn't spawn its own threads.  All listeners are notified in
     * the thread of the caller.  Misbehaved listeners can therefore
     * indefinitely block the calling thread.  Callers should beware of
     * deadlock situations.  
     */
    مجازی خالی notify_بدلا();
    
محفوظ: 
    /**
     * Subclasses implement this to return true if the listener is
     * of the appropriate type.
     */
    مجازی UBool قبول_کرتا_Listener(ثابت EventListener& l) ثابت = 0;
    
    /**
     * Subclasses implement this to notify the listener.
     */
    مجازی خالی notifyListener(EventListener& l) ثابت = 0;
};

U_NAMESPACE_انتہا

/* UCONFIG_NO_SERVICE */
#اگر_ختم

/* ICUNOTIF_H */
#اگر_ختم
