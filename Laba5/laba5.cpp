#include <iostream>
#include <locale.h>
#include <ctime>
#include <Windows.h>
#define BITMAP_HEADER_SIZE 40 + 14 
#define CANT_OPEN_C_OUTPUT -2
#define CANT_OPEN_ASSEMBLER_OUTPUT -3
#define CANT_OPEN_MMX_OUTPUT -4
#define STEP 64

int getInt(int left = 0, int right = 255)
{
	while (true) // цикл продолжается до тех пор, пока пользователь не введет корректное значение
	{
		int number;
		std::cin >> number;

		// Проверка на предыдущее извлечение
		if (std::cin.fail()) // если предыдущее извлечение оказалось неудачным,
		{
			std::cin.clear(); // то возвращаем cin в 'обычный' режим работы
			std::cin.ignore(32767, '\n'); // и удаляем значения предыдущего ввода из входного буфера
		}
		else
		{
			std::cin.ignore(32767, '\n'); // удаляем лишние значения
			if (number >= left && number <= right) return number;
		}
		std::cout << "\n\tУпс, введённое значение неверно. Попробуйте ещё раз.\n\t";
	}
}

time_t ci_prog(FILE* in, FILE* out, unsigned width, unsigned heigth, __int8 alpha)
{
	const unsigned width_pix = width * 4;
	unsigned __int8* buffer = new unsigned unsigned __int8[width_pix];
	unsigned long long int start_time = clock();
	for (int i = 0; i < heigth; i++)
	{
		fread(buffer, 1, width_pix, in);

		for (int j = 0, step = 0; j < width_pix; j++, step++)

			if (step == STEP * 4)
			{
				step = 0;
				j += STEP * 4;
			}
			else
				if (buffer[j + 0] + alpha > 255)
					buffer[j + 0] = 255;
				else	 buffer[j + 0] += alpha;


		fwrite(buffer, 1, width_pix, out);
	}
	fseek(in, BITMAP_HEADER_SIZE, SEEK_SET);
	return clock() - start_time;
}

time_t asm_scal(FILE* in, FILE* out, unsigned width, unsigned height, __int8 alpha)
{
	__int32 ost = 4 * STEP,
		vh = height * (((width - (width % STEP)) / STEP) / 2),
		x = (width % STEP) * 4,
		y = (width - (width % STEP)) * 4,
		a;

	unsigned __int8 *buffer = new unsigned __int8[width * height * 4],
		*buffer1 = new unsigned __int8[width * height * 4];
	fread(buffer, 1, width * height * 4, in);
	for (int i = 0; i < width * height * 4; i++)
		buffer1[i] = buffer[i];

	unsigned long long int start_time = clock();
	__asm
	{
		mov edx, 0
		mov esi, buffer
		mov edi, buffer1
		mov ecx, vh
		lvn :
		push ecx
			mov ecx, 64
			l :
			mov al, 255
			sub al, [esi]
			cmp al, alpha
			ja label1
			mov al, 255
			jmp l1
			label1 :
		mov al, [esi]
			add al, alpha
			l1 :
		mov[edi], al
			inc esi
			inc edi
			mov al, 255
			sub al, [esi]
			cmp al, alpha
			ja label2
			mov al, 255
			jmp l2
			label2 :
		mov al, [esi]
			add al, alpha
			l2 :
		mov[edi], al
			inc esi
			inc edi
			mov al, 255
			sub al, [esi]
			cmp al, alpha
			ja label3
			mov al, 255
			jmp l3
			label3 :
		mov al, [esi]
			add al, alpha
			l3 :
		mov[edi], al
			add esi, 2
			add edi, 2
			add edx, 4
			loop l
			add esi, ost
			add edi, ost
			add edx, ost
			cmp edx, y
			ja l4
			jnge l5
			l4 :
		add esi, x
			add edi, x
			mov edx, 0
			l5 :
			pop ecx
			sub ecx, 1
			cmp ecx, 0
			jnle lvn
	}

	fwrite(buffer1, 1, width * height * 4, out);
	delete[] buffer;
	delete[] buffer1;
	fseek(in, BITMAP_HEADER_SIZE, SEEK_SET);
	return clock() - start_time;
}

