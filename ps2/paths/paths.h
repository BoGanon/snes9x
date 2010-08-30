#ifndef __PATHS_H__
#define __PATHS_H__

#ifdef __cplusplus
extern "C" {
#endif

	void _splitpath(const char *path, char *device, char *dir, char *filename, char *ext);
	void _makepath(char *path, const char *device, const char *dir, const char *filename, const char *ext);

#ifdef __cplusplus
};
#endif

#endif /*__PATH_H__*/
