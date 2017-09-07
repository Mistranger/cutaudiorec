#include "main.h"
#include "wad.h"

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
		return "";
	}
    return dot + 1;
}

// cutaudiorec by cybermind - removes AUDUIREC lump from demo footer
// (wrote in 15 mins, haven't bothered about code style)
int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: cutaudiorec demofile\n");
		exit(1);
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("error opening file\n");
		exit(1);
	}

	int size, wpos;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char buf[4];
	int c = 0;
	do {
		fseek(f, c++, SEEK_SET);
		fread(buf, 1, 4, f);
	} while (strncmp(buf, "PWAD", 4));
	fseek(f, c - 1, SEEK_SET);
	c = wpos = ftell(f);
	int wsize = size - c;
	char *wad = new char[wsize];
	fread(wad, 1, wsize, f);
	fclose(f);

	f = fopen("demoexfooter.wad", "wb");
	if (!f) {
		printf("error opening footer\n");
		exit(1);
	}
	fwrite(wad, 1, wsize, f);
	fclose(f);

	wFile_t *footer = Wad_Open("demoexfooter.wad");
	wEntry_t *audio = Wad_FindEntry(footer, "AUDIOREC", NULL, false);
	if (!audio) {
		printf("audiorec lump isn't in demo\n");
		exit(1);
	}
	Wad_DeleteEntry(footer, audio);
	Wad_Output(footer, "demoexfooternew.wad");
	Wad_Close(footer);

	char newfilename[256];
	const char *dot = strrchr(argv[1], '.');
	if (!dot || dot == argv[1]) {
		sprintf(newfilename, "%s-novoice.lmp", argv[1]);
	} else {
		memcpy(newfilename, argv[1], dot - argv[1]);
		memcpy(newfilename + (dot - argv[1]), "-novoice.lmp", strlen("-novoice.lmp"));
		newfilename[dot - argv[1] + strlen("-novoice.lmp")] = 0;
	}

	f = fopen(newfilename, "wb");
	FILE *f2 = fopen(argv[1], "rb");
	int f2c = 0;
	while (f2c < wpos) {
		fputc(fgetc(f2), f);
		++f2c;
	}
	FILE *f3 = fopen("demoexfooternew.wad", "rb");
	fseek(f3, 0, SEEK_END);
	int f2size = ftell(f3);
	fseek(f3, 0, SEEK_SET);
	int f3c = 0;
	while (f3c < f2size) {
		fputc(fgetc(f3), f);
		++f3c;
	}
	fclose(f);
	fclose(f2);
	fclose(f3);
	remove("demoexfooternew.wad");
	remove("demoexfooter.wad");
	remove("demoexfooter.wad");



	return 0;
}
