#define MESSAGE_CD "new dir (? for help):" 
#define MESSAGE_FILE "Choose file:"
#define OK "OK\n"
#define MAX_REQUEST 2000
#define MESSAGE_CD_HELP "\n\t..\tthe parent directory\n\t/\ta new absolute directory\n\t\ta new directory relative to the current position\n\t[?]\tthis menu\n\t[q]\tleave this menu\nnew dir (? for help): "
#define MESSAGE_HELP "[1]: lists content of current directory\n[2] print name of current directory\n[3]change current directory\n[4] gets file information\n[5] displays a file\n[?] gets you some help\n[q] lets you leave a menu\nHit any key and enter to leave this menu\n"
#define MESSAGE_MAIN "\nPlease press a key:\n [1] list content of current directory (ls)\n [2] print name of current directory (pwd)\n [3] change current directory (cd)\n [4] get file information\n [5] display file (cat)\n [?] this menu\n [q] quit\ncmd (? for help)> "
#define ERROR "Something went wrong, dude\n"
#define ERROR_CD "You're trying the forbidden land.\n"

int proto_recv(int sock, char *buf, int max_size, int flags);
int proto_send(int sock, char *msg, int len, int flags);
