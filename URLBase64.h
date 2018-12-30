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

#ifndef URLBase64_h
#define URLBase64_h

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/**
 * Returns 0 on success or an error from errno.h
 * String length is urlBase64MinStrBufLen(), including null-terminator.
 */
int urlBase64FromBuf(const void* inBuf,
                     size_t inBufLen,
                     char* base64Str,
                     size_t base64StrBufLen);

/**
 * Returns 0 on success. May return ENOSPC if *bufLenPtr is too small.
 * If urlBase64StrLen >= 0 and urlBase64 != NULL, strlen(urlBase64)
 * will be used.
 */
int urlBase64Parse(const char* urlBase64,
                   ssize_t urlBase64StrLen,
                   uint8_t* buf,
                   size_t bufLen,
                   size_t* usedBytes);

size_t urlBase64MinBufSize(size_t urlBase64StrLen);

///return value includes space for null-terminator
size_t urlBase64MinStrBufLen(size_t dataBufLen);

#endif /* URLBase64_h */
