#include <iostream>
#include <cctype>
#include <stack>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <string>
#include <cmath>

using namespace std;

void getRowCol(const std::string &cell, int &i, int &j);
bool isExpressionValid(std::string postfix);
bool evaluatePostfix(std::vector<vector<std::string> > &CSVData, std::string postfix, std::vector<std::vector<bool> > &visited, int &expVal, int row, int col);

int eval(int op1, int op2, char oper) 
{
  cout<<"Operator --> "<<oper<<endl;
  switch (oper) 
  {
    case '*': 
        return op1 * op2;
        break;

    case '/': 
        return op1 / op2;
            break;

    case '+': 
        return op1 + op2;
        break;

    case '-': 
        return op1 - op2;
        break;

    default : 
       cout<<"Invalid operator"<<endl;
       return -1;
   }
}

bool HasDigit(std::string &str)
{
  return (str.find_first_of("0123456789") != std::string::npos) ? true : false;
}

bool IsDigitOnly(std::string &str)
{
  return (str.find_first_not_of("0123456789") == std::string::npos) ? true : false;
}

bool IsAlphaNumericOnly(std::string &str)
{
  return (str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == std::string::npos) ? true : false;
}

bool IsOperatorOnly(std::string &str)
{
  return ((1 == str.length()) && (str.find_first_not_of("*/+-") == std::string::npos)) ? true : false;
}

/*
valid data
---------------
B1 B2 + --> B1 + B2

invalid data
---------------
no. of operands <= no. of operators - invalid
1B B2 + --> 1B not a valid row/col
B11 B12 * --> if no. of rows are < 11
B1 B2 ++ --> additional operator
+- B1 B2 --> not a postfix expression
B2 B1/ --> invalid since there is no space between B1 and /
*/

// check expression first for no. of operators and operands
// tokenize each operator or operand and check validity of each
bool isExpressionValid(std::string postfix)
{
  int numOperators = 0, numOperands = 0;
  istringstream sstream(postfix);
  
  while(!sstream.eof())
  {
    std::string str;
    sstream>>str;
        
    if(IsOperatorOnly(str))
    {
      numOperators++;
      // check no. of operators vs operands
      // should handle this case too --> "+ B1 B2"
      if(numOperators >= numOperands) 
      {
        cout<<"No. of operators vs No. of operands mismatched !!!"<<endl;
        return false; 
      }
    }
    else
    {
      // sub string might have +- or -+ or */ etc, so it falls under else case due to length != 1 in oeprator check
      // if it falls under else case, check if sub string has a digit. If not, its invalid
      if(!HasDigit(str))
      {
        cout<<"Expression does NOT have digits !!! Invalid"<<endl;
        return false;
      }
      else
      {
        // check if alphabets are in lower case or upper case with digits and NO operators
        if(IsAlphaNumericOnly(str))
        {
          if(IsDigitOnly(str))
          {
            //cout<<"Digit ONLY - valid"<<endl;
          }
          // check if number is first - ex: 12B, 3AB etc
          else if(str[0] <= '9')
          {
            return false;
          }
        }
        else
        {
          // has a character which is neither alpha or a number
          cout<<"Has a character which is NON-alpha numeric"<<endl;
          return false;
        }
      }
      numOperands++;
    }
  }
  
  // should handle this case too --> "B1 B2 + -"
  if(numOperators >= numOperands) 
  {
    cout<<"No. of operators vs No. of operands mismatched !!!"<<endl;
    return false; 
  }
  
  return true;
}

/* evaluation will be as follows:
---------------------------------------------
1. If expression is valid, then perform push/pop on stack
2. For each operand, calculate the index in the 2D grid and value of the cell
3. If cell value is an expression, check validity and evaluate recursively
4. If cell value is a number, then return the value to the caller
5. Mark the cell as valid. For expressions which create a cycle, we need to use the visited value and mark all cells involved in the cycle as ERR
6. return TRUE/FALSE from the function and value stored in the cell
*/

