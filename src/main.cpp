// (c) 2022 Jens Ganter-Benzing. Licensed under the MIT license.
#include <limits>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <colmc/setup.h>
#include <colmc/sequences.h>

using namespace std;
using namespace std::filesystem;
using namespace colmc;

namespace {

bool starts_with(const std::string& s, const char* p) {
	const auto len = strlen(p);
	return (len <= s.length() && (memcmp(s.c_str(), p, len) == 0));
}

void print_help() {
	// TODO
}

void remove_comments(vector<string>& lines)
{
	for (auto l = lines.begin(); l != lines.end();) {
		if (starts_with(*l, "#")) {
			l = lines.erase(l);
		}
		else {
			++l;
		}
	}
}

struct pixel {
	char fore    = ' ';
	char back    = ' ';
	char content = ' ';
};

enum class parsing_mode { unknown, parsing_fore, parsing_back, parsing_content };

void validate_color(char c) {
	static const char valid_colors[] = " xrgybmcwXRGYBMCW";
	bool valid = false;
	for (size_t i = 0; i < (sizeof(valid_colors) - 1u); ++i) {
		if (c == valid_colors[i]) {
			valid = true;
			break;
		}
	}
	if (!valid) {
		cout << fore::red << "Unknown color character '" << c << "'" << fore::reset << endl;
		exit(1);
	}
}

void line_to_row(const string& line, vector<pixel>& row, parsing_mode m) {
	if (row.size() < line.length()) {
		row.resize(line.length());
	}
	switch (m) {
		case parsing_mode::parsing_back: {
			for (size_t i = 0; i < line.length(); ++i) {
				validate_color(line[i]);
				row[i].back = line[i];
			}
			break;
		}
		case parsing_mode::parsing_fore: {
			for (size_t i = 0; i < line.length(); ++i) {
				validate_color(line[i]);
				row[i].fore = line[i];
			}
			break;
		}
		default: {
			assert(m == parsing_mode::parsing_content);
			for (size_t i = 0; i < line.length(); ++i) {
				row[i].content = line[i];
			}
			break;
		}
	}
}

void lines_to_screen(const vector<string>& lines, vector<vector<pixel>>& screen) {
	parsing_mode	m = parsing_mode::unknown;
	size_t curr_row = 0;
	for (const auto& line: lines) {
		if (line.starts_with("--- ")) {
			const auto cmd = line.substr(4u);
			if ((cmd == "foreground") || (cmd == "fg"))
			{
				m = parsing_mode::parsing_fore;
				curr_row = 0;
			}
			else if ((cmd == "background") || (cmd == "bg"))
			{
				m = parsing_mode::parsing_back;
				curr_row = 0;
			}
			else if ((cmd == "content") || (cmd == "ct"))
			{
				m = parsing_mode::parsing_content;
				curr_row = 0;
			}
			else {
				cout << fore::red << "Unknown command '--- " << cmd << "'" << fore::reset << endl;
				exit(1);
			}
		}
		else { // content
			if (m == parsing_mode::unknown) {
				cout << fore::red << "Unknown mode. Please add some '--- <mode>' statements" << fore::reset << endl;
				exit(1);
			}
			if (curr_row >= screen.size()) {
				screen.resize(curr_row + 1u);
			}
			auto& row = screen[curr_row];
			line_to_row(line, row, m);
			++curr_row;
		}
	}
}

void print(const vector<vector<pixel>>& screen) {
	char fore = ' ';
	char back = ' ';
	for (const auto& row : screen) {
		for (const auto& pix : row) {
			const auto back_wo_bright = static_cast<char>(tolower(pix.back));
			if (back_wo_bright != back) {
				switch (back_wo_bright) {
					case ' ': cout << back::reset;   break;
					case 'x': cout << back::black;   break;
					case 'r': cout << back::red;     break;
					case 'g': cout << back::green;   break;
					case 'y': cout << back::yellow;  break;
					case 'b': cout << back::blue;    break;
					case 'm': cout << back::magenta; break;
					case 'c': cout << back::cyan;    break;
					case 'w': cout << back::white;   break;
					default:                         break;
				}
				back = back_wo_bright;
			}
			if (pix.fore != fore) {
				char fore_wo_bright = pix.fore;
				if (fore_wo_bright == ' ') {
					cout << fore::reset;
				}
				else if (isupper(fore_wo_bright)) {
					cout << fore::bright;
					fore_wo_bright = static_cast<char>(tolower(fore_wo_bright));
				}
				switch (fore_wo_bright) {
					case 'x': cout << fore::black;   break;
					case 'r': cout << fore::red;     break;
					case 'g': cout << fore::green;   break;
					case 'y': cout << fore::yellow;  break;
					case 'b': cout << fore::blue;    break;
					case 'm': cout << fore::magenta; break;
					case 'c': cout << fore::cyan;    break;
					case 'w': cout << fore::white;   break;
					default:                         break;
				}
				fore = pix.fore;
			}
			cout << pix.content;
		}
		cout << fore::reset << back::reset << endl;
		fore = back = ' ';
	}
}

void process(vector<string> lines) {
	vector<vector<pixel>> screen;
	lines_to_screen(lines, screen);
	print(screen);
}

}

int main(int argc, char *argv[]) {
	setup();
	if (argc != 2) {
		cout << fore::red << "Please pass either '--help' or a file name to render on the console" << fore::reset << endl;
		return 1;
	}
	const string arg{argv[1]};
	if (arg == "--help") {
		print_help();
		return 0;
	}
	path p{arg};
	if (!exists(p) || !is_regular_file(p)) {
		cout << fore::red << "No file passed." << fore::reset << endl;
		return 1;
	}
	ifstream file{p};
	string line;
	vector<string> lines;
	while (getline(file, line)) {
		lines.push_back(move(line));
	}
	remove_comments(lines);
	file.close();
	process(move(lines));
	return 0;
}
