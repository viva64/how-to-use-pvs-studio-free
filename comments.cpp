//  2006-2008 (c) Viva64.com Team
//  2008-2019 (c) OOO "Program Verification Systems"

#include "comments.h"
#include <cassert>
#include <sstream>

namespace PvsStudioFreeComments
{
  const std::vector<Comment> Comments = {
    #include "comments_msg.h"
  };

  Comment::Comment(const char* t) : m_text(t)
  {
    std::string line;
    std::istringstream stream(t);
    while (std::getline(stream, line))
    {
      addLine(line.data(), line.data() + line.length());
    }
  }

  bool Comment::addLine(const char *begin, const char *end)
  {
    size_t width = 0;

    while (begin != end && !isalnum(static_cast<const unsigned char>(*begin)))
    {
      width += (*begin == '\t' ? 4 : (*begin == ' ' ? 1 : 0));
      ++begin;
    }

    if (width >= 80)
    {
      return false;
    }

    while (begin != end && isspace(static_cast<const unsigned char>(*(end - 1))))
    {
      --end;
    }

    if (begin != end)
    {
      if (!m_trimmedText.empty())
      {
        m_trimmedText += ' ';
      }
      m_trimmedText.append(begin, end);
    }

    return true;
  }


  FreeComments::const_iterator CommentsParser::readFreeComment(const char* buf)
  {
    m_comment.m_trimmedText.clear();
    m_skippedLines = 0;
   
    while (buf != nullptr && *buf != '\0')
    {
      buf = readComment(buf);

      for (auto it = Comments.begin(); it != Comments.end(); ++it)
      {
        bool isCommentsEqual = std::equal(it->m_trimmedText.begin(),       it->m_trimmedText.end(),
                                          m_comment.m_trimmedText.begin(), m_comment.m_trimmedText.end(),
                                          [](auto a, auto b) { return tolower(a) == tolower(b); });
        if (isCommentsEqual)
        {
          return it;
        }
      }
    }

    return Comments.end();
  }

  void CommentsParser::changeState(State state)
  {
    if (m_state != state)
    {
      m_comment.m_trimmedText.clear();
      std::swap(m_state, state);
    }
  }

  const char* CommentsParser::skipNewLine(const char *it)
  {
    if (it == nullptr)
    {
      assert(false);
      return nullptr;
    }

    if (it[0] == '\r')
    {
      it = (it[1] == '\n') ? it + 2 : it + 1;
    }
    else
    {
      it = it[0] == '\n' ? it + 1 : it;
    }

    return it;
  }

  const char* CommentsParser::readComment(const char* it)
  {
    while (isspace(static_cast<const unsigned char>(*it)) && m_skippedLines < MaxSkippedLines)
    {
      if (it[0] == '\n')
        ++m_skippedLines;
      ++it;
    }

    if (m_skippedLines >= MaxSkippedLines)
      return nullptr;

    if (it[0] == '/' && it[1] == '/')
    {
      changeState(State::Oneline);
      return readOnelineComment(it);
    }
    else if (it[0] == '/' && it[1] == '*')
    {
      changeState(State::Multiline);
      m_comment.m_begin = it;
      return readMultilineComment(it + 2);
    }
    else
    {
      m_comment.m_begin = m_comment.m_end = nullptr;
      changeState(State::Unknown);
    }

    return nullptr;
  }

  const char* CommentsParser::readOnelineComment(const char *it)
  {
    if (m_comment.m_begin == nullptr)
    {
      m_comment.m_begin = it;
    }

    const char *begin = it + 2;
    const char *end = begin;

    while (*end != '\n' && *end != '\r' && *end != '\0')
    {
      ++end;
    }

    it = skipNewLine(end);
    m_comment.m_end = it;
    if (!m_comment.addLine(begin, end))
    {
      changeState(State::Unknown);
      it = nullptr;
    }

    return it;
  }

  const char* CommentsParser::readMultilineComment(const char* it)
  {
    while (true)
    {
      const char *begin = it;
      const char *end = begin;

      while (*end != '\n' && *end != '\r' && *end != '\0' && (end[0] != '*' || end[1] != '/'))
      {
        ++end;
      }

      if (*end == '*')
      {
        it = skipNewLine(end + 2);

        m_comment.m_end = it;
        m_state = State::Unknown;
        return it;
      }
      else
      {
        it = skipNewLine(end);
      }

      if (!m_comment.addLine(begin, end))
      {
        changeState(State::Unknown);
        return nullptr; 
      }
    }
  }

  bool HasAnyComment(const char* buf)
  {
    CommentsParser parser;
    return parser.readFreeComment(buf) != Comments.end();
  }
}