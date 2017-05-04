// From: http://www.openssl.org/docs/crypto/EVP_EncryptInit.html
// A simple encryption function, using the EVP interface of OpenSSL
// Compile: gcc -o e0 encrypt0.c -lcrypto
// Run:     ./e0
// Decrypt: openssl bf -d -in cipher0.bin -K 000102030405060708090A0B0C0D0E0F -iv 0102030405060708

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// include the EVP headers
#include <openssl/evp.h>

//BytesToKey recommened
/*ou know that
aes-128-cbc
was used to generate the ciphertext from the plaintext. No salting was used during encryption. You also
know that the numbers in the initialisation vector (IV) are all zeros (not the ASCII character ‘0’). Another
clue is that the key, used to encrypt this plaintext, is an English word shorter than 16 characters; the word
that can be found from a typical English dictionary.*/

// see the above URL for this example function, with more explanation
// Encrypt a string using blowfish: 
int do_crypt(char *outfile)
        {
        unsigned char outbuf[1024];
        int outlen, tmplen;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        unsigned char iv[] = {1,2,3,4,5,6,7,8};
        char intext[] = "Some Crypto Text";
        EVP_CIPHER_CTX ctx;
        FILE *out;
        EVP_CIPHER_CTX_init(&ctx);
        EVP_EncryptInit_ex(&ctx, EVP_bf_cbc(), NULL, key, iv);
        if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, intext, strlen(intext)))
                {
                /* Error */
                return 0;
                }
        /* Buffer passed to EVP_EncryptFinal() must be after data just
         * encrypted to avoid overwriting it.
         */
        if(!EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen))
                {
                /* Error */
                return 0;
                }
        outlen += tmplen;
        EVP_CIPHER_CTX_cleanup(&ctx);
        /* Need binary mode for fopen because encrypted data is
         * binary data. Also cannot use strlen() on it because
         * it wont be null terminated and may contain embedded
         * nulls.
         */
        out = fopen(outfile, "wb");
        fwrite(outbuf, 1, outlen, out);
        fclose(out);
        return 1;
        }

// fairly minimal main function, driving the encryption
main () {
  // output file
  static char filename[80] = "cipher0.bin";
  // explain what's happening
  printf("Encrypting some hard-wired text to file %s\n", filename);
  // call the main encryption function
  do_crypt(filename);
}
