// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#如果 !UCONFIG_NO_SERVICE

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
    如果 (U_SUCCESS(status)) {
        如果 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            返回;
        }

        如果 (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            如果 (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                如果 (U_FAILURE(status)) {
                    返回;
                }
                listeners = lpListeners.orphan();
            } 否则 {
                为 (整数 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    如果 (l == el) {
                        返回;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        否则 {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    如果 (U_SUCCESS(status)) {
        如果 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            返回;
        }

        {
            Mutex lmx(&notifyLock);
            如果 (listeners != nullptr) {
                // identity equality check
                为 (整数 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    如果 (l == el) {
                        listeners->removeElementAt(i);
                        如果 (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        返回;
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
    如果 (listeners != nullptr) {
        为 (整数 i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

