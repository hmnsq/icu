// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#නම් !UCONFIG_NO_SERVICE

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
    නම් (U_SUCCESS(status)) {
        නම් (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ආපසු;
        }

        නම් (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            නම් (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                නම් (U_FAILURE(status)) {
                    ආපසු;
                }
                listeners = lpListeners.orphan();
            } නැතහොත් {
                සඳහා (පූර්ණ_සංඛ්‍යා i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    නම් (l == el) {
                        ආපසු;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        නැතහොත් {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    නම් (U_SUCCESS(status)) {
        නම් (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ආපසු;
        }

        {
            Mutex lmx(&notifyLock);
            නම් (listeners != nullptr) {
                // identity equality check
                සඳහා (පූර්ණ_සංඛ්‍යා i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    නම් (l == el) {
                        listeners->removeElementAt(i);
                        නම් (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ආපසු;
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
    නම් (listeners != nullptr) {
        සඳහා (පූර්ණ_සංඛ්‍යා i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

