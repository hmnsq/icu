// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_ENABLE_TRACING

#அந்தர்பாவ "restrace.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "utracimp.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = இயல்பு;

வெறுமை ResourceTracer::trace(மாறிலி எழுத்து* resType) மாறிலி {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    மாறிலி எழுத்து kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, அளவு(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

வெறுமை ResourceTracer::traceOpen() மாறிலி {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) மாறிலி {
    எனில் (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } இல்லையெனில் {
        fParent->getFilePath(output, status);
    }
    திரும்பு output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) மாறிலி {
    எனில் (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } இல்லையெனில் {
        fParent->getResPath(output, status);
    }
    எனில் (f_சாவி) {
        output.append('/', status);
        output.append(f_சாவி, status);
    }
    எனில் (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_எண்(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    திரும்பு output;
}

வெறுமை FileTracer::traceOpen(மாறிலி எழுத்து* path, மாறிலி எழுத்து* type, மாறிலி எழுத்து* name) {
    எனில் (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } இல்லையெனில் {
        traceOpenDataFile(path, type, name);
    }
}

வெறுமை FileTracer::traceOpenDataFile(மாறிலி எழுத்து* path, மாறிலி எழுத்து* type, மாறிலி எழுத்து* name) {
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

வெறுமை FileTracer::traceOpenResFile(மாறிலி எழுத்து* path, மாறிலி எழுத்து* name) {
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
