/*
*************************************************************************************
*                         			eGon
*					        Application Of eGon2.0
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: BootOS.c
*
* Author 		:
*
* Description 	: ����ϵͳ����
*
*
*************************************************************************************
*/
#include  "include.h"
#include  "boot_img.h"
#include  "BootOS_i.h"
#include  "common.h"
#include  "setup.h"

#define    ANDROID_FASTBOOT_MODE		1
#define    ANDROID_RECOVERY_MODE		2

#define    ANDROID_BOOT_BASE            0x40007800

#define    SUNXI_FLASH_READ_FIRST_SIZE  (512 * 1024)

extern  boot1_private_head_t  boot1_priv_para;

extern  int boot_recovery_status;
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static __s32 check_key_to_recovery(void)
{
    __s32 ret1, ret2;
	__s32 key_high, key_low;
	__s32 keyvalue;

	if(boot_recovery_status == 1)
	{
		return ANDROID_RECOVERY_MODE;
	}
	keyvalue = boot1_priv_para.uart_port;
	__inf("key %d\n", keyvalue);

    ret1 = wBoot_script_parser_fetch("recovery_key", "key_max", &key_high, 1);
	ret2 = wBoot_script_parser_fetch("recovery_key", "key_min", &key_low, 1);
	if((ret1) || (ret2))
	{
		__inf("cant find rcvy value\n");
	}
	else
	{
		__inf("recovery key high %d, low %d\n", key_high, key_low);
		if((keyvalue >= key_low) && (keyvalue <= key_high))
		{
			__inf("key found, android recovery\n");
			return ANDROID_RECOVERY_MODE;
		}
	}
    ret1 = wBoot_script_parser_fetch("fastboot_key", "key_max", &key_high, 1);
	ret2 = wBoot_script_parser_fetch("fastboot_key", "key_min", &key_low, 1);
	if((ret1) || (ret2))
	{
		__inf("cant find fstbt value\n");
	}
	else
	{
		__inf("fastboot key high %d, low %d\n", key_high, key_low);
		if((keyvalue >= key_low) && (keyvalue <= key_high))
		{
			__inf("key found, android fastboot\n");
			return ANDROID_FASTBOOT_MODE;
		}
	}

	return -1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
__s32 boot_show_logo(char *logo_name, __u32 logo_show, __u32 logo_address)
{
    if((!logo_show ) || (!logo_name))
    {
        __inf("logo name is invalid or dont need show logo\n");

        return -1;
    }
    ShowPictureEx1(logo_name, logo_address);

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static __s32 boot_dsipatch_kernal(boot_sys_img_set_t  *os_img, __u32 *kernal_addr, __u32 *para_addr)
{
    H_FILE  hd_file = NULL;
    __s32   count;
    __s32   i, file_length = 0;
    //__u32   *address;

	__inf("load kernel start\n");
#if 0
    count = os_img->img_count;           //��ȡ����ĸ���
    for(i = 0; i < count; i++)
    {
        if(1 != i)
        {
        	address = (void *)os_img->img_set[i].img_base;
        }
        else
        {
        	address = wBoot_malloc(64 * 1024);
        }
        if(!address)
        {
        	__inf("img file %s base addres is NULL\n", (__u32)address);

        	return -1;
        }
        hd_file = wBoot_fopen(os_img->img_set[i].img_name, "rb");
        if(!hd_file)
        {
            __inf("open img file %s failed\n", os_img->img_set[i].img_name);

            goto dispath_error;
        }
        file_length = wBoot_flen(hd_file);
        if(file_length > os_img->img_set[i].img_size)
        {
            __inf("the img file %s length %d is larger than img max size %d\n", os_img->img_set[i].img_name, file_length, os_img->img_set[i].img_size);

            goto dispath_error;
        }
        wBoot_fread(address, 1, file_length, hd_file);
        wBoot_fclose(hd_file);

        os_img->img_set[i].img_full_size = file_length;
        if(1 == i)
        {
        	do_boot_linux((char *)os_img->img_set[1].img_base, (char *)address, file_length);
        	wBoot_free(address);
        }
        address = NULL;
        hd_file = NULL;
        file_length = 0;
    }
    address = (void *)os_img->script_info.img_base;
    if(!address)
    {
        __inf("no script could be filed\n");
    }
    else
    {
        memcpy(address, (void *)SCRIPT_BASE, os_img->script_info.img_size);
        os_img->script_info.img_full_size = os_img->script_info.img_size;
    }
    *kernal_addr = os_img->img_set[0].img_base;
    *para_addr 	 = os_img->img_set[1].img_base;
#else
	*kernal_addr = 0x4a000000;
	hd_file = wBoot_fopen("c:\\linux\\u-boot.bin", "rb");
	if(!hd_file)
	{
    	__inf("open img file c:\\linux\\u-boot.bin failed\n");

    	goto dispath_error;
	}
	file_length = wBoot_flen(hd_file);
	wBoot_fread((void *)(*kernal_addr), 1, file_length, hd_file);
	wBoot_fclose(hd_file);

#endif
    __inf("load kernel successed\n");

    return 0;

dispath_error:
    if(hd_file)
    {
        wBoot_fclose(hd_file);
    }
    __inf("load kernel failed\n");

    return -1;
}
/*
*******************************************************************************
*                       PreBootOS
*
* Description:
*    �����������ϵͳ�����׼��������
*
* Parameters:
*    os_para:  input.  Boot�׶εĲ�����
*    Type  :  input.  ����ϵͳ����
*
* Return value:
*    ���سɹ�����ʧ�ܡ�
*
* note:
*    ��
*
*******************************************************************************
*/
__s32 PreBootOS(char *os_para, __u32 *kernal_addr, __u32 *para_addr, __s32 *logo_status)
{
    //boot_sys_img_set_t  *os_img;
    __s32  ret = -1;

    //��ʼ�ű��������ű�������ʼ��
    //os_img = (boot_sys_img_set_t *)os_para;
    //�˴����ӽӿڣ���ʾ����logo
    //*logo_status = os_img->logo_off;
    //boot_show_logo(os_img->logo_name, os_img->logo_show, os_img->logo_address);
    //boot_show_logo("c:\\os_show\\bootlogo.bmp", os_img->logo_show, os_img->logo_address);
    //boot_show_logo("c:\\os_show\\bootlogo.bmp", os_img->logo_show, BOOT_LOGO_MEM_BASE);
    //boot_show_logo("c:\\os_show\\bootlogo.bmp", 1, BOOT_LOGO_MEM_BASE);
    //os_img->img_set = (char *)os_img + ((boot_sys_img_set_t *)0)->img_set;
    //������в���
    //���μ���OS�������ֵ��ڴ���
    //�ھ�������ϣ���ͬOSҲ����Ҫ�����������
    //�������е�OS����ͬ���Ĺ���ʼ���䵽�ڴ�
    //ret = boot_dsipatch_kernal(os_img, kernal_addr, para_addr);
    ret = boot_dsipatch_kernal(0, kernal_addr, para_addr);
    if(0 > ret)
    {
        goto PreBootOS_error;
    }
    //����ϵͳ����Ҫ�������Ѿ����ص��ڴ��У����ڸ��ݲ�ͬ��OS�������ԵĲ�������
    //�ҳ���Ҫ������OS
    __inf("start address = %x\n", *kernal_addr);

    return 0;

PreBootOS_error:

    return -1;
}
/*
*******************************************************************************
*                       BootOS_detect_os_type
*
* Description:
*
*
* Parameters:
*    Para  :
*
* Return value:
*    ��
*
* note:
*    ��
*
*******************************************************************************
*/
//��ʱ�����������û�����Ĳ�����������������һ��OS
__s32 BootOS_detect_os_type(__u32 *para_addr, __u32 *kernal_addr, void *os_info, int *logo_status)
{
    __s32  ret = -1;
    __s32  recovery = 0;
    //boot_global_info_t  *global_info = (boot_global_info_t *)os_info;
    //boot_sys_img_set_t   os_img_info;

    //��������ʾ�г����п��õ�OS���Ժ���display�滻
    //���ֻ��һ��OS����ֱ�ӽ����OS
    //memset(&os_img_info, 0, sizeof(boot_sys_img_set_t));
    //���� fast boot ����, ͨ����ϰ����ķ�ʽ
	recovery = check_key_to_recovery();
	//if(recovery > 0)
    {
        char   buffer[MBR_SIZE];
        char   data[2048];
        MBR    *mbr;
        int    start = 0, i;
        bootloader_msg  *bt_msg;

        wBoot_block_read(0, MBR_SIZE/512, buffer);
        mbr = (MBR *)buffer;
        for(i=0;i<mbr->PartCount;i++)
        {
            if(!strcmp("misc", (const char *)mbr->array[i].name))
            {
                start = mbr->array[i].addrlo;
                wBoot_block_read(start, 1, data);
                bt_msg = (bootloader_msg *)data;

				if(!strcmp(bt_msg->command, "efex"))
				{
					memset((char *)bt_msg->command, 0, 32);
					wBoot_block_write(start, 1, data);
					BoardExit_Display(1,1);

					wBoot_jump_fel();
				}
				if(recovery > 0)
				{
					memset((char *)bt_msg->command, 0, 32);
					if(recovery == ANDROID_FASTBOOT_MODE)
					{
						strcpy((char *)bt_msg->command, "bootloader");
						__inf("fastboot mode\n");
					}
					else if(recovery == ANDROID_RECOVERY_MODE)
					{
						strcpy((char *)bt_msg->command, "boot-recovery");
						__inf("recovery mode\n");
					}
					wBoot_block_write(start, 1, data);
				}

				break;
            }
        }
    }
//    ret = script_patch("c:\\linux\\linux.ini", &os_img_info, 1);
//    if(ret < 0)
//    {
//        __inf("NO OS to Boot\n");
//
//        goto _boot_pre_err;
//    }
	__inf("try to boot\n");
	//��ȡ��Ҫboot��ϵͳ�Ľű�
	//while((*(volatile unsigned int *)0x40000000) != 0x1234);
	ret = PreBootOS(0, kernal_addr, para_addr, logo_status);
	if(ret < 0)
	{
	    goto _boot_pre_err;
    }
    WaitForDeInitFinish();
	//���￪ʼ��ת��������OS���
_boot_pre_err:

    //wBoot_free(global_info);

	return ret;
}

/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
void BootOS(__u32 para_addr, __u32 kernal_addr)
{
    __inf("jump to\n");
    wBoot_jump_to_linux(0, 3892, para_addr, kernal_addr);

    {
        __u32 delay = 0x55;

        while(delay == 0x55);
    }
}
