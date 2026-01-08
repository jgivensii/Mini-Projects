#include <QString>
#include <QVector>
#include <QChar>
#include <QTextStream>
#include "Calculator.h"

QTextStream cout(stdout);
QTextStream cin(stdin);

QVector<double> numbers;
QVector<char> operators;
QString numberStr;
QString input;
Calculator Calc;
void Calculator::inputAnalyzer(QString input){
    numbers.clear();
    operators.clear();
    numberStr.clear();
   for (QChar ch : input)
    {
        if (ch.isDigit()) {
            numberStr += ch;
        } 
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^'  ) 
        {
            if (!numberStr.isEmpty()) {
                numbers.push_back(numberStr.toDouble());
                numberStr.clear();
            }
            operators.push_back(ch.toLatin1());
        }
    }
    if (!numberStr.isEmpty()) {
    numbers.push_back(numberStr.toDouble());
    numberStr.clear();
}
}


double Calculator::exponent(qsizetype i) {
    return  Calc.result = pow(numbers[i], numbers[i+1]);
}
double Calculator::add(qsizetype i) {
    return  Calc.result = numbers[i] + numbers[i+1];
}
double Calculator::subtract(qsizetype i) {
    return Calc.result = numbers[i] - numbers[i+1];
}
double Calculator::multiply(qsizetype i){
    return Calc.result = numbers[i] * numbers[i+1];
}
double Calculator::divide(qsizetype i) {
    if (numbers[i+1] == 0) throw std::runtime_error("Division by zero");
    return Calc.result = numbers[i] / numbers[i+1];
   
}
void Calculator::evaluateExponents(qsizetype i) {
bool found;
    do {
        found = false;
    for (i = 0; i < operators.size(); ++i) {  
        switch(operators[i])
            {
                 case '^': Calc.exponent(i);
                 found = true;
                break;
          continue;
            } 
            if (found) {
                numbers[i] = Calc.result;
                numbers.remove(i + 1);
                operators.remove(i);
                --i;
                break; // restart loop
            }
        }} while (found);}
 void Calculator::evaluateMultDiv(qsizetype i) {
    bool found;
    do {
        found = false;
    for (i = 0; i < operators.size(); ++i) {  
        switch(operators[i])
            {
                 case '*': Calc.multiply(i);
                 found = true;
                break;
                case '/': Calc.divide(i);
                found = true;
                 break;
          continue;
            } 
            if (found) {
                numbers[i] = Calc.result;
                numbers.remove(i + 1);
                operators.remove(i);
                --i;
                break; // restart loop
            }
        }} while (found);}
         void Calculator::evaluateAddSub(qsizetype i) {
   bool found;
    do {
        found = false;
    for (i = 0; i < operators.size(); ++i) {  
        switch(operators[i])
            {
                 case '+': Calc.add(i);
                 found = true;
                break;
                case '-': Calc.subtract(i);
                found = true;
                 break;
          continue;
            } 
            if (found) {
                numbers[i] = Calc.result;
                numbers.remove(i + 1);
                operators.remove(i);
                --i;
                break; // restart loop
            }
        }} while (found);}
double Calculator::orderOfOperations(qsizetype i)
{
        evaluateExponents(i);
        evaluateMultDiv(i);
        evaluateAddSub(i);
 return numbers[0];
}

int Calculator::evaluation(QString q)
{ 
 qsizetype i; 
 inputAnalyzer(q);
  if (numbers.isEmpty())
  {
    cout << "No numbers found in input!" << Qt::endl;
    return 1;
  }

 i = 0; 

Calc.result = 0;  
if (operators.isEmpty())
 { 
 Calc.result = numbers[0];
 }
 else {
  Calc.result = orderOfOperations(i);
 }
    cout << "Result: " << Calc.result << Qt::endl;
 return 0;

}

