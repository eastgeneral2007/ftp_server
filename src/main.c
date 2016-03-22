#include "helper_fnc.h"

int
main(int argc, char *argv[])
{
	char * cur;
	char root[] = "/home/ggrrin/Documents/ftp_server/bin/";
	char params[] = "taky jen tak pro radost../player.blend2";
	char *full_path = get_full_path(root, &cur, params);
	free(cur);
	return 0;
}
