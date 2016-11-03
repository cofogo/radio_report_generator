/* This program is meant to generate reports given the CSV files exported by
 * the DAD software used by many radio stations.
 * The reports will contain data showing how many times which record was played
 * */

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <string>
using std::string;
#include <vector>
using std::vector;

vector<short> get_key_col_numbers(); //TODO - implementation

int main()
{
	cout << "Welcome to the Radio Report Generator\n\n";
	cout << "All the files you wish to process should be located inside the\n"
					"'raw_files' directory\n\n";

	string inp_filename;

	cout << "filename to process:";
	cin >> inp_filename;
	
	ifstream inp_stream;
	inp_stream.open();
	
	if(inp_stream.fail()) {
		cerr << "ERROR: Failed to open file '" << inp_filename << "' for reading\n";
		cerr << "The program will now exit.\n";
		return 1;
	}
	
	//TODO - hardcoding such things might not be a good idea... think of a better
	vector<string> key_columns;
	key_columns.push_back("TITLE");
	key_columns.push_back("ARTIST");
	
	string linebuffer;
	while(!inp_stream.eof()) {
		getline(inp_stream, linebuffer);
		//TODO
		/* Get key column numbers
		 * get key column fields for each row
		 * if there was allready such a combination, increase occurance counter
		 * else add new entry to memory with occurance counter set to 1
		 * */
	}
	//TODO generate the report from entries in memory
	
	return 0;
}
