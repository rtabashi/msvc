#include <stdio.h>
#include <string.h>

void usage(char *prog_name)
{
	printf("Usage:\n");
	printf("	rc4.exe hex_key hex_data\n");
}

void swap(unsigned char* c1, unsigned char* c2)
{
	unsigned char temp;

	temp = *c1;
	*c1 = *c2;
	*c2 = temp;
}

void init_rc4(unsigned char* table, unsigned char* key, size_t key_size)
{
	int				i;
	unsigned char	index1 = 0;
	unsigned char	index2 = 0;

	for (i = 0; i < 0x100; i++) {
		table[i] = i;
	}
	for (i = 0; i < 0x100; i++) {
		index2 = key[index1] + table[i] + index2;
		swap(&table[i], &table[index2]);
		index1++;
		if ((size_t)index1 == key_size) {
			index1 = 0;
		}
	}
}

void rc4(unsigned char* table, unsigned char** dst_data, unsigned char* src_data, size_t data_size)
{
	unsigned char*	data;
	unsigned int	i;
	unsigned char	x = 0;
	unsigned char	y = 0;

	data = (unsigned char*)malloc(data_size + 1);
	memset(data, 0, data_size + 1);

	for (i = 0; i < data_size; i++) {
		x = x + 1;
		y = table[x] + y;
		swap(&table[x], &table[y]);
		data[i] = src_data[i] ^ table[(table[x] + table[y]) & 0xff];
	}
	*dst_data = data;
}

void do_rc4(unsigned char** dst_data, unsigned char* key, unsigned char* src_data)
{
	unsigned char	table[0x100];
	size_t			key_size = strlen(key);
	size_t			data_size = strlen(src_data);

	init_rc4(table, key, key_size);
	rc4(table, dst_data, src_data, data_size);
}

unsigned char char_to_hex(unsigned char c)
{
	if ('0' <= c && c <= '9')
		return c - '0';
	else if ('A' <= c && c <= 'Z')
		return c - 0x37;
	else
		return c - 0x57;
}

unsigned char* ascii_to_bin(unsigned char* ascii)
{
	unsigned char*	bin;
	unsigned char	h_hex = 0;
	unsigned char	l_hex = 0;
	size_t			len;
	int				i;

	len = strlen(ascii) / 2;
	bin = (unsigned char*)malloc(len + 1);
	memset(bin, 0, len + 1);

	for (i = 0; i < len; i++) {
		h_hex = char_to_hex(ascii[i*2]);
		l_hex = char_to_hex(ascii[i*2+1]);
		bin[i] = (h_hex << 4) ^ l_hex;
	}
	return bin;
}

int main(int argc, char* argv[])
{
	unsigned char*	key;
	unsigned char*	src_data;
	unsigned char*	dst_data;

	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	key = ascii_to_bin(argv[1]);
	src_data = ascii_to_bin(argv[2]);

	do_rc4(&dst_data, key, src_data);

	for (int i = 0; i < strlen(dst_data); i++) {
		printf("%02x", dst_data[i]);
	}
	printf("\n");

	return 0;
}