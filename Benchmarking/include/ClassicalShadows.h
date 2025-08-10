/**
 * Classical Shadows Protocol class
 */
#ifndef CLASSICALSHADOWS_H
#define CLASSICALSHADOWS_H
#include <iostream>
#include <string>
//#include <bits/stdc++.h>
//#include <random>
#include <map>
#include <vector>
#include "Circuit.h"
#include "quest.h"

//using namespace std;

map<string, vector<double>> beta_values();

void classicalshadows_protocol(int qubits, int depth, string backend);

#endif