#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#define String std::string

using namespace std;

int main(int argc, char *argv[])
{
	vector< vector<String> > coInputVector;

	ifstream coInputFile(argv[1]);
	String lsLine;

	if (coInputFile.is_open())
	{
		while (getline(coInputFile, lsLine))
		{

			String lsAddress = lsLine.substr(0, lsLine.find(" "));
			String lsAction = lsLine.substr(lsAddress.length());

			vector<String> aoTempVector;
			aoTempVector.push_back(lsAddress);
			aoTempVector.push_back(lsAction);

			coInputVector.push_back(aoTempVector);
		}

		coInputFile.close();
	}
	else
	{
		cout << "Error opening file " << argv[1] << "\n";
	}

	for (int i = 0; i < coInputVector.size(); i++)
	{
    	for (int j = 0; j < coInputVector[i].size(); j++)
    	{
        	cout << coInputVector[i][j] << "\t";
    	}
    	cout << "\n";
	}

	//cout << coInputVector;
}