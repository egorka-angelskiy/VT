#include <iostream>

/*
На промежутке от -128 до 127. 
Подсчитать количество таких пар чисел X и У,
что (Х * У - (X div 2)) = 0. Ответ вывести на экран.
*/

int main()
{

	__int16 counter = 0;
	__int8 X, Y;
	__int8 X_temp, Y_temp;
	bool flag = false;
	const char* msg = "\nThe number of pairs is less than 200";

	_asm
	{
		        
        mov X, -128 // X = -128
        M1:
            
            mov Y, -128 // Y = -128 
            M2:
                
                mov al, X   // в регистр al передаем значение X на каждом шаге
                cbw			// переводим знаковое число
                mov dl, 2;           // в регистр dl заносим 2 для деление(данные)
                idiv dl             // al = ax получаем, что ((al = X) / (dl = 2))
                cbw					// сохраняем (X div 2)
                push ax     // уменьшает значение регистра стека на размер операнда (2 или 4) и 
							// копирует содержимое операнда в память по адресу SS:SP.
							// получаем, что в регист ax помещаем (X div 2)
               
                mov al, X // снова помещаем в регистр al X для умножения
                cbw
                imul Y // помещаем в регистр ax результат al = X * Y
                pop dx // забираем значение из стека
                sub al, dl // производим вычитание (al = (Х * У)) – (dl = (X div 2))
                sbb ah, dh 
                cmp ax, 0	// если наше выражение равно 0, то выполняем действия ниже
                jne M3
                    inc counter		// добавляем кол-во таких пар
                    cmp counter, 200	// если встретили 200-ю пару, то выполняем след. действия
                    jne M3			// перепрыгиваем, если не равно
                        cmp flag, 1		// если flag равен 1, то делаем действия ниже
                        je M3		// перепригиваем, если равно
                            mov flag, 1		// помещаем значение true во flag
                            mov al, X		// значение X кладем в регистр al
                            mov X_temp, al	// помещаем по временную переменную, значения al = X
                            mov al, Y		// -//-
                            mov Y_temp, al	// -//-
                M3:
                
            inc Y		// шагаем по Y
            cmp Y, -128		// если Y не равен -128, то идем дальше
                jne M2		// перепригиваем в Y, чтобы дальше его перебирать
              
        inc X		// -//-
        cmp X, -128		// -//-
        jne M1		// -//-
	};

	std::cout << "\nNumber of pairs: " << static_cast<int>(counter);
	if (flag)
		std::cout << "\n200-th pair\nx = " << static_cast<int>(X_temp) << "; y = " << static_cast<int>(Y_temp) << std::endl;
	else
		std::cout << msg;


	flag = false;
	counter = 0;
	int RX, RY;
	for (int i = -128; i < 128; ++i)
		for (int j = -128; j < 128; ++j)
			if ((i * j - (i / 2)) == 0)
			{
				counter++;
				if (counter == 200 && flag == false)
				{
					flag = true;
					RX = i; RY = j;
				}
			}

	std::cout << "\nNumber of pairs: " << counter;
	if (flag)
		std::cout << "\n200-th pair\nx = " << RX << "; y = " << RY << std::endl;
	else
		std::cout << msg;
}
