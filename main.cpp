#include <cstdio>
#include <string>
#include <fstream>
#include <cstdint>

// manuals/rinex/rinex211.txt line 393
static constexpr uint32_t rinex_211_max_line_length = 80;
// manuals/rinex/rinex211.txt line 384, minus 1 since c++ is 0 indexed
static constexpr uint32_t rinex_221_header_label_loc = 60;

static const std::string rinex_211_verion_label("RINEX VERSION / TYPE");
static const std::string rinex_211_end_of_header_label("END OF HEADER");

enum class RINEX_VERSION {
  Unknown = 0,
  v2_11,
  v3_00,
  v3_XX,
};

static RINEX_VERSION rinex_get_version_from_header_label(const std::string& label);

static const char* rinex_get_version_str(RINEX_VERSION version);

int main(int argc, char* argv[]) {
  printf("Hello from RTKLIB-CPP\n");

  std::string input_filename(argv[1]);
  printf("Working on file %s\n", input_filename.c_str());

  std::fstream input_file(input_filename, std::ios::in);
  if (!input_file.is_open()) {
    printf("Could not open file\n");
    return 1;
  }
  
  std::string line;
  while (!input_file.eof()) {
    std::getline(input_file, line);
    // sometimes will reach an empty line at end of file or in between rinex epochs
    if (line.empty()) {
      continue;
    }

    const std::string header_label = line.substr(rinex_221_header_label_loc);
    if (header_label == rinex_211_verion_label) {
      auto version = rinex_get_version_from_header_label(line);
      printf("Got rinex version %s\n", rinex_get_version_str(version));
    }

    // End of header, move onto observation parsing
    if (header_label == rinex_211_end_of_header_label) {
      printf("Found end of header %s\n", line.c_str());
      break;
    }
  }

  return 1;
}

static RINEX_VERSION rinex_get_version_from_header_label(const std::string& label) {
  // manuals/rinex/rinex211.txt line 1073 specifies version is 9 characters long and is 
  // first element in the label
  // round to integer to avoid floating point comparison and * 100 to remove decimal point
  const int32_t version = (int32_t)(stod(label.substr(0, 9)) * 100);

  if (version == 211) {
    return RINEX_VERSION::v2_11;
  }
  else if (version == 300) {
    return RINEX_VERSION::v3_00;
  }

  return RINEX_VERSION::Unknown;
}

static const char* rinex_get_version_str(RINEX_VERSION version) {
  switch(version) {
    case RINEX_VERSION::v2_11: return "v2.11";
    case RINEX_VERSION::v3_00: return "v3.00";
    
    case RINEX_VERSION::Unknown:
    default:
      return "Unknown";
  }
}