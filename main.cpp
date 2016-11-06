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

string version("v0.9.01");

struct cell {
	cell(string _c, unsigned _x) : contents(_c), x_pos(_x) {}
	
	string contents;
	unsigned x_pos;
};

struct record {
	record(string _ti, string _ar) : title(_ti), artist(_ar), times_aired(1)
	{};
	
	string title;
	string artist;
	unsigned times_aired;
};

string get_cell_text(unsigned _col_num, string _s, char _delim);
void add_unique(const record& _src, vector<record>& _dest);

int main()
{
	char delim = ',';
	string raw_file_path = "raw_files/";
	string processed_file_path = "processed_files/";
	
	cout << "Welcome to the Radio Report Generator " << version << "\n\n";
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
	
	vector<record> records;
	unsigned itr = 0;
	while(!inp_stream.eof()) {
		++itr;
		getline(inp_stream, linebuffer);
		cout << "line no." << itr << endl;
		
		string title = get_cell_text(key_columns[0].x_pos, linebuffer, delim);
		string artist = get_cell_text(key_columns[1].x_pos, linebuffer, delim);
		if(title == "" && artist == "") {continue;}
		
		record rec(title, artist);
		add_unique(rec, records);
	}
	
	/*
	cout << "***RECORDS***\n";
	for(unsigned i = 0; i < records.size(); ++i) {
		cout << records[i].title << " | " << records[i].times_aired << endl;
	}
	*/
	
	ofstream out_stream;
	out_stream.open(processed_file_path + "proc_" + inp_filename);
	
	out_stream << key_columns[0].contents << "," << key_columns[1].contents <<
	              "," << "TIMES AIRED" << endl;
	if(out_stream.fail()) {
		cerr << "ERROR: Failed to open file '" << inp_filename << "' for writing\n";
		cerr << "The program will now exit.\n";
		return 1;
	}
	
	cout << "***WRITING TO FILE***\n";
	for(unsigned i = 0; i < records.size(); ++i) {
		out_stream << records[i].title << "," << records[i].artist << "," <<
		              records[i].times_aired << endl;
	}
	
	out_stream.close();
	inp_stream.close();
	return 0;
}

string get_cell_text(unsigned _col_num, string _s, char _delim)
{
	size_t start = 0, end = 0;
	for(unsigned i = 0; i != _col_num; ++i) {
		if(end == string::npos) {return "";}
		start = end + 1;
		end = _s.find(_delim, start);
	}
	size_t substr_len = end - start;
	cout << "DEBUG: substr len/start/ed: " << substr_len << "/" << start << "/" << end << endl;
	return _s.substr(start, substr_len);
}

void add_unique(const record& _src, vector<record>& _dest)
{
	for(unsigned i = 0; i < _dest.size(); ++i) {
		if(_src.title == _dest[i].title && _src.artist == _dest[i].artist) {
			++_dest[i].times_aired;
			return;
		}
	}
	
	_dest.push_back(_src);
}
