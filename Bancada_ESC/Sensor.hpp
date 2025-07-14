/*
 * CÓDIGO PARA BANCADA DE CARACTERIZAÇÃO DE GRUPO MOTOPROPULSOR DE DRONES
 * Versão: 1.0
 * Data: 07/2025
 * Autor: Euler Torres
 * * Classe genérica de sensor
 *
 */
#pragma once
#include <stdint.h>
#include <Arduino.h>

class Sensor{
public:
	
	Sensor(const String& label) : label(label) {};
	
	
	virtual ~Sensor() = default;
	
	virtual float calibra() {return 0;}
	virtual float read() = 0;
	virtual String getLabel() const{return label;};
	
protected:
	String label;
};