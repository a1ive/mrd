#include "MyRamDisk.h"
///���������������Ŀ¼�ĵ�һ��iso�ļ�������pxe	
EFI_FILE_HANDLE
	OpenFirstIsoFileInDir(
		IN	EFI_FILE_HANDLE			DirToSearch
	)
	{
		EFI_STATUS							Status;
		CHAR16 								*DidoFoundStr=NULL;
		EFI_FILE_INFO 						*DidoFileInfoBuffer=NULL;
		BOOLEAN 							FoundIsoFile=FALSE;
		BOOLEAN 							DirHaveNoFile=FALSE;
		EFI_FILE_PROTOCOL					*DidoFileHandle=NULL;


		Print(L"Searching iso file in current dir\n");
		//�ҵ���Ŀ¼��һ��iso�ļ�
		Status=FileHandleFindFirstFile(DirToSearch,&DidoFileInfoBuffer);
		//�򵥵��жϣ���.iso��������Ϊ��iso�ļ�	
		DidoFoundStr=StrStr(DidoFileInfoBuffer->FileName,L".iso");
		if(NULL!=DidoFoundStr){
			FoundIsoFile=TRUE;
			}	
		while(FALSE==FoundIsoFile){
			Status=FileHandleFindNextFile (DirToSearch,DidoFileInfoBuffer,&DirHaveNoFile);
			if(DirHaveNoFile){
				break;
				}
			//�򵥵��жϣ���.iso��������Ϊ��iso�ļ�	
			DidoFoundStr=StrStr(DidoFileInfoBuffer->FileName,L".iso");
			if(NULL!=DidoFoundStr){
				FoundIsoFile=TRUE;
				break;
				}	
			}
		if(TRUE==FoundIsoFile){
			Print(L"Found iso file:%s\n",DidoFileInfoBuffer->FileName);	
			Status=DirToSearch->Open(DirToSearch,&DidoFileHandle,DidoFileInfoBuffer->FileName,EFI_FILE_MODE_READ,EFI_FILE_READ_ONLY);
			if(EFI_SUCCESS==Status){
				return DidoFileHandle;
				}		
			}
		//����ҵ�iso�ļ����ͷ���ʱ�ڴ�
			
		if(NULL!=DidoFileInfoBuffer)gBS->FreePool(DidoFileInfoBuffer);
		//���û�ҵ�������һ���ļ�ϵͳ
			
			
		if(FALSE==FoundIsoFile){
			Print(L"Can not find iso file.Error=[%r]\n",Status);
			}			
		Print(L"Boot iso file failed.Error=[%r]\n",Status);
		return NULL;		
	}