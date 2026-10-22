// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#အကယ်၍ !UCONFIG_NO_SERVICE

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
    အကယ်၍ (U_SUCCESS(status)) {
        အကယ်၍ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ပြန်ပေး;
        }

        အကယ်၍ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            အကယ်၍ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                အကယ်၍ (U_FAILURE(status)) {
                    ပြန်ပေး;
                }
                listeners = lpListeners.orphan();
            } မဟုတ်ပါက {
                အတွက် (ကိန်းပြည့် i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    အကယ်၍ (l == el) {
                        ပြန်ပေး;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        မဟုတ်ပါက {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    အကယ်၍ (U_SUCCESS(status)) {
        အကယ်၍ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ပြန်ပေး;
        }

        {
            Mutex lmx(&notifyLock);
            အကယ်၍ (listeners != nullptr) {
                // identity equality check
                အတွက် (ကိန်းပြည့် i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    အကယ်၍ (l == el) {
                        listeners->removeElementAt(i);
                        အကယ်၍ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ပြန်ပေး;
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
    အကယ်၍ (listeners != nullptr) {
        အတွက် (ကိန်းပြည့် i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

