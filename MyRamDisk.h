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
#include 	<Protocol/DiskIo.h>
#include 	<IndustryStandard/ElTorito.h>
#include 	<IndustryStandard/Mbr.h>

//
// Block size for RAM disk
//
#define RAM_DISK_BLOCK_SIZE 2048        //�����޸�����Ӧ������� ԭ����512
#define FLOPPY_DISK_BLOCK_SIZE 512
#define BLOCK_OF_1_44MB	0xB40
#define CD_BOOT_SECTOR	17
#define IS_EFI_SYSTEM_PARTITION 239
#define DIDO_DISK_PRIVATE_DATA_BLOCKIO_TO_PARENT(a) ((DIDO_DISK_PRIVATE_DATA *)((CHAR8 *)(a)-(CHAR8 *) &(((DIDO_DISK_PRIVATE_DATA *) 0)->BlockIo)))
#define DIDO_DISK_PRIVATE_DATA_BLOCKIO2_TO_PARENT(a) ((DIDO_DISK_PRIVATE_DATA *)((CHAR8 *)(a)-(CHAR8 *) &(((DIDO_DISK_PRIVATE_DATA *) 0)->BlockIo2)))
#define MAX_FILE_NAME_STRING_SIZE 255
#define MBR_START_LBA 0
// {CF03E624-DD29-426D-86A8-CB21E97E4C9B}
static const EFI_GUID MyGuid = 
{ 0xcf03e624, 0xdd29, 0x426d, { 0x86, 0xa8, 0xcb, 0x21, 0xe9, 0x7e, 0x4c, 0x9b } };

typedef enum {
	///ָ�������ļ�����
	ISOFILE,
	HARDDISKFILE,
	FLOPPYFILE
	}IMAGE_FILE_TYPE;


typedef struct {
	///�����в����Ľṹ
	CHAR16								*OptionString;
	UINTN								OptionStringSizeInByte;	
	BOOLEAN								LoadInMemory;
	BOOLEAN								DebugDropToShell;
	IMAGE_FILE_TYPE						ImageFileType;
	CHAR16								*DevicePathToFindImage;
	CHAR16								*ImageFileName;  
	UINTN								WaitTimeSec;
	BOOLEAN								UseBuildInNtfsDriver;
	}DIDO_OPTION_STATUS;



typedef struct {

	///�����豸��˽�����ݽṹ
	BOOLEAN							Present;					//��ͬ
	EFI_HANDLE                      VirDiskHandle;				//��ͬ
	EFI_DEVICE_PATH_PROTOCOL        *VirDiskDevicePath;			//��ͬ
	EFI_FILE_HANDLE				  	VirDiskFileHandle;			//��ͬ
	BOOLEAN						  	InRam;						//��ͬ
	UINTN                          	StartAddr;					//��ͬ
	UINT64	                        Size;						//��ͬ
	UINT32							UniqueMbrSignature;        	//Ӳ�̷�����ǰ���ǩ��
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

///������ָ�����豸(�ɹ����ϵ�bootimage����)�е�/efi/boot/bootx64.efi�������������ļ��ľ��
///���BootImageDiskHandleΪNULL���������������
EFI_STATUS
LoadBootFileInVirtualDisk(
	IN		EFI_HANDLE							*BootImageDiskHandle, 
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

///�������ļ������ݴ���OptionStatus->ImageFileName
EFI_STATUS
	ProcCfgFile(
		DIDO_OPTION_STATUS						*OptionStatus,	
		EFI_FILE_HANDLE							CurrDirHandle,
		IN 				 CHAR16					*ConfigFileName			//�����ļ���
		);
///�ҳ����̷�����Ϣ�����ص���4��������
EFI_STATUS
	FindPartitionInIsoFile(
		IN		EFI_FILE_HANDLE			FileDiskFileHandle,
		OUT		UINTN					*NoBootStartAddr,
		OUT		UINT64					*NoBootSize,
		OUT		UINTN					*BootStartAddr,
		OUT		UINT64					*BootSize
		);
///�ҳ�Ӳ�̷�����Ϣ�����ص���4��������
EFI_STATUS
	FindPartitionInHdFile(
		IN		EFI_FILE_HANDLE			FileDiskFileHandle,
		OUT		UINTN					*NoBootStartAddr,
		OUT		UINT64					*NoBootSize,
		OUT		UINTN					*BootStartAddr,
		OUT		UINT64					*BootSize,
		OUT		UINT32					*UniqueMbrSignature	
		);
///��OptionStatusָ�����ļ������ؾ��	
EFI_FILE_HANDLE
	OpenFileInOptionStatus(
		DIDO_OPTION_STATUS					*OptionStatus,
		EFI_FILE_HANDLE						CurrDirHandle
	);

///������������Ƿ�ɹ���װ

EFI_HANDLE
	FindBootPartitionHandle();

///���������У����������OptionStatus
EFI_STATUS
	ProcCmdLine(
		DIDO_OPTION_STATUS					*OptionStatus
	);
///��װ����Ӳ��
EFI_STATUS
	HdFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	);
///��װ�������
EFI_STATUS
	IsoFileDiskInstall(
		IN 	EFI_FILE_HANDLE 	FileDiskFileHandle,
		IN	BOOLEAN				DiskInRam
	);
///����ntfs����
EFI_STATUS
	LoadNtfsDriver(
	);
	
///���������д������������OptionStatus�ĳ�Ա
EFI_STATUS
	DispatchOptions(
		DIDO_OPTION_STATUS					*OptionStatus
	);	
	
	
	
#endif