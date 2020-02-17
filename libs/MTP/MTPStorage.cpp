
#include <string.h>

#include "SD.h"
#include "MTP.h"

// These should probably be weak.


MTPStorage_SD storage;

void yield(void);

void mtp_lock_storage(bool lock)
{
}

bool MTPStorage_SD::readonly()
{
	return false;
}

bool MTPStorage_SD::has_directories()
{
	return true;
}

uint64_t MTPStorage_SD::size()
{
	return (uint64_t) 512 * (uint64_t) SD.clusterCount() * (uint64_t) SD.sectorsPerCluster();
}

uint64_t MTPStorage_SD::free()
{
	uint64_t volFree = SD.freeClusterCount();
	uint64_t ssize = (uint64_t) 512 * volFree * (uint64_t) SD.sectorsPerCluster();
	return ssize;
}

#if !USE_RAM

void MTPStorage_SD::OpenIndex()
{
	if (index_) return;
	mtp_lock_storage(true);
	index_ = SD.open("mtpindex.dat", FILE_WRITE);
	mtp_lock_storage(false);
}

void MTPStorage_SD::WriteIndexRecord(uint32_t i, const Record& r)
{
	OpenIndex();
	mtp_lock_storage(true);
	index_.seek(sizeof(r) * i);
	index_.write((char*) &r, sizeof(r));
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
	index_.seek(sizeof(ret) * i);
	index_.read(&ret, sizeof(ret));
	mtp_lock_storage(false);
	return ret;
}

void MTPStorage_SD::GenerateIndex()
{
	if (index_generated) return;
	index_generated = true;

	mtp_lock_storage(true);
	SD.remove("mtpindex.dat");
	mtp_lock_storage(false);
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

void MTPStorage_SD::ConstructFilename(int i, char* out)
{
	if (i == 0)
	{
		strcpy(out, "/");
	}
	else
	{
		Record tmp = ReadIndexRecord(i);
		ConstructFilename(tmp.parent, out);
		if (out[strlen(out) - 1] != '/')
			strcat(out, "/");
		strcat(out, tmp.name);
	}
}

void MTPStorage_SD::OpenFileByIndex(uint32_t i, uint8_t mode = O_RDONLY)
{
	if (open_file_ == i && mode_ == mode) return;

	char filename[256];
	ConstructFilename(i, filename);
	mtp_lock_storage(true);

	f_.close();
	f_ = SD.open(filename, mode);

	open_file_ = i;
	mode_ = mode;
	mtp_lock_storage(false);
}

// MTP object handles should not change or be re-used during a session.
// This would be easy if we could just have a list of all files in memory.
// Since our RAM is limited, we'll keep the index in a file instead.

void MTPStorage_SD::ScanDir(uint32_t i)
{

/*	Record record = ReadIndexRecord(i);
	if (record.isdir && !record.scanned)
	{
		OpenFileByIndex(i, O_RDONLY);
		if (!f_) return;
		int sibling = 0;
		while (true)
		{
			mtp_lock_storage(true);
			//FsFile child = f_.openNextFile();
			SdFile child;
			if (!child.openNext(&f_, O_READ))
			{
				mtp_lock_storage(false);
				break;
			}

			mtp_lock_storage(false);

			if (!child) break;

			Record r;
			r.parent = i;
			r.sibling = sibling;
			r.isdir = child.isDirectory();
			r.child = r.isdir ? 0 : child.size();
			r.scanned = false;
			child.getName(r.name, 64);
			sibling = AppendIndexRecord(r);
			child.close();
		}
		record.scanned = true;
		record.child = sibling;
		WriteIndexRecord(i, record);
	}*/

}

void MTPStorage_SD::ScanAll()
{
	if (all_scanned_) return;
	all_scanned_ = true;

	GenerateIndex();
	for (uint32_t i = 0; i < index_entries_; i++)
	{
		ScanDir(i);
	}
}

void MTPStorage_SD::StartGetObjectHandles(uint32_t parent)
{
	GenerateIndex();
	if (parent)
	{
		if (parent == 0xFFFFFFFF) parent = 0;

		ScanDir(parent);
		follow_sibling_ = true;
		// Root folder?
		next_ = ReadIndexRecord(parent).child;
	}
	else
	{
		ScanAll();
		follow_sibling_ = false;
		next_ = 1;
	}
}

uint32_t MTPStorage_SD::GetNextObjectHandle()
{
	while (true)
	{
		if (next_ == 0) return 0;

		int ret = next_;
		Record r = ReadIndexRecord(ret);
		if (follow_sibling_)
		{
			next_ = r.sibling;
		}
		else
		{
			next_++;
			if (next_ >= index_entries_)
				next_ = 0;
		}
		if (r.name[0]) return ret;
	}
}

void MTPStorage_SD::GetObjectInfo(uint32_t handle,
									char* name,
									uint32_t* size,
									uint32_t* parent)
{
	Record r = ReadIndexRecord(handle);
	strcpy(name, r.name);
	*parent = r.parent;
	*size = r.isdir ? 0xFFFFFFFFUL : r.child;
}

uint32_t MTPStorage_SD::GetSize(uint32_t handle)
{
	return ReadIndexRecord(handle).child;
}

void MTPStorage_SD::read(uint32_t handle,
							uint32_t pos,
							char* out,
							uint32_t bytes)
{
	OpenFileByIndex(handle, O_RDONLY);
	mtp_lock_storage(true);
	f_.seek(pos);
	f_.read(out, bytes);
	mtp_lock_storage(false);
}

bool MTPStorage_SD::DeleteObject(uint32_t object)
{
	char filename[256];
	Record r;
	while (true)
	{
		r = ReadIndexRecord(object == 0xFFFFFFFFUL ? 0 : object);
		if (!r.isdir) break;
		if (!r.child) break;
		if (!DeleteObject(r.child))
			return false;
	}

	// We can't actually delete the root folder,
	// but if we deleted everything else, return true.
	if (object == 0xFFFFFFFFUL) return true;

	ConstructFilename(object, filename);
	bool success;
	mtp_lock_storage(true);
	if (r.isdir)
	{
		success = SD.rmdir(filename);
	}
	else
	{
		success = SD.remove(filename);
	}
	mtp_lock_storage(false);
	if (!success) return false;
	r.name[0] = 0;
	int p = r.parent;
	WriteIndexRecord(object, r);
	Record tmp = ReadIndexRecord(p);
	if (tmp.child == object)
	{
		tmp.child = r.sibling;
		WriteIndexRecord(p, tmp);
	}
	else
	{
		int c = tmp.child;
		while (c)
		{
			tmp = ReadIndexRecord(c);
			if (tmp.sibling == object)
			{
				tmp.sibling = r.sibling;
				WriteIndexRecord(c, tmp);
				break;
			}
			else
			{
				c = tmp.sibling;
			}
		}
	}
	return true;
}

uint32_t MTPStorage_SD::Create(uint32_t parent,
								bool folder,
								const char* filename)
{
	uint32_t ret;
	if (parent == 0xFFFFFFFFUL) parent = 0;
	Record p = ReadIndexRecord(parent);
	Record r;
	if (strlen(filename) > 62) return 0;
	strcpy(r.name, filename);
	r.parent = parent;
	r.child = 0;
	r.sibling = p.child;
	r.isdir = folder;
	// New folder is empty, scanned = true.
	r.scanned = 1;
	ret = p.child = AppendIndexRecord(r);
	WriteIndexRecord(parent, p);
	if (folder)
	{
		char filename[256];
		ConstructFilename(ret, filename);
		mtp_lock_storage(true);
		SD.mkdir(0, filename);
		mtp_lock_storage(false);
	}
	else
	{
		OpenFileByIndex(ret, FILE_WRITE);
	}
	return ret;
}

void MTPStorage_SD::write(const char* data, uint32_t bytes)
{
	mtp_lock_storage(true);
	f_.write(data, bytes);
	mtp_lock_storage(false);
}

void MTPStorage_SD::close()
{
	mtp_lock_storage(true);
	uint64_t size = f_.size();
	f_.close();
	mtp_lock_storage(false);
	Record r = ReadIndexRecord(open_file_);
	r.child = size;
	WriteIndexRecord(open_file_, r);
	open_file_ = 0xFFFFFFFEUL;
}

