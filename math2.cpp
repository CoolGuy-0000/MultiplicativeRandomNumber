#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <Windows.h>


int main() {

	FILE* result = fopen("result.txt", "w");

	 
	uint32_t data = 7;
	uint32_t data2 = 2;
	uint32_t data3 = 3;
	for (size_t i = 0; i < 1000000; i++) {
		data *= 3;
		data2 *= 3;
		data3 *= 3;

		if (data & 0xFFFF0000) {
			uint16_t low_part = data & 0xFFFF;
			uint16_t high_part = (data >> 16) & 0xFF;

			data = high_part + low_part;
			data2 += data;
			data3 += data;

			data2 &= 0xFFFF;
			data3 &= 0xFFFF;
		}
		if (data2 & 0xFFFF0000) {
			uint16_t low_part = data2 & 0xFFFF;
			uint16_t high_part = (data2 >> 16) & 0xFF;

			data2 = high_part + low_part;
			data += data2;
			data3 += data2;

			data &= 0xFFFF;
			data3 &= 0xFFFF;
		}
		if (data3 & 0xFFFF0000) {
			uint16_t low_part = data3 & 0xFFFF;
			uint16_t high_part = (data3 >> 16) & 0xFF;

			data3 = high_part + low_part;
			data += data3;
			data2 += data3;
			data &= 0xFFFF;
			data2 &= 0xFFFF;
		}


		fprintf(result, "%X,", data);

		if (i % 100 == 99) {
			fprintf(result, "\n");
		}
	}

	fclose(result);
	return 0;
}