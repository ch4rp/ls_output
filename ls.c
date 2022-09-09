
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

void getls(const char *path);

int main(void)
{
	DIR *dir;
	struct dirent *dent;

	if ((dir = opendir(".")) == NULL) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	while (errno = 0, (dent = readdir(dir)) != NULL)
		getls(dent->d_name);

	if (errno != 0) {
		perror("readdir");
		exit(EXIT_FAILURE);
	}

	closedir(dir);

	return 0;
}

void getls(const char *path)
{
	struct stat finfo;
	struct passwd *pass;
	struct group *gr;
	struct tm *modif;
	static mode_t modes[] = {
		S_IRUSR, S_IWUSR, S_IXUSR,
		S_IRGRP, S_IWGRP, S_IXGRP,
		S_IROTH, S_IWOTH, S_IXOTH
	};
	static char *months[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
		"Jul", "Aug", "Sept", "Oct", "Nov", "Dec"
	};
	
	int i;

	if (stat(path, &finfo) == (-1))
		printf("stat?\n");

	modif = localtime(&finfo.st_mtime);

	if (S_ISREG(finfo.st_mode))
		printf("-");
	else if (S_ISDIR(finfo.st_mode))
		printf("d");
	else if (S_ISCHR(finfo.st_mode))
		printf("c");
	else if (S_ISBLK(finfo.st_mode))
		printf("b");
	else if (S_ISFIFO(finfo.st_mode))
		printf("p");
	else if (S_ISLNK(finfo.st_mode))
		printf("l");
	else if (S_ISSOCK(finfo.st_mode))
		printf("s");

	for (i = 0; i < 9; ++i)
		printf("%c", (modes[i] & finfo.st_mode) ? "rwx"[i % 3] : '-');

	printf(" %2llu ", (unsigned long long)finfo.st_nlink);

	if (!(pass = getpwuid(finfo.st_uid)))
		printf("%15s ", pass->pw_name);
	
	if (!(gr = getgrgid(finfo.st_gid)))
		printf("%15s ", gr->gr_name);
	
	printf("%20lld ", (long long)finfo.st_size);

	printf("%4s %2d %02d:%02d ", months[modif->tm_mon], modif->tm_mday, modif->tm_hour, modif->tm_min);
	printf("%s\n", path);
}
