// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#якщо !UCONFIG_NO_SERVICE

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
    якщо (U_SUCCESS(status)) {
        якщо (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            повернути;
        }

        якщо (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            якщо (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                якщо (U_FAILURE(status)) {
                    повернути;
                }
                listeners = lpListeners.orphan();
            } інакше {
                для (ціле i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    якщо (l == el) {
                        повернути;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        інакше {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    якщо (U_SUCCESS(status)) {
        якщо (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            повернути;
        }

        {
            Mutex lmx(&notifyLock);
            якщо (listeners != nullptr) {
                // identity equality check
                для (ціле i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    якщо (l == el) {
                        listeners->removeElementAt(i);
                        якщо (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        повернути;
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
    якщо (listeners != nullptr) {
        для (ціле i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

