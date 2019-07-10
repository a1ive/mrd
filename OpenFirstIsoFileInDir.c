#include "MyRamDisk.h"
///���������������Ŀ¼�ĵ�һ��iso�ļ�������pxe	
EFI_FILE_HANDLE
	OpenFirstIsoFileInDir(
		IN	EFI_FILE_HANDLE			DirToSearch
	)
	{
		EFI_STATUS							Status;
		EFI_FILE_INFO 						*DidoFileInfoBuffer=NULL;
		BOOLEAN 							FoundIsoFile=FALSE;
		BOOLEAN 							DirHaveNoFile=FALSE;
		EFI_FILE_PROTOCOL					*DidoFileHandle=NULL;
		UINTN 								StringLenth;

		Print(L"Searching iso file in current dir\n");
		//�ҵ���Ŀ¼��һ��iso�ļ�
		Status=FileHandleFindFirstFile(DirToSearch,&DidoFileInfoBuffer);
		StringLenth=StrLen(DidoFileInfoBuffer->FileName);
		if(	StringLenth>=5&&
			(DidoFileInfoBuffer->FileName[StringLenth-4]==L'.'||DidoFileInfoBuffer->FileName[StringLenth-4]==L'.')&&
			(DidoFileInfoBuffer->FileName[StringLenth-3]==L'i'||DidoFileInfoBuffer->FileName[StringLenth-3]==L'I')&&
			(DidoFileInfoBuffer->FileName[StringLenth-2]==L's'||DidoFileInfoBuffer->FileName[StringLenth-2]==L'S')&&
			(DidoFileInfoBuffer->FileName[StringLenth-1]==L'o'||DidoFileInfoBuffer->FileName[StringLenth-1]==L'O')){
			FoundIsoFile=TRUE;
			}	
		while(FALSE==FoundIsoFile){
			Status=FileHandleFindNextFile (DirToSearch,DidoFileInfoBuffer,&DirHaveNoFile);
			if(DirHaveNoFile){
				break;
				}
			StringLenth=StrLen(DidoFileInfoBuffer->FileName);
			if(	StringLenth>=5&&
				(DidoFileInfoBuffer->FileName[StringLenth-4]==L'.'||DidoFileInfoBuffer->FileName[StringLenth-4]==L'.')&&
				(DidoFileInfoBuffer->FileName[StringLenth-3]==L'i'||DidoFileInfoBuffer->FileName[StringLenth-3]==L'I')&&
				(DidoFileInfoBuffer->FileName[StringLenth-2]==L's'||DidoFileInfoBuffer->FileName[StringLenth-2]==L'S')&&
				(DidoFileInfoBuffer->FileName[StringLenth-1]==L'o'||DidoFileInfoBuffer->FileName[StringLenth-1]==L'O')){
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
		Print(L"Can not find iso file.Error=[%r]\n",Status);
		return NULL;		
	}