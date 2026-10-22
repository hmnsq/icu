// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#अन्तर्भाव "unicode/utypes.h"

#जर U_ENABLE_TRACING

संरचना UResourceBundle;

U_NAMESPACE_BEGIN

class CharString;

/**
 * Instances of this class store information used to trace reads from resource
 * bundles when ICU is built with --enable-tracing.
 *
 * All arguments of type const UResourceBundle*, const char*, and
 * const ResourceTracer& are stored as pointers. The caller must retain
 * ownership for the lifetime of this ResourceTracer.
 *
 * Exported as U_COMMON_API for Windows because it is a value field
 * in other exported types.
 */
class U_COMMON_API ResourceTracer {
सार्वजनिक:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_किल्ली(nullptr),
        fIndex(-1) {}

    ResourceTracer(स्थिरांक UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_किल्ली(nullptr),
        fIndex(-1) {}

    ResourceTracer(स्थिरांक UResourceBundle* resB, स्थिरांक अक्षर* किल्ली) :
        fResB(resB),
        fParent(nullptr),
        f_किल्ली(किल्ली),
        fIndex(-1) {}

    ResourceTracer(स्थिरांक UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_किल्ली(nullptr),
        fIndex(index) {}

    ResourceTracer(स्थिरांक ResourceTracer& parent, स्थिरांक अक्षर* किल्ली) :
        fResB(nullptr),
        fParent(&parent),
        f_किल्ली(किल्ली),
        fIndex(-1) {}

    ResourceTracer(स्थिरांक ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_किल्ली(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    शून्य trace(स्थिरांक अक्षर* type) स्थिरांक;
    शून्य traceOpen() स्थिरांक;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    शून्य maybeTrace(स्थिरांक अक्षर* type) स्थिरांक {
        जर (fResB || fParent) {
            trace(type);
        }
    }

खाजगी:
    स्थिरांक UResourceBundle* fResB;
    स्थिरांक ResourceTracer* fParent;
    स्थिरांक अक्षर* f_किल्ली;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) स्थिरांक;

    CharString& getResPath(CharString& output, UErrorCode& status) स्थिरांक;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(स्थिरांक अक्षर* path, स्थिरांक अक्षर* type, स्थिरांक अक्षर* name);

खाजगी:
    स्थिर शून्य traceOpenDataFile(स्थिरांक अक्षर* path, स्थिरांक अक्षर* type, स्थिरांक अक्षर* name);
    स्थिर शून्य traceOpenResFile(स्थिरांक अक्षर* path, स्थिरांक अक्षर* name);
};

U_NAMESPACE_END

#अन्यथा // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
सार्वजनिक:
    ResourceTracer() {}

    ResourceTracer(स्थिरांक शून्य*) {}

    ResourceTracer(स्थिरांक शून्य*, स्थिरांक अक्षर*) {}

    ResourceTracer(स्थिरांक शून्य*, int32_t) {}

    ResourceTracer(स्थिरांक ResourceTracer&, स्थिरांक अक्षर*) {}

    ResourceTracer(स्थिरांक ResourceTracer&, int32_t) {}

    शून्य trace(स्थिरांक अक्षर*) स्थिरांक {}

    शून्य traceOpen() स्थिरांक {}

    शून्य maybeTrace(स्थिरांक अक्षर*) स्थिरांक {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(स्थिरांक अक्षर*, स्थिरांक अक्षर*, स्थिरांक अक्षर*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
