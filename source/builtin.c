#include "gsh.h"
#include <ctype.h>
#define BUFFSIZE 256
//#define DEBUG
static int createDir(const char* path);
static int copy (const char* src_path, const char* dst_path);
static int compare (const char* fir_path, const char* sec_path);
static int buf_get (int fd, char* buf, int length, int* n);
static int buf_cmp (const char* buf1, const char* buf2, int length, int* lines);
static int buf_cnt (const char* buf, int* lines, int* words, int* chars, int length);
int cd (char** parameters)
{
    char* path = NULL;
    extern struct passwd* pwd;
    extern int errno;
    pwd = getpwuid(getuid());
    if (parameters[1] == NULL)
    {
        path = (char*) malloc(3 * sizeof(char));
        strcpy(path, "..");
    }
    else if (parameters[1][0] == '~')
    {
        path = (char* ) malloc((strlen(pwd->pw_dir) + 
                    strlen(parameters[1]) * sizeof(char)));
        strcpy(path, pwd->pw_dir);
        strcpy(path+strlen(path), parameters[1]+1);
    }
    else
    {
        path = (char*) malloc((strlen(parameters[1])+1) * sizeof(char));
        strcpy(path, parameters[1]);
    }
    if (chdir(path) != 0)
    {
        puts(strerror(errno));
        free(path);
        return -1;
    }
    return 0;
}

int cp (char** parameters)
{
    extern int errno;
    int i, args, flag;
    struct stat isdir;
    
    extern struct passwd* pwd;

    char** src_path, * dst_path = NULL;
    flag = 0;
    src_path = (char** ) malloc((MAXARG+1) * sizeof(char*));
    for (i = 0; i <= MAXARG; i++)
        src_path[i] = NULL;
    args = 0;
    for (i = 1; i < MAXARG; ++i) 
    {
        if (parameters[i] == NULL)
            break;
        else
            args++;
    }
#ifdef DEBUG
    printf("args: %d\n", args);
    puts("Before Processing: ");
    for (i = 1; i <= args; ++i) 
    {
        printf("src_path[%d]: %s\n", i, src_path[i]);
    }
#endif
    if (args < 2)
    {
        puts("Invalid Input");
        return -1;
    }

    for (i = 1; i < args; ++i) 
    {
        if (parameters[i][0] == '~')
        {
            src_path[i] = (char* ) malloc(strlen(pwd->pw_dir) + 
                        strlen(parameters[i]));
            strcpy(src_path[i], pwd->pw_dir);
            strcpy(src_path[i]+strlen(src_path[i]), parameters[1]+1);
        }
        else
        {
            src_path[i] = (char* ) malloc(strlen(parameters[i])+1);
            strcpy(src_path[i], parameters[i]);
        }
        if (stat(src_path[i], &isdir) == -1)
        {
            perror("source");
            flag = 1;
            goto clear;
        }
        else if(! S_ISREG(isdir.st_mode))
        {
            puts("not supported source: directory");
            flag = 1;
            goto clear;
        }
    }
    if (parameters[i][0] == '~')
    {
        src_path[i] = (char* ) malloc(strlen(pwd->pw_dir) + 
                        strlen(parameters[i]));
        strcpy(src_path[i], pwd->pw_dir);
        strcpy(src_path[i]+strlen(src_path[i]), parameters[1]+1);
    }
    else
    {
        src_path[i] = (char* ) malloc(strlen(parameters[i])+1);
        strcpy(src_path[i], parameters[i]);
    }
#ifdef DEBUG
    puts("After Processing");
    for (i = 1; i <= args; ++i) 
    {
        printf("src_path[%d]: %s\n", i, src_path[i]);
    }
#endif
    if (stat(src_path[args], &isdir) == -1)
    {
        //the destiny does not exist
        if (args > 2)
        {
            perror("destination: not found");
            flag = 1;
            goto clear;
        }
        else
        {
            if (strlen(src_path[args]) > 1)
            {
                char* p;
                p = src_path[args] + strlen(src_path[args]) - 2;
                for (;*p != '/' && p != src_path[args];p--);
                if (p != src_path[args])
                {
                    *p = '\0';
                    if (access(src_path[args], F_OK) == -1)
                    {
                        strerror(errno);
                        *p = '/';
                        flag = 1;
                        goto clear;
                    }
                    else
                    {
                        *p = '/';
                        dst_path = src_path[args];
                    }
                }
                else dst_path = src_path[args];
            }
            else dst_path = src_path[args];
            flag = copy(src_path[1], dst_path);
        }
    }
    else if (S_ISREG(isdir.st_mode))
    {
        if (args == 2)
            dst_path = src_path[args];
        else
        {
            puts("Invalid Input");
            flag = 1;
            goto clear;
        }
        flag = copy(src_path[1], dst_path);
    }
    else if (S_ISDIR(isdir.st_mode))
    {
        for (i = 1; i < args; ++i) 
        {
            char* p = src_path[i]+strlen(src_path[i])-1;
            while(p != src_path[i] && *p != '/')
                p--;
            if (p != src_path[i])
                p++;
            dst_path = (char*) malloc(strlen(src_path[args]) + 
                    strlen(p)+2);
            strcpy(dst_path, src_path[args]);
            if (dst_path[strlen(dst_path)-1] != '/')
            {
                strcpy(dst_path+strlen(dst_path), "/");
            }
            strcpy(dst_path+strlen(dst_path), p);
            flag = copy(src_path[i], dst_path);
            free(dst_path);
        }
    }

clear: for (i = 1; i <= args; ++i) 
    {
        free(src_path[i]);
    }
    free(src_path);
    return flag;
}

