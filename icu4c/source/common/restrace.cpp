// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا U_ENABLE_TRACING

#ضمن "restrace.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "utracimp.h"
#ضمن "uresimp.h"
#ضمن "uassert.h"
#ضمن "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = افتراضي;

فراغ ResourceTracer::trace(ثابت حرف* resType) ثابت {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ثابت حرف kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, حجم(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

فراغ ResourceTracer::traceOpen() ثابت {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ثابت {
    إذا (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } وإلا {
        fParent->getFilePath(output, status);
    }
    أرجع output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ثابت {
    إذا (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } وإلا {
        fParent->getResPath(output, status);
    }
    إذا (f_مبودي) {
        output.append('/', status);
        output.append(f_مبودي, status);
    }
    إذا (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_لمب(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    أرجع output;
}

فراغ FileTracer::traceOpen(ثابت حرف* path, ثابت حرف* type, ثابت حرف* name) {
    إذا (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } وإلا {
        traceOpenDataFile(path, type, name);
    }
}

فراغ FileTracer::traceOpenDataFile(ثابت حرف* path, ثابت حرف* type, ثابت حرف* name) {
    UTRACE_ENTRY(UTRACE_UDATA_DATA_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

فراغ FileTracer::traceOpenResFile(ثابت حرف* path, ثابت حرف* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING
