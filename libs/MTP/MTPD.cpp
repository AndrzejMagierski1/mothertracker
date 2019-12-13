
#include "SD.h"
#include "MTP.h"

MTPD mtpd(&storage);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

elapsedMillis loopTime;

void MTPD::loop()
{
	usb_packet_t *receive_buffer;
	if ((receive_buffer = usb_rx(MTP_RX_ENDPOINT)))
	{
		PrintPacket(receive_buffer);
		uint32_t return_code = 0;
		uint32_t p1 = 0;
		if (receive_buffer->len >= 12)
		{
			loopTime = 0;

			return_code = 0x2001;  // Ok
			receive_buffer->len = 16;
			if (CONTAINER->type == 1)
			{ // command
				switch (CONTAINER->op)
				{
				case 0x1001: // GetDescription
					TRANSMIT(WriteDescriptor());
					break;
				case 0x1002:  // OpenSession
					break;
				case 0x1003:  // CloseSession
					break;
				case 0x1004:  // GetStorageIDs
					TRANSMIT(WriteStorageIDs());
					break;
				case 0x1005:  // GetStorageInfo
					ColectStorageInfo();
					TRANSMIT(GetStorageInfo(CONTAINER->params[0]));
					break;
				case 0x1006:  // GetNumObjects
					if (CONTAINER->params[1])
					{
						return_code = 0x2014; // spec by format unsupported
					}
					else
					{
						p1 = GetNumObjects(CONTAINER->params[0],
						CONTAINER->params[2]);
					}
					break;
				case 0x1007:  // GetObjectHandles
					if (CONTAINER->params[1])
					{
						return_code = 0x2014; // spec by format unsupported
					}
					else
					{
						TRANSMIT(GetObjectHandles(CONTAINER->params[0],
									CONTAINER->params[2]));
					}
					break;
				case 0x1008:  // GetObjectInfo
					TRANSMIT(GetObjectInfo(CONTAINER->params[0]));
					break;
				case 0x1009:  // GetObject
					TRANSMIT(GetObject(CONTAINER->params[0]));
					break;
				case 0x100B:  // DeleteObject
					if (CONTAINER->params[1])
					{
						return_code = 0x2014; // spec by format unsupported
					}
					else
					{
						if (!storage_->DeleteObject(CONTAINER->params[0]))
						{
							return_code = 0x2012; // partial deletion
						}
					}
					break;
				case 0x100C:  // SendObjectInfo
					CONTAINER->params[2] =
							SendObjectInfo(CONTAINER->params[0], // storage
									CONTAINER->params[1]); // parent
					p1 = CONTAINER->params[0];
					if (!p1) p1 = 1;
					CONTAINER->len = receive_buffer->len = 12 + 3 * 4;
					break;
				case 0x100D:  // SendObject
					SendObject();
	  				break;
				case 0x1014:  // GetDevicePropDesc
					TRANSMIT(GetDevicePropDesc(CONTAINER->params[0]));
					break;
				case 0x1015:  // GetDevicePropvalue
					TRANSMIT(GetDevicePropValue(CONTAINER->params[0]));
					break;
				default:
					return_code = 0x2005;  // operation not supported
					break;
				}
			}
			else
			{
				return_code = 0x2000;  // undefined
			}

			Serial.print("loop time: ");
			Serial.println(loopTime);
			if(loopTime > 1000)
			{
			//	__asm__ __volatile__ ("bkpt #0");
				(void)receive_buffer;
			}


		}
		if (return_code)
		{
			CONTAINER->type = 3;
			CONTAINER->op = return_code;
			CONTAINER->params[0] = p1;
			PrintPacket(receive_buffer);
			usb_tx(MTP_TX_ENDPOINT, receive_buffer);
			receive_buffer = 0;
		}
		else
		{
			usb_free(receive_buffer);
		}
	}
	// Maybe put event handling inside mtp_yield()?
	if ((receive_buffer = usb_rx(MTP_EVENT_ENDPOINT)))
	{
		usb_free(receive_buffer);
	}
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void MTPD::PrintPacket(const usb_packet_t *x)
{
#if 0
	for (int i = 0; i < x->len; i++)
	{
		Serial1.print("0123456789ABCDEF"[x->buf[i] >> 4]);
		Serial1.print("0123456789ABCDEF"[x->buf[i] & 0xf]);
		if ((i & 3) == 3) Serial1.print(" ");
	}
	Serial1.println("");
#endif
#if 0
	MTPContainer *tmp = (struct MTPContainer*)(x->buf);
	Serial.print(" len = ");
	Serial.print(tmp->len, HEX);
	Serial.print(" type = ");
	Serial.print(tmp->type, HEX);
	Serial.print(" op = ");
	Serial.print(tmp->op, HEX);
	Serial.print(" transaction_id = ");
	Serial.print(tmp->transaction_id, HEX);
	for (int i = 0; i * 4 < x->len - 12; i ++)
	{
		Serial.print(" p");
		Serial.print(i);
		Serial.print(" = ");
		Serial.print(tmp->params[i], HEX);
	}
	Serial.println("");
#endif
}


void MTPD::get_buffer()
{
	while (!data_buffer_)
	{
		data_buffer_ = usb_malloc();
		if (!data_buffer_) yield();
	}
}

void MTPD::receive_buffer()
{
	while (!data_buffer_)
	{
		data_buffer_ = usb_rx(MTP_RX_ENDPOINT);
		if (!data_buffer_) yield();
	}
}


void MTPD::write(const char *data, int len)
{
	if (write_get_length_)
	{
		write_length_ += len;
	}
	else
	{
		int pos = 0;
		while (pos < len)
		{
			get_buffer();
			int avail = sizeof(data_buffer_->buf) - data_buffer_->len;
			int to_copy = min(len - pos, avail);
			memcpy(data_buffer_->buf + data_buffer_->len,
					data + pos,
					to_copy);
			data_buffer_->len += to_copy;
			pos += to_copy;
			if (data_buffer_->len == sizeof(data_buffer_->buf))
			{
				usb_tx(MTP_TX_ENDPOINT, data_buffer_);
				data_buffer_ = NULL;
				// Serial1.println("SENT...");
			}
		}
	}
}



void MTPD::writestring(const char* str)
{
	if (*str)
	{
		write8(strlen(str) + 1);
		while (*str)
		{
			write16(*str);
			++str;
		}
		write16(0);
	}
	else
	{
		write8(0);
	}
}

void MTPD::WriteDescriptor()
{
	write16(100);  // MTP version
	write32(6);    // MTP extension
//    write32(0xFFFFFFFFUL);    // MTP extension
	write16(100);  // MTP version
	writestring("microsoft.com: 1.0;");
	write16(0);    // functional mode

	// Supported operations (array of uint16)
	write32(14);
	write16(0x1001);  // GetDeviceInfo
	write16(0x1002);  // OpenSession
	write16(0x1003);  // CloseSession
	write16(0x1004);  // GetStorageIDs

	write16(0x1005);  // GetStorageInfo
	write16(0x1006);  // GetNumObjects
	write16(0x1007);  // GetObjectHandles
	write16(0x1008);  // GetObjectInfo

	write16(0x1009);  // GetObject
	write16(0x100B);  // DeleteObject
	write16(0x100C);  // SendObjectInfo
	write16(0x100D);  // SendObject

	write16(0x1014);  // GetDevicePropDesc
	write16(0x1015);  // GetDevicePropValue

//    write16(0x1010);  // Reset
//    write16(0x1019);  // MoveObject
//    write16(0x101A);  // CopyObject

	write32(0);       // Events (array of uint16)

	write32(1);       // Device properties (array of uint16)
	write16(0xd402);  // Device friendly name

	write32(0);       // Capture formats (array of uint16)

	write32(2);       // Playback formats (array of uint16)
	write16(0x3000);  // Undefined format
	write16(0x3001);  // Folders (associations)

	writestring("Polyend");     // Manufacturer
	writestring("Tracker");   // Model
	writestring("1.0");      // version
	writestring("0000");      // serial
}

void MTPD::WriteStorageIDs()
{
	write32(1); // 1 entry
	write32(1); // 1 storage
}

void MTPD::ColectStorageInfo()
{
	storage_size = storage_->size();
	storage_free = storage_->free();
}


void MTPD::GetStorageInfo(uint32_t storage)
{
	write16(storage_->readonly() ? 0x0001 : 0x0004); // storage type (removable RAM)
	write16(storage_->has_directories() ? 0x0002 : 0x0001); // filesystem type (generic hierarchical)
	write16(0x0000);   // access capability (read-write)
	write64(storage_size);  // max capacity
	write64(storage_free);  // free space (100M)
	write32(0xFFFFFFFFUL);  // free space (objects)
	writestring("SD Card");  // storage descriptor
	writestring("");  // volume identifier
}

uint32_t MTPD::GetNumObjects(uint32_t storage, uint32_t parent)
{
	storage_->StartGetObjectHandles(parent);
	int num = 0;
	while (storage_->GetNextObjectHandle())
		num++;
	return num;
}

void MTPD::GetObjectHandles(uint32_t storage, uint32_t parent)
{
	uint32_t num = 0;
	if (!write_get_length_)
	{
		num = GetNumObjects(storage, parent);
	}
	write32(num);
	int handle;
	storage_->StartGetObjectHandles(parent);
	while ((handle = storage_->GetNextObjectHandle()))
	{
		write32(handle);
	}
}

void MTPD::GetObjectInfo(uint32_t handle)
{
	char filename[256];
	uint32_t size, parent;
	storage_->GetObjectInfo(handle, filename, &size, &parent);

	write32(1); // storage
	write16(size == 0xFFFFFFFFUL ? 0x3001 : 0x0000); // format
	write16(0);  // protection
	write32(size); // size
	write16(0); // thumb format
	write32(0); // thumb size
	write32(0); // thumb width
	write32(0); // thumb height
	write32(0); // pix width
	write32(0); // pix height
	write32(0); // bit depth
	write32(parent); // parent
	write16(size == 0xFFFFFFFFUL ? 1 : 0); // association type
	write32(0); // association description
	write32(0);  // sequence number
	writestring(filename);
	writestring("");  // date created
	writestring("");  // date modified
	writestring("");  // keywords
}

void MTPD::GetObject(uint32_t object_id)
{
	uint32_t size = storage_->GetSize(object_id);
	if (write_get_length_)
	{
		write_length_ += size;
	}
	else
	{
		uint32_t pos = 0;
		while (pos < size)
		{
			get_buffer();
			uint32_t avail = sizeof(data_buffer_->buf) - data_buffer_->len;
			uint32_t to_copy = min(size - pos, avail);
			// Read directly from storage into usb buffer.
			storage_->read(object_id, pos,
							(char*) (data_buffer_->buf + data_buffer_->len),
							to_copy);
			pos += to_copy;
			data_buffer_->len += to_copy;
			if (data_buffer_->len == sizeof(data_buffer_->buf))
			{
				usb_tx(MTP_TX_ENDPOINT, data_buffer_);
				data_buffer_ = NULL;
			}
		}
	}
}

void MTPD::read(char* data, uint32_t size)
{
	while (size)
	{
		receive_buffer();
		uint32_t to_copy = data_buffer_->len - data_buffer_->index;
		to_copy = min(to_copy, size);
		if (data)
		{
			memcpy(data, data_buffer_->buf + data_buffer_->index, to_copy);
			data += to_copy;
		}
		size -= to_copy;
		data_buffer_->index += to_copy;
		if (data_buffer_->index == data_buffer_->len)
		{
			usb_free(data_buffer_);
			data_buffer_ = NULL;
		}
	}
}

void MTPD::readstring(char* buffer)
{
	int len = read8();
	if (!buffer)
	{
		read(NULL, len * 2);
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			*(buffer++) = read16();
		}
	}
}

