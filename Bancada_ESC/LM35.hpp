/*
 * CÓDIGO PARA BANCADA DE CARACTERIZAÇÃO DE GRUPO MOTOPROPULSOR DE DRONES
 * Versão: 1.0
 * Data: 07/2025
 * Autor: Euler Torres
 * * Código para leitura de temperatura °C usando uma classe genérica de Sensor.
 *
 */
#pragma once
#include <stdint.h>

#include "Sensor.hpp"
#include <Arduino.h>

class LM35 : public Sensor{
	
public:
	LM35(const uint8_t PIN_An, const String Label);
	~LM35() override = default;

	float	read()		override;
private:
	uint8_t _PIN_An;
	String _Label;

	float scale;
	float readingVolts;
	float temperatureCelsius = 0;
	float voltDrop;
};