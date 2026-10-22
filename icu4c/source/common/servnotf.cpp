// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#もし !UCONFIG_NO_SERVICE

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
    もし (U_SUCCESS(status)) {
        もし (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            戻る;
        }

        もし (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            もし (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                もし (U_FAILURE(status)) {
                    戻る;
                }
                listeners = lpListeners.orphan();
            } 否则 {
                ために (整数 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    もし (l == el) {
                        戻る;
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
    もし (U_SUCCESS(status)) {
        もし (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            戻る;
        }

        {
            Mutex lmx(&notifyLock);
            もし (listeners != nullptr) {
                // identity equality check
                ために (整数 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    もし (l == el) {
                        listeners->removeElementAt(i);
                        もし (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        戻る;
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
    もし (listeners != nullptr) {
        ために (整数 i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

