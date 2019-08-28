/******************************************************************************
Copyright (C), 2011-2021, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : sample_rng.c
Version       : Initial Draft
Author        : Hisilicon
Created       : 2012/07/10
Last Modified :
Description   : sample for cipher
Function List :
History       :
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_types.h"
#include "hi_unf_cipher.h"
#include "hi_common.h"
#include "hi_mmz_api.h"

#define HI_ERR_CIPHER(format, arg...)     printf( "\033[0;1;31m" format "\033[0m", ## arg)
#define HI_INFO_CIPHER(format, arg...)    printf( "\033[0;1;32m" format "\033[0m", ## arg)
#define TEST_END_PASS()                   HI_INFO_CIPHER("****************** %s test PASS !!! ******************\n", __FUNCTION__)
#define TEST_END_FAIL()                   HI_ERR_CIPHER("****************** %s test FAIL !!! ******************\n", __FUNCTION__)
#define TEST_RESULT_PRINT()               { if (ret) TEST_END_FAIL(); else TEST_END_PASS();}

typedef HI_S32 (*list_func)(HI_VOID);

/*
static HI_U8 aes_128_cbc_IV[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
static HI_U8 aes_128_cbc_key[16]= {0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
static HI_U8 aes_128_src_buf[16] = {0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static HI_U8 aes_128_dst_buf[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
*/

static HI_S32 printBuffer(HI_CHAR *string, HI_U8 *pu8Input, HI_U32 u32Length)
{
    HI_U32 i = 0;

    if ( NULL != string )
    {
        printf("%s\n", string);
    }

    for ( i = 0 ; i < u32Length; i++ )
    {
        if( (i % 16 == 0) && (i != 0)) printf("\n");
        printf("0x%02x ", pu8Input[i]);
    }
    printf("\n");

    return HI_SUCCESS;
}


static HI_S32 Setconfiginfo(HI_HANDLE chnHandle, HI_BOOL bKeyByCA, HI_UNF_CIPHER_ALG_E alg, HI_UNF_CIPHER_WORK_MODE_E mode, HI_UNF_CIPHER_KEY_LENGTH_E keyLen,
                                                const HI_U8 u8KeyBuf[16], const HI_U8 u8IVBuf[16])
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_UNF_CIPHER_CTRL_S CipherCtrl;

    memset(&CipherCtrl, 0, sizeof(HI_UNF_CIPHER_CTRL_S));
    CipherCtrl.enAlg = alg;
    CipherCtrl.enWorkMode = mode;
    CipherCtrl.enBitWidth = HI_UNF_CIPHER_BIT_WIDTH_128BIT;
    CipherCtrl.enKeyLen = keyLen;
    CipherCtrl.bKeyByCA = bKeyByCA;
    if(CipherCtrl.enWorkMode != HI_UNF_CIPHER_WORK_MODE_ECB)
    {
        CipherCtrl.stChangeFlags.bit1IV = 1;  //must set for CBC , CFB mode
        memcpy(CipherCtrl.u32IV, u8IVBuf, 16);
    }

    memcpy(CipherCtrl.u32Key, u8KeyBuf, 16);

    s32Ret = HI_UNF_CIPHER_ConfigHandle(chnHandle, &CipherCtrl);
    if(HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* encrypt data using special chn*/
static HI_S32 CBC_AES128(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32TestDataLen = 16;
    HI_U8 *pInputAddrVir = HI_NULL;
    HI_U8 *pOutputAddrVir = HI_NULL;
    HI_HANDLE hTestchnid = 0;
    HI_UNF_CIPHER_ATTS_S stCipherAttr;

/*
    HI_U8 aes_src[16] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00};
    HI_U8 aes_dst[16]  = {0xc1,0x1b,0x54,0x4a,0x12,0x9c,0x08,0xa5,0xcc,0xd3,0xeb,0xec,0x7a,0x3b,0x00,0x2b};
    HI_U8 aes_key[16] = {0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04};
    HI_U8 aes_IV[16]  = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
*/

    HI_U8 aes_key[16] = {0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C};
    HI_U8 aes_IV[16]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    HI_U8 aes_src[16] = {0x6B,0xC1,0xBE,0xE2,0x2E,0x40,0x9F,0x96,0xE9,0x3D,0x7E,0x11,0x73,0x93,0x17,0x2A};
    HI_U8 aes_dst[16] = {0x76,0x49,0xAB,0xAC,0x81,0x19,0xB2,0x46,0xCE,0xE9,0x8E,0x9B,0x12,0xE9,0x19,0x7D};

    s32Ret = HI_UNF_CIPHER_Init();
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stCipherAttr.enCipherType = HI_UNF_CIPHER_TYPE_NORMAL;
    s32Ret = HI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(HI_SUCCESS != s32Ret)
    {
        HI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    printf("hTestchnid 0x%d\n", hTestchnid);

    pInputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pInputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pInputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pInputAddrVir, 0, u32TestDataLen);

    pOutputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pOutputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pOutputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pOutputAddrVir, 0, u32TestDataLen);


    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            HI_FALSE,
                            HI_UNF_CIPHER_ALG_AES,
                            HI_UNF_CIPHER_WORK_MODE_CBC,
                            HI_UNF_CIPHER_KEY_AES_128BIT,
                            aes_key,
                            aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = HI_UNF_CIPHER_EncryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-ENC:", pOutputAddrVir, sizeof(aes_dst));

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        HI_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    HI_FALSE,
                                    HI_UNF_CIPHER_ALG_AES,
                                    HI_UNF_CIPHER_WORK_MODE_CBC,
                                    HI_UNF_CIPHER_KEY_AES_128BIT,
                                    aes_key,
                                    aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = HI_UNF_CIPHER_DecryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-DEC:", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        HI_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    if (HI_NULL != pInputAddrVir)
    {
        free(pInputAddrVir);
        pInputAddrVir = HI_NULL;
    }
    if (HI_NULL != pOutputAddrVir)
    {
        free(pOutputAddrVir);
        pOutputAddrVir = HI_NULL;
    }

    HI_UNF_CIPHER_DestroyHandle(hTestchnid);
    HI_UNF_CIPHER_DeInit();

    return s32Ret;
}

