

#include "MyRamDisk.h"

extern DIDO_OPTION_STATUS				*OptionStatus;


//���־�������
IMAGE_FILE_TYPE
	CheckImageType()		

		{
			MASTER_BOOT_RECORD     			*MasterBootRecord=NULL;
			UINTN							MasterBootRecordSize=FLOPPY_DISK_BLOCK_SIZE;	
			
			MasterBootRecord = AllocateZeroPool (MasterBootRecordSize);
			if (MasterBootRecord == NULL) {
				return FLOPPYFILE;
				}
			//�ж�MBR�Ƿ���Ч
			FileHandleSetPosition(pridata[0].VirDiskFileHandle,0); 	
			FileHandleRead(pridata[0].VirDiskFileHandle,&MasterBootRecordSize,MasterBootRecord);
			if(	MasterBootRecord->Signature!=MBR_SIGNATURE){
				FreePool(MasterBootRecord);
				return ISOFILE;
				}
			//�жϷ������ͣ������mbr����󷵻�
			if(MasterBootRecord->Partition[0].OSIndicator!=PMBR_GPT_PARTITION){
				FreePool(MasterBootRecord);
				return MBR;	
				}else{
					FreePool(MasterBootRecord);
					return GPT;
					}
		}


//�����ڴ��̵�Э�鰲װ
EFI_STATUS
	FileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle
	)
	{
		EFI_STATUS 							Status=EFI_SUCCESS;
		EFI_DEVICE_PATH_PROTOCOL 			*TempPath1;
		EFI_DEVICE_PATH_PROTOCOL			*VirDevicePath;
		UINTN								StartAddr;
		UINT64								Size;
		IMAGE_FILE_TYPE						RealImageType;
		

		
		pridata[0].VirDiskFileHandle=FileDiskFileHandle;		
		RealImageType=CheckImageType();		
		
		//�������С
		if(OptionStatus->ImageFileType==ISOFILE||RealImageType==ISOFILE){
			pridata[0].BlockSize=CD_BLOCK_SIZE;
			}else{
				pridata[0].BlockSize=FLOPPY_DISK_BLOCK_SIZE;
				}

		//δָ��-type����
		if(OptionStatus->ImageFileType==UNKNOWNTYPE||OptionStatus->ImageFileType==HARDDISKFILE){
			OptionStatus->ImageFileType=RealImageType;
			}		
				
		//��ȡ�ļ���С
		Status=FileHandleGetSize(FileDiskFileHandle,&Size);
		Print(L"ImageSize is %ld byte\n",Size);
		//�Ƿ���Ҫ�����ڴ�
		if(OptionStatus->LoadInMemory){				
			//�����ڴ�
			StartAddr = (UINTN)AllocatePool ((UINTN)Size+8); 
			if(NULL==(VOID*)StartAddr) {
				Print(L"Allocate Memory failed!\n");
				return EFI_NOT_FOUND;
				}
			Print(L"Creating ramdisk.Please wait.\n");
			//ȫ��iso�ļ�
			FileHandleSetPosition(FileDiskFileHandle,0);	
			Status=FileHandleRead(FileDiskFileHandle,(UINTN*)&Size,(VOID*)StartAddr);
			if(EFI_ERROR (Status)){
				Print(L"ReadFile failed.Error=[%r]\n",Status);
				Status=gBS->FreePool((VOID*)StartAddr);
				return EFI_NOT_FOUND;
				}
			//�������ڴ���StartAddr�����ļ���ʼƫ����
			}else{
				StartAddr=0;
				}
				

					
						
		//��װ����Ӳ�̾����DP
		TempPath1=CreateDeviceNode  ( HARDWARE_DEVICE_PATH ,HW_VENDOR_DP ,  sizeof(VENDOR_DEVICE_PATH));
		((VENDOR_DEVICE_PATH*)TempPath1)->Guid=MyGuid;
		VirDevicePath=AppendDevicePathNode(NULL,TempPath1);
		if(TempPath1!=NULL)FreePool(TempPath1);

		///��ʼ���̵�˽������
		pridata[0].Present=TRUE;
		pridata[0].VirDiskHandle=NULL;
		pridata[0].VirDiskDevicePath=VirDevicePath;	
//		pridata[0].VirDiskFileHandle=FileDiskFileHandle;
		pridata[0].InRam=OptionStatus->LoadInMemory;
		pridata[0].StartAddr= StartAddr;
		pridata[0].Size = Size;
		pridata[0].ImageType= OptionStatus->ImageFileType;
		pridata[0].AltDiskSign=OptionStatus->AltDiskSign;
		
		//blockio�ĳ�ʼ��
		CopyMem (&pridata[0].BlockIo, &mFileDiskBlockIoTemplate, sizeof (EFI_BLOCK_IO_PROTOCOL));
		CopyMem (&pridata[0].BlockIo2, &mFileDiskBlockIo2Template, sizeof (EFI_BLOCK_IO2_PROTOCOL));
		
		//Media�ĳ�ʼ��
		pridata[0].BlockIo.Media          		= &pridata[0].Media;
		pridata[0].BlockIo2.Media      	 		= &pridata[0].Media;
		pridata[0].Media.MediaId		  		= VIRTUAL_MEDIA_ID;	
		pridata[0].Media.RemovableMedia   		= FALSE;				
		pridata[0].Media.MediaPresent     		= TRUE;
		pridata[0].Media.LogicalPartition 		= FALSE;						
		pridata[0].Media.ReadOnly         		= TRUE;								
		pridata[0].Media.WriteCaching     		= FALSE;
		pridata[0].Media.IoAlign 		  		= 16;	//���������������ܱ�diskioʶ��,С��16��ntfs vdf,ntfs�����޷��ҵ�bootx64.efi
		pridata[0].Media.BlockSize        		= pridata[0].BlockSize;
		pridata[0].Media.LastBlock        		= DivU64x32 (pridata[0].Size + pridata[0].BlockSize - 1, pridata[0].BlockSize) - 1;

		//��װ�����Ŀ�Э��
		if(pridata[0].ImageType!=FLOPPYFILE){
			Status=PartitionInstall();
			if(EFI_ERROR (Status)) {
				Print(L"No bootable partition on the disk image!\n");
				}					
			}
		//�����̷ŵ�������������װ����Ϊvirtualbox�Ĺ̼��£����ɵķ�����ʱ���д���
		//��װ���̵Ŀ�Э��		
		Print(L"installing disk blockio protocol\n");
		Status=gBS->InstallMultipleProtocolInterfaces (
			&pridata[0].VirDiskHandle,
			
			&gEfiDevicePathProtocolGuid,
			pridata[0].VirDiskDevicePath,
			&gEfiBlockIoProtocolGuid,
			&pridata[0].BlockIo,
			&gEfiBlockIo2ProtocolGuid,
			&pridata[0].BlockIo2,

			NULL
			);		
		if(EFI_ERROR(Status)){
			Print(L"%r\n",Status);
			Exit(Status);
			}
		gBS->ConnectController (pridata[0].VirDiskHandle, NULL, NULL, TRUE);
		

		return EFI_SUCCESS;
	}