bool evaluatePostfix(std::vector<vector<std::string> > &CSVData, std::string postfix, std::vector<std::vector<bool> > &visited, int &expVal, int row, int col)
{
  stack<std::string> stk;
  int val1 = 0, val2 = 0, i = 0, j = 0, opCount = 0;
  bool op1Flag = false, op2Flag = false;
  char c;
  std::string op1, op2;
  istringstream mystream(postfix);
   
  while(!mystream.eof())
  {
    std::string str;
    mystream>>str;
    cout<<"evaluatePostfix | Current Token --> "<<str<<endl;
    
    if(str == postfix)
    {
      cout<<"Only one operand in the expression !!!"<<endl;
    
      if(IsDigitOnly(str))
      {
        expVal = atoi(str.c_str());
        cout<<"Only one operand in the expression and it is a number !!!"<<endl;
        CSVData[row][col] = str;
        return true;
      }
      else // its not a numerical value, so it has to be a string
      {
        getRowCol(str, i, j);
        cout<<"evaluatePostfix | Row --> "<<i<<" | Col --> "<<j<<endl;
        if((i >= 0) && (j >= 0))
        {
          std::string cell = CSVData[i][j];
          if(isExpressionValid(cell))
          {
            if(!visited[i][j])
            {
              visited[i][j] = true;
              op1Flag = evaluatePostfix(CSVData, cell, visited, expVal, i, j);
              if(op1Flag)
              {
                cout<<"received value for only operand --> "<<expVal<<endl;
                CSVData[i][j] = std::to_string(expVal);
                CSVData[row][col] = std::to_string(expVal);
                return true;
              }
              else
              {
                cout<<"Expression returned ERR "<<endl;
                CSVData[i][j] = "#ERR";
                CSVData[row][col] = "#ERR";
                return false;
              }
            }
            else
            {
              cout<<"Cell already visited"<<endl;
              // check if value is a number, if not we entered a cycle
              if(IsDigitOnly(cell))
              {
                cout<<"Value computed for cell --> "<<cell<<endl;
                expVal = atoi(cell.c_str());
                return true;
              }
              else
              {
                cout<<"Value is NOT computed - CYCLE detected !!!"<<endl;
                CSVData[row][col] = "#ERR";
                return false;
              }
            }
          }
          else
          {
            cout<<"evaluatePostfix | INVALID expression"<<endl;
            CSVData[i][j] = "#ERR";
            CSVData[row][col] = "#ERR";
            return false;
          }
        }
        else
        {
          // row or col values are invalid, so return false
          CSVData[i][j] = "#ERR";
          return false;
        }
      }
    }
    
    if(IsOperatorOnly(str))
    {
      // Character is an operator - so pop the operators off the stack and evaluate - op2 is popped first (op1 operator op2)
      c = str[0];
      if(!stk.empty())
      {
        op2 = stk.top();
        stk.pop();
        cout<<"Op2 --> "<<op2<<endl;
      }
      else
      {
        cout<<"stack is EMPTY !!!"<<endl;
      }
      
      if(!stk.empty())
      {
        op1 = stk.top();
        stk.pop();
        cout<<"Op1 --> "<<op1<<endl;
      }
      else
      {
        cout<<"stack is EMPTY !!!"<<endl;
      }

      cout<<"Now checking op1 --> "<<op1<<endl;
      // check if op1 and op2 are numerical values or expressions
      // get index of the cell and check if its a valid expression
      if(IsDigitOnly(op1))
      {
        op1Flag = true;
        val1 = atoi(op1.c_str());
        cout<<"Val1 --> "<<val1<<endl;
      }
      else // its not a numerical value, so it has to be a string
      {
        getRowCol(op1, i, j);
        cout<<"evaluatePostfix | Row --> "<<i<<" | Col --> "<<j<<endl;
        if((i >= 0) && (j >= 0))
        {
          std::string cell = CSVData[i][j];
          if(isExpressionValid(cell))
          {
            if(!visited[i][j])
            {
              visited[i][j] = true;
              op1Flag = evaluatePostfix(CSVData, cell, visited, expVal, i, j);
              if(op1Flag)
              {
                cout<<"received value for op1 --> "<<expVal<<endl;
                val1 = expVal;
              }
            }
            else
            {
              cout<<"Cell already visited"<<endl;
              // check if value is a number, if not we entered a cycle
              if(IsDigitOnly(cell))
              {
                cout<<"Value computed for cell --> "<<cell<<endl;
                expVal = atoi(cell.c_str());
                val1 = expVal;
                op1Flag = true;
              }
              else
              {
                cout<<"Value is NOT computed - CYCLE detected !!!"<<endl;
                CSVData[row][col] = "#ERR";
                return false;
              }
            }
          }
          else
          {
            cout<<"evaluatePostfix | op1 | INVALID expression"<<endl;
            CSVData[i][j] = "#ERR";
          }
        }
        else
        {
          // row or col values are invalid, so return false
          CSVData[i][j] = "#ERR";
        }
      }

      cout<<"Now checking op2 --> "<<op2<<endl;
      if(IsDigitOnly(op2))
      {
        op2Flag = true;
        val2 = atoi(op2.c_str());
        cout<<"Val2 --> "<<val2<<endl;
      }
      else // its not a numerical value, so it has to be a string
      {
        getRowCol(op2, i, j);
        cout<<"evaluatePostfix | Row --> "<<i<<" | Col --> "<<j<<endl;
        if((i >= 0) && (j >= 0))
        {
          std::string cell = CSVData[i][j];
          if(isExpressionValid(cell))
          {
            if(!(visited[i][j]))
            {
              visited[i][j] = 1;
              op2Flag = evaluatePostfix(CSVData, cell, visited, expVal, i, j);
              if(op2Flag)
              {
                cout<<"received value for op2 --> "<<expVal<<endl;
                val2 = expVal;
              }
            }
            else
            {
              cout<<"Cell already visited"<<endl;
              // check if value is a number, if not we entered a cycle
              if(IsDigitOnly(cell))
              {
                cout<<"Value computed for cell --> "<<cell<<endl;
                expVal = atoi(cell.c_str());
                val2 = expVal;
                op2Flag = true;
              }
              else
              {
                cout<<"Value is NOT computed - CYCLE detected !!!"<<endl;
                CSVData[row][col] = "#ERR";
                return false;
              }
            }
          }
          else
          {
            cout<<"evaluatePostfix | op2 | INVALID expression"<<endl;
            CSVData[i][j] = "#ERR";
          }
        }
        else
        {
          // row or col values are invalid, so return false
          CSVData[row][col] = "#ERR";
        }
      }
      
      cout<<"op1 flag --> "<<op1Flag<<" | op2Flag --> "<<op2Flag<<endl;
      if(op1Flag && op2Flag)
      {
        expVal = eval(val1, val2, c);
        CSVData[row][col] = to_string(expVal);
        cout<<"Evaluated value --> "<<CSVData[row][col]<<endl;
        stk.push(to_string(expVal)); // push the evaluated value again on the stack to proceed further
      }
      else
      {
        CSVData[row][col] = "#ERR";
        return false;
      }
    }
    else if(IsDigitOnly(str))
    {
      expVal = std::stoi(str, NULL);
      cout<<"Expression is NOW a number ONLY | pushing on to stack --> "<<expVal<<endl;
      stk.push(str);
    }
    else
    {
      cout<<"Pushing onto stack --> "<<str<<endl;
      stk.push(str); // if string is an operand, then push the character onto the stack
    }
  }
  return true;
}

int evaluatePostfix(std::string postfix) 
{
   stack<int> stk;
   int i = 0, val = 0;
   char c;
   while (i < postfix.length()) 
   {
      c = postfix[i];
      if (isdigit(c)) 
      {
         stk.push(c - '0'); // if character is an operand, then push the character onto the stack
      }
      else 
      {
         // Character is an operator - so pop the operators off the stack and evaluate - op2 is popped first (op1 operator op2)
         int op2 = stk.top();
         stk.pop();
         int op1 = stk.top();
         stk.pop();
         val = eval(op1, op2, c);
         stk.push(val); // push the evaluated value again on the stack to proceed further
      }
      i++;
   }
   return val;
}