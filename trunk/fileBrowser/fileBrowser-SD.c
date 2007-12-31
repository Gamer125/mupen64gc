/* fileBrowser-SD.c - fileBrowser SD module
   by Mike Slegeir for Mupen64-GC
 */

#include <sdcard.h>
#include "fileBrowser.h"

fileBrowser_file topLevel_SD_SlotA =
	{ "dev0:\\N64ROMS", // file name
	  0, // sector
	  0, // offset
	  0, // size
	  FILE_BROWSER_ATTR_DIR
	 };

fileBrowser_file topLevel_SD_SlotB =
	{ "dev1:\\N64ROMS", // file name
	  0, // sector
	  0, // offset
	  0, // size
	  FILE_BROWSER_ATTR_DIR
	 };

fileBrowser_file saveDir_SD_SlotA =
	{ "dev0:\\N64SAVES\\",
	  0,
	  0,
	  0,
	  FILE_BROWSER_ATTR_DIR
	 };

fileBrowser_file saveDir_SD_SlotB =
	{ "dev1:\\N64SAVES\\",
	  0,
	  0,
	  0,
	  FILE_BROWSER_ATTR_DIR
	 };

int fileBrowser_SD_readDir(fileBrowser_file* file, fileBrowser_file** dir){
	DIR* sddir = NULL;
	// Call the corresponding SDCARD function
	int num_entries = SDCARD_ReadDir(&file->name, &sddir);
	
	// If it was not successful, just return the error
	if(num_entries <= 0) return num_entries;
	
	// Convert the SDCARD data to fileBrowser_files
	*dir = malloc( num_entries * sizeof(fileBrowser_file) );
	int i;
	for(i=0; i<num_entries; ++i){
		sprintf((*dir)[i].name, "%s\\%s", file->name, sddir[i].fname);
		(*dir)[i].offset = 0;
		(*dir)[i].size   = sddir[i].fsize;
		(*dir)[i].attr   = sddir[i].fattr;
	}
	
	return num_entries;
}

int fileBrowser_SD_seekFile(fileBrowser_file* file, unsigned int where, unsigned int type){
	if(type == FILE_BROWSER_SEEK_SET) file->offset = where;
	else if(type == FILE_BROWSER_SEEK_CUR) file->offset += where;
	else file->offset = file->size + where;
	
	return 0;
}

int fileBrowser_SD_readFile(fileBrowser_file* file, void* buffer, unsigned int length){
	sd_file* f;
	f = SDCARD_OpenFile( &file->name, "rb");
	if(!f) return FILE_BROWSER_ERROR;
	
	SDCARD_SeekFile(f, file->offset, SDCARD_SEEK_SET);
	int bytes_read = SDCARD_ReadFile(f, buffer, length);
	if(bytes_read > 0) file->offset += bytes_read;
	
	SDCARD_CloseFile(f);
	return bytes_read;
}

int fileBrowser_SD_writeFile(fileBrowser_file* file, void* buffer, unsigned int length){
	sd_file* f;
	f = SDCARD_OpenFile( &file->name, "wb");
	if(!f) return FILE_BROWSER_ERROR;
	
	SDCARD_SeekFile(f, file->offset, SDCARD_SEEK_SET);
	int bytes_read = SDCARD_WriteFile(f, buffer, length);
	if(bytes_read > 0) file->offset += bytes_read;
	
	SDCARD_CloseFile(f);
	return bytes_read;
}

int fileBrowser_SD_init(fileBrowser_file* f){ }

int fileBrowser_SD_deinit(fileBrowser_file* f){ }


/* Special for ROM loading only */
static sd_file* fd;

int fileBrowser_SDROM_deinit(fileBrowser_file* f){
	if(fd)
		SDCARD_CloseFile(fd);
	fd = NULL;
	return 0;
}
	
int fileBrowser_SDROM_readFile(fileBrowser_file* file, void* buffer, unsigned int length){
	if(!fd)
	fd = SDCARD_OpenFile( &file->name, "rb");
	SDCARD_SeekFile(fd, file->offset, SDCARD_SEEK_SET);
	int bytes_read = SDCARD_ReadFile(fd, buffer, length);
	if(bytes_read > 0) file->offset += bytes_read;

	return bytes_read;
}

