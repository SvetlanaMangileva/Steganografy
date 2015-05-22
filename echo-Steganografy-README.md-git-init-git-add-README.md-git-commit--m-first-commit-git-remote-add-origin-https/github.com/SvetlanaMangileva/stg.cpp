фи#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

struct Bits_Wav
{
	bool bits[8];
};
struct Bits_text
{
	bool bits[8];
};
struct Bit_pack
{
	bool pack[2];
};

bool checkInWav(FILE *fp, char instring_music[120]);
bool checkInTxt(FILE *fp, char instring_text[120]);
bool checkSize(int size_wav);
void Byte_to_bits(int size, char Buffer[], Bits_Wav _bits[], Bits_text _bits_t[], int key);
void Bits_to_bytes (int size_text, char Buffer[], Bits_Wav _bits[]);
void Make_pack_of_two(int size_text, Bits_text _bits_t[], Bit_pack b_p[]);
void Substitute_information_bits(int size_text, Bit_pack b_p[], Bits_Wav _bits[]);
bool cross_check(int size_wav, int size_text);

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		cerr << "Lost arguments. Enter [input_file.wav , input_file.txt, output_file.wav]" << endl;
		return 0;
	}

	ifstream in, intext, in_head;
	ofstream out;
	FILE *fp = NULL;
	char instring_music[120], instring_text[120], B[54], variant[3]; 
	int size_wav, size_text;
	bool check = 0, entry = 0;

	strcpy(instring_music, argv[1]);
	check = checkInWav(fp, instring_music); //чтение файла
	if (check == 0)
		return 0;

	strcpy(instring_text, argv[2]);
	check = checkInTxt(fp, instring_text);
	if (check == 0)
		return 0;

	in_head.open(instring_music, ios::binary|ios::in); 
	in_head.read(B,54); // читать расшир заголовок
	in_head.close();

	in.open(instring_music, ios::binary|ios::in);
	in.ignore(4);
	in.read((char*)&size_wav, 4); // чтение размера wav файла
	in.ignore(50);

	check = checkSize(size_wav); // если размер == 0
	if (check == 0)
		return 0;

	char *Buffer = new char [size_wav];
	Bits_Wav *_bits = new Bits_Wav[size_wav];

	in.read(Buffer,size_wav); // чтение файла в Buffer
	in.close();

	Byte_to_bits(size_wav, Buffer, _bits, NULL, 0); //   преобразования значений в буфере с байт в биты

	intext.open(instring_text, ios::in); 
	intext.seekg (0, std::ios::end);
    size_text = intext.tellg(); // размер текста

	check = cross_check(size_wav, size_text); // if size_text == 0 and size_text*4 >= size_wav
	if (check == 0)
	{
		delete [] Buffer;
		delete [] _bits;
		return 0;
	}

	char *Buffer_text = new char [size_text];

	intext.seekg(0);
	intext.read(Buffer_text, size_text); // чтение файла в буфер_текст
	intext.close();

	Bits_text *_bits_t = new Bits_text[size_text];
	Bit_pack * b_p = new Bit_pack[size_text*4];

	Byte_to_bits(size_text, Buffer_text, NULL, _bits_t, 1); // преобразования значений в Buffer_text из байтов в _bits_t

	Make_pack_of_two(size_text, _bits_t, b_p);

	Substitute_information_bits(size_text, b_p, _bits); // подставляем инф биты в наименее значимые биты музыкального файла 

	Bits_to_bytes (size_text, Buffer, _bits); // преодбразование значений из битоы в байты in Buffer

	if (strcmp(argv[1],argv[3]) == 0)
	{
		cout << "\nInput and output filenames of music file are same. Overwrite anyway? [yes/no]: ";
		cin.getline(variant,3);
		entry = 1;
	}
	if (entry == 1)
	{
		if (strcmp(variant,"yes") == 0)
		{
			out.open(argv[3], ios::binary|ios::out);
			out.write(B,54);
			out.write(Buffer,size_wav-46);
			out.close();

			cout << "\nSucceded!";
		}
		else
		{
			delete [] Buffer;
			delete [] _bits;
			delete [] _bits_t;
			delete [] b_p;
			return 0;
		}
	}
	out.open(argv[3], ios::binary|ios::out);
	out.write(B,54);
	out.write(Buffer,size_wav-46);
	out.close();

	cout << "\nSucceded!";
	
	delete [] Buffer;
	delete [] _bits;
	delete [] _bits_t;
	delete [] b_p;
	return 0;
}
bool checkInWav(FILE *fp, char instring_music[120])
{
	fp = fopen(instring_music,"r");
	if (fp == NULL)
	{
		cout << "\nMusic file is not exist!";
		getchar();
		return 0;
	}
	fclose(fp);
	return 1;
}
bool checkInTxt(FILE *fp, char instring_text[120])
{
	fp = fopen(instring_text,"r");
	if (fp == NULL)
	{
		cout << "\nText file is not exist!";
		getchar();
		return 0;
	}
	fclose(fp);
	return 1;
}
bool checkSize(int size_wav)
{
	if (size_wav == 0)
	{
		cout << "Size of music file is 0!";
		getchar();
		return 0;
	}
	return 1;
}
void Byte_to_bits(int size, char Buffer[], Bits_Wav _bits[], Bits_text _bits_t[], int key)
{
	if (key == 0)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				_bits[i].bits[j] = Buffer[i] & ( 1 << j );
			}
		}
	}
	if (key == 1)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				_bits_t[i].bits[j] = Buffer[i] & ( 1 << j );
			}
		}
	}
}
void Bits_to_bytes (int size_text, char Buffer[], Bits_Wav _bits[])
{
	for (int i = 0; i < size_text*4; i++)
	{
		Buffer[i] = _bits[i].bits[0]*1 + _bits[i].bits[1]*2 + _bits[i].bits[2]*4 + _bits[i].bits[3]*8 + _bits[i].bits[4]*16 + _bits[i].bits[5]*32 + _bits[i].bits[6]*64 + _bits[i].bits[7]*128;
	}
}
void Make_pack_of_two(int size_text, Bits_text _bits_t[], Bit_pack b_p[])
{
	int z = 0;
	int k = 0;
	for (int i = 0; i < size_text*4; i++)
	{
		if (k == 7)
		{
			k = 0;
			z++;
		}
		for (int j = 0; j < 2; j++)
		{
			b_p[i].pack[j] = _bits_t[z].bits[k];
			k++;
		}
	}
}
void Substitute_information_bits(int size_text, Bit_pack b_p[], Bits_Wav _bits[])
{
	for (int i = 0; i < size_text*4; i++)
	{
		_bits[i].bits[0] = b_p[i].pack[0];
		_bits[i].bits[1] = b_p[i].pack[1];
	}
}
bool cross_check(int size_wav, int size_text)
{
	if (size_wav - 54 <= size_text*4)
	{
		cout << "Size of text is bigger than container!";
		getchar();
		return 0;
	}
	if (size_text == 0)
	{
		cout << "Size of text is 0!";
		getchar();
		return 0;
	}
	return 1;
}
