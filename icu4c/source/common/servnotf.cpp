// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "servnotf.h"
#ifdef NOTIFIER_DEBUG
#અન્તર્ભાવ <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

સ્થિર UMutex notifyLock;

ICUNotifier::ICUNotifier() 
: listeners(nullptr) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notifyLock);
        delete listeners;
        listeners = nullptr;
    }
}


શૂન્ય 
ICUNotifier::addListener(અચલ EventListener* l, UErrorCode& status) 
{
    જો (U_SUCCESS(status)) {
        જો (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }

        જો (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            જો (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(નવું UVector(5, status), status);
                જો (U_FAILURE(status)) {
                    પાછા;
                }
                listeners = lpListeners.orphan();
            } નહીંતર {
                માટે (પૂર્ણાંક i = 0, e = listeners->size(); i < e; ++i) {
                    અચલ EventListener* el = static_cast<અચલ EventListener*>(listeners->elementAt(i));
                    જો (l == el) {
                        પાછા;
                    }
                }
            }

            listeners->addElement((શૂન્ય*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        નહીંતર {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

શૂન્ય 
ICUNotifier::removeListener(અચલ EventListener *l, UErrorCode& status) 
{
    જો (U_SUCCESS(status)) {
        જો (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }

        {
            Mutex lmx(&notifyLock);
            જો (listeners != nullptr) {
                // identity equality check
                માટે (પૂર્ણાંક i = 0, e = listeners->size(); i < e; ++i) {
                    અચલ EventListener* el = static_cast<અચલ EventListener*>(listeners->elementAt(i));
                    જો (l == el) {
                        listeners->removeElementAt(i);
                        જો (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        પાછા;
                    }
                }
            }
        }
    }
}

શૂન્ય 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    જો (listeners != nullptr) {
        માટે (પૂર્ણાંક i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

