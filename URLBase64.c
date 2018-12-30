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
#include <string.h>
#include <stdlib.h>
#include <errno.h>


static const char urlBase64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static int32_t charToNum[256] = { 0 };

static void initCharToNumMap(){
    int32_t len = strlen(urlBase64Chars);

    memset(charToNum, 0, sizeof(charToNum));

    for(size_t i = 0; i < sizeof(charToNum) / sizeof(charToNum[0]); i++)
        charToNum[i] = -1;

    for(int32_t i = 0; i < len; i++)
        charToNum[(size_t)urlBase64Chars[i]] = i;
}

size_t urlBase64MinBufSize(size_t urlBase64StrLen){
    return urlBase64StrLen * 6 / 8;
}

size_t urlBase64MinStrBufLen(size_t dataBufLen){
    return (dataBufLen * 8 + 5) / 6 + 1;
}

int urlBase64FromBuf(const void* inBuf,
                     size_t inBufLen,
                     char* base64Str,
                     size_t base64BufLen)
{
    size_t usedBytes = 0;
    int32_t num = 0;
    size_t usedBits = 0;
    size_t base64DigitIndex = 0;

    size_t requiredStrLen = urlBase64MinStrBufLen(inBufLen);

    if(base64BufLen < requiredStrLen)
        return ENOSPC;

    while(usedBytes < inBufLen){
        num <<= 8;
        num |= ((uint8_t*)inBuf)[usedBytes++];
        usedBits += 8;

        while(usedBits >= 6){
            uint8_t digit = (uint8_t)(num >> (usedBits - 6));
            usedBits -= 6;
            num &= ~(0xFFFFFFFF << usedBits);
            base64Str[base64DigitIndex++] = urlBase64Chars[digit];
        }
    }

    if(usedBits > 0){
        uint8_t digit = num << (6 - usedBits); //unused bits are 0 (used only to pad to a multiple of 8 bits)
        base64Str[base64DigitIndex++] = urlBase64Chars[digit];
    }

    base64Str[base64DigitIndex] = 0; //null-terminate string

    return 0;
}

static int isWhitespace(char c){
    return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}

int urlBase64Parse(const char* urlBase64,
                   ssize_t urlBase64StrLen,
                   uint8_t* buf,
                   size_t bufLen,
                   size_t* usedBytes)
{

    int32_t digit;
    size_t base64DigitCount = urlBase64StrLen >= 0
                                  ? urlBase64StrLen
                                  : strlen(urlBase64);

    size_t minBufLen = urlBase64MinBufSize(base64DigitCount);
    int num = 0;
    int usedBits = 0;
    size_t wroteBytes = 0;
    size_t i;

    if(charToNum[0] == 0)
        initCharToNumMap();


    if(bufLen < minBufLen)
        return ENOSPC;

    for(i = 0; i < base64DigitCount; i++){
        char digitChar = urlBase64[i];

        if(isWhitespace(digitChar))
            continue;

        digit = charToNum[(size_t)digitChar];
        if(digit < 0)
            return EINVAL;

        usedBits += 6;
        num <<= 6;
        num |= digit;

        if(usedBits >= 8){
            uint8_t byte = (uint8_t)(num >> (usedBits - 8));

            buf[wroteBytes] = byte;

            usedBits -= 8;
            wroteBytes++;
        }
    }

    if(usedBytes)
        *usedBytes = wroteBytes;

    return 0;
}
