// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "servnotf.h"
#यदि_परिभाषितम् NOTIFIER_DEBUG
#अन्तर्भाव <stdio.h>
#यदि_अन्तः

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

स्थिर UMutex notify_तालकम्;

ICUNotifier::ICUNotifier() 
: listeners(शून्यसूचकः) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notify_तालकम्);
        विलोपय listeners;
        listeners = शून्यसूचकः;
    }
}


शून्य 
ICUNotifier::सङ्कलय_Listener(ध्रुव EventListener* l, UError_संहिता& स्थितिः) 
{
    यदि (U_SUCCESS(स्थितिः)) {
        यदि (l == शून्यसूचकः) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }

        यदि (acceptsListener(*l)) {
            Mutex lmx(&notify_तालकम्);
            यदि (listeners == शून्यसूचकः) {
                LocalPointer<UVector> lpListeners(नूतन UVector(5, स्थितिः), स्थितिः);
                यदि (U_FAILURE(स्थितिः)) {
                    प्रत्यागम;
                }
                listeners = lpListeners.orphan();
            } अन्यथा {
                प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    ध्रुव EventListener* el = static_cast<ध्रुव EventListener*>(listeners->elementAt(i));
                    यदि (l == el) {
                        प्रत्यागम;
                    }
                }
            }

            listeners->सङ्कलय_Element((शून्य*)l, स्थितिः); // cast away const
        }
#यदि_परिभाषितम् NOTIFIER_DEBUG
        अन्यथा {
            fprintf(stderr, "Listener invalid for this notifier.");
            निर्गच्छ(1);
        }
#यदि_अन्तः
    }
}

शून्य 
ICUNotifier::removeListener(ध्रुव EventListener *l, UError_संहिता& स्थितिः) 
{
    यदि (U_SUCCESS(स्थितिः)) {
        यदि (l == शून्यसूचकः) {
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }

        {
            Mutex lmx(&notify_तालकम्);
            यदि (listeners != शून्यसूचकः) {
                // identity equality check
                प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
                    ध्रुव EventListener* el = static_cast<ध्रुव EventListener*>(listeners->elementAt(i));
                    यदि (l == el) {
                        listeners->removeElementAt(i);
                        यदि (listeners->size() == 0) {
                            विलोपय listeners;
                            listeners = शून्यसूचकः;
                        }
                        प्रत्यागम;
                    }
                }
            }
        }
    }
}

शून्य 
ICUNotifier::notify_परिवर्तितम्() 
{
    Mutex lmx(&notify_तालकम्);
    यदि (listeners != शून्यसूचकः) {
        प्रति (पूर्णाङ्क i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_अन्तः

/* UCONFIG_NO_SERVICE */
#यदि_अन्तः

