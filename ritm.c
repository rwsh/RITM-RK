#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int N = 5; // Размерность системы дифференциальных уравнений

double x0 = 0.0; // начальная точка
double x1 = 5.0; // конечная точка

double h = 0.01; // шаг 

double *Y; // вектор текущего решения
double *K1, *K2, *K3, *K4; // вектора для метода Рунге-Кутта
double *YK; // временный вектор

double x; // текущая точка

FILE *fout; // файл для вывода результатов

// функция - точное решение дифференциального уравнения
double exact_solution(double x)
{
	return -1.0/12.0 * exp(-3.0*x) * x * (129.0 * x*x*x + 16.0 * x*x - 54.0 * x - 36.0);
}

// векторная функция - правая часть для системы дифференциальных уравнений
// результат вычисление сохраняется в массиве RigthPart
void f(double x, double *Y, double *RightPart)
{
	RightPart[0] = Y[1];
	RightPart[1] = Y[2];
	RightPart[2] = Y[3];
	RightPart[3] = Y[4];
	RightPart[4] = -15.0 * Y[4] - 90.0 * Y[3] - 270.0 * Y[2] - 405.0 * Y[1] - 243 * Y[0];
}

// вспомогательная функция копирования массивов A <- B
void copy(double *A, double *B)
{
	for (int n = 0; n < N; n++)
	{
		A[n] = B[n];
	}
}

// добавить поэлементно в массив A значения из массива B, умножая на c
// A <- A + cB
void add_with_c(double *A, double *B, double c)
{
	for (int n = 0; n < N; n++)
	{
		A[n] += c * B[n];
	}
}

// расчитать решение на x + dt
// метод Рунге-Кутта
void step(double dt)
{
	f(x, Y, K1); 
	
	copy(YK, Y);
	add_with_c(YK, K1, 0.5 * h);
	f(x + 0.5 * h, YK, K2);

	copy(YK, Y);
	add_with_c(YK, K2, 0.5 * h);
	f(x + 0.5 * h, YK, K3);

	copy(YK, Y);
	add_with_c(YK, K3, h);
	f(x + h, YK, K4);

	add_with_c(Y, K1, h / 6.0);
	add_with_c(Y, K2, h / 3.0);
	add_with_c(Y, K3, h / 3.0);
	add_with_c(Y, K4, h / 6.0);
}

// печать текщего результата
// текущая точка | решение | точное решение | модуль разности точного решения и численного
void print()
{
	double exact = exact_solution(x); // получаем точное решение
	fprintf(fout, "%e\t%e\t%e\t%e\n", x, Y[0], exact, fabs(Y[0] - exact));
}

// начало расчета
void run()
{
	print(); // печатем начальное условие

	// если мы не перейдем конечную точку, то продолжаем расчет
	while(x <= x1 - h)
	{
		step(h); // расчитываем решение на следующий шаг
		x += h; // сдвигаем текущую точку
		print(); // печатаем результат
	}
}

// инициализация программы
// открываем файл, выделяем память, устанавливаем начальные значения решения
// если будут ошибки при инициализации - возвращаем -1, иначе 0
int init()
{
	fout = fopen("result.txt", "w"); // создаем файл с результатом
	if (fout == NULL)
	{
		return -1; // ошибка при создании (открытии файла)
	}

	// выделяем память и проверяем результат
	Y = calloc(N, sizeof(double));
	if (Y == NULL) { return -1; }
	K1 = calloc(N, sizeof(double));
	if (K1 == NULL) { return -1; }
	K2 = calloc(N, sizeof(double));
	if (K2 == NULL) { return -1; }
	K3 = calloc(N, sizeof(double));
	if (K3 == NULL) { return -1; }
	K4 = calloc(N, sizeof(double));
	if (K4 == NULL) { return -1; }
	YK = calloc(N, sizeof(double));
	if (YK == NULL) { return -1; }

	// устанавливаем начальные условия
	x = x0;
	Y[0] = 0; Y[1] = 3; Y[2] = -9; Y[3] = -8; Y[4] = 0;

	return 0; // если дошли до сюда, то все хорошо
}


// освобождаем память, закрываем файл
void done()
{
	free(Y);
	free(K1);
	free(K2);
	free(K3);
	free(K4);
	free(YK);

	fclose(fout);
}

int main(void)
{
	// инициализируем программу
	if (init() != 0)
	{
		printf("\nError!\n\n");
		return -1; // при инициализации была ошибка, завершаем программу
	}
	run(); // начинаем расчет
	done(); // освобождаем ресурсы

	return 0;
}