#include <QTextStream>
#include "Calculator.h"
#include "Calculator.cpp"

Calculator C;

int main()
{
    while(input != "exit")
    {
    cout << "Input your math expression:" << Qt::endl;
    cin >> input; 
    C.evaluation(input);
    }
    return 0;
}