static HI_S32 CFB_AES128(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32TestDataLen = 32;
    HI_U8 *pInputAddrVir = HI_NULL;
    HI_U8 *pOutputAddrVir = HI_NULL;
    HI_HANDLE hTestchnid = 0;
    HI_UNF_CIPHER_ATTS_S stCipherAttr;
    HI_U8 aes_key[16] = {"\x2B\x7E\x15\x16\x28\xAE\xD2\xA6\xAB\xF7\x15\x88\x09\xCF\x4F\x3C"};
    HI_U8 aes_IV[16]  = {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"};
    HI_U8 aes_src[32] = {"\x6B\xC1\xBE\xE2\x2E\x40\x9F\x96\xE9\x3D\x7E\x11\x73\x93\x17\x2A\xAE\x2D\x8A\x57\x1E\x03\xAC\x9C\x9E\xB7\x6F\xAC\x45\xAF\x8E\x51"};
    HI_U8 aes_dst[32] = {"\x3B\x3F\xD9\x2E\xB7\x2D\xAD\x20\x33\x34\x49\xF8\xE8\x3C\xFB\x4A\xC8\xA6\x45\x37\xA0\xB3\xA9\x3F\xCD\xE3\xCD\xAD\x9F\x1C\xE5\x8B"};

    s32Ret = HI_UNF_CIPHER_Init();
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    stCipherAttr.enCipherType = HI_UNF_CIPHER_TYPE_NORMAL;
    s32Ret = HI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(HI_SUCCESS != s32Ret)
    {
        HI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    pInputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pInputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pInputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pInputAddrVir, 0, u32TestDataLen);

    pOutputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pOutputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pOutputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pOutputAddrVir, 0, u32TestDataLen);

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            HI_FALSE,
                            HI_UNF_CIPHER_ALG_AES,
                            HI_UNF_CIPHER_WORK_MODE_CFB,
                            HI_UNF_CIPHER_KEY_AES_128BIT,
                            aes_key,
                            aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CFB-AES-128-ORI:", aes_src, u32TestDataLen);

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = HI_UNF_CIPHER_EncryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CFB-AES-128-ENC:", pOutputAddrVir, u32TestDataLen);

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    HI_FALSE,
                                    HI_UNF_CIPHER_ALG_AES,
                                    HI_UNF_CIPHER_WORK_MODE_CFB,
                                    HI_UNF_CIPHER_KEY_AES_128BIT,
                                    aes_key,
                                    aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = HI_UNF_CIPHER_DecryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CFB-AES-128-DEC", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    if (HI_NULL != pInputAddrVir)
    {
        free(pInputAddrVir);
        pInputAddrVir = HI_NULL;
    }
    if (HI_NULL != pOutputAddrVir)
    {
        free(pOutputAddrVir);
        pOutputAddrVir = HI_NULL;
    }

    HI_UNF_CIPHER_DestroyHandle(hTestchnid);
    HI_UNF_CIPHER_DeInit();

    return s32Ret;
}

