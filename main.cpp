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

/*vector<unsigned short> get_key_col_numbers(vector<string> _conts, string _row,
																					 char _delim);*/
string get_cell_text(unsigned _col_num, string _s, char _delim);

struct cell {
	cell(string _c, unsigned _x) : contents(_c), x_pos(_x) {}
	
	string contents;
	unsigned x_pos;
};

int main()
{
	char delim = ',';
	string raw_file_path = "raw_files/";
	
	cout << "Welcome to the Radio Report Generator\n\n";
	cout << "All the files you wish to process should be located inside the\n"
					"'raw_files' directory\n\n";

	string inp_filename;
	cout << "filename to process:";
	cin >> inp_filename;
	if(inp_filename.find('.') == string::npos) {
		inp_filename = inp_filename + ".csv";
	}
	
	ifstream inp_stream;
	inp_stream.open(raw_file_path + inp_filename);
	if(inp_stream.fail()) {
		cerr << "ERROR: Failed to open file '" << inp_filename << "' for reading\n";
		cerr << "The program will now exit.\n";
		return 1;
	}
	
	/* TODO - hardcoding such things might not be a good idea...
	 * think of a better solution*/
	vector<cell> key_columns;
	key_columns.push_back(cell("TITLE", 3));
	key_columns.push_back(cell("ARTIST", 4));
	
	string linebuffer;
	// reading the header
	getline(inp_stream, linebuffer);
	
	unsigned test_iter = 5;
	unsigned itr = 0;
	while(!inp_stream.eof()) {
		++itr;
		getline(inp_stream, linebuffer);
		cout << "line no." << itr << endl;
		cout << get_cell_text(key_columns[0].x_pos, linebuffer, delim);
		cout << ", " << get_cell_text(key_columns[1].x_pos, linebuffer, delim);
		cout << endl;
		//TODO
		/* Get key column numbers
		 * get key column fields for each row
		 * if there was allready such a combination, increase occurance counter
		 * else add new entry to memory with occurance counter set to 1
		 * */
		test_iter++;
		if(test_iter == 0) {break;}
	}
	//TODO generate the report from entries in memory
	
	inp_stream.close();
	return 0;
}
/*
vector<unsigned short> get_key_col_numbers(vector<string> _conts, string _row,
																					 char _delim)
{
	vector<unsigned short> ret_key_col_numbers;
	
	size_t col_start = 0, col_end = 0;
	for(unsigned i = 0; i < _conts.size(); ++i) {
		unsigned short col_number = 0;
		
		while(col_end != string::npos) {
			col_start = _row.find(_delim, col_end + 1);
			col_end = _row.find(_delim, col_start + 1);
			
			cerr << "DEBUG\n";
			cerr << "col_start = " << col_start << endl;
			cerr << "col_end = " << col_end << endl;
			
			if(col_end < 2) {
				cerr << "ERROR: Unexpected input file format!\n";
				cerr << "The program will now exit.";
			}
			//adjusting cell text bounds for the single " mark at cell borders
			size_t col_text_start = col_start + 2;
			size_t col_text_end = col_end - 2;
			size_t col_text_len = (col_text_end - col_text_start) - 1;
			
			if(_row.compare(col_text_end, col_text_len, _conts[i]) == 0) {
				ret_key_col_numbers.push_back(col_number);
			}
			
			++col_number;
		}
	}
}*/

string get_cell_text(unsigned _col_num, string _s, char _delim)
{
	size_t start = 0, end = 0;
	for(unsigned i = 0; i != _col_num; ++i) {
		if(end == string::npos) {return "";}
		start = end + 1;
		end = _s.find(_delim);
	}
	size_t substr_len = end - start - 1;
	++start; //skipping the delimiter itself
	cout << "DEBUG: substr len/start/ed: " << substr_len << "/" << start << "/" << end << endl;
	return _s.substr(start, substr_len);