// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_ENABLE_TRACING

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
        f_साँचो(nullptr),
        fIndex(-1) {}

    ResourceTracer(अचर UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_साँचो(nullptr),
        fIndex(-1) {}

    ResourceTracer(अचर UResourceBundle* resB, अचर वर्ण* साँचो) :
        fResB(resB),
        fParent(nullptr),
        f_साँचो(साँचो),
        fIndex(-1) {}

    ResourceTracer(अचर UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_साँचो(nullptr),
        fIndex(index) {}

    ResourceTracer(अचर ResourceTracer& parent, अचर वर्ण* साँचो) :
        fResB(nullptr),
        fParent(&parent),
        f_साँचो(साँचो),
        fIndex(-1) {}

    ResourceTracer(अचर ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_साँचो(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    शून्य trace(अचर वर्ण* type) अचर;
    शून्य traceOpen() अचर;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    शून्य maybeTrace(अचर वर्ण* type) अचर {
        यदि (fResB || fParent) {
            trace(type);
        }
    }

गुप्त:
    अचर UResourceBundle* fResB;
    अचर ResourceTracer* fParent;
    अचर वर्ण* f_साँचो;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) अचर;

    CharString& getResPath(CharString& output, UErrorCode& status) अचर;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(अचर वर्ण* path, अचर वर्ण* type, अचर वर्ण* name);

गुप्त:
    स्थिर शून्य traceOpenDataFile(अचर वर्ण* path, अचर वर्ण* type, अचर वर्ण* name);
    स्थिर शून्य traceOpenResFile(अचर वर्ण* path, अचर वर्ण* name);
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

    ResourceTracer(अचर शून्य*) {}

    ResourceTracer(अचर शून्य*, अचर वर्ण*) {}

    ResourceTracer(अचर शून्य*, int32_t) {}

    ResourceTracer(अचर ResourceTracer&, अचर वर्ण*) {}

    ResourceTracer(अचर ResourceTracer&, int32_t) {}

    शून्य trace(अचर वर्ण*) अचर {}

    शून्य traceOpen() अचर {}

    शून्य maybeTrace(अचर वर्ण*) अचर {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(अचर वर्ण*, अचर वर्ण*, अचर वर्ण*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
