// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#nếu !UCONFIG_NO_SERVICE

#include "servnotf.h"
#ifdef NOTIFIER_DEBUG
#include <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

static UMutex notifyLock;

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


void 
ICUNotifier::addListener(const EventListener* l, UErrorCode& status) 
{
    nếu (U_SUCCESS(status)) {
        nếu (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            trả_về;
        }

        nếu (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            nếu (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                nếu (U_FAILURE(status)) {
                    trả_về;
                }
                listeners = lpListeners.orphan();
            } không_thì {
                cho (số_nguyên i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    nếu (l == el) {
                        trả_về;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        không_thì {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    nếu (U_SUCCESS(status)) {
        nếu (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            trả_về;
        }

        {
            Mutex lmx(&notifyLock);
            nếu (listeners != nullptr) {
                // identity equality check
                cho (số_nguyên i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    nếu (l == el) {
                        listeners->removeElementAt(i);
                        nếu (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        trả_về;
                    }
                }
            }
        }
    }
}

void 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    nếu (listeners != nullptr) {
        cho (số_nguyên i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