void MTPD::read_until_short_packet()
{
	bool done = false;
	while (!done)
	{
		receive_buffer();
		done = data_buffer_->len != sizeof(data_buffer_->buf);
		usb_free(data_buffer_);
		data_buffer_ = NULL;
	}
}

uint32_t MTPD::SendObjectInfo(uint32_t storage, uint32_t parent)
{
	ReadMTPHeader();
	char filename[256];

	read32(); // storage
	bool dir = read16() == 0x3001; // format
	read16();  // protection
	read32(); // size
	read16(); // thumb format
	read32(); // thumb size
	read32(); // thumb width
	read32(); // thumb height
	read32(); // pix width
	read32(); // pix height
	read32(); // bit depth
	read32(); // parent
	read16(); // association type
	read32(); // association description
	read32(); // sequence number

	readstring(filename);
	read_until_short_packet();  // ignores dates & keywords
	return storage_->Create(parent, dir, filename);
}

void MTPD::SendObject()
{
	uint32_t len = ReadMTPHeader();
	while (len)
	{
		receive_buffer();
		uint32_t to_copy = data_buffer_->len - data_buffer_->index;
		to_copy = min(to_copy, len);
		storage_->write((char*) (data_buffer_->buf + data_buffer_->index),
						to_copy);
		data_buffer_->index += to_copy;
		len -= to_copy;
		if (data_buffer_->index == data_buffer_->len)
		{
			usb_free(data_buffer_);
			data_buffer_ = NULL;
		}
	}
	storage_->close();
}

void MTPD::GetDevicePropValue(uint32_t prop)
{
	switch (prop)
	{
	case 0xd402: // friendly name
		// This is the name we'll actually see in the windows explorer.
		// Should probably be configurable.
		writestring("Polyend Tracker");
		break;
	}
}

void MTPD::GetDevicePropDesc(uint32_t prop)
{
	switch (prop)
	{
	case 0xd402: // friendly name
		write16(prop);
		write16(0xFFFF); // string type
		write8(0);       // read-only
		GetDevicePropValue(prop);
		GetDevicePropValue(prop);
		write8(0);       // no form
	}
}
