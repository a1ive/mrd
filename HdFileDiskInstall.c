//�����ڴ��̵�Э�鰲װ

#include "MyRamDisk.h"






EFI_STATUS
	HdFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	)
	{
		UINTN	i;
		EFI_STATUS 							Status=EFI_SUCCESS;
		EFI_DEVICE_PATH_PROTOCOL 			*TempPath1;
//		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL		*PartitionSFSProtocol=NULL;				//���ڲ����Ƿ��Ѱ�װfatЭ��,û����
		//��ʼ��ȫ��ָ��pridata,����3��˽�����ݽṹ����һ�������������̣��ڶ������ڴ�ͳ��������������������efi��������
		pridata=AllocatePool(3*sizeof(DIDO_DISK_PRIVATE_DATA)+8);
		///��ʼ����ͬ����
		//��ȡ�ļ���С
		Status=FileHandleGetSize(FileDiskFileHandle,&pridata[0].Size);
		
		//��ȡ��һ��������Ϣ����д��Present,StartAddr,Size
		Status=FindPartitionInHdFile(FileDiskFileHandle,&pridata[1].StartAddr,&pridata[1].Size,
			&pridata[2].StartAddr,&pridata[2].Size,&pridata[2].UniqueMbrSignature);
		if(EFI_ERROR (Status)) {
			Print(L"No bootable partition on the disk image!\n");
			return EFI_NOT_FOUND;
			}		

		if(DiskInRam){				//�Ƿ���Ҫ�����ڴ�
			//�����ڴ�
			pridata[0].StartAddr = (UINTN)AllocatePool ((UINTN)pridata[0].Size+8); 
			if(NULL==(VOID*)pridata[0].StartAddr) {
				Print(L"Allocate Memory failed!\n");
				return EFI_NOT_FOUND;
				}
			Print(L"Creating ramdisk.Please wait.\n");
			//ȫ��iso�ļ�
			FileHandleSetPosition(FileDiskFileHandle,0);	
			Status=FileHandleRead(FileDiskFileHandle,(UINTN*)&pridata[0].Size,(VOID*)pridata[0].StartAddr);
			if(EFI_ERROR (Status)){
				Print(L"ReadFile failed.Error=[%r]\n",Status);
				Status=gBS->FreePool((VOID*)pridata[0].StartAddr);
				return EFI_NOT_FOUND;
				}
			//�������ڴ���StartAddr�����ļ�ƫ����
			}else{
				pridata[0].StartAddr=0;
				
				}

		pridata[1].StartAddr=pridata[1].StartAddr+pridata[0].StartAddr;
		pridata[2].StartAddr=pridata[2].StartAddr+pridata[0].StartAddr;		

		

				

		
		
		///��дDP
		//����Ӳ�̾����DP
		TempPath1=CreateDeviceNode  ( HARDWARE_DEVICE_PATH ,HW_VENDOR_DP ,  sizeof(VENDOR_DEVICE_PATH));
		((VENDOR_DEVICE_PATH*)TempPath1)->Guid=MyGuid;
		pridata[0].VirDiskDevicePath=AppendDevicePathNode(NULL,TempPath1);
		FreePool(TempPath1);
		
		//������DP
		TempPath1=CreateDeviceNode  ( MEDIA_DEVICE_PATH ,MEDIA_HARDDRIVE_DP ,  sizeof(HARDDRIVE_DEVICE_PATH));
		((HARDDRIVE_DEVICE_PATH*)TempPath1)->PartitionNumber	=1;
		((HARDDRIVE_DEVICE_PATH*)TempPath1)->PartitionStart		=pridata[2].StartAddr/FLOPPY_DISK_BLOCK_SIZE;
		((HARDDRIVE_DEVICE_PATH*)TempPath1)->PartitionSize		=pridata[2].Size/FLOPPY_DISK_BLOCK_SIZE+1;
		*(UINT32*)((HARDDRIVE_DEVICE_PATH*)TempPath1)->Signature=pridata[2].UniqueMbrSignature;
		((HARDDRIVE_DEVICE_PATH*)TempPath1)->MBRType			=1;
		((HARDDRIVE_DEVICE_PATH*)TempPath1)->SignatureType		=1;
		pridata[2].VirDiskDevicePath=AppendDevicePathNode(pridata[0].VirDiskDevicePath,TempPath1);
		FreePool(TempPath1);		

		///��ʼ����ͬ����
		
		for(i=0;i<3;i++){
			//pridata[i].Present
			pridata[i].VirDiskHandle=NULL;
			//pridata[i].VirDiskDevicePath	
			pridata[i].VirDiskFileHandle=FileDiskFileHandle;
			pridata[i].InRam=DiskInRam;
			//pridata[i].StartAddr
			//pridata[i].Size
			
			//blockio�ĳ�ʼ��
			CopyMem (&pridata[i].BlockIo, &mFileDiskBlockIoTemplate, sizeof (EFI_BLOCK_IO_PROTOCOL));
			CopyMem (&pridata[i].BlockIo2, &mFileDiskBlockIo2Template, sizeof (EFI_BLOCK_IO2_PROTOCOL));

			pridata[i].BlockIo.Media          		= &pridata[i].Media;
			pridata[i].BlockIo2.Media      	 		= &pridata[i].Media;
			pridata[i].Media.MediaId		  		= 0x1;	
			pridata[i].Media.RemovableMedia   		= FALSE;				
			pridata[i].Media.MediaPresent     		= TRUE;
			pridata[i].Media.LogicalPartition 		= TRUE;					//��ͬ	
			pridata[i].Media.ReadOnly         		= TRUE;								
			pridata[i].Media.WriteCaching     		= FALSE;
			pridata[i].Media.IoAlign 		  		= 0;				//���������������ܱ�diskioʶ��
			pridata[i].Media.BlockSize        		= FLOPPY_DISK_BLOCK_SIZE;
			pridata[i].Media.LastBlock        		= DivU64x32 (
														  pridata[i].Size + FLOPPY_DISK_BLOCK_SIZE - 1,
														  FLOPPY_DISK_BLOCK_SIZE
														  ) - 1;
			

			}
		pridata[0].Media.LogicalPartition 		= FALSE;

				


		//��װ�����Ŀ�Э��		
		Print(L"installing partition blockio protocol\n");
		Status = gBS->InstallMultipleProtocolInterfaces (
			&pridata[2].VirDiskHandle,
			&gEfiDevicePathProtocolGuid,
			pridata[2].VirDiskDevicePath,
			&gEfiBlockIoProtocolGuid,
			&pridata[2].BlockIo,
	  
			&gEfiBlockIo2ProtocolGuid,
			&pridata[2].BlockIo2,

			NULL
			);
		if(EFI_ERROR(Status)){
			Print(L"%r\n",Status);
			Exit(Status);
			}
		
		Print(L"Virtual partition handle is : %X\n",pridata[2].VirDiskHandle);	
		gBS->ConnectController (pridata[2].VirDiskHandle, NULL, NULL, TRUE);	
		

		//�����̷ŵ�������������װ����Ϊvirtualbox�Ĺ̼��£����ɵķ�����ʱ���д���
		//��װ���̵Ŀ�Э��		
		Print(L"installing disk blockio protocol\n");
		gBS->InstallMultipleProtocolInterfaces (
			&pridata[0].VirDiskHandle,
			&gEfiDevicePathProtocolGuid,
			pridata[0].VirDiskDevicePath,
			&gEfiBlockIoProtocolGuid,
			&pridata[0].BlockIo,
	  
			&gEfiBlockIo2ProtocolGuid,
			&pridata[0].BlockIo2,

			NULL
			);		
		Print(L"Virtual disk handle is : %X\n",pridata[0].VirDiskHandle);
		gBS->ConnectController (pridata[0].VirDiskHandle, NULL, NULL, TRUE);



			
			
		return EFI_SUCCESS;
	}