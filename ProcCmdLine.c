#include "MyRamDisk.h"
///���������У����������OptionStatus
EFI_STATUS
	ProcCmdLine(
		DIDO_OPTION_STATUS					*OptionStatus
		)
		{
			EFI_STATUS							Status;
			EFI_LOADED_IMAGE_PROTOCOL			*ThisFileLIP;

			//���Լ���ӳ��DP��Ϣ
			Print(L"Process cmdline\n");		
			Status=gBS->HandleProtocol(gImageHandle,&gEfiLoadedImageProtocolGuid,(VOID**)&ThisFileLIP);
			if(EFI_ERROR (Status)){
				Print(L"LoadedImageProtocol not found.Error=[%r]\n",Status);
				return EFI_NOT_FOUND;
				}
			//�ַ������Ƶ�OptionStatus	
			OptionStatus->OptionStringSizeInByte=ThisFileLIP->LoadOptionsSize;
			OptionStatus->OptionString=AllocateCopyPool(ThisFileLIP->LoadOptionsSize,ThisFileLIP->LoadOptions);
			//���ַ����ֽ⵽����Ա
			DispatchOptions(OptionStatus);
			FreePool(OptionStatus->OptionString);
			if(OptionStatus->ImageFileName==NULL){
				Print(L"Cmdline didn't have a file name\n");
				return EFI_NOT_FOUND;
				}		
			Print(L"Image file name is:%s\n",OptionStatus->ImageFileName);
			return EFI_SUCCESS;	
		}