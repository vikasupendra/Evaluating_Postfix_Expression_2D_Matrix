#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <cctype>

using namespace std;

void ParseCSVFile(std::istream &file);
void getRowCol(const std::string &cell, int &i, int &j);
extern bool isExpressionValid(std::string postfix);
extern bool evaluatePostfix(std::vector<vector<std::string> > &CSVData, std::string postfix, std::vector<std::vector<bool> > &visited, int &expVal, int row, int col);

void CalcValues(std::vector<vector<std::string> > &CSVData, int rows, int cols)
{
  int i = 0, j = 0, expVal = -1;
  if(CSVData.size() < 1)
    return;

  std::vector<bool> tmp;
  tmp.assign(cols, false);
  std::vector<std::vector<bool> > visited;
  
  cout<<"Rows --> "<<rows<<" | Cols --> "<<cols<<endl;
  for(i = 0; i < rows; i++)
  {
    // mark visited as false
    visited.push_back(tmp);
  }

  for(i = 0; i < rows; i++)
  {
    for(j = 0; j < cols; j++)
    {
      if(isExpressionValid(CSVData[i][j]))
      {
        if(!visited[i][j])
        {
          visited[i][j] = 1;
          // evaluate the expression
          cout<<"Evaluate Cell --> "<<CSVData[i][j]<<endl;
          evaluatePostfix(CSVData, CSVData[i][j], visited, expVal, i, j);
        }
        else
        {
          cout<<"Cell already visited, value --> "<<CSVData[i][j]<<endl;
        }
      }
      else
      {
        // expression is invalid or the cell is already visited with expression evaluated
        cout<<"Expression is INVALID !!!"<<endl;
      }
    }
  }
}

int convertToCol(const std::string &alpha)
{
  std::string::const_iterator it = alpha.end()-1;
  int i = 0, index = 0, alphaDigit = 0;

  while(i < alpha.length())
  {
    if((*it) >= 'a' && (*it) <= 'z')
    {
      alphaDigit = (*it) - 32 - 'A' + 1; // convert lower case to upper case
    }
    else
    {
      alphaDigit = (*it) - 'A' + 1;
    }
    
    // AA --> 1 + (1 * 26) --> 27 (index will be 27-1 --> 26)
    // BB --> 2 + (2 * 26) --> 54 (index will be 54-1 --> 53)
    index = index + (alphaDigit * (pow(26, i)));
    i++;
    it--;
  }
  index--;
  //cout<<"Index --> "<<index<<endl;
  return index;
}

void getRowCol(const std::string &cell, int &i, int &j)
{
  // split the alphabet half and the numerical half
  // check until the point of first occurence of a number and extract the values
  // ex: BB12 --> BB, 12
  // Convert both the values to a (row,col) combination

  cout<<"Cell --> "<<cell<<endl;
  std::size_t pos = cell.find_first_of("0123456789");
  if(pos != std::string::npos)
  {
    std::string alpha(cell, 0, pos);
    std::string num = cell.substr(pos);

    j = convertToCol(alpha);
    i = std::stoi(num, NULL)-1;
    cout<<"x --> "<<i<<" | "<<"y --> "<<j<<endl;
  }
  else
  {
    i = -1;
    j = -1;
  }
}

void WriteResultToFile(std::vector<vector<std::string> > &CSVData, int &rows, int &cols)
{
  cout<<"WriteResultToFile | rows --> "<<rows<<" | cols --> "<<cols<<endl;
  std::ofstream output("output.csv");
  int i = 0, j = 0;

  if(output.is_open())
  {
    cout<<"WriteResultToFile | File opened successfully"<<endl;

    for(i = 0; i < rows; i++)
    {
      for(j = 0; j < cols-1; j++)
      {
        output << CSVData[i][j] << ",";
      }
      output << CSVData[i][j] << endl;
    }
  }
  else
  {
    cout << "File cannot be opened, cannot write to file"<<endl;
  }
  output.close();
}

void ParseCSVFile(std::istream &file, std::vector<vector<std::string> > &CSVData, int &rows, int &cols)
{
  std::string strLine;
  int Column = 0;
  while(getline(file, strLine))
  {
    rows++;
    istringstream strStream(strLine);
    std::vector<std::string> currRow;
    std::string currCol;
    // read every element from the line that is seperated by commas
    // and put it into the vector or strings
    while(getline(strStream, currCol, ','))
    {
      currRow.push_back(currCol);
      cols++;
    }
    CSVData.push_back(currRow);
  }
  cols = cols / rows;
}

void printRows(vector<std::string> &currRow)
{
  if(currRow.size() < 1)
  {
    return;
  }
  std::vector<std::string>::iterator it = currRow.begin();
  std::cout<<*(it++);
  for(;it != currRow.end();++it)
    std::cout<<" , "<<*it;
}

void display(std::vector<vector<std::string> > &CSVData)
{
  if(CSVData.size() < 1)
    return;

  cout<<"Vector size --> "<<CSVData.size()<<endl;
  std::vector<vector<std::string> >::iterator it = CSVData.begin();
  for(; it != CSVData.end(); ++it)
  {
    printRows(*it);
    std::cout<<std::endl;
  }
}

int main(int argc, const char **argv)
{
  std::vector<vector<std::string> > CSVData;
  int rows = 0, cols = 0;
  fstream file(argv[1], std::ios::in);
  if(!file.is_open())
  {
    cout << "File not found!\n";
    return 1;
  }
  ParseCSVFile(file, CSVData, rows, cols);

  CalcValues(CSVData, rows, cols);

  WriteResultToFile(CSVData, rows, cols);
}