time_t asm_MMX(FILE* in, FILE* out, unsigned width, unsigned height, __int8 alpha)
{
	__int32 ost = 4 * STEP,
		vh = height * (((width - (width % STEP)) / STEP) / 2), // счетчик выделения памяти под высветлившуюся область
		x = (width % STEP) * 4,
		y = (width - (width % STEP)) * 4,
		a;

	unsigned __int8 *buffer = new unsigned __int8[width * height * 4],
		*buffer1 = new unsigned __int8[width * height * 4];
	fread(buffer, 1, width * height * 4, in);
	for (int i = 0; i < width * height * 4; i++)
		buffer1[i] = buffer[i];

	unsigned __int64 step1 = (256 * 256 + 256 + 1) * alpha; //=0x0030303000303030;
	step1 <<= 32;
	step1 += (256 * 256 + 256 + 1) * alpha;

	unsigned long long int start_time = clock();
	__asm
	{
		mov eax, 0
		mov esi, buffer
		mov edi, buffer1
		mov ecx, vh
		lvn :
		push ecx
			mov ecx, STEP / 2 // цикл 32-> нужно получить 256 байт(по 4 байта в один ммх регист за проход цикла)
			l :
			movq mm0, [esi]
			paddusb mm0, step1
			movq[edi], mm0
			add esi, 8
			add edi, 8
			add eax, 8
			mov a, eax
			loop l
			add esi, ost
			add edi, ost
			add eax, ost
			mov a, eax
			cmp eax, y
			jne l3
			l2 :
		mov eax, 0
			mov a, eax
			add esi, x
			add edi, x
			l3 :
		pop ecx
			loop lvn
			emms
	}

	fwrite(buffer1, 1, width * height * 4, out);
	delete[] buffer;
	delete[] buffer1;
	fseek(in, BITMAP_HEADER_SIZE, SEEK_SET);
	return clock() - start_time;
}



char* get_Width_Heigth(FILE* file, unsigned& width, unsigned& heigth, char *header)
{
	fread(header, 1, BITMAP_HEADER_SIZE, file);
	fseek(file, BITMAP_HEADER_SIZE, SEEK_SET);

	width = *((unsigned*)(header + 18));
	heigth = *((unsigned*)(header + 22));
	return header;
}

int main()
{
	setlocale(LC_ALL, "Rus");
	FILE* in;
	fopen_s(&in, "twilight.bmp", "rb");
	if (!in)
	{
		printf("Error: Can't open twilight.bmp for reading\n");
		return -1;
	}
	unsigned width, heigth;
	char *header = new char[BITMAP_HEADER_SIZE];
	header = get_Width_Heigth(in, width, heigth, header);


	std::cout << "\n\tШирина: " << width
		<< "\n\tВысота: " << heigth
		<< "\n\tВведите значение прозрачности закраски: ";
	__int8 alpha = getInt(0, 255);

	time_t t1 = 0, t2 = 0, t3 = 0, curr_t;
	unsigned count = 100;
	for (unsigned i = 0; i < count; i++)
	{
		std::cout << "\n\tИтерация #" << i << ": " << t1 + t2 + t3;
		// С++
		{
			FILE* out;
			fopen_s(&out, "twilight_c++.bmp", "wb");
			if (!out)
			{
				std::cout << "Error: Can't open twilight_c++.bmp \n";
				return CANT_OPEN_C_OUTPUT;
			}
			fwrite(header, 1, BITMAP_HEADER_SIZE, out);
			fseek(out, BITMAP_HEADER_SIZE, SEEK_SET);
			curr_t = ci_prog(in, out, width, heigth, alpha);
			t1 += curr_t;
			std::cout << "\n\tВремя работы на С++: " << curr_t << std::endl;
			fclose(out);
		}

		//  Cкалярные операции
		{
			FILE* out;
			fopen_s(&out, "twilight_scal.bmp", "wb");
			if (!out)
			{
				printf("Error: Can't twilight_scal.bmp \n");
				return CANT_OPEN_ASSEMBLER_OUTPUT;
			}
			fwrite(header, 1, BITMAP_HEADER_SIZE, out);
			fseek(out, BITMAP_HEADER_SIZE, SEEK_SET);
			curr_t = asm_scal(in, out, width, heigth, alpha);
			t2 += curr_t;
			std::cout << "\n\tВремя работы со скалярными операциями: " << curr_t << std::endl;
			fclose(out);
		}

		// MMX
		{
			FILE* out;
			fopen_s(&out, "twilight_mmx.bmp", "wb");
			if (out == NULL)
			{
				printf("Error: Can't open twilight_mmx.bmp \n");
				return CANT_OPEN_MMX_OUTPUT;
			}
			fwrite(header, 1, BITMAP_HEADER_SIZE, out);
			fseek(out, BITMAP_HEADER_SIZE, SEEK_SET);
			curr_t = asm_MMX(in, out, width, heigth, alpha);
			t3 += curr_t;
			std::cout << "\n\tВремя работы с MMX: " << curr_t << std::endl;
			fclose(out);
		}
	}

	std::cout << "\n\tСреднее время работы программы на С++ "
		<< count << " итераций: " << t1 / count << std::endl
		<< "\n\tСреднее время работы со скалярными операциями на "
		<< count << " итераций: " << t2 / count << std::endl
		<< "\n\tСреднее время работы с MMX на "
		<< count << " итераций: " << t3 / count << std::endl;

	return 0;
}