static HI_S32 CFB_AES128_COPY_AVOID(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32TestDataLen = 32;
    HI_U8 *pInputAddrVir = HI_NULL;
    HI_U8 *pOutputAddrVir = HI_NULL;
    HI_HANDLE hTestchnid = 0;
    HI_UNF_CIPHER_ATTS_S stCipherAttr;
    HI_U8 aes_key[16] = {"\x2B\x7E\x15\x16\x28\xAE\xD2\xA6\xAB\xF7\x15\x88\x09\xCF\x4F\x3C"};
    HI_U8 aes_IV[16]  = {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"};
    HI_U8 aes_src[32] = {"\x6B\xC1\xBE\xE2\x2E\x40\x9F\x96\xE9\x3D\x7E\x11\x73\x93\x17\x2A\xAE\x2D\x8A\x57\x1E\x03\xAC\x9C\x9E\xB7\x6F\xAC\x45\xAF\x8E\x51"};
    HI_U8 aes_dst[32] = {"\x3B\x3F\xD9\x2E\xB7\x2D\xAD\x20\x33\x34\x49\xF8\xE8\x3C\xFB\x4A\xC8\xA6\x45\x37\xA0\xB3\xA9\x3F\xCD\xE3\xCD\xAD\x9F\x1C\xE5\x8B"};

    s32Ret = HI_UNF_CIPHER_Init();
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    stCipherAttr.enCipherType = HI_UNF_CIPHER_TYPE_COPY_AVOID;
    s32Ret = HI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(HI_SUCCESS != s32Ret)
    {
        HI_UNF_CIPHER_DeInit();
        return s32Ret;
    }
    pInputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pInputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pInputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pInputAddrVir, 0, u32TestDataLen);

    pOutputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pOutputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pOutputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pOutputAddrVir, 0, u32TestDataLen);

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            HI_FALSE,
                            HI_UNF_CIPHER_ALG_AES,
                            HI_UNF_CIPHER_WORK_MODE_CFB,
                            HI_UNF_CIPHER_KEY_AES_128BIT,
                            aes_key,
                            aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CFB-AES128-COPY-AVOID-ORI:", aes_src, u32TestDataLen);

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = HI_UNF_CIPHER_EncryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher encrypt failed.\n");
        goto __CIPHER_EXIT__;
    }

    printBuffer("CFB-AES128-COPY-AVOID-ENC:", pOutputAddrVir, u32TestDataLen);

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    HI_FALSE,
                                    HI_UNF_CIPHER_ALG_AES,
                                    HI_UNF_CIPHER_WORK_MODE_CFB,
                                    HI_UNF_CIPHER_KEY_AES_128BIT,
                                    aes_key,
                                    aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = HI_UNF_CIPHER_DecryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher decrypt failed.\n");
        goto __CIPHER_EXIT__;
    }

    printBuffer("CFB-AES128-COPY-AVOID-DEC", pOutputAddrVir, u32TestDataLen);

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    if (HI_NULL != pInputAddrVir)
    {
        free(pInputAddrVir);
        pInputAddrVir = HI_NULL;
    }
    if (HI_NULL != pOutputAddrVir)
    {
        free(pOutputAddrVir);
        pOutputAddrVir = HI_NULL;
    }

    HI_UNF_CIPHER_DestroyHandle(hTestchnid);
    HI_UNF_CIPHER_DeInit();

    return s32Ret;
}

