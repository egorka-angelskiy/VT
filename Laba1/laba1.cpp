#include <stdio.h>
#include <conio.h>
#include <iostream>

int main()
{

	long int A = 0x11223344;
	int B = 0x5566;

	// A = 55226644
	// B = 3311

	std::cout.unsetf(std::ios::dec);
	std::cout.setf(std::ios::hex);

	std::cout << "Originall\nA = " << A << '\n' << "B = " << B << std::endl;

	__asm
	{
		mov al, byte ptr A[1];		// A[1] = 33 кладем в al
		xchg byte ptr B[1], al;		// al = A[1] кладем в B[1] и забираем в al = B[1] = 55, итог А = 11223344, B = 3366
		xchg byte ptr A[3], al;		// al = 55 кладем в A[3] и забираем al = A[3] = 11, итог A = 55223344, B = 3366
		xchg byte ptr B, al;		// al = 11 кладем в B[0] и забираем al = B[0] = 66, итог A = 55223344, B = 3311
		mov byte ptr A[1], al;		// al = 66 кладем в A[1] и получаем A = 55226644, B = 3311
	};
	
	std::cout << "\n\nResult:\nA = " << A << '\n' << "B = " << B << std::endl;
	_getch();

	return 0;
}
