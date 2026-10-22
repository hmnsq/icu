// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#если !UCONFIG_NO_SERVICE

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
    если (U_SUCCESS(status)) {
        если (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }

        если (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            если (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                если (U_FAILURE(status)) {
                    вернуть;
                }
                listeners = lpListeners.orphan();
            } иначе {
                для (целое i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    если (l == el) {
                        вернуть;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        иначе {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    если (U_SUCCESS(status)) {
        если (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }

        {
            Mutex lmx(&notifyLock);
            если (listeners != nullptr) {
                // identity equality check
                для (целое i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    если (l == el) {
                        listeners->removeElementAt(i);
                        если (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        вернуть;
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
    если (listeners != nullptr) {
        для (целое i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