int cmp (char** parameters)
{
    extern int errno;
    int i, args, flag;
    struct stat isdir;
    extern struct passwd* pwd;
    char** src_path;
    flag = 0;
    src_path = (char** ) malloc((MAXARG+1) * sizeof(char*));
    for (i = 0; i <= MAXARG; i++)
        src_path[i] = NULL;
    args = 0;
    for (i = 1; i < MAXARG; ++i) 
    {
        if (parameters[i] == NULL)
            break;
        else
            args++;
    }
    if (args != 2)
    {
        puts("Invalid Input");
        return -1;
    }
    for (i = 1; i <= args; ++i) 
    {
        if (parameters[i][0] == '~')
        {
            src_path[i] = (char* ) malloc(strlen(pwd->pw_dir) + 
                        strlen(parameters[i]));
            strcpy(src_path[i], pwd->pw_dir);
            strcpy(src_path[i]+strlen(src_path[i]), parameters[1]+1);
        }
        else
        {
            src_path[i] = (char* ) malloc(strlen(parameters[i])+1);
            strcpy(src_path[i], parameters[i]);
        }
        if (stat(src_path[i], &isdir) == -1)
        {
            printf("cmp: %s: %s\n", src_path[i], strerror(errno));
            flag = 1;
            goto clear;
        }
        else if(! S_ISREG(isdir.st_mode))
        {
            printf("cmp: %s: Is a directory", src_path[i]);
            flag = 1;
            goto clear;
        }
    }
#ifdef DEBUG
    printf("args: %d\n", args);
    puts("Before Processing: ");
    for (i = 1; i <= args; ++i) 
    {
        printf("src_path[%d]: %s\n", i, src_path[i]);
    }
#endif
    flag = compare(src_path[1], src_path[2]);
    
clear: for (i = 1; i <= args; ++i) 
    {
        free(src_path[i]);
    }
    free(src_path);
    return flag;
}
int wc (char** parameters)
{
    extern int errno;
    int i, args, flag, src_fd, readin;
    struct stat isdir;
    extern struct passwd* pwd;
    char** src_path;
    char buffer[BUFFSIZE];
    int lines, words, chars;
    flag = 0;
    lines = words = chars = 0;
    src_path = (char** ) malloc((MAXARG+1) * sizeof(char*));
    for (i = 0; i <= MAXARG; i++)
        src_path[i] = NULL;
    args = 0;
    for (i = 1; i < MAXARG; ++i) 
    {
        if (parameters[i] == NULL)
            break;
        else
            args++;
    }
    if (args > 2 || args == 0)
    {
        puts("Invalid Input");
        return -1;
    }
    if (parameters[args][0] == '~')
    {
        src_path[args] = (char* ) malloc(strlen(pwd->pw_dir) + 
                    strlen(parameters[args]));
        strcpy(src_path[args], pwd->pw_dir);
        strcpy(src_path[args]+strlen(src_path[args]), parameters[1]+1);
    }
    else
    {
        src_path[args] = (char* ) malloc(strlen(parameters[args])+1);
        strcpy(src_path[args], parameters[args]);
    }
    //up till now, the path has been parsed
    if (stat(src_path[args], &isdir) < 0)
    {
        printf("wc: %s: %s\n", src_path[args], strerror(errno));
        flag = -1;
        goto clear;
    }
    else if (S_ISDIR(isdir.st_mode))
    {
#ifdef DEBUG
        printf("args: %d\nargs[1]: %s\n", args, src_path[1]);
#endif
        printf("wc: %s: Is a directory\n", src_path[args]);
        if (args == 1)
            printf("      0       0       0 %s\n", 
                src_path[args]);
        else if (strcmp("-l", parameters[1]) == 0)
            printf("0 %s\n", src_path[args]);
        goto clear;
    }

    src_fd = open(src_path[args], O_RDONLY, 0755);
    if (src_fd == -1)
    {
        printf("wc: %s: %s\n", src_path[args], strerror(errno));
    }
    else
    {
        while((readin = read(src_fd, buffer, BUFFSIZE)) > 0)
            buf_cnt(buffer, &lines, &words, &chars, readin);
        if (readin < 0)
        {
            printf("wc: %s: %s\n", 
                    src_path[args], strerror(errno));
            flag = -1;
            goto clear;
        }
        if (args == 1)
            printf("%d %d %d %s\n", 
                    lines, words, chars, src_path[args]);
        else if (strcmp(parameters[1], "-l") == 0)
            printf("%d %s\n", lines, src_path[args]);
        else
        {
            printf("wc: %s: Invalid Input\n", src_path[1]);
            flag = -1;
            goto clear;
        }
    }
clear:  free(src_path[args]);
    free(src_path);
    return flag;
}

