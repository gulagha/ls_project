//Written by Gulagha Gozalov
//ggozalov2018@ada.edu.az

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

char *filenames[INT_MAX];
int size;
int sorted = 1;
int ish = 0;
int isu = 0;
int isn = 0;
int isi = 0;
int isF = 0;
char argvs[INT_MAX];


void allArgv(int argc, char ** argv){ //this func combines all command line argument into char array
	int j = 0;
	for(int i = 2; i<argc; i++){
		for(int d = 0; d<strlen(argv[i]); d++){
			if(argv[i][0]!='-'){ 
				if(chdir(argv[i])<0) { perror("error: "); exit(1); }
				if(argc == 3) break;
				if(i==argc-1) break; else i++;
			}
			if (argv[i][d] == '-' && d==0) {}
			if (argv[i][d] == '-' && d!=0) {printf("illegal option\n"); exit(0);}
			else argvs[j] = argv[i][d]; j++;
		}
	}
	//printf("%s\n", argvs);
}

bool isThere(char * arguments, char argument){ //this func checks arguments in the array
	int flag = 0;
	for(int i = 0; i<strlen(arguments); i++){
		if (argument == arguments[i]) flag++;
	}
	return (flag == 0) ? false : true;
}

char * formatdate(char* str, time_t val){ //formats the date
    strftime(str, 36, "%b %d %H:%M", localtime(&val));
    return str;
}

//---------- alphabetical sorting funcitons -----------
int compare(const void *v1, const void *v2){
    char *a = *(char **)v1;
    char *b = *(char **)v2;
    return strcasecmp(a,b);
}
 
void sort(char *arr[], int n){
    qsort (arr, n, sizeof(*arr), compare);
}
//-----------------------------------------------

//---------- size sorting funcitons ---------------
void sizeSorter2(char * filename1, char * filename2){
	char temp[1024];
	struct stat fileStat1;
	struct stat fileStat2;
	if(stat(filename1, &fileStat1) < 0) {}
	if(stat(filename2, &fileStat2) < 0) {}
	if(fileStat2.st_size > fileStat1.st_size){ strcpy(temp, filename1); strcpy(filename1, filename2); strcpy(filename2, temp);}
}

void sizeSorter(char ** filenames){
	for (int j = 0; j < size - 2; ++j){
		for(int i = 0; i < size-1; i++){
			sizeSorter2(filenames[i], filenames[i+1]);
		}
	}
}
//-----------------------------------------------

//---------------helper functions-----------------
void ihelper(char * filename){
	struct stat fileStat;
	char result [1024];
	if(stat(filename, &fileStat) < 0) {} //perror("error occured");
    sprintf(result, "%10llu %-15s", fileStat.st_ino, filename);
    strcpy(filename, result);
}

void FHelper(char * filename){
	char temp[100];
	struct stat fileStat;
	if(stat(filename, &fileStat) < 0) perror("error: ");
	if(S_ISDIR(fileStat.st_mode)) sprintf(temp, "%s/", filename);
	else if(!access(filename,X_OK)) sprintf(temp, "%s*", filename);
	else if(S_ISLNK(fileStat.st_mode)) sprintf(temp, "%s@", filename);
	else if(S_ISREG(fileStat.st_mode)) sprintf(temp, "%s", filename);
	else if(S_ISSOCK(fileStat.st_mode)) sprintf(temp, "%s=", filename);
	else if(S_ISFIFO(fileStat.st_mode)) sprintf(temp, "%s|", filename);
	else if(S_ISCHR(fileStat.st_mode)) sprintf(temp, "%s>", filename);	
	strcpy(filename, temp);
}

int fileInfoL(char * filename){
	char * emp ="";
	char date [512];
    struct stat fileStat;
    if(stat(filename, &fileStat) < 0)return 1;
 	struct passwd *pd = getpwuid(fileStat.st_uid);
 	struct group *g = getgrgid(fileStat.st_gid);

 	if(isi) {printf("%10llu ", fileStat.st_ino);}
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf(" %3d",fileStat.st_nlink); //number of links
    if(isn) {
    	printf("%4d", fileStat.st_uid) ;
    	printf("%4d", fileStat.st_gid);
    } else {
    	printf((NULL != pd) ? " %13s" : "?", pd->pw_name);
    	printf((NULL != g) ? " %5s" : "?", g->gr_name);
    }
    if(ish){
    	if(fileStat.st_size/1024 < 1) printf(" %6lldB",fileStat.st_size); 
    	if(fileStat.st_size/1024 >= 1 && fileStat.st_size/1024 < 10) printf(" %6.2fK",fileStat.st_size/1024.0);
    	if(fileStat.st_size/1024 >= 10 && fileStat.st_size/1024 < 1000) printf(" %6lldK",fileStat.st_size/1024);
    	if(fileStat.st_size/1024 >= 1000 && fileStat.st_size/1024 < 1000000) printf(" %6lldMB",fileStat.st_size/1024);
    }
    else {printf(" %10lld",fileStat.st_size);} //file size
    if(isu) printf(" %10s", formatdate(date, fileStat.st_atime));
    else {printf(" %10s", formatdate(date, fileStat.st_mtime));}
    printf(" %s\n",filename);
    //printf("\n The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");
 	return 0;
}

