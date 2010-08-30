#include <stdio.h>
#include <string.h>
#include <libgen.h>

void _makepath(char *path, const char *device, const char *dir, const char *filename, const char *ext)
{

	char *pos;
	char slash;

	path[0] = 0;

	if ((device != NULL) && (device != '\0'))
	{
		strcat(path,device);
		if (strrchr(path,':') == NULL)
		{
			strcat(path,":");
		}
	}

	if ((dir != NULL) && (dir != '\0'))
	{
		strcat(path,dir);
		if ((pos = strrchr(path,'/')) != NULL);
		else if ((pos = strrchr(path,'\\')) != NULL);
		else
		{
			strcat(path,"/");
		}

		if (*(pos+1) != '\0')
		{
			slash = *pos;
			pos = strrchr(path,'\0');
			*pos = slash;
			pos++;
			*pos = '\0';
		}
	}

	if ((filename != NULL) && (filename != '\0'))
	{
		strcat(path,filename);
	}

	if ((ext != NULL) && (ext != '\0'))
	{

		if (strrchr(ext,'.') != NULL)
		{
			strcat(path,ext);
		}
		else
		{
			strcat(path,".");
			strcat(path,ext);
		}
	}

}

void _splitpath(const char *path, char *device, char *dir, char *filename, char *ext)
{

	char split[256];

	char *pos, *start;

	device[0] = 0;
	dir[0] = 0;
	filename[0] = 0;
	ext[0] = 0;

	strcpy(split,path);

	// device name
	pos = strchr(split,':');

	// start of directory
	pos++;

	memcpy(device, split, pos-split);
	device[pos-split] = '\0';

	// directories
	start = pos;

	if ((pos = strrchr(split,'/')) != NULL)
	{
		pos++;
		memcpy(dir, start, pos-start);
		dir[pos-start] = '\0';
	}
	else
	{
		dir[0] = '/';
		dir[1] = '\0';
	}

	// filename
	start = basename(split);
	if ((pos = strrchr(start,'.')) != NULL)
	{

		memcpy(filename, start, pos-start);
		filename[pos-start] = '\0';

	}
	else
	{
		strcpy(filename,basename(split));
		return;
	}

	// extension
	// pos should be at the .
	if ((pos = strrchr(split,'.')) != NULL)
	{
		strcpy(ext,pos);
	}
	else
	{
		ext[0] = 0;
	}

}
