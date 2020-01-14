
#ifndef MTP_H
#define MTP_H

//----------------------------------------------

#define USE_RAM 1

//----------------------------------------------

//#include <Arduino.h>
// #include <HardwareSerial.h>
#include <usb_dev.h>
//#include <SPI.h>
//#include "SD.h"


void mtp_yield();
void mtp_lock_storage(bool lock);

// TODO:
//   support multiple storages
//   support serialflash
//   partial object fetch/receive
//   events (notify usb host when local storage changes)
struct Record
{
  uint32_t parent;
  uint32_t child;  // size stored here for files
  uint32_t sibling;
  uint8_t isdir;
  uint8_t scanned;
  char name[64];
};

// This interface lets the MTP responder interface any storage.
// We'll need to give the MTP responder a pointer to one of these.
class MTPStorageInterface
{
public:
  // Return true if this storage is read-only
  virtual bool readonly() = 0;

  // Does it have directories?
  virtual bool has_directories() = 0;

  // Return size of storage in bytes.
  virtual uint64_t size() = 0;

  // Return free space in bytes.
  virtual uint64_t free() = 0;
//
//  void  MTPStorage_SD::OpenIndex()
//  void  MTPStorage_SD::WriteIndexRecord(uint32_t i, const Record& r)
//  uint32_t  MTPStorage_SD::AppendIndexRecord(const Record& r)
//  Record  MTPStorage_SD::ReadIndexRecord(uint32_t i)
//  void  MTPStorage_SD::ConstructFilename(int i, char* out)
//  void  MTPStorage_SD::OpenFileByIndex(uint32_t i, uint8_t mode = O_RDONLY)
//  void  MTPStorage_SD::GenerateIndex()
//  void  MTPStorage_SD::ScanDir(uint32_t i)
//  void MTPStorage_SD::ScanAll()
//
  // parent = 0 means get all handles.
  // parent = 0xFFFFFFFF means get root folder.
  virtual void StartGetObjectHandles(uint32_t parent) = 0;
  virtual uint32_t GetNextObjectHandle() = 0;

  // Size should be 0xFFFFFFFF if it's a directory.
  virtual void GetObjectInfo(uint32_t handle,
        char* name,
        uint32_t* size,
        uint32_t* parent) = 0;
  virtual uint32_t GetSize(uint32_t handle) = 0;
  virtual void read(uint32_t handle,
        uint32_t pos,
        char* buffer,
        uint32_t bytes) = 0;
  virtual uint32_t Create(uint32_t parent,
        bool folder,
        const char* filename) = 0;
  virtual void write(const char* data, uint32_t size) = 0;
  virtual void close() = 0;
  virtual bool DeleteObject(uint32_t object) = 0;
};



// Storage implementation for SD. SD needs to be already initialized.
class MTPStorage_SD : public MTPStorageInterface
{
private:
	SdFile index_;

  uint8_t mode_ = 0;
  uint32_t open_file_ = 0xFFFFFFFEUL;
  SdFile f_;
  uint32_t index_entries_ = 0;



  bool index_generated = false;
  bool all_scanned_ = false;
  uint32_t next_;
  bool follow_sibling_;






 virtual bool readonly();

 virtual bool has_directories();
 virtual uint64_t size();
 virtual uint64_t free();
 void  OpenIndex();
 void  WriteIndexRecord(uint32_t i, const Record& r);
 uint32_t  AppendIndexRecord(const Record& r);
 Record  ReadIndexRecord(uint32_t i);
 void  ConstructFilename(int i, char* out);
 void  OpenFileByIndex(uint32_t i, uint8_t mode);
 void  GenerateIndex();
 void  ScanDir(uint32_t i);
 void  ScanAll();
 virtual void StartGetObjectHandles(uint32_t parent);
 virtual uint32_t GetNextObjectHandle();

 virtual void GetObjectInfo(uint32_t handle,
       char* name,
       uint32_t* size,
       uint32_t* parent);

