/*
 * CÓDIGO PARA BANCADA DE CARACTERIZAÇÃO DE GRUPO MOTOPROPULSOR DE DRONES
 * Versão: 1.0
 * Data: 07/2025
 * Autor: Euler Torres
 * * Código para leitura de tensão a partir de um divisor de tensão
 *
 */
#include "VoltDiv.hpp"

VoltDiv::VoltDiv(const uint8_t AnPin, const String Label) :
	Sensor(Label),
	_AnPin(AnPin),
	_Label(Label),
	scale(3.36f)	//		16.8 * R2/(R1+R2)= 5V	
	//offset(0.0f)
{
	pinMode(_AnPin, INPUT);
}

float VoltDiv::read(){
	raw = analogRead(_AnPin);
	value = (float)raw*4.84/1023.0;
	return value/scale;
}