//------------------

//-----------printing functions------------

int simplePrint(char ** filename){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int maxlen = 0;
	int len = 0;
	for (int i = 0; i < size; ++i){
		len = strlen(filenames[i]);
		if (maxlen < len) maxlen = len;
	}
	maxlen = maxlen + 5;
	int columns = w.ws_col / maxlen;
	int rows = (size) / columns;
	int k = 0;
	if(size%columns != 0) rows++;
	for (int j = 0; j <rows; ++j){
		k = j;
		for (int d = 0; d < columns; ++d){
			if (k<size) printf("%-20s", filenames[k]);
			else printf("");
			k = k+rows;
		}
		printf("\n");
	}
	return 0;
}

void R(const char *pathname, int indent) //not completed
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(pathname)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", pathname, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            R(path, indent + 2);
        } else {
            printf("%*s- %s\n", indent, "", entry->d_name);
        }
    }
    closedir(dir);
}

int m(){
	for (int i = 0; i < size - 1; ++i)
	{
		printf("%s,", filenames[i]);
	}
	printf("\n");
	return 0;
}

int l(char ** filenames){
	for (int i = 0; i < size - 1; ++i)
	{
		fileInfoL(filenames[i]);
	}
	return 0;
}

//----								   ----
//----		starting ls arguments	   ----
//---------------------   -----------------
//---------------------   -----------------

void ls(char ** filenames){
	int j = 0;
	for (int i = 0; i < size; ++i)
	{
		if(filenames[i][0] == '.') {}
		  	else {
			filenames[j] = filenames[i];
			j++;
		}
	}
	size = j;
	sort(filenames, size);
}

void iFunc(char ** filenames){
	for (int i = 0; i < size; ++i){
		ihelper(filenames[i]);
	}
}

void A(char ** filenames){
	int j = 0;
	for (int i = 0; i < size; ++i){
		if(!strcmp(filenames[i], ".")  || !strcmp(filenames[i], "..")){} 
			else {
			filenames[j] =  filenames[i];
			j++;
		}
	}
	size = j;
	sort(filenames, size);
}

void a(char * path){ //core ls function
	// Pointer for directory entry
	struct dirent *de;
	int i = 0;  
	DIR *dr = opendir(path);
	if (dr == NULL) printf("Could not open current directory" );
	while ((de = readdir(dr)) != NULL){
			filenames[i] = de->d_name;
			printf("");
			i++;
	 	}
	size = i;
	if(sorted) { sort(filenames, size); }
	closedir(dr);
}

void r(char ** filenames){
	int j = 0;
	char * temp[size];
	for (int i = size-1; i >= 0; i--){
		//printf("%s\n", filenames[i]);	
		temp[j] = filenames[i];
		j++;
	}
	for (int i = 0; i < size; i++){	
		filenames[i] = temp[i];
	}
}

void d(char ** filenames){
	strcpy(filenames[0], ".");
	size = 1;
}

void F(char ** filenames){
	for(int i = 0; i < size; i++) {
		FHelper(filenames[i]);
	}
}

//----		end of ls arguments		   ----
//---------------------   -----------------
//---------------------   -----------------
void lsFunction(int argc, char ** argv){
	
	allArgv(argc, argv);
	if(!strcmp(argv[1], "ls")) { a("."); ls(filenames); }//l(filenames);
	if(!strcmp(argv[1], "ls") && argc > 2){
		if(isThere(argvs, 'A')) { a("."); A(filenames);}
		if(isThere(argvs, 'a')) { a("."); }
		if(isThere(argvs, 'f')) { sorted = 0; a("."); }
		if(isThere(argvs, 'S')) sizeSorter(filenames);
		if(isThere(argvs, 'r')) r(filenames);
		if(isThere(argvs, 'd')) d(filenames);
		if(isThere(argvs, 'F')) F(filenames);
		if(isThere(argvs, 'h')) ish=1;
		if(isThere(argvs, 'i') && !isThere(argvs, 'l') && !isThere(argvs, 'n')) { iFunc(filenames); }
		if(isThere(argvs, 'i') && (isThere(argvs, 'l') || isThere(argvs, 'n'))) { isi = 1; }
		if(isThere(argvs, 'R')) { R(".", 0); }
	}
	if (isThere(argvs, 'l')) { l(filenames); }
	if (isThere(argvs, 'm')) { m(); } 
	if (isThere(argvs, 'n')) { isn =1; l(filenames); } 
	if(!isThere(argvs, 'l') && !isThere(argvs, 'm') && !isThere(argvs, 'n')) simplePrint(filenames);

}
int main(int argc, char ** argv){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	lsFunction(argc, argv);
	//printf("lines: %d    columns: %d\n", w.ws_row, w.ws_col);
	return 0;

}