int quit (void)
{
    exit(0);
}

int philosophy (void)
{
	puts("DDDDDDD#W;,.. . .... ::.,;DDDDDDDDDDDDDD");
	puts("DDDDDDK#D:. .. . .     .::DDDDDDDDDDDDDD");
	puts("DDDDEtjGt:; .:::..    .. .tDDDDDDDDDDDDD");
	puts("DDDDDt,,;;..,;ii;;,,,:    .DDDDDDDDDDDDD");
	puts("DDDEGii,ii,;ftfffjtii;:.   DDDDDDDDDDDDD");
	puts("EDDWLj;ifWEtfGGDDDLjii;:.  ,EDDDDDDDEDDD");
	puts("DEDLjttfLKWGGLLEDDGftti;.  .DDDDDDDDDDDD");
	puts("EDDLjfLDEKDDGDDKGGGftiti:.  DDDDDEEDDDDD");
	puts("DDDtLfLKEELGKEDKGGGLjtit;,..DDDDDDEDDDDD");
	puts("DEDjDfGKKDKKKKKKDGGLjjitt;:.DDEEDEDDEEDD");
	puts("EEEL;tLEKKWWWKDEELftii;itt,:.EDEDDEDDDDD");
	puts("EEED;;fEKKWWWEGGGt,..,;;ii;:.EDEDEEDDEDE");
	puts("EEEE;,jEKKKKGjffftttiittti;.;tEEEEDDEDEE");
	puts("EEEED;tDKKGi:,ijfjj,::;iitt.j;GEEEEEEEEE");
	puts("EEEEE,tEKj;,;;itDjt.,;tiiit;jfiDEEEEEEEE");
	puts("EEEEE,iKDiji:.:GWLt;,;;;ittt,itEEDEEEEEE");
	puts("EEEEEE,KKti.,;;KWLtiii;iitjt,,jEEEEEEEED");
	puts("EEEEEE;LELEfi;WK#Ltjjjfttjjj,;jEEEEEEEEE");
	puts("EEEEEEjtKKWWW##WWEGDfitjjjjjiftEEEEEEEEE");
	puts("EEEEEEjDKEKK#KjWKDjjft;tjjftitjEEEEEEEEE");
	puts("EEEEEELLEEEKKfiEGjji;ii;tftjtKEEEEEEEEEE");
	puts("EEEEEEKDjEEELt;G:,ti;iiitttjtGEEEEEEEEEE");
	puts("EEEEEEEWKDEGji;GL;,,tjjtiijtjtEEEEEEEEEE");
	puts("EEEEEEEWKGDLj;iGLtitjLt.;;jitfEEEEEEEEEE");
	puts("EKEKEEEEWDGif;iiijGLfj.ii;tiiGEKEEEEEEEE");
	puts("EKKEEEEEKKGtLi;.,fG,i:iti;tiiEEEEEEEEEEE");
	puts("KKEEEEEKEELfDt;;WWKDittti;titLEEEEEEEEEE");
	puts("EEEEEKKEKEEfjj;tDGLjtiti;;iiitEKEEEEEEEE");
	puts("KKKEKKKKEEEDtt;tGLt;,;;;,;i;itKEEEEEEEEE");
	puts("KKKKKKEKKEEKjiitfGfj,itii;;itt;EKEKEEEKE");
	puts("KKKKKKKKEKEKGjitfLDEGLt;i,iitttjKKKKKEEE");
	puts("KEEEKKKKKKKEEGttjLLfjii;,;ititt;tKEEEEEK");
	puts("KKKKKKKKKKKKKDLtiij;;iii;;;itttti;EKKKEK");
	puts("KKKKKKKKKKKKKDDji,;i;i;;;iiittti;;;DKKKK");
	puts("KKKKKKKKKKKKKEDLj;;;;iii;;iiittti;i;;KEE");
	puts("KKKKKKKKKKKKWKEDLfi;iiii;;iiiiiii;;iii;E");
	puts("KKKKKKKKKKKW#KEDLt;iiii;;;iiiiiiii;iiiii");
	puts("KKKKKKKKKW##WWEELt;iiii;;;;;;;iiii;iiiii");
	puts("KKKKKKKK###WWWEEDt;;iii;;;,;;;;;;;;iiii;");
	puts("KKKKKKWW##WKKKKEELt;;;i;;,,,,,,,,,;;;;ii");
	puts("KKKKKKEKWKKKKKKKEGLjfDKEKi;;;;;;,;;;;;;;");
	puts("KKKEEEEWKKKKKWKEDGGGGEEKEEK;;;;;,,,;,,;;");
	puts("KEEEEKKKEKKKKWKEDGLLEKEEDDDGL;;;,,;,;;,i");
	puts("KEKKKKEEKKKKKKKEGGGLEKKDDDGDDDDjiijjjjjG");
	puts("KKKWKKEEEEKEEEEDDDDEKEEDDDGDDEEEEEEEDDDE");
	puts("KKWKEEEKEEDKEEEDGDDEKKEDEDEDDDEEKEEDEDEE");
	puts("KWKEEEKDEDEKEDDDDEEWWKEKDEEKEEEEKKEKEEEE");
	puts("KEEDEEDEEEEEDDDGDKKEKKKKKDDEKEKEKEKKKEDE");
	puts("KKKKEEEEEDEEfGLLDWKWEKKEEEGEEEEKEDEEDKDE");
	puts("WKKDEKEELEGLGGLDDKWEWWKEEEEKKKKEKEDEDEDE");
	puts("WWKKDKEGGDDDDGfLEWEDWWKEDKKEEEKEKEEDEEED");
	puts("WWKKGDfDDGDGGLLLWEKWKKKWKKKKEEKDEEDEDDDD");
	puts("WKKKEDDGDDGGfLLGEKKKKKWEWEKEEEEEDDGGDGDD");
	puts("WWWWEKELGGDjGjfLKEKWKEKKKKKKDDGDDDDGGGGG");
	puts("W#WKEEDGLfLLLfjDKEKEEWKEKEKEEDDGLfGGGLLG");
	puts("KWKWKKGGfjLffjiLEEKKKWKEKKEDGDGLGLfjfLLf");
    return 0;
}

