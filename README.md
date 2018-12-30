# url-base-64
URL-base64 allows this base-64 variant to be used in URL path components/filenames, as well as query string values.

The Base-64 mapping is the same except '+' is replaced with '-', and '/' is replaced with '_'.

Trailing '=' characters are also omitted. It is assumed that the output buffer will be an integer number of bytes. Since each Base64 digit represents 6 bits, any dangling bits are set to 0 when encoding, and discarded when decoding. It is an error to have non-zero dangling bits.