 virtual uint32_t GetSize(uint32_t handle);

 virtual  void read(uint32_t handle,
       uint32_t pos,
       char* out,
       uint32_t bytes);

 virtual  bool DeleteObject(uint32_t object);

 virtual uint32_t Create(uint32_t parent,
     bool folder,
     const char* filename);

 virtual void write(const char* data, uint32_t bytes);

 virtual void close();

};

// MTP Responder.
class MTPD
{
public:
	explicit MTPD(MTPStorageInterface* storage) :
			storage_(storage)
	{
	}

	uint8_t state = 0;

private:

	uint64_t storage_size  = 0;
	uint64_t storage_free = 0;

	MTPStorageInterface* storage_;

	struct MTPHeader
	{
		uint32_t len;  // 0
		uint16_t type; // 4
		uint16_t op;   // 6
		uint32_t transaction_id; // 8
	};

	struct MTPContainer
	{
		uint32_t len;  // 0
		uint16_t type; // 4
		uint16_t op;   // 6
		uint32_t transaction_id; // 8
		uint32_t params[5];    // 12
	};


  	void PrintPacket(const usb_packet_t *x);

	usb_packet_t *data_buffer_ = NULL;
	bool write_get_length_ = false;
	uint32_t write_length_ = 0;
	void get_buffer();
	void receive_buffer();

	void write(const char *data, int len);
	void write8 (uint8_t  x) { write((char*)&x, sizeof(x)); }
	void write16(uint16_t x) { write((char*)&x, sizeof(x)); }
	void write32(uint32_t x) { write((char*)&x, sizeof(x)); }
	void write64(uint64_t x) { write((char*)&x, sizeof(x)); }

	void writestring(const char* str);
	void WriteDescriptor();
	void WriteStorageIDs();
	void ColectStorageInfo();
	void GetStorageInfo(uint32_t storage);
	uint32_t GetNumObjects(uint32_t storage, uint32_t parent);
	void GetObjectHandles(uint32_t storage, uint32_t parent);
	void GetObjectInfo(uint32_t handle);
	void GetObject(uint32_t object_id);

	inline MTPContainer *contains (usb_packet_t *receive_buffer)
	{
	  return (MTPContainer*)(receive_buffer->buf);
	}

#define CONTAINER contains(receive_buffer)

#define TRANSMIT(FUN) do {                              \
    write_length_ = 0;                                  \
    write_get_length_ = true;                           \
    FUN;                                                \
    write_get_length_ = false;                          \
    MTPHeader header;                                   \
    header.len = write_length_ + 12;                    \
    header.type = 2;                                    \
    header.op = CONTAINER->op;                          \
    header.transaction_id = CONTAINER->transaction_id;  \
    write((char *)&header, sizeof(header));             \
    FUN;                                                \
    get_buffer();                                       \
    usb_tx(MTP_TX_ENDPOINT, data_buffer_);              \
    data_buffer_ = NULL;                                \
  } while(0)

	void read(char* data, uint32_t size);

	uint32_t ReadMTPHeader()
	{
		MTPHeader header;
		read((char *) &header, sizeof(MTPHeader));
		// check that the type is data
		return header.len - 12;
	}

	uint8_t read8()
	{
		uint8_t ret;
		read((char*) &ret, sizeof(ret));
		return ret;
	}

	uint16_t read16()
	{
		uint16_t ret;
		read((char*) &ret, sizeof(ret));
		return ret;
	}

	uint32_t read32()
	{
		uint32_t ret;
		read((char*) &ret, sizeof(ret));
		return ret;
	}

	void readstring(char* buffer);
	void read_until_short_packet();
	uint32_t SendObjectInfo(uint32_t storage, uint32_t parent);
	void SendObject();
	void GetDevicePropValue(uint32_t prop);
	void GetDevicePropDesc(uint32_t prop);

public:

	void loop();

};

extern MTPStorage_SD storage;
extern MTPD mtpd;

#endif
