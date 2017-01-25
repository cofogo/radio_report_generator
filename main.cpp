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

string version = "v1.10";

struct cell {
	cell(string _c, unsigned _x) : contents(_c), x_pos(_x) {}
	
	string contents;
	unsigned x_pos;
};

struct record {
	record(string _ti, string _ar, string _rl)
	: title(_ti), artist(_ar), duration(_rl), times_aired(1)
	{}
	
	string title;
	string artist;
	string duration;
	unsigned times_aired;
};

string get_cell_text(unsigned _col_num, string _s, char _delim, char _tx_delim);
//TODO update_recordset - implement key-based uniqueness, key to be passed as argument (vector<cell>)
int update_recordset(const record& _src, vector<record>& _dest, char _txt_delim); //updates passed recordset (_dest) by adding passed record if _rec is unique, otherwise iterates existing record in _dest
unsigned strtime_to_n(string _s, char _txt_delim, bool& _flag_err); //takes string time contents formatted as "00:00:00" and returns seconds

int main()
{
	//read and set values from config
	string config_filename = "raregen.cfg";
	ifstream config_stream;
	config_stream.open(config_filename);
	if(config_stream.fail()) {
		cerr << "ERROR: Failed to open config file '" << config_filename << "' for reading\n";
		cerr << "The program will now exit.\n";
		return 1;
	}

	string linebuffer;

	getline(config_stream, linebuffer);
	char delim = linebuffer.back();

	getline(config_stream, linebuffer);
	char txt_delim = linebuffer.back();

	//paths hardcoded for now
	//TODO make a proper separate config reading procedure (use a map probably)
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
	// defining column headers for parsing
	// key columns
	const cell head_title("TITLE", 3);
	const cell head_artist("ARTIST", 4);
	// included columns
	const cell head_duration("ACTDUR", 5);
	
	linebuffer;
	// reading the header
	getline(inp_stream, linebuffer);
	
	unsigned iter = 0;
	vector<record> records;
	while(!inp_stream.eof()) {
		getline(inp_stream, linebuffer);
		
		string title = get_cell_text(head_title.x_pos, linebuffer,
		                             delim, txt_delim);
		string artist = get_cell_text(head_artist.x_pos, linebuffer,
		                              delim, txt_delim);
		string rec_length = get_cell_text(head_duration.x_pos, linebuffer,
		                                  delim, txt_delim);
		if(title == "" && artist == "") {continue;}
		
		record rec(title, artist, rec_length);
		if(update_recordset(rec, records, txt_delim) < 0) {
			//something was not right with this line, show
			cout << "line: " << iter << endl;
		}
		++iter;
	}
	inp_stream.close();
	
	/*
	cout << "***RECORDS***\n";
	for(unsigned i = 0; i < records.size(); ++i) {
		cout << records[i].title << " | " << records[i].times_aired << endl;
	}
	*/
	
	ofstream out_stream;
	out_stream.open(processed_file_path + "proc_" + inp_filename);
	
	out_stream << head_title.contents << ","
	           << head_artist.contents << ","
	           << head_duration.contents << "," << "TIMES AIRED" << endl;
	if(out_stream.fail()) {
		cerr << "ERROR: Failed to open file 'proc_" << inp_filename << "' for writing\n";
		cerr << "The program will now exit.\n";
		return 1;
	}
	
	cout << "***WRITING TO FILE***\n";
	for(unsigned i = 0; i < records.size(); ++i) {
		out_stream << records[i].title << ","
		           << records[i].artist << ","
		           << records[i].duration << ","
		           << records[i].times_aired << endl;
	}
	
	out_stream.close();
	return 0;
}

string get_cell_text(unsigned _col_num, string _s, char _delim, char _tx_delim)
{
	size_t start = 0, end = 0;
	if(_s.size() == 0) {return "";}
	
	for(unsigned i = 0; i != _col_num; ++i) {
		if(end == string::npos) {return "";}
		
		if(end > 0) {start = end + 1;} //if not first column - move start right of delimiter
		if(end > 0 && _s[end] == _tx_delim) {++start;} //if last cell was of text type - have to move start another position
		if(_s[start] == _tx_delim) {end = _s.find(_tx_delim, start + 1);} //if cell contents are text = set end to text delimiter
		else {end = _s.find(_delim, start);} //else set end to next column delimiter
	}
	
	size_t substr_len = end - start;
	if(_s[start] == _tx_delim) {++substr_len;} //if cell is of text type - include text delimiter in output
	//cout << "DEBUG: substr len/start/ed: " << substr_len << "/" << start << "/" << end << endl;
	return _s.substr(start, substr_len);
}

int update_recordset(const record& _src, vector<record>& _dest, char _txt_delim)
{
	for(unsigned i = 0; i < _dest.size(); ++i) {
		if(_src.title == _dest[i].title && _src.artist == _dest[i].artist) {
			++_dest[i].times_aired;
			
			bool flag_err = false;
			// normalizing time, as DAD sometimes cuts songs off too early
			unsigned src_dur = strtime_to_n(_src.duration, _txt_delim, flag_err);
			if(flag_err) {return -1;}
			unsigned dest_dur = strtime_to_n(_dest[i].duration, _txt_delim, flag_err);
			if(flag_err) {return -1;}
			if(src_dur > dest_dur) {
				_dest[i].duration = _src.duration;
			}
			
			return 0;
		}
	}
	
	_dest.push_back(_src);
	return 0;
}

unsigned strtime_to_n(string _s, char _txt_delim, bool& _flag_err)
{
	//dealing with text delimiters
	if(_s[0] == _txt_delim) {
		_s = _s.substr(1, _s.size() - 2);
	}
	
	if(_s.size() == 0) {return 0;}
	
	if(_s[2] != ':' || _s[5] != ':') {
		cerr << "WARNING: Encountered wrong duration time format!\n";
		_flag_err = true;
		return 0;
	}
	
	unsigned short hours = ((_s[0] - '0') * 10) + (_s[1] - '0');
	unsigned short minutes = ((_s[3] - '0') * 10) + (_s[4] - '0');
	unsigned short seconds = ((_s[6] - '0') * 10) + (_s[7] - '0');
	
	return seconds + (minutes * 60) + (hours * 3600);
}
