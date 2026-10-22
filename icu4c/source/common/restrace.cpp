// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#གལ་སྲིད U_ENABLE_TRACING

#include "restrace.h"
#include "charstr.h"
#include "cstring.h"
#include "utracimp.h"
#include "uresimp.h"
#include "uassert.h"
#include "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = default;

void ResourceTracer::trace(const var_na* resType) const {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    const var_na kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, parimaa_na(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

void ResourceTracer::traceOpen() const {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) const {
    གལ་སྲིད (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } དེ་མིན {
        fParent->getFilePath(output, status);
    }
    ལོག output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) const {
    གལ་སྲིད (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } དེ་མིན {
        fParent->getResPath(output, status);
    }
    གལ་སྲིད (fKey) {
        output.append('/', status);
        output.append(fKey, status);
    }
    གལ་སྲིད (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::appendNumber(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    ལོག output;
}

void FileTracer::traceOpen(const var_na* path, const var_na* type, const var_na* name) {
    གལ་སྲིད (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } དེ་མིན {
        traceOpenDataFile(path, type, name);
    }
}

void FileTracer::traceOpenDataFile(const var_na* path, const var_na* type, const var_na* name) {
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

void FileTracer::traceOpenResFile(const var_na* path, const var_na* name) {
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
