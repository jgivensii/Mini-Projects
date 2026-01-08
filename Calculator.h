#ifndef _Calculator_H_
#define _Calculator_H_

#include <Qstring>

class Calculator{
public:
    void inputAnalyzer(QString s);
    int evaluation(QString q);
    double exponent(qsizetype i);
    double add(qsizetype i) ;
    double subtract(qsizetype i) ;
    double multiply(qsizetype i);
    double divide(qsizetype i) ;
    void inputAnalyzer();
    double orderOfOperations(qsizetype i);
    void evaluateExponents(qsizetype i);
    void evaluateMultDiv(qsizetype i);
    void evaluateAddSub(qsizetype i);
    double result;

};

#endif