#include <fstream>
#include<stdint.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	fstream sound(argv[1], ios::binary | ios::in);
	fstream wave(argv[1], ios::binary | ios::in);
	fstream text(argv[2], ios::binary | ios::in);
	int index1 = 0;
	int sizeSound;
	char header[44];
	//открываем файл для считывания заголовка
	if (!sound.is_open())
	{
		cout << "Error open wave file! Try again! Exit.." << endl;
		getchar();
		return 0;
	}
	sound.read(header, 44);
	sound.close();

	uint8_t dwWave[4];//Cекция WAVE

	wave.ignore(4); //пропускаем 4 байта, служебная инф-я
	wave.read((char*)&sizeSound, 4);//читаем 4 байта
	for (int i = 0; i<4; i++)
		wave.read((char*)&dwWave[i], 1); //Читаем 4 байта, там для Wav-файла должно быть название секции WAVE

	if ((dwWave[0] != 0x57) || (dwWave[1] != 0x41) || (dwWave[2] != 0x56) || (dwWave[3] != 0x45)) //Проверим 57415645
	{
		cout << "File doesn't have section WAVE! Exit.." << endl;
		return 0;
	}

	wave.ignore(36);
	char *Buffer = new char[sizeSound];
	wave.read(Buffer, sizeSound);
	wave.close();
	bool *bits = new bool[sizeSound * 8];
	int index = 0;
	int x;
	for (int i = 0; i < sizeSound; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bits[index + j] = Buffer[i] & (1 << j);
			x = j;
		}
		index = i + x;
	}


	if (!text.is_open())
	{
		cout << "Error open text file! Try again!" << endl;
		getchar();
		return 0;
	}

	text.seekg(0, std::ios::end); //устанавливаем относительную позицию чтения
	int sizeText = text.tellg();//для установления размера
	char *bufferText = new char[sizeText];
	text.seekg(0);//обнуляем позицию
	text.read(bufferText, sizeText);//чтение текста
	text.close();
	bool *bitText = new bool[sizeText * 8];

	index = 0;
	x = 0;

	for (int i = 0; i < sizeText; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bitText[index + j] = bufferText[i] & (1 << j); //переводим текст в массив битов	
			x = j;
		}
		index = i + x;
	}


	bool *bitPack = new bool[sizeText * 8];
	for (int i = 0; i < sizeText * 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			i = i + j;
			bitPack[i] = bitText[index1];//передаем в package массив битов
			index1++;
		}
	}
	for (int i = 0; i < sizeText * 4; i++)
	{
		bits[i] = bitPack[i];
		bits[i + 1] = bitPack[i + 1];
		i = i + 1;
	}

	for (int i = 0; i < sizeText * 4; i++)
	{
		Buffer[i] = bits[i] * 1 + bits[i + 1] * 2
			+ bits[i + 2] * 4 + bits[i + 3] * 8 
			+ bits[i + 4] * 16 + bits[i + 5] * 32 
			+ bits[i + 6] * 64 + bits[i + 7] * 128;
		i = i + 7;
	}

	fstream outFile(argv[1], ios::binary | ios::out);
	outFile.write(header, 44);//запись заголовка
	outFile.write(Buffer, sizeSound - 44);//запись самого текста
	outFile.close();

	delete[] Buffer;
	delete[] bits;
	delete[] bitText;
	delete[] bitPack;
	cout << "The file is recorded" << endl;
	return 0;
}
