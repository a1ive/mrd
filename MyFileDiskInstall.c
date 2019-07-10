//�����ڴ��̵�Э�鰲װ

#include "MyRamDisk.h"






EFI_STATUS
	MyFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	)
	{
		EFI_STATUS 						Status=EFI_SUCCESS;
		EFI_DEVICE_PATH_PROTOCOL 		*TempPath1;
		
		//��ʼ��ȫ��ָ��pridata
		pridata=AllocatePool(3*sizeof(DIDO_DISK_PRIVATE_DATA)+8);
		///��ʼ����ͬ����
		//��ȡ�ļ���С
		Status=FileHandleGetSize(FileDiskFileHandle,&pridata[0].Size);
		
		if(DiskInRam){				//�Ƿ���Ҫ�����ڴ�
			//�����ڴ�
			Status = gBS->AllocatePool (EfiBootServicesData,pridata[0].Size+8,(VOID**)&pridata[0].StartAddr); 
			if(EFI_ERROR (Status)) {
				Print(L"Allocate Memory failed!\n");
				return Status;
				}
			Print(L"Creating ramdisk.Please wait.\n");
			//ȫ��iso�ļ�
			FileHandleSetPosition(FileDiskFileHandle,0);	
			Status=FileHandleRead(FileDiskFileHandle,&pridata[0].Size,(VOID*)pridata[0].StartAddr);
			if(EFI_ERROR (Status)){
				Print(L"ReadFile failed.Error=[%r]\n",Status);
				Status=gBS->FreePool((VOID*)pridata[0].StartAddr);
				
				return Status;
				}
			}else{
				pridata[0].StartAddr=0;
				}

		

		
		//��ȡ������Ϣ����д��Present,StartAddr,Size
		Status=FindPartitionInFile(FileDiskFileHandle,&pridata[1].StartAddr,&pridata[1].Size,&pridata[2].StartAddr,&pridata[2].Size);
		if(EFI_ERROR (Status)) {
			Print(L"No bootable partition on the virtual disk!\n");
			return Status;
			}
				

		
		
		//��дDP
		TempPath1=CreateDeviceNode  ( HARDWARE_DEVICE_PATH ,HW_VENDOR_DP ,  sizeof(VENDOR_DEVICE_PATH));
		((VENDOR_DEVICE_PATH*)TempPath1)->Guid=MyGuid;
		pridata[0].VirDiskDevicePath=AppendDevicePathNode(NULL,TempPath1);
		FreePool(TempPath1);
		
		TempPath1=CreateDeviceNode  ( MEDIA_DEVICE_PATH ,MEDIA_CDROM_DP ,  sizeof(CDROM_DEVICE_PATH));		
		((CDROM_DEVICE_PATH*)TempPath1)->BootEntry=0;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionStart=pridata[1].StartAddr;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionSize=pridata[1].Size;
		pridata[1].VirDiskDevicePath=AppendDevicePathNode(pridata[0].VirDiskDevicePath,TempPath1);
		FreePool(TempPath1);
		
		TempPath1=CreateDeviceNode  ( MEDIA_DEVICE_PATH ,MEDIA_CDROM_DP ,  sizeof(CDROM_DEVICE_PATH));		
		((CDROM_DEVICE_PATH*)TempPath1)->BootEntry=1;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionStart=pridata[2].StartAddr;
		((CDROM_DEVICE_PATH*)TempPath1)->PartitionSize=pridata[2].Size;
		pridata[2].VirDiskDevicePath=AppendDevicePathNode(pridata[0].VirDiskDevicePath,TempPath1);
		FreePool(TempPath1);
		
		pridata[0].Media.LogicalPartition 		= FALSE;
		pridata[1].Media.LogicalPartition 		= TRUE;
		pridata[2].Media.LogicalPartition 		= TRUE;
		///��ʼ����ͬ����
		for(INTN	i=0;i<3;i++){
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
			//pridata[i].Media.LogicalPartition 		= TRUE;					//��ͬ	
			pridata[i].Media.ReadOnly         		= TRUE;								//�����޸�����Ӧ�������ԭ����FALSE
			pridata[i].Media.WriteCaching     		= FALSE;
			pridata[i].Media.IoAlign 		  		= 0x200;				//���������������ܱ�diskioʶ��
			pridata[i].Media.BlockSize        		= RAM_DISK_BLOCK_SIZE;
			pridata[i].Media.LastBlock        		= DivU64x32 (
														  pridata[i].Size + RAM_DISK_BLOCK_SIZE - 1,
														  RAM_DISK_BLOCK_SIZE
														  ) - 1;
			
			
			
/*			//��װЭ��		
			Print(L"installing protocol\n");
			Status = gBS->InstallMultipleProtocolInterfaces (
				&pridata[i].VirDiskHandle,
				&gEfiDevicePathProtocolGuid,
				pridata[i].VirDiskDevicePath,
				&gEfiBlockIoProtocolGuid,
				&pridata[i].BlockIo,
		  
				&gEfiBlockIo2ProtocolGuid,
				&pridata[i].BlockIo2,

				NULL
				);
*/			
			}
			
			//��װЭ��		
			Print(L"installing protocol\n");
			Status = gBS->InstallMultipleProtocolInterfaces (
				&pridata[0].VirDiskHandle,
				&gEfiDevicePathProtocolGuid,
				pridata[0].VirDiskDevicePath,
				&gEfiBlockIoProtocolGuid,
				&pridata[0].BlockIo,
		  
				&gEfiBlockIo2ProtocolGuid,
				&pridata[0].BlockIo2,

				NULL
				);

			//��װЭ��		
			Print(L"installing protocol\n");
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
		gBS->ConnectController (pridata[2].VirDiskHandle, NULL, NULL, TRUE);		  

		Print(L"Virtual disk handle is : %X\n",pridata[0].VirDiskHandle);
		
		
		return Status;
	}