static HI_S32 CTR_AES128(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32TestDataLen = 32;
    HI_U8 *pInputAddrVir = HI_NULL;
    HI_U8 *pOutputAddrVir = HI_NULL;
    HI_HANDLE hTestchnid = 0;
    HI_UNF_CIPHER_ATTS_S stCipherAttr;
    HI_U8 aes_key[16] = {"\x7E\x24\x06\x78\x17\xFA\xE0\xD7\x43\xD6\xCE\x1F\x32\x53\x91\x63"};
    HI_U8 aes_IV[16]  = {"\x00\x6C\xB6\xDB\xC0\x54\x3B\x59\xDA\x48\xD9\x0B\x00\x00\x00\x01"};
    HI_U8 aes_src[32] = {"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"};
    HI_U8 aes_dst[32] = {"\x51\x04\xA1\x06\x16\x8A\x72\xD9\x79\x0D\x41\xEE\x8E\xDA\xD3\x88\xEB\x2E\x1E\xFC\x46\xDA\x57\xC8\xFC\xE6\x30\xDF\x91\x41\xBE\x28"};

    s32Ret = HI_UNF_CIPHER_Init();
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    stCipherAttr.enCipherType = HI_UNF_CIPHER_TYPE_NORMAL;
    s32Ret = HI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(HI_SUCCESS != s32Ret)
    {
        HI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    pInputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pInputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pInputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pInputAddrVir, 0, u32TestDataLen);

    pOutputAddrVir = (HI_U8*)malloc(u32TestDataLen);
    if (pOutputAddrVir == NULL)
    {
        HI_ERR_CIPHER("malloc pOutputAddrVir failed\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }
    memset(pOutputAddrVir, 0, u32TestDataLen);

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            HI_FALSE,
                            HI_UNF_CIPHER_ALG_AES,
                            HI_UNF_CIPHER_WORK_MODE_CTR,
                            HI_UNF_CIPHER_KEY_AES_128BIT,
                            aes_key,
                            aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CTR-AES-128-ORI:", aes_src, u32TestDataLen);

    s32Ret = HI_UNF_CIPHER_EncryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CTR-AES-128-ENC:", pOutputAddrVir, u32TestDataLen);

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    HI_FALSE,
                                    HI_UNF_CIPHER_ALG_AES,
                                    HI_UNF_CIPHER_WORK_MODE_CTR,
                                    HI_UNF_CIPHER_KEY_AES_128BIT,
                                    aes_key,
                                    aes_IV);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = HI_UNF_CIPHER_DecryptVir(hTestchnid, pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    if(HI_SUCCESS != s32Ret)
    {
        HI_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CTR-AES-128-DEC", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        HI_ERR_CIPHER("Memcmp failed!\n");
        s32Ret = HI_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    if (HI_NULL != pInputAddrVir)
    {
        free(pInputAddrVir);
        pInputAddrVir = HI_NULL;
    }
    if (HI_NULL != pOutputAddrVir)
    {
        free(pOutputAddrVir);
        pOutputAddrVir = HI_NULL;
    }

    HI_UNF_CIPHER_DestroyHandle(hTestchnid);
    HI_UNF_CIPHER_DeInit();

    return s32Ret;
}

int sample_cipher_via(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = CBC_AES128();
    if (s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }

    s32Ret = CFB_AES128();
    if (s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }

    s32Ret = CFB_AES128_COPY_AVOID();
    if (s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }

    s32Ret = CTR_AES128();
    if (s32Ret != HI_SUCCESS)
    {
        return s32Ret;
    }

    //CCM_AES128();
    //CCM_AES128_2();
    //GCM_AES128();

    return HI_SUCCESS;
}

