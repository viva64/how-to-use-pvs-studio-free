//  2006-2008 (c) Viva64.com Team
//  2008-2016 (c) OOO "Program Verification Systems"

#ifndef PVS_STUDIO_FREE_COMMENTS_COMMENTS_H
#define PVS_STUDIO_FREE_COMMENTS_COMMENTS_H

#include <string>
#include <vector>
#include <sstream>

namespace PvsStudioFreeComments
{
  struct Comment
  {
    Comment() = default;

    Comment(const char* t) : text(t), lines(0)
    {
      std::string line;
      std::istringstream stream(t);
      while (std::getline(stream, line))
      {
        addLine(line.data(), line.data() + line.length());
      }
    }

    void addLine(const char *begin, const char *end)
    {
      while (begin != end && isspace(*begin))
        ++begin;
      while (begin != end && isspace(*(end - 1)))
        --end;

      if (begin != end)
      {
        ++lines;
        trimmedText.append(begin, end);
      }
    }

    std::string text;
    std::string trimmedText;
    size_t lines;
  };

  extern const std::vector<Comment> Comments;
  bool HasAnyComment(const char* buf);
}

#endif

