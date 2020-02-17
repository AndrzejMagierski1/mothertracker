
#include "SD.h"
#include "MTP.h"

#include <string.h>

#if USE_RAM

extern uint8_t sdram_mtpIndex[8192];

static uint8_t *const writeLoadBufferPointer = sdram_mtpIndex;



void MTPStorage_SD::OpenIndex()
{
//	if (index_) return;
//	mtp_lock_storage(true);
//	index_ = SD.open("mtpindex.dat", FILE_WRITE);
//	mtp_lock_storage(false);
}

void MTPStorage_SD::WriteIndexRecord(uint32_t i, const Record& r)
{
	mtp_lock_storage(true);

	memcpy(&sdram_mtpIndex[sizeof(r) * i], &r, sizeof(r));

	mtp_lock_storage(false);
}

uint32_t MTPStorage_SD::AppendIndexRecord(const Record& r)
{
	uint32_t new_record = index_entries_++;
	WriteIndexRecord(new_record, r);
	return new_record;
}

// TODO(hubbe): Cache a few records for speed.
Record MTPStorage_SD::ReadIndexRecord(uint32_t i)
{
	Record ret;
	if (i > index_entries_)
	{
		memset(&ret, 0, sizeof(ret));
		return ret;
	}
	OpenIndex();
	mtp_lock_storage(true);

	memcpy(&ret, &sdram_mtpIndex[sizeof(ret) * i], sizeof(ret));

	mtp_lock_storage(false);
	return ret;
}



// MTP object handles should not change or be re-used during a session.
// This would be easy if we could just have a list of all files in memory.
// Since our RAM is limited, we'll keep the index in a file instead.

void MTPStorage_SD::GenerateIndex()
{
	if (index_generated) return;
	index_generated = true;

	//mtp_lock_storage(true);
	//SD.remove("mtpindex.dat");
	//mtp_lock_storage(false);
	index_entries_ = 0;

	Record r;
	r.parent = 0;
	r.sibling = 0;
	r.child = 0;
	r.isdir = true;
	r.scanned = false;
	strcpy(r.name, "/");
	AppendIndexRecord(r);
}


#endif