int builtin_command(char** command, char** parameters)
{
    if (strcmp(*command, "cd") == 0)
    {
        if(cd(parameters) == 0)
            return 1;
        else
            return -1;
    }
    else if (strcmp(*command, "cp") == 0)
    {
        if(cp(parameters) == 0)
            return 1;
        else
            return -1;
    }
    else if (strcmp(*command, "cmp") == 0)
    {
        if(cmp(parameters) == 0)
            return 1;
        else
            return -1;
    }
    else if (strcmp(*command, "wc") == 0)
    {
        if(wc(parameters) == 0)
            return 1;
        else
            return -1;
    }
    else if (strcmp(*command, "quit") == 0)
    {
        quit();
        return 1;
    }
    else if (strcmp(*command, "philosophy") == 0)
    {
        if(philosophy() == 0)
            return 1;
        else
            return -1;
    }
    return 0;
}

int createDir(const char *path)
{
    char DirName[BUFFSIZE];
    strcpy(DirName, path);
    int i,len = strlen(DirName);
    if(DirName[len-1]!='/')
        strcat(DirName, "/");
 
    len = strlen(DirName);
 
    for(i=1; i<len; i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i] = 0;
            if(access(DirName, F_OK)!=0)
                {
                    if(mkdir(DirName, 0755)==-1)
                        { 
                            perror("mkdir error"); 
                            return -1; 
                        }
                }
            DirName[i] = '/';
        }
    }
 
    return 0;
} 

