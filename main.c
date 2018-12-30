/*
 * Copyright 2018 8 Birds Video Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "URLBase64.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>


static const size_t initialReadBufferSize = 1 << 20;

/**
 * Reads the contents of f into a buffer and assigns it to *outBuf.
 * The file size is assigned to *outSize.
 */
static int readAll(FILE* inputFile, uint8_t** outBuf, size_t* outSize){
    uint8_t* binaryData = (uint8_t*)malloc(initialReadBufferSize);
    size_t bufSize = initialReadBufferSize;
    size_t offset = 0;
    ssize_t readBytes = -2;
    static const size_t READ_IN_INCREMENTS = 2048;
    int status = 0;

    if(!binaryData){
        fprintf(stderr, "Out of memory.\n");
        status = ENOMEM;
        goto done;
    }

    do{
        if(bufSize - offset < READ_IN_INCREMENTS){
            uint8_t* newBuf = (uint8_t*)realloc(binaryData, bufSize * 2);
            if(!newBuf){
                fprintf(stderr, "Out of memory.\n");
                status = ENOMEM;
                goto done;
            }

            binaryData = newBuf;
        }

        readBytes = fread(binaryData + offset, 1, READ_IN_INCREMENTS, inputFile);

        if(readBytes > 0)
            offset += readBytes;
    }while(readBytes > 0);

    *outBuf = binaryData;
    *outSize = offset;
    //status == 0 here

done:
    if(status != 0 && binaryData != NULL)
        free(binaryData);

    return status;
}

static int encodeBase64(const char* readFromFilename){
    uint8_t* binaryData = NULL;
    size_t inputSize = 0;
    size_t strBufLen = 0;
    char* urlBase64Str = NULL;
    FILE* readFromFile;
    int status;

    if(!readFromFilename)
        readFromFile = stdin;
    else
        readFromFile = fopen(readFromFilename, "rb");

    if(!readFromFile){
        status = errno;
        fprintf(stderr,
                "Error opening %s: %s.\n",
                readFromFilename,
                strerror(status));

        goto done;
    }

    status = readAll(readFromFile, &binaryData, &inputSize);
    if(status != 0)
        goto done;

    strBufLen = urlBase64MinStrBufLen(inputSize);
    urlBase64Str = (char*)malloc(strBufLen);
    if(!urlBase64Str){
        status = ENOMEM;
        goto done;
    }


    status = urlBase64FromBuf(binaryData,
                              inputSize,
                              urlBase64Str,
                              strBufLen);

    if(status != 0){
        fprintf(stderr,
                "Error converting buffer to url-base-64: %s.\n",
                strerror(status));

        goto done;
    }

    printf("%s\n", urlBase64Str);

done:
    if(binaryData)
        free(binaryData);

    if(urlBase64Str)
        free(urlBase64Str);

    return status;
}

int decodeBase64(const char* readFromFilename){
    FILE* base64File = NULL;
    uint8_t* base64Str = NULL;
    size_t base64Len = 0;
    int status = 0;
    size_t binaryBufLen = 0;
    uint8_t* binaryBuf = NULL;


    if(!readFromFilename)
        base64File = stdin;
    else
        base64File = fopen(readFromFilename, "rb");

    if(!base64File){
        fprintf(stderr, "Error opening %s.\n", readFromFilename);
        status = errno;
        goto done;
    }

    status = readAll(base64File, &base64Str, &base64Len);
    if(status != 0)
        goto done;

    binaryBufLen = urlBase64MinBufSize(base64Len);

    binaryBuf = (uint8_t*)malloc(binaryBufLen);
    if(!binaryBuf){
        status = ENOMEM;
        goto done;
    }

    status = urlBase64Parse((const char*)base64Str,
                            base64Len,
                            binaryBuf,
                            binaryBufLen,
                            NULL);

    if(status != 0){
        fprintf(stderr,
                "Error parsing url-base-64 string: %s.\n",
                strerror(status));

        return status;
    }

    fwrite(binaryBuf, binaryBufLen, 1, stdout);
    printf("\n");

done:
    if(base64Str)
        free(base64Str);

    if(binaryBuf)
        free(binaryBuf);

    return status;
}

int main(int argc, const char * argv[]) {
    int status;

    if(argc < 2){
        fprintf(stderr, "Usage: %s [--encode|--decode]\n", argv[0]);
        return EINVAL;
    }

    if(!strcmp(argv[1], "--encode")){
        const char* inputFilename = argc >= 3 ? argv[2] : NULL;

        status = encodeBase64(inputFilename);
    } else if(!strcmp(argv[1], "--decode")){
        const char* inputFilename = argc >= 3 ? argv[2] : NULL;

        status = decodeBase64(inputFilename);
    }
    else{
        fprintf(stderr, "Usage: %s [--encode|--decode]\n", argv[0]);
        status = EINVAL;
    }

    return status;
}
