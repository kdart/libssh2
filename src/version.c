/* Copyright (C) 2009 Daniel Stenberg.  All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   Redistributions of source code must retain the above
 *   copyright notice, this list of conditions and the
 *   following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names
 *   of any other contributors may be used to endorse or
 *   promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include "libssh2_priv.h"

/*
  libssh2_version() can be used like this:

  if (!libssh2_version(LIBSSH2_VERSION_NUM)) {
    fprintf (stderr, "Runtime libssh2 version too old!\n");
    exit(1);
  }
*/
LIBSSH2_API
const char *libssh2_version(int req_version_num)
{
    if(req_version_num <= LIBSSH2_VERSION_NUM)
        return LIBSSH2_VERSION;
    return NULL; /* this is not a suitable library! */
}

LIBSSH2_API
libssh2_crypto_engine_t libssh2_crypto_engine(void)
{
#if defined LIBSSH2_OPENSSL
    return libssh2_openssl;
#elif defined LIBSSH2_LIBGCRYPT
    return libssh2_gcrypt;
#elif defined LIBSSH2_MBEDTLS
    return libssh2_mbedtls;
#elif defined LIBSSH2_WINCNG
    return libssh2_wincng;
#else
    return libssh2_no_crypto;
#endif
}