static int copy (const char* src_path, const char* dst_path)
{
    int src_fd, dst_fd;
    int readin = 0;
    char buffer[BUFFSIZE];
    extern int errno;
    src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1)
    {
        perror("open");
        return -1;
    }
    dst_fd = open(dst_path, O_WRONLY | O_CREAT, 0755);
    if (dst_fd == -1)
    {
        perror("open");
        return -1;
    }
#ifdef DEBUG
    printf("Copying %s to %s now. \n", src_path, dst_path);
#endif
    while ((readin = read(src_fd, buffer, BUFFSIZE)) > 0)
    {
        if (readin == -1)
        {
            perror("read");
            close(src_fd);
            close(dst_fd);
            return -1;
        }
        else if (write(dst_fd, buffer, readin) == -1)
        {
            perror("write");
            close(src_fd);
            close(dst_fd);
            return -1;
        }
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}
static int compare (const char* fir_path, const char* sec_path)
{
    extern int errno;
    int fir_fd, sec_fd;
    int fir_readin, sec_readin;
    int lines, chars, letters;
    char fir_b[BUFFSIZE], sec_b[BUFFSIZE];
    lines = 1;
    chars = 0;
    fir_readin = 0;
    sec_readin = 0;
#ifdef DEBUG
    printf("Comparing %s to %s now. \n", fir_path, sec_path);
#endif
    fir_fd = open(fir_path, O_RDONLY, 0755);
    if (fir_fd == -1)
    {
        perror("cmp: open");
        return -1;
    }
    sec_fd = open(sec_path, O_RDONLY, 0755);
    if (sec_fd == -1)
    {
        perror("cmp: open");
        close(fir_fd);
        return -1;
    }
    buf_get(fir_fd, fir_b, BUFFSIZE, &fir_readin);
    buf_get(sec_fd, sec_b, BUFFSIZE, &sec_readin);
    while ((fir_readin > 0) || (sec_readin > 0))
    {
        if (fir_readin == -1)
        {
            printf("cmp: %s: %s\n", 
                    fir_path, strerror(errno));
            close(fir_fd);
            close(sec_fd);
            return -1;
        }
        else if (sec_readin == -1)
        {
            printf("cmp: %s: %s\n", 
                    sec_path, strerror(errno));
            close(fir_fd);
            close(sec_fd);
            return -1;
        }
        else if (fir_readin == 0)
        {
            if (sec_readin != 0) 
                printf("cmp: EOF on %s\n", fir_path);
            close(fir_fd);
            close(sec_fd);
            return 0;
        }
        else if (sec_readin == 0)
        {
#ifdef DEBUF
            printf("fir_readin: %d\nsec_readin: %d\n", 
                    fir_readin, sec_readin);
#endif
            if (fir_readin != 0)
                printf("cmp: EOF on %s\n", sec_path);
            close(fir_fd);
            close(sec_fd);
            return 0;
        }
        //up til now, line-gaps can be ignored
        else if (fir_readin == sec_readin)
        {
            letters = buf_cmp(fir_b, sec_b, fir_readin, &lines);
            if (letters == 0)
            {
                chars += fir_readin;
                buf_get(fir_fd, fir_b, BUFFSIZE, &fir_readin);
                buf_get(sec_fd, sec_b, BUFFSIZE, &sec_readin);
                continue;
            }
            else
            {
#ifdef DEBUG
                printf("%c != %c\n", 
                        fir_b[letters-1], sec_b[letters-1]);
                for (int i = 0; i < fir_readin; i++)
                    putchar(fir_b[i]);
                putchar('\n');
                for (int i = 0; i < sec_readin; i++)
                    putchar(sec_b[i]);
                putchar('\n');
#endif
                chars += letters;
                printf("%s %s differ: byte %d, line %d\n", fir_path, 
                        sec_path, chars, lines);
                break;
            }
        }
        else
        {
            if (fir_readin < sec_readin)
                printf("cmp: EOF on %s\n", fir_path);
            else
                printf("cmp: EOF on %s\n", sec_path);
        }
        buf_get(fir_fd, fir_b, BUFFSIZE, &fir_readin);
        buf_get(sec_fd, sec_b, BUFFSIZE, &sec_readin);
    }
    if (fir_readin == -1)
    {
        printf("cmp: %s: %s\n", 
                fir_path, strerror(errno));
        close(fir_fd);
        close(sec_fd);
        return -1;
    }
    else if (sec_readin == -1)
    {
        printf("cmp: %s: %s\n", 
                sec_path, strerror(errno));
        close(fir_fd);
        close(sec_fd);
        return -1;
    }

    close(fir_fd);
    close(sec_fd);
    return 0;
}

static int buf_get (int fd, char* buf, int length, int* n)
{
    *n = read(fd, buf, length);
    return *n;
}
static int buf_cmp (const char* buf1, const char* buf2, int length, int* lines)
{
    int i;
    for(i = 0; i < length; i++)
    {
        if (buf1[i] != buf2[i])
            return i+1;
        else if (buf1[i] == '\n')
            (*lines)++;
    }
    return 0;
}
static int buf_cnt (const char* buf, int* lines, int* words, int* chars, int length)
{
    static int update_word = false;
    static int i;
    (*chars) += length;
    for (i = 0; i < length; i++)
    {
        if (isgraph(buf[i]))
        {
            if (update_word == false)
            {
                update_word = true;
                (*words)++;
            }
            else continue;
        }
        else
        {
            update_word = false;
            if (buf[i] == '\n' || buf[i] == EOF)
                (*lines)++;
        }
    }
    return 0;
}
