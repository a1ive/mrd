#ifndef		_MY_RAM_DISK_H_
#define 	_MY_RAM_DISK_H_

#include 	<Uefi.h>
#include 	<Library/UefiApplicationEntryPoint.h>
#include 	<Library/BaseLib.h>
#include	<Library/UefiBootServicesTableLib.h>
#include 	<Library/UefiLib.h>
#include 	<Library/DevicePathLib.h>
#include 	<Library/FileHandleLib.h>
#include 	<Library/MemoryAllocationLib.h>
#include	<Library/BaseMemoryLib.h>
#include 	<Protocol/SimpleFileSystem.h>
#include 	<Protocol/RamDisk.h>
#include 	<Protocol/DevicePathToText.h>
#include 	<Protocol/LoadedImage.h>
#include 	<Protocol/BlockIo.h>
#include 	<Protocol/BlockIo2.h>

#include 	<IndustryStandard/ElTorito.h>


//
// Block size for RAM disk
//
#define RAM_DISK_BLOCK_SIZE 2048        //�����޸�����Ӧ������� ԭ����512
#define CD_BOOT_SECTOR	17
#define IS_EFI_SYSTEM_PARTITION 239
#define DIDO_DISK_PRIVATE_DATA_BLOCKIO_TO_PARENT(a) ((DIDO_DISK_PRIVATE_DATA *)((CHAR8 *)(a)-(CHAR8 *) &(((DIDO_DISK_PRIVATE_DATA *) 0)->BlockIo)))
#define DIDO_DISK_PRIVATE_DATA_BLOCKIO2_TO_PARENT(a) ((DIDO_DISK_PRIVATE_DATA *)((CHAR8 *)(a)-(CHAR8 *) &(((DIDO_DISK_PRIVATE_DATA *) 0)->BlockIo2)))

// {E4F3CE96-064F-4DA5-8C71-33232B9D9F2A}
static const EFI_GUID MyGuid = 
{ 0xe4f3ce96, 0x64f, 0x4da5, { 0x8c, 0x71, 0x33, 0x23, 0x2b, 0x9d, 0x9f, 0x2a } };




typedef EFI_STATUS (*FUN)(   IN EFI_HANDLE ,  IN EFI_SYSTEM_TABLE*);



typedef struct {

	///�������̵���Ϣ
	BOOLEAN							Present;					//��ͬ
	EFI_HANDLE                      VirDiskHandle;				//��ͬ
	EFI_DEVICE_PATH_PROTOCOL        *VirDiskDevicePath;			//��ͬ
	EFI_FILE_HANDLE				  	VirDiskFileHandle;			//��ͬ
	BOOLEAN						  	InRam;						//��ͬ
	UINT64                          StartAddr;					//��ͬ
	UINT64                          Size;						//��ͬ
	
	//����������blockinit��䣬������install���
	EFI_BLOCK_IO_PROTOCOL           BlockIo;
	EFI_BLOCK_IO2_PROTOCOL          BlockIo2;
	EFI_BLOCK_IO_MEDIA              Media;
	


} DIDO_DISK_PRIVATE_DATA;

extern EFI_BLOCK_IO_PROTOCOL  mFileDiskBlockIoTemplate;
extern EFI_BLOCK_IO2_PROTOCOL  mFileDiskBlockIo2Template;
extern DIDO_DISK_PRIVATE_DATA *pridata;






///˵��FileBlockIo�ĺ���



EFI_STATUS
EFIAPI
FileDiskBlkIoReset (
  IN EFI_BLOCK_IO_PROTOCOL        *This,
  IN BOOLEAN                      ExtendedVerification
  );
  
EFI_STATUS
EFIAPI
FileDiskBlkIoReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL        *This,
  IN UINT32                       MediaId,
  IN EFI_LBA                      Lba,
  IN UINTN                        BufferSize,
  IN OUT VOID                     *Buffer
  );  

EFI_STATUS
EFIAPI
FileDiskBlkIoWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL        *This,
  IN UINT32                       MediaId,
  IN EFI_LBA                      Lba,
  IN UINTN                        BufferSize,
  IN VOID                         *Buffer
  );

EFI_STATUS
EFIAPI
FileDiskBlkIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL        *This
  );

EFI_STATUS
EFIAPI
FileDiskBlkIo2Reset (
  IN EFI_BLOCK_IO2_PROTOCOL       *This,
  IN BOOLEAN                      ExtendedVerification
  );

EFI_STATUS
EFIAPI
FileDiskBlkIo2ReadBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL   *This,
  IN     UINT32                   MediaId,
  IN     EFI_LBA                  Lba,
  IN OUT EFI_BLOCK_IO2_TOKEN      *Token,
  IN     UINTN                    BufferSize,
     OUT VOID                     *Buffer
  );

EFI_STATUS
EFIAPI
FileDiskBlkIo2WriteBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL   *This,
  IN     UINT32                   MediaId,
  IN     EFI_LBA                  Lba,
  IN OUT EFI_BLOCK_IO2_TOKEN      *Token,
  IN     UINTN                    BufferSize,
  IN     VOID                     *Buffer
  );

EFI_STATUS
EFIAPI
FileDiskBlkIo2FlushBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL   *This,
  IN OUT EFI_BLOCK_IO2_TOKEN      *Token
  );





///����������
EFI_STATUS
	MyFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	);
///ȡ�õ�ǰĿ¼
EFI_DEVICE_PATH_PROTOCOL* 
	GetCurrDirDP(
		IN	EFI_HANDLE 			FileHandle,
		IN 	const 	CHAR16		*FileName
		);

///���벢����ָ���������ļ������ش򿪵�iso�ļ������������������	
EFI_FILE_HANDLE
	OpenFileByDevicePath(
		IN 		EFI_DEVICE_PATH_PROTOCOL 			*CurrDevicePath			//��ǰ�豸���豸·��
		);
///����bootx64.efi׼������
EFI_STATUS
LoadBootFileInVirtualDisk(
	IN		EFI_DEVICE_PATH_PROTOCOL			*RegedRamDiskDevicePath,
	OUT		EFI_FILE_HANDLE						*BootMyFileHandle
	);
///�ȴ�ָ��ʱ����
EFI_STATUS
	DidoWaitSec(
		UINTN 		Sec
		);

///���������������Ŀ¼�ĵ�һ��iso�ļ�������pxe	
EFI_FILE_HANDLE
	OpenFirstIsoFileInDir(
		IN	EFI_FILE_HANDLE						DirToSearch
		);

///���������ļ�����iso�ļ����
EFI_FILE_HANDLE
	ProcCfgFile(
		EFI_DEVICE_PATH_PROTOCOL		*CurrDirDP,
		EFI_FILE_HANDLE					CurrDirHandle,
		IN 		 CHAR16					*ConfigFileName			//�����ļ���
		);
///�ҳ�������Ϣ�����ص���4��������
EFI_STATUS
	FindPartitionInFile(
		IN		EFI_FILE_HANDLE			FileDiskFileHandle,
		OUT		UINT64					*NoBootStartAddr,
		OUT		UINT64					*NoBootSize,
		OUT		UINT64					*BootStartAddr,
		OUT		UINT64					*BootSize
		);

///��������ָ����iso�ļ������ؾ��	
EFI_FILE_HANDLE
	OpenIsoFileInCmdLineStr(
//		IN	CHAR16*			CmdLine,
		EFI_FILE_HANDLE			CurrDirHandle
	);



#endif