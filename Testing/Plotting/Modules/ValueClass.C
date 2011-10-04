#include <iostream>
#include <TMath.h>
#define ValueClassLoaded

using namespace std;


/*
 * 
 * 	This file contains the class "Unit", the version is 1.0
 * 	This class stores the value and error of a given parameter and is able to perform error propagation for addition, subtraction, multiplication, division and even powers. Please add any bugs you find to the change log below.
 * 
 * 	V1.0
 * 	 - First implementation with operators +,-,*,/ and pow; also implemented a custom "<<" operator (i.e. overloaded it)
 * 
 */

class Value
{
  float value;
  float error;

public:
  Value(float value, float error);
  float getValue();
  float getError();
  Value operator+(const Value& c);
  Value operator-(const Value& c);
  Value operator/(const Value& c);
  Value operator*(const Value& c);
  Value Pow(Value a, Value b);
};

Value::Value(float newvalue = 0, float newerror = -999) : value(newvalue) {
  if(newerror!=-999) {
    error=newerror;
  } else {
    error = TMath::Sqrt(newvalue);
  }
}

float Value::getValue()
{
  return value;
}
float Value::getError()
{
  return error;
}

Value Value::operator+(const Value& c)
{//verified
  Value result;
  result.value = (this->value+c.value);
  result.error = TMath::Sqrt((this->error)*(this->error)+(c.error)*(c.error));
  return result;
}

Value Value::operator-(const Value& c)
{//verified
  Value result;
  result.value = (this->value-c.value);
  result.error = TMath::Sqrt((this->error)*(this->error)+(c.error)*(c.error));
  return result;
}

Value Value::operator*(const Value& c)
{//verified
  Value result;
  result.value = (this->value*c.value);
  result.error = TMath::Sqrt(c.value*c.value*(this->error)*(this->error)+(this->value)*(this->value)*(c.error)*(c.error));
  return result;
}

Value Value::operator/(const Value& c)
{//verified
  Value result;
  result.value = (this->value/c.value);
  result.error = TMath::Sqrt((1/(c.value*c.value))*(this->error)*(this->error) + ((this->value/(c.value*c.value))*(this->value/(c.value*c.value)))*(c.error*c.error));
  return result;
}

Value Value::Pow(Value a, Value b)
{
  //this calculates e.g. sqrt(a), where b=(0.5,0)
  Value result;
  result.value = TMath::Power(a.value,b.value);
  result.error=TMath::Power(a.value,b.value)*TMath::Sqrt(TMath::Power(a.value,-2)*(b.value-1)*(b.value-1)*(a.error*a.error)+(TMath::Log(a.value)*TMath::Log(a.value))*(b.error*b.error));
  return result;
}

std::ostream &operator<<(std::ostream &ostr, Value v)
{//this leads to an output like 2.000 +/- 1.000
  return ostr << v.getValue() << " +/- " << v.getError();
}

/*

int main()
{
  cout << "This is my value class" << endl;
  Value a(2,2);
  Value b(16,7);
  Value sq(0.5);
  Value c = a/b;
  Value d = d.Pow(b,sq);
  cout << c << endl;
  cout << "log 11 " << TMath::Log(11) << endl;
  cout << "Power: " << d << endl;
  return 0;
}
*/