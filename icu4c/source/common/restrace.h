// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#यदि_अपरिभाषितम् __RESTRACE_H__
#परिभाषय __RESTRACE_H__

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
        f_कुञ्चिका(nullptr),
        f_सूचकांक(-1) {}

    ResourceTracer(ध्रुव UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_कुञ्चिका(nullptr),
        f_सूचकांक(-1) {}

    ResourceTracer(ध्रुव UResourceBundle* resB, ध्रुव वर्ण* कुञ्चिका) :
        fResB(resB),
        fParent(nullptr),
        f_कुञ्चिका(कुञ्चिका),
        f_सूचकांक(-1) {}

    ResourceTracer(ध्रुव UResourceBundle* resB, int32_t सूचकांक) :
        fResB(resB),
        fParent(nullptr),
        f_कुञ्चिका(nullptr),
        f_सूचकांक(सूचकांक) {}

    ResourceTracer(ध्रुव ResourceTracer& parent, ध्रुव वर्ण* कुञ्चिका) :
        fResB(nullptr),
        fParent(&parent),
        f_कुञ्चिका(कुञ्चिका),
        f_सूचकांक(-1) {}

    ResourceTracer(ध्रुव ResourceTracer& parent, int32_t सूचकांक) :
        fResB(nullptr),
        fParent(&parent),
        f_कुञ्चिका(nullptr),
        f_सूचकांक(सूचकांक) {}

    ~ResourceTracer();

    शून्य trace(ध्रुव वर्ण* type) ध्रुव;
    शून्य traceOpen() ध्रुव;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    शून्य maybeTrace(ध्रुव वर्ण* type) ध्रुव {
        यदि (fResB || fParent) {
            trace(type);
        }
    }

गुप्त:
    ध्रुव UResourceBundle* fResB;
    ध्रुव ResourceTracer* fParent;
    ध्रुव वर्ण* f_कुञ्चिका;
    int32_t f_सूचकांक;

    CharString& get_File_मार्गः(CharString& output, UError_संहिता& स्थितिः) ध्रुव;

    CharString& get_Res_मार्गः(CharString& output, UError_संहिता& स्थितिः) ध्रुव;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* type, ध्रुव वर्ण* name);

गुप्त:
    स्थिर शून्य trace_Open_दत्त_File(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* type, ध्रुव वर्ण* name);
    स्थिर शून्य traceOpenResFile(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* name);
};

U_NAMESPACE_अन्तः

#अन्यथा // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
सार्वजनिक:
    ResourceTracer() {}

    ResourceTracer(ध्रुव शून्य*) {}

    ResourceTracer(ध्रुव शून्य*, ध्रुव वर्ण*) {}

    ResourceTracer(ध्रुव शून्य*, int32_t) {}

    ResourceTracer(ध्रुव ResourceTracer&, ध्रुव वर्ण*) {}

    ResourceTracer(ध्रुव ResourceTracer&, int32_t) {}

    शून्य trace(ध्रुव वर्ण*) ध्रुव {}

    शून्य traceOpen() ध्रुव {}

    शून्य maybeTrace(ध्रुव वर्ण*) ध्रुव {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
सार्वजनिक:
    स्थिर शून्य traceOpen(ध्रुव वर्ण*, ध्रुव वर्ण*, ध्रुव वर्ण*) {}
};

U_NAMESPACE_अन्तः

#यदि_अन्तः // U_ENABLE_TRACING

#यदि_अन्तः //__RESTRACE_H__
