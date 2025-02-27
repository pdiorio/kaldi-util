#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/regex.h>
#include <unicode/numfmt.h>
#include <iostream>
#include <fstream>
#include <map>

std::map<int, icu::UnicodeString> read_lines(std::string filename)
{
	// initialize lookup dictionary
	std::map<int, icu::UnicodeString> dictionary;

	// regex setup; split on whitespace upto a max of 3 per line
	// first field is integer number, second field is word, the rest of the line is stored in third field
	UErrorCode status = U_ZERO_ERROR;
	icu::RegexMatcher m("\\s+", 0, status); 
	const int maxWords = 3;

	// setup file
	std::ifstream words_file;

	words_file.open(filename);	// file.seekg(0, file.beg);

	if (words_file.fail()) {
		std::cerr << "Can't open kaldi words file: " << filename << std::endl;
		return dictionary;
	}

	// setup temporary line variable
	std::string line;
	while (std::getline(words_file, line))
	{
		// convert line to ICU unicode string
		icu::UnicodeString ucs = icu::UnicodeString::fromUTF8(StringPiece(line.c_str()));

		// initialize array of unicodestrings, upto max set previously
		icu::UnicodeString words[maxWords];    
    	
		// split string into components
		int numWords = m.split(ucs, words, maxWords, status);
    	
		// skip to next line if less than 2 elements
		if (numWords < 2) {
			continue;
		}

		// add entry to dictionary mapping; enforce uppercase
		try {
			std::string int_string;
			words[0].toUTF8String(int_string);
			dictionary[atoi(int_string.c_str())] = words[1].toUpper();		
		} 
		catch (std::exception &e) {
			std::cerr << "ERROR Standard exception: " << e.what() << std::endl;
		}
	}
	words_file.close();
	return dictionary;
}

int main(int argc, char* argv[])
{
	// read first argument; must be a file path
	std::string filename = argv[1];

	// parse lines from filename into a dictionary mapping from int->word
	std::map<int, icu::UnicodeString> dict = read_lines(filename);

	UErrorCode success = U_ZERO_ERROR;
	icu::NumberFormat *nf = icu::NumberFormat::createInstance(success);
	// seems to run faster when not absolutely enforcing min/max fractional digits
	//nf->setMinimumFractionDigits(0);
    //nf->setMaximumFractionDigits(0);
    nf->setGroupingUsed(false);

	// display entire map using iterators
	for (std::map<int, icu::UnicodeString>::iterator it = dict.begin(); it != dict.end(); ++it) {
		//std::cout << it->first << ": " << it->second << std::endl;

		icu::UnicodeString temp_ustring;
		std::cout << nf->format(it->first, temp_ustring) << ": " << it->second << std::endl;
	}

	return 0;
}