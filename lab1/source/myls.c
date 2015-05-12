#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>          // for printf().  man 3 printf
#include <stdlib.h>         // for exit().    man 3 exit
#include <pwd.h>            // for getpwuid()
#include <grp.h>            // for getgrgid()
#include <time.h>           // for ctime()
#include <string.h>         // for strlen()


char* getfPermissions(char str[], char *str_path);
char* getfOwner(char *str_path);
char* getfGroupName(char *str_path);
char* getfSymbolicLink(char *str_path);
char* getfTimeStamp(char *str_path, char* type);
char* getfTimeM(time_t inTime);
char* getfTimeD(time_t inTime);
char* getfTimeHHMM(time_t inTime);
time_t resolveTimeStamp(char* type, struct stat buf);
long long getfSize(char *str_path);

int main(int argc, char *argv[])
{
	DIR *p_dir;
	struct dirent *p_dirent;

	if (argc == 1) {
		printf("Usage: %s <directory name>\n", argv[0]);
		exit(0);
	}

	if ((p_dir = opendir(argv[1])) == NULL) {
		printf("opendir(%s) failed\n", argv[1]);
		exit(1);
	}

	if (chdir (argv[1]) == -1) {
		exit(1);
	}

	while ((p_dirent = readdir(p_dir)) != NULL) {
		char permission_string[] = "----------\0";
		char *str_path = p_dirent->d_name;
		
		//Sample
		char *permission = getfPermissions(permission_string, str_path);
		char *symbolic_link = getfSymbolicLink(str_path);
		long long file_size = getfSize(str_path);
		char *user_name = getfOwner(str_path);
		char *group_name = getfGroupName(str_path);
		
		printf("%s %s %s %lld %s \n", permission, user_name, group_name, file_size, symbolic_link);
	}

	return 0;
}

char* getfPermissions(char str[], char *str_path){

	struct stat buf;

	if (lstat(str_path, &buf) < 0) {
		perror("lstat error");
	}   
	
	mode_t mode = buf.st_mode;

	//file type
	switch (buf.st_mode & S_IFMT) {
    case S_IFDIR:  str[0] = 'd';            break;
    case S_IFLNK:  str[0] = 'l';            break;
    case S_IFREG:  str[0] = '-';            break;
    default:       str[0] = '?';            break;
    }

	//permission
	str[1] = (mode & S_IRUSR) ? 'r' : '-';
	str[2] = (mode & S_IWUSR) ? 'w' : '-';
	str[3] = (mode & S_IXUSR) ? 'x' : '-';
	str[4] = (mode & S_IRGRP) ? 'r' : '-';
	str[5] = (mode & S_IWGRP) ? 'w' : '-';
	str[6] = (mode & S_IXGRP) ? 'x' : '-';
	str[7] = (mode & S_IROTH) ? 'r' : '-';
	str[8] = (mode & S_IWOTH) ? 'w' : '-';
	str[9] = (mode & S_IXOTH) ? 'x' : '-';

	return str;
}

char* getfOwner(char *str_path){
	struct stat buf;

	if (lstat(str_path, &buf) < 0) {
		perror("lstat error");
	} 

	uid_t user_id = buf.st_uid;

	struct passwd *owner =  getpwuid(user_id);
	return owner->pw_name;
}

char* getfGroupName(char *str_path){
	struct stat buf;

	if (lstat(str_path, &buf) < 0) {
		perror("lstat error");
	} 

	gid_t group_id = buf.st_gid;

	struct group *group_name = getgrgid(group_id);
	return group_name->gr_name;
}

char* getfTimeStamp(char *str_path, char *type){
	struct stat buf;
	
	if(lstat(str_path, &buf) < 0) {
		perror("lstat error");
	}
	
	time_t tmpTime = resolveTimeStamp(type, buf);
	
	char* result;
	return result;
}

char* getfTimeM(time_t inTime){
	char buff[3];
	strftime(buff, 3, "%b", localtime(&inTime));
	return buff;
}

char* getfTimeD(time_t inTime){
	char buff[2];
	strftime(buff, 2, "%e", localtime(&inTime));
	return buff;
}

char* getfTimeHHMM(time_t inTime){
	char buff[4];
	strftime(buff, 4, "%H:%M", localtime(&inTime));
	return buff;
}

time_t resolveTimeStamp(char* type, struct stat buf){
	if(strcmp(type, "-u")){
		return &buf.st_atime;
	}else if(strcmp(type, "-c")){
		return &buf.st_ctime;
	}else if(strcmp(type, "-l")){
		return &buf.st_mtime;
	}else{		
		perror(strcat("Unable to extract time from command: ", type));
	}
}

char* getfSymbolicLink(char *str_path){
	
	char *arrow = "-->";
    char linkname[256];
    ssize_t r;

    r = readlink(str_path, linkname, 256);
   
   	if(linkname < 0) {
   		strcat(str_path, arrow);
   		strcat(str_path, linkname);
   	}
   
    return str_path;
}

long long getfSize(char *str_path){

	struct stat buf;

	if (lstat(str_path, &buf) < 0) {
		perror("lstat error");
	} 

	off_t file_size = buf.st_size;

	return file_size;
}
