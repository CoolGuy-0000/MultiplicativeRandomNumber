#include <stdio.h>
#include <stdint.h>

int main() {

	uint32_t data = 1;
	uint32_t data2 = 2;
	for (size_t i = 0; i < 10000; i++) {
		data *= 3;
		data2 *= 7;

		if (data & 0xFFFF0000) {
			uint16_t carried_num = ((data & 0xFFFF0000) >> 16) & 0xFFFF;

			data += carried_num + data2;
			data &= 0xFFFF;
		}
		if (data2 & 0xFFFF0000) {
			uint16_t carried_num = ((data2 & 0xFFFF0000) >> 16) & 0xFFFF;

			data2 += carried_num + data;
			data2 &= 0xFFFF;
		}

		fprintf(stdout, "%u\n", data);
	}

	return 0;
}