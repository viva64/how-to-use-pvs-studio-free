//  2006-2008 (c) Viva64.com Team
//  2008-2016 (c) OOO "Program Verification Systems"

#include "comments.h"
#include "encoding.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#ifdef __cpp_lib_filesystem
#include <filesystem>
#else
#include <experimental/filesystem>
using namespace std::experimental;
#endif
using namespace std;

static bool IsSourceFile(const filesystem::path &path)
{
  static const vector<string> sourceExtensions = {
      ".c"
    , ".cc"
    , ".cpp"
    , ".cp"
    , ".cxx"
    , ".c++"
    , ".cs"
    , ".java"
  };
  string ext = path.extension().string();
  transform(ext.begin(), ext.end(), ext.begin(), [](char ch){return tolower(ch);});
  return find(sourceExtensions.begin(), sourceExtensions.end(), ext) != sourceExtensions.end();
}

class ProgramOptions
{
  ProgramOptions() = default;
  ProgramOptions(const ProgramOptions& root) = delete;
  ProgramOptions& operator=(const ProgramOptions&) = delete;
public:
  size_t m_commentType     = -1;
  vector<string> m_files;
  bool m_multiline         = false;
  bool m_readSymlinks      = false;
  
  static ProgramOptions& Instance()
  {
    static ProgramOptions programOptions;
    return programOptions;
  }
};

void WriteComment(const filesystem::path &path,
                  const string &comment,
                  const string &source,
                  bool isUnixLineEnding,
                  Encoding enc,
                  size_t bomLen)
{
  ofstream osrc(path, ios::binary);
  if (!osrc.is_open())
  {
    cerr << "Error: Couldn't open " << path << endl;
    return;
  }

  if (bomLen != 0)
  {
    osrc.write(source.c_str(), bomLen);
  }

  ConvertEncoding(osrc, comment, enc, !isUnixLineEnding);
  osrc.write(source.c_str() + bomLen, source.length() - bomLen);
}

static void AddCommentsToFile(const filesystem::path &path, const string &comment, const ProgramOptions &options)
{
  if (!IsSourceFile(path))
    return;

  cout << "+ " << path << endl;

  ifstream isrc(path, ios::binary);
  if (!isrc.is_open())
  {
    cerr << "Error: Couldn't open " << path << endl;
    return;
  }

  string str;
  isrc.seekg(0, ios::end);
  str.reserve(isrc.tellg());
  isrc.seekg(0, ios::beg);

  str.assign((istreambuf_iterator<char>(isrc)), istreambuf_iterator<char>());
  isrc.close();
  string source = str;

  Encoding enc;
  size_t bomLen;
  ConvertEncoding(str, enc, bomLen);

  bool isUnixLineEnding = true;
  size_t idx = str.find('\n');
  if (idx != string::npos && idx != 0 && str[idx - 1] == '\r')
    isUnixLineEnding = false;

  PvsStudioFreeComments::CommentsParser parser;
  auto itFreeComment = parser.readFreeComment(source.c_str());
  const char *beg = parser.freeCommentBegin();
  const char *end = parser.freeCommentEnd();
  
  if (itFreeComment != PvsStudioFreeComments::Comments.end())
  {
    bool needToChangeComment =    (!options.m_multiline && beg[0] == '/' && beg[1] == '*')
                               || (options.m_multiline  && beg[0] == '/' && beg[1] == '/')
                               || itFreeComment != PvsStudioFreeComments::Comments.begin() + (ProgramOptions::Instance().m_commentType - 1);
    if (needToChangeComment)
    {
      source.erase(source.begin() + distance(source.c_str(), beg), source.begin() + distance(source.c_str(), end));
      WriteComment(path, comment, source, isUnixLineEnding, enc, bomLen);
    }
  }
  else
  {
    WriteComment(path, comment, source, isUnixLineEnding, enc, bomLen);
  }
}

static void ProcessFile(filesystem::path path, const string &comment, const ProgramOptions &options)
{
  if (options.m_readSymlinks && filesystem::is_symlink(path))
  {
    std::error_code error;
    path = filesystem::canonical(filesystem::read_symlink(path), error);
    if (error != std::error_code())
    {
      return;
    }
  }

  if (filesystem::is_regular_file(path))
  {
    AddCommentsToFile(path, comment, options);
  }
}

static void AddComments(const string &path, const string &comment, const ProgramOptions &options)
{
  auto fsPath = filesystem::canonical(path);
  if (!filesystem::exists(fsPath))
  {
    cerr << "File not exists: " << path << endl;
    return;
  }
  if (filesystem::is_directory(fsPath))
  {
    filesystem::directory_options symlink_flag = options.m_readSymlinks
                                                 ? filesystem::directory_options::follow_directory_symlink
                                                 : filesystem::directory_options::none;
    for (auto &&p : filesystem::recursive_directory_iterator(fsPath, symlink_flag))
    {
      ProcessFile(p.path(), comment, options);
    }
  }
  else
  {
    ProcessFile(fsPath, comment, options);
  }
}

