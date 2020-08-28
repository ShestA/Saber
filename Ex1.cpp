#include <iostream>
//Печать в двоичном коде
void BinPrint(int in) {
	const int Bytes(sizeof(int));
	const int Bits(Bytes << 3);
	unsigned long cast(static_cast<unsigned long> (in));
	char Bin[Bits];
	
	for (auto Pos = Bits, itr = 0; Pos > 0;) {
		Bin[itr++] = ((cast & (unsigned long(1) << --Pos % Bits)) != 0 ? '1' : '0');
	}
	for (auto Pos = 0; Pos < Bits;) {
		std::cout << Bin[Pos++];
	}
}