#!/bin/bash
#!/Various encryption types
openssl enc -aes-128-cbc -e -in ./some.txt -out ./cipher_aes_cbc.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-cfb1 -e -in ./some.txt -out ./cipher_aes_cfb1.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-ofb -e -in ./some.txt -out ./cipher_aes_ofb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -bf-cbc -e -in ./some.txt -out ./cipher_bf_cbc.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -bf-ofb -e -in ./some.txt -out ./cipher_bf_ofb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -bf-ecb -e -in ./some.txt -out ./cipher_bf_ecb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -des-cbc -e -in ./some.txt -out ./cipher_des_cbc.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -des-cfb1 -e -in ./some.txt -out ./cipher_des_cfb1.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -des-ecb -e -in ./some.txt -out ./cipher_des_ecb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708

#!Encrypt picture
openssl enc -aes-128-cbc -e -in ./pic_original.bmp -out ./pic_cbc.bmp -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-ecb -e -in ./pic_original.bmp -out ./pic_ecb.bmp -K 00112233445566778889aabbccddeeff -iv 0102030405060708

#!Encrypt long text files
openssl enc -aes-128-ecb -e -in ./plain.txt -out ./cipher_long_ecb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-cbc -e -in ./plain.txt -out ./cipher_long_cbc.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-cfb -e -in ./plain.txt -out ./cipher_long_cfb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-ofb -e -in ./plain.txt -out ./cipher_long_ofb.bin -K 00112233445566778889aabbccddeeff -iv 0102030405060708

#!Decrypt corrupted files
openssl enc -aes-128-ecb -d -in ./cipher_long_ecb_corrupted.bin -out ./recovered_ecb.txt -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-cbc -d -in ./cipher_long_cbc_corrupted.bin -out ./recovered_cbc.txt -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-cfb -d -in ./cipher_long_cfb_corrupted.bin -out ./recovered_cfb.txt -K 00112233445566778889aabbccddeeff -iv 0102030405060708
openssl enc -aes-128-ofb -d -in ./cipher_long_ofb_corrupted.bin -out ./recovered_ofb.txt -K 00112233445566778889aabbccddeeff -iv 0102030405060708