static string Format(const string &comment, bool multiline)
{
  ostringstream ostream;
  istringstream stream(comment);
  string line;

  if (multiline)
  {
    ostream << "/*" << endl;

    while (getline(stream, line))
    {
      ostream << "* " << line << endl;
    }

    ostream << "*/" << endl;
  }
  else
  {
    while (getline(stream, line))
    {
      ostream << "// " << line << endl;
    }
  }

  return ostream.str();
}

static const char Help[] = R"HELP(How to use PVS-Studio for FREE?

  You can use PVS-Studio code analyzer for free, if you add special comments
  to your source code. Available options are provided below.

Usage:

  how-to-use-pvs-studio-free -c <1|2|3> [-m] [-s] [-h] <strings> ...

Options:

  -c <1|2|3>,  /c <1|2|3>,  --comment <1|2|3>
    (required)  Type of comment prepended to the source file.

  -m, /m, --multiline
    Use multi-line comments instead of single-line.

  -s, /s, --symlinks
    Follow the symbolic links and add comment to the files to which symbolic links point (files and directories).
        
  -h, /?, --help
    Display usage information and exit.
    
  <string>  (accepted multiple times)
    (required)  Files or directories.
      
Description:

  The utility will add comments to the files located in the specified folders
  and subfolders. The comments are added to the beginning of the files with the
  extensions .c, .cc, .cpp, .cp, .cxx, .c++, .cs. You don't have to change
  header files. If you use files with other extensions, you can customize this
  utility for your needs. 

  Options of comments that will be added to the code (-c NUMBER):

    1. Personal academic project;
    2. Open source non-commercial project;
    3. Independent project of an individual developer.

  If you are using PVS-Studio as a Visual Studio plugin, then enter the
  following license key: 

    Name: PVS-Studio Free
    Key:  FREE-FREE-FREE-FREE

  If you are using PVS-Studio for Linux, you do not need to do anything else
  besides adding comments to the source code. Just check your code.

  In case none of the options suits you, we suggest considering a purchase of a
  commercial license for PVS-Studio. We are ready to discuss this and other
  questions via e-mail: support@viva64.com.
  
  You can find more details about the free version of PVS-Studio
  here: https://www.viva64.com/en/b/0457/
)HELP";

void help()
{
  cout << Help;
}

struct Option
{
  vector<string> names;
  bool hasArg;
  function<void(string&&)> found;
};

struct ProgramOptionsError {};

typedef vector<Option> ParsedOptions;

static void ParseProgramOptions(int argc, const char *argv[], const ParsedOptions &options, function<void(string&&)> found)
{
  auto findOpt = [&options](const string& arg) {
    return find_if(begin(options), end(options), [arg](const Option &o) {
      return find(o.names.begin(), o.names.end(), arg) != o.names.end();
    });
  };

  for (int i = 1; i < argc; ++i)
  {
    string arg = argv[i];
    auto it = findOpt(arg);
    if (it == end(options))
    {
      found(move(arg));
    }
    else if (it->hasArg)
    {
      ++i;
      if (i == argc)
      {
        cout << "No argument specified for " << arg << endl;
        throw ProgramOptionsError();
      }

      string val = argv[i];
      auto itNext = findOpt(val);
      if (itNext != end(options))
      {
        cout << "No argument specified for " << arg << endl;
        throw ProgramOptionsError();
      }
      it->found(move(val));
    }
    else
    {
      it->found(string());
    }
  }
}

int main(int argc, const char *argv[])
{
  auto &progOptions = ProgramOptions::Instance();
  ParsedOptions options = {
    { { "-c", "/c", "--comment" },   true,   [&](string &&arg) { progOptions.m_commentType  = stoull(arg); } },
    { { "-m", "/m", "--multiline" }, false,  [&](string &&)    { progOptions.m_multiline    = true; } },
    { { "-s", "/s", "--symlinks" },  false,  [&](string &&)    { progOptions.m_readSymlinks = true; } },
    { { "-h", "/?", "--help" },      false,  [&](string &&)    { throw ProgramOptionsError(); } },
  };

  try
  {
    ParseProgramOptions(argc, argv, options, [&files = progOptions.m_files](string &&arg){files.emplace_back(move(arg));});

    unsigned long long n = progOptions.m_commentType;
    if (n == 0 || n > PvsStudioFreeComments::Comments.size())
    {
      throw invalid_argument("");
    }

    string comment = Format(PvsStudioFreeComments::Comments[n - 1].m_text, progOptions.m_multiline);
    for (const string &file : progOptions.m_files)
    {
      AddComments(file, comment, progOptions);
    }
  }
  catch (ProgramOptionsError &)
  {
    help();
    return 1;
  }
  catch (invalid_argument &)
  {
    cout << "Invalid comment type specified" << endl;
    help();
    return 1;
  }

  return 0;
}
