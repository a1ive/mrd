/** @file
	��UEFI�����°���iso�ļ�ֱ��������Ҳ֧��ixpe����

**/
#include "MyRamDisk.h"
///ȫ��˽������ָ��,���������ַ���
DIDO_DISK_PRIVATE_DATA *pridata;

///������ڣ���Ϊ��������ò�Ʋ�����main��Ϊ���
EFI_STATUS UefiMain(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_SYSTEM_TABLE     *SystemTable
		)
	{
        EFI_STATUS               		Status;

		EFI_FILE_HANDLE					BootFileHandleInRamDisk;
		EFI_DEVICE_PATH_PROTOCOL		*CurrDirDP;
		EFI_FILE_HANDLE					IsoFileHandle;
		EFI_FILE_HANDLE					CurrDirHandle;
		
		
		///��ȡ��ǰĿ¼
		CurrDirDP=GetCurrDirDP(gImageHandle,L"");
		if(NULL==CurrDirDP){
			Print(L"GetCurrDirDP Error\n");
			return EFI_SUCCESS;
			}
		///�򿪵�ǰĿ¼	
		CurrDirHandle=OpenFileByDevicePath(CurrDirDP);
		if(NULL==CurrDirHandle){
			Print(L"Open CurrDir Error\n");
			return EFI_SUCCESS;
			}
		///���������в���
		IsoFileHandle=OpenIsoFileInCmdLineStr(CurrDirHandle);
		if(NULL==IsoFileHandle){	
			///���������ļ�
			IsoFileHandle=ProcCfgFile(CurrDirDP,CurrDirHandle,L"isoboot.cfg");
			if(NULL==IsoFileHandle){
				///�ڵ�ǰĿ¼������һ��iso�ļ�����
				IsoFileHandle=OpenFirstIsoFileInDir(CurrDirHandle);
				if(NULL==IsoFileHandle){
					Print(L"Open ISO file Error\n");
					goto errordroptoshell;
					}
				}
			}		

		///���ڴ淽ʽ��װ������	
		Status=MyFileDiskInstall(IsoFileHandle,FALSE);
		if(EFI_ERROR (Status)) {
			Print(L"Install virtual disk failed!\n");
			goto errordroptoshell;
			}
		///���������ϵ������ļ�	
		Status=LoadBootFileInVirtualDisk(pridata[0].VirDiskDevicePath,&BootFileHandleInRamDisk);
		if(EFI_ERROR (Status)) {
			Print(L"Sorry!Can't boot this iso file.\n");
			goto errordroptoshell;
			}
		///�ȴ�10��
//		DidoWaitSec(10);
		///����
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Startimage failed!\n");
			goto errordroptoshell;
			}
errordroptoshell:			
		///ʧ������һ��shellx64.efi
		CurrDirDP=GetCurrDirDP(gImageHandle,L"shellx64.efi");
		if(NULL==CurrDirDP){
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}		
		Status=gBS->LoadImage(
			FALSE,
			gImageHandle,                   //parent����Ϊ�գ����뱾�ļ���Handle
			CurrDirDP,			//�ļ���devicepath
			NULL,
			0,
			&BootFileHandleInRamDisk				//����HANDLE��ַ	
			);				
	
		Status=gBS->StartImage(	BootFileHandleInRamDisk,0,	NULL);
		if(EFI_ERROR (Status)) {
			Print(L"Start shellx64.efi failed!\n");
			return EFI_SUCCESS;
			}			
/**/	return EFI_SUCCESS;
	}

