#include <stdio.h>
#include <locale.h>
#ifdef unix
#include <iconv.h>
#include <string.h>
#endif
#define BUF_SIZE 256
#define _STR(x) #x
#define STR(x) _STR(x)

int main()
{
	FILE *fp;
	unsigned char buf[BUF_SIZE] = {0};
	int nhanzi = 0, nother = 0;

	if ((fp = fopen("HZK16", "rb")) == NULL) {
		perror("HZK16");
		return 1;
	}
	printf("输入一行汉字: ");
	scanf("%" STR(BUF_SIZE) "s", (char*)buf);
#ifdef unix
	{
		iconv_t cd;
		size_t inleft = BUF_SIZE, outleft = BUF_SIZE;
		unsigned char gbk[BUF_SIZE];
		char *inptr = (char*)buf, *outptr = (char*)gbk;
		if ((cd = iconv_open("GBK", "UTF-8")) == (iconv_t)-1) {
			perror("iconv");
			return 1;
		}
		if (iconv(cd, &inptr, &inleft, &outptr, &outleft) == -1) {
			perror("iconv");
			return 1;
		}
		strncpy((char*)buf, (char*)gbk, BUF_SIZE);
	}
#endif
	for (int i = 0; buf[i]; i++) {
		// GB2312 encoding range 0xA1A1~0XFEFE
		if (buf[i] < 0xA1 || buf[i] > 0xFE || buf[i + 1] < 0xA1 || buf[i + 1] > 0xFE) {
			nother++;
			// GBK character that does not exist in GB2312
			if (buf[i] >= 0x81 && buf[i] <= 0xFE && buf[i + 1] >= 0x40 && buf[i + 1] <= 0xFE)
				i++;
			continue;
		}
		fseek(fp, (94 * (buf[i] - 0xA1) + (buf[i + 1] - 0xA1)) * 32, SEEK_SET);
		for (int j = 0; j < 32; j++) {
			int halfrow = fgetc(fp);
			for (int b = 0x80; b; b >>= 1)
				putchar(halfrow & b ? '*' : ' ');
			if (j & 1) {
				putchar('\n');
				printf("\033[%dC", nhanzi * 16);
			}
		}
		nhanzi++;
		printf("\033[16A\033[16C");
		i++;
	}
	printf("\033[15B\n");
	if (nother)
		printf("跳过了%d个不支持的字符\n", nother);
